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
        //ITK_ABORT(true, "forced app finishing.\n");
        Platform::Thread::getMainThread()->interrupt(); });

    std::string stdout_child_str_output;

#if defined(_WIN32)

    Platform::WindowsPipe child_stdout;
    Platform::Process process("win_script.bat",
                              ITKCommon::StringUtil::parseArgv("\"String from parent process passed as argument to the script.\""), INT32_MAX,
                              NULL, &child_stdout, NULL);

#elif defined(__linux__) || defined(__APPLE__)

    Platform::UnixPipe child_stdout;
    Platform::Process process("bash_script.sh",
                              ITKCommon::StringUtil::parseArgv("\"String from parent process passed as argument to the script.\""), INT32_MAX,
                              NULL, &child_stdout, NULL);

#endif

    // read pipe from process
    {
        std::vector<uint8_t> raw_output;
        Platform::ObjectBuffer buffer;
        printf("[Child App] reading stdout...\n");
        while (!child_stdout.isReadSignaled())
        {
            if (child_stdout.read(&buffer)) {
                raw_output.insert(raw_output.end(), buffer.data, buffer.data + buffer.size);
                for(int i=0;i<buffer.size;i++)
                    putc(buffer.data[i], stdout);
                fflush(stdout);
            }
            fflush(stdout);
        }

        raw_output.push_back(0);
        stdout_child_str_output = ITKCommon::StringUtil::trim((char *)raw_output.data());
    }

    int exit_code = 0;
    printf("[Parent App] waiting process to exit...\n");
    if (process.waitExit(&exit_code, UINT32_MAX))
    {
        printf("[Parent App] Application Exit Detected:\n"
               "    ExitCode: %i\n"
               "    Output: {%s}\n",
               exit_code,
               stdout_child_str_output.c_str());
    }
    else
        printf("[Parent App] Application Exit Detection Error.");

    return 0;
}
