#include <InteractiveToolkit/InteractiveToolkit.h>
#include <InteractiveToolkit/Platform/Platform.h>

#include <iostream>
#include <cstdlib>

using namespace ITKCommon;

//
// Shared queue
//
Platform::ObjectQueue<int> queue;

int main(int argc, char *argv[])
{
    Path::setWorkingPath(Path::getExecutablePath(argv[0]));
    Platform::ThreadDataSet::Instance()->setGlobalThreadPriority(Platform::ThreadPriority::Normal);
    Platform::Thread::staticInitialization();

    Platform::Signal::Set([](int sngl)
                          {
                              ITK_ABORT(true, "forced app finishing.\n")
                              // Platform::Thread::getMainThread()->interrupt();
                          });

    using ThreadInt = Platform::ThreadWithParameters<void(int)>;
    std::vector<ThreadInt *> threads;

    ThreadInt *thread_instance;
    for (int i = 0; i < 5; i++)
    {
        threads.push_back(thread_instance = new ThreadInt([](int thread_num)
                                                          {
            bool isSignaled;
            printf("(+) Thread start: %i\n", thread_num);
            while ( !Platform::Thread::isCurrentThreadInterrupted() ) {
                int v = queue.dequeue(&isSignaled);
                if (isSignaled)
                    break;
                printf("    [thread %i]: %i\n", thread_num, v);
                
                Platform::Sleep::millis( ITKCommon::Random::Instance()->getRange(500,2000) );
            }
            printf("(-) Thread end: %i\n", thread_num); }));

        thread_instance->setParameters(i);
        thread_instance->start();
    }

    // read keyboard
    {
        printf("--- Type any number to the Thread (-1 to exit) ---\n");

        char data[1024];
        while (true)
        {
            char *readed_size = fgets(data, 1024, stdin);
            int i = atoi(data);
            if (i == -1)
                break;
            for (int j = 0; j < (int)threads.size(); j++)
                queue.enqueue(i + j);
        }
    }

    for (auto thread : threads)
        delete thread;

    return 0;
}
