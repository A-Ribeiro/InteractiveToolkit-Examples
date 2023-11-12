#include <InteractiveToolkit/InteractiveToolkit.h>

#include <iostream>

using namespace ITKCommon;

#include "Condition.h"

int main(int argc, char *argv[])
{
    Path::setWorkingPath(Path::getExecutablePath(argv[0]));
    Platform::ThreadPool threadPool(Platform::Thread::QueryNumberOfSystemThreads());

    int count = 5;

    Platform::Mutex counter_m;
    Platform::Condition counter_cv;

    std::vector<Platform::Thread*> threads;

    for(int i=0;i<10;i++){
        threads.push_back(new Platform::Thread([&](){
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
            printf("thread exit\n");
        }));
        (*(threads.end()-1))->start();
    }

    while (count > 0){
        printf("%i\n", count);
        Platform::Sleep::millis(1000);
        count--;
    }

    printf("notify\n");
    counter_cv.notify_all();

    for(auto thread:threads)
        delete thread;

    return 0;
}
