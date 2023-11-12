#include <InteractiveToolkit/InteractiveToolkit.h>

#include <iostream>

using namespace ITKCommon;

void single_app_condition_test()
{
    int count = 5;

    Platform::Mutex counter_m;
    Platform::Condition counter_cv;

    std::vector<Platform::Thread *> threads;

    for (int i = 0; i < 10; i++)
    {
        threads.push_back(new Platform::Thread([&]()
                                               {
            printf("thread start\n");
            bool signaled;
            {
                Platform::AutoLock lock(&counter_m);
                while( count > 0 ){
                    printf("thread wait\n");
                    counter_cv.wait(&counter_m,&signaled);
                    if (signaled) {
                        printf("thread exit with signal...\n");
                        return;
                    }
                }
                //critical area
            }
            printf("thread exit\n"); }));
        (*(threads.end() - 1))->start();
    }

    while (count > 0)
    {
        printf("%i\n", count);
        Platform::Sleep::millis(1000);
        count--;
    }

    printf("notify\n");
    counter_cv.notify_all();

    for (auto thread : threads)
        delete thread;
}

void ipc_wait()
{
    Platform::IPC::ConditionIPC cond_cv("shared_cond");
    Platform::IPC::SemaphoreIPC mutex_sem("shared_sem", 1, cond_cv.bufferIPC()->isFirstProcess());
#if defined(__linux__) || defined(__APPLE__)
    cond_cv.bufferIPC()->OnLastBufferFree.add([](Platform::IPC::BufferIPC *)
                                              { Platform::IPC::SemaphoreIPC::force_shm_unlink("shared_sem"); });
#endif

    {
        Platform::IPC::AutoLockSemaphoreIPC lock(&mutex_sem);
        if (lock.signaled)
        {
            printf("signaled\n");
            return;
        }

        printf("waiting...\n");
        bool signaled = false;
        cond_cv.wait(&mutex_sem, &signaled);
        if (signaled)
        {
            printf("signaled\n");
            return;
        }

        printf("normal critial area entering.\n");
    }

    printf("Finished.\n");
}

void ipc_notify()
{
    Platform::IPC::ConditionIPC cond_cv("shared_cond");
    Platform::IPC::SemaphoreIPC mutex_sem("shared_sem", 1, cond_cv.bufferIPC()->isFirstProcess());
#if defined(__linux__) || defined(__APPLE__)
    cond_cv.bufferIPC()->OnLastBufferFree.add([](Platform::IPC::BufferIPC *)
                                              { Platform::IPC::SemaphoreIPC::force_shm_unlink("shared_sem"); });
#endif

    {
        Platform::IPC::AutoLockSemaphoreIPC lock(&mutex_sem);
        if (lock.signaled)
        {
            printf("signaled\n");
            return;
        }

        printf("normal critial area entering.\n");

        cond_cv.notify();
    }

    printf("Finished.\n");
}

void ipc_notify_all()
{
    Platform::IPC::ConditionIPC cond_cv("shared_cond");
    Platform::IPC::SemaphoreIPC mutex_sem("shared_sem", 1, cond_cv.bufferIPC()->isFirstProcess());
#if defined(__linux__) || defined(__APPLE__)
    cond_cv.bufferIPC()->OnLastBufferFree.add([](Platform::IPC::BufferIPC *)
                                              { Platform::IPC::SemaphoreIPC::force_shm_unlink("shared_sem"); });
#endif

    {
        Platform::IPC::AutoLockSemaphoreIPC lock(&mutex_sem);
        if (lock.signaled)
        {
            printf("signaled\n");
            return;
        }

        printf("normal critial area entering.\n");

        cond_cv.notify_all();
    }

    printf("Finished.\n");
}

int main(int argc, char *argv[])
{
    Path::setWorkingPath(Path::getExecutablePath(argv[0]));
    Platform::ThreadDataSet::Instance()->setGlobalThreadPriority(Platform::ThreadPriority::Normal);
    Platform::Thread::staticInitialization();
    Platform::Signal::Set([](int sngl)
                          {
        //ITK_ABORT(true, "forced app finishing.\n");
        Platform::Thread::getMainThread()->interrupt(); });

    if (argc == 2 && (strcmp(argv[1], "single-app") == 0))
        single_app_condition_test();
    else if (argc == 2 && (strcmp(argv[1], "ipc-wait") == 0))
        ipc_wait();
    else if (argc == 2 && (strcmp(argv[1], "ipc-notify") == 0))
        ipc_notify();
    else if (argc == 2 && (strcmp(argv[1], "ipc-notify-all") == 0))
        ipc_notify_all();
    else
    {
        printf("Examples: \n"
               "\n"
               "./condition-variable-using-semaphore single-app\n"
               "\n"
               "./condition-variable-using-semaphore ipc-wait\n"
               "\n"
               "./condition-variable-using-semaphore ipc-notify\n"
               "\n"
               "./condition-variable-using-semaphore ipc-notify-all\n"
               "\n");
    }

    return 0;
}
