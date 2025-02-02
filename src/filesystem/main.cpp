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

#define COMPOSED_ERROR "\x1B[47;31m" // B comes before F setup

#endif

    FileSystem::Directory dir(relative_dir_str);

    size_t max_size = 5; // " file"
    for (auto &file : dir)
    {
        int increase_size = 0;
        if (file.isLink) {
            auto link = file.readLink();
            increase_size = link.length() + 4; // " -> "
        }

        if (file.name.length() + increase_size + 1 > max_size)
            max_size = file.name.length()  + increase_size + 1;
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
        auto name_to_render = file.name;
        if (file.isLink){
            // broken link test
            if (file.isDirectory || file.isFile)
                name_to_render = name_to_render + " -> " + file.readLink();
        }

        if (file.isDirectory) {
            if (file.isLink)
                printf(GEN_BRIGHT F_CYAN " %s" GEN_RESET, name_to_render.c_str());
            else
                printf(GEN_BRIGHT F_GREEN " %s" GEN_RESET, name_to_render.c_str());
        } else if (file.isFile) {
            if (file.isLink)
                printf( F_CYAN " %s" GEN_RESET, name_to_render.c_str());
            else
                printf(" %s", name_to_render.c_str());
        } else {
            // broken link case
            if (file.isLink){
                printf( " " F_YELLOW B_RED "%s" GEN_RESET " -> " F_YELLOW B_RED "%s" GEN_RESET, name_to_render.c_str(), file.readLink().c_str());
                name_to_render = name_to_render + " -> " + file.readLink();
            }
            else 
                printf( " " F_YELLOW B_RED "%s" GEN_RESET, name_to_render.c_str());
        }

        // stats
        size_t needed_size = max_size - name_to_render.length() - 1;
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
