#include <InteractiveToolkit/InteractiveToolkit.h>
#include <InteractiveToolkit/Platform/Platform.h>

#include <iostream>

using namespace ITKCommon;

struct GlobalInfo {
    uint32_t instance_count;
};

int main(int argc, char* argv[]) {
    Path::setWorkingPath(Path::getExecutablePath(argv[0]));
    //Platform::ThreadDataSet::Instance()->setGlobalThreadPriority(Platform::ThreadPriority::Normal);
    //Platform::Thread::staticInitialization();

    Platform::Signal::Set([](int sngl){
        ITK_ABORT(true, "forced app finishing.\n");
        //Platform::Thread::getMainThread()->interrupt();
    });

    Platform::IPC::BufferIPC global_info_ipc("app_name", sizeof(GlobalInfo));
    {
        GlobalInfo *globalInfo = (GlobalInfo *)global_info_ipc.data;
        if (global_info_ipc.isFirstProcess()) {
            // initialize the global data
            globalInfo->instance_count = 1;
        }
        else {
            // update the global data
            globalInfo->instance_count++;
        }
        printf("Global Initialization (count: %i)\n", globalInfo->instance_count);
        fflush(stdout);
        global_info_ipc.finishInitialization();
    }


    printf("<press enter to exit>\n");
    fgetc(stdin);fgetc(stdin);fgetc(stdin);fgetc(stdin);fgetc(stdin);

    return 0;
}
