#include <InteractiveToolkit/InteractiveToolkit.h>
#include <InteractiveToolkit/Platform/Platform.h>

#include <iostream>

using namespace ITKCommon;

int main(int argc, char *argv[])
{
    Path::setWorkingPath(Path::getExecutablePath(argv[0]));
    Platform::ThreadDataSet::Instance()->setGlobalThreadPriority(Platform::ThreadPriority::Normal);
    Platform::Thread::staticInitialization();

    Platform::Signal::Set([](int sngl)
                          {
        //ITK_ABORT(true, "forced app finishing.\n");
        Platform::Thread::getMainThread()->interrupt(); });

    if (argc == 2 && argv[1][0] == 'w')
    {
        printf("Openning a queue (normal write)\n");

        Platform::IPC::QueueIPC queue("default", Platform::IPC::QueueIPC_WRITE, 2, sizeof(float) * 2);
        Platform::ObjectBuffer buffer;

        buffer.setSize(sizeof(float) * 2);
        float *data = (float *)buffer.data;
        data[0] = 0.0f;
        data[1] = 100.0f;

        float last_write[2] = {0, 0};

        uint32_t count = 0;
        fflush(stdout);
        while (!Platform::Thread::isCurrentThreadInterrupted())
        {
            fflush(stdout);
            if (queue.write(buffer))
            {
                // write sucess
                fprintf(stdout, "writing: %f %f (%u) ... \n", data[0], data[1], count++);
                fflush(stdout);
            }
            else
                break;
            
            memcpy(last_write, data, sizeof(float) * 2);

            data[0] += 0.001f;
            data[1] += 0.002f;
        }

        fprintf(stdout, "last write: %f %f (%u) ... \n", last_write[0], last_write[1], count++);
        fflush(stdout);
    }
    else if (argc == 2 && argv[1][0] == 'r')
    {
        printf("Openning a queue (normal read)\n");

        Platform::IPC::QueueIPC queue("default", Platform::IPC::QueueIPC_READ, 2, sizeof(float) * 2);
        Platform::ObjectBuffer buffer;

        uint32_t count = 0;
        fflush(stdout);
        while (!Platform::Thread::isCurrentThreadInterrupted())
        {
            if (!queue.read(&buffer))
                break;
            ITK_ABORT(buffer.size != 8, "error to get value from the queue...\n");

            float *data = (float *)buffer.data;
            printf("readed: %f %f (%u)\n", data[0], data[1], count++);
            fflush(stdout);
        }

        {
            float *data = (float *)buffer.data;
            fprintf(stdout, "last readed: %f %f (%u)\n", data[0], data[1], count++);
            fflush(stdout);
        }
    }
    else if (argc == 2 && (strcmp(argv[1], "low_latency_w") == 0))
    {
        printf("Openning a queue (low latency write)\n");

        Platform::IPC::LowLatencyQueueIPC queue("default", Platform::IPC::QueueIPC_WRITE, 2, sizeof(float) * 2);
        Platform::ObjectBuffer buffer;

        buffer.setSize(sizeof(float) * 2);
        float *data = (float *)buffer.data;
        data[0] = 0.0f;
        data[1] = 100.0f;

        float last_write[2] = {0, 0};

        uint32_t count = 0;
        fflush(stdout);
        while (!Platform::Thread::isCurrentThreadInterrupted())
        {
            if (queue.write(buffer))
            {
                // write sucess
                fprintf(stdout, "writing: %f %f (%u) ... \n", data[0], data[1], count++);
                fflush(stdout);
            }
            else
                break;
            memcpy(last_write, data, sizeof(float) * 2);
            data[0] += 0.001f;
            data[1] += 0.002f;
        }

        fprintf(stdout, "last write: %f %f (%u) ... \n", last_write[0], last_write[1], count++);
        fflush(stdout);
    }
    else if (argc == 2 && (strcmp(argv[1], "low_latency_r") == 0))
    {
        printf("Openning a queue (low latency read)\n");
        Platform::IPC::LowLatencyQueueIPC queue("default", Platform::IPC::QueueIPC_READ, 2, sizeof(float) * 2);

        Platform::ObjectBuffer buffer;
        buffer.setSize(sizeof(float) * 2);

        uint32_t count = 0;
        fflush(stdout);
        while (!Platform::Thread::isCurrentThreadInterrupted())
        {

            bool signaled;
            bool has_data = queue.read(&buffer, &signaled);
            if (signaled)
            {
                printf("Signaled!!!\n");
                break;
            }

            ITK_ABORT(!has_data, "Error, the queue returned without any data...\n");
            ITK_ABORT(buffer.size != 8, "error to get value from the queue...\n");

            float *data = (float *)buffer.data;
            printf("readed: %f %f (%u)\n", data[0], data[1], count++);
            fflush(stdout);

            // Platform::Sleep::millis(2000);
        }

        {
            float *data = (float *)buffer.data;
            fprintf(stdout, "last readed: %f %f (%u)\n", data[0], data[1], count++);
            fflush(stdout);
        }
    }
    else
    {
        printf("Examples: \n"
               "\n"
               "# write normal queue:\n"
               "./ipc-queue w\n"
               "\n"
               "# read normal queue:\n"
               "./ipc-queue r\n"
               "\n"
               "# write low latency queue:\n"
               "./ipc-queue low_latency_w\n"
               "\n"
               "# read low latency queue:\n"
               "./ipc-queue low_latency_r\n"
               "\n");
    }

    return 0;
}
