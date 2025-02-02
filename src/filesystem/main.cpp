#include <InteractiveToolkit/InteractiveToolkit.h>
#include <InteractiveToolkit/Platform/Platform.h>

#include <iostream>
#include <stdlib.h>

using namespace ITKCommon;

void list_commands()
{
    printf("Examples: \n"
           "\n"
           "# list files and directories\n"
           "./filesystem ls <dir>\n"
           "\n");
}

void list_dir(const std::string relative_dir_str)
{

#if defined(_WIN32)

#define GEN_RESET
#define GEN_BRIGHT
#define GEN_UNDERSCORE

#define F_BLACK
#define F_RED
#define F_GREEN
#define F_YELLOW
#define F_BLUE
#define F_MAGENTA
#define F_CYAN
#define F_WHITE

#define B_BLACK
#define B_RED
#define B_GREEN
#define B_YELLOW
#define B_BLUE
#define B_MAGENTA
#define B_CYAN
#define B_WHITE

#else

    // https://en.wikipedia.org/wiki/ANSI_escape_code

#define GEN_RESET "\x1B[0m"
#define GEN_BRIGHT "\x1B[1m"
#define GEN_UNDERSCORE "\x1B[4m"

#define F_BLACK "\x1B[30m"
#define F_RED "\x1B[31m"
#define F_GREEN "\x1B[32m"
#define F_YELLOW "\x1B[33m"
#define F_BLUE "\x1B[34m"
#define F_MAGENTA "\x1B[35m"
#define F_CYAN "\x1B[36m"
#define F_WHITE "\x1B[37m"

#define B_BLACK "\x1B[40m"
#define B_RED "\x1B[41m"
#define B_GREEN "\x1B[42m"
#define B_YELLOW "\x1B[43m"
#define B_BLUE "\x1B[44m"
#define B_MAGENTA "\x1B[45m"
#define B_CYAN "\x1B[46m"
#define B_WHITE "\x1B[47m"

#endif

    FileSystem::Directory dir(relative_dir_str);

    size_t max_size = 5; // " file"
    for (auto &file : dir)
    {
        if (file.name.length() + 1 > max_size)
            max_size = file.name.length() + 1;
    }

    max_size++;

    {
        printf(GEN_UNDERSCORE " file");
        size_t needed_size = max_size - 5;
        while (needed_size > 0)
        {
            putc(' ', stdout);
            needed_size--;
        }

        printf("| size      ");
        printf("| last modified       ");
        printf("| created at          " GEN_RESET "\n");
    }

    for (auto &file : dir)
    {
        if (file.isDirectory)
            printf(GEN_BRIGHT F_GREEN " %s" GEN_RESET, file.name.c_str());
        else
            printf(" %s", file.name.c_str());

        // stats
        size_t needed_size = max_size - file.name.length() - 1;
        while (needed_size > 0)
        {
            putc(' ', stdout);
            needed_size--;
        }

        auto size_str = ITKCommon::ByteUtils::byteSmartPrint(file.size, "B");

        printf("| %s", size_str.c_str());

        needed_size = 12 - size_str.length() - 2;
        while (needed_size > 0)
        {
            putc(' ', stdout);
            needed_size--;
        }

        printf("| %s ", file.lastWriteTime.toString(true).c_str());
        printf("| %s\n", file.creationTime.toString(true).c_str());
    }
}

int main(int argc, char *argv[])
{
    if (argc == 3 && strcmp(argv[1], "ls") == 0)
    {
        list_dir(argv[2]);
    }
    else
        list_commands();

    return 0;
}
