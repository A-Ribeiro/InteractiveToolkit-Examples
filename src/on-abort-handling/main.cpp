#include <InteractiveToolkit/InteractiveToolkit.h>
#include <iostream>

using namespace ITKCommon;
using namespace EventCore;

int main(int argc, char* argv[]) {
    Path::setWorkingPath(Path::getExecutablePath(argv[0]));

    ITKAbort::Instance()->OnAbort.add([](const char *file, int line, const char *message){
        // It is possible to write the abort event info to a log file
        printf("Custom abort event handling on:\n[%s:%i]\n%s\n", file, line, message);
    });

    //force program abort
    ITK_ABORT(true, "Forced abort!")

    return 0;
}
