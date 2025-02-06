#include <InteractiveToolkit/InteractiveToolkit.h>
#include "TablePrintf.h"

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
    FileSystem::Directory dir(relative_dir_str);

    TablePrintf table;

    table.setPrintToOutput(false);

    for (int i = 0; i < 2; i++)
    {

        // header
        table.setIsHeader(true);
        table.addItem("file").addItem("size").addItem("last modified").addItem("created at").endRow();
        table.setIsHeader(false);

        for (auto &file : dir)
        {
            // file
            {
                Align align = Align::Left;
                Color foreground = Color::Default;
                Color background = Color::Default;
                Weight weight = Weight::Normal;

                if (file.isDirectory)
                {
                    weight = Weight::Bold;
                    foreground = Color::Green;
                }

                if (file.isLink)
                    foreground = Color::Cyan;

                if (!file.isDirectory && !file.isFile)
                {
                    background = Color::Red;
                    foreground = Color::Yellow;
                }

                if (file.isLink)
                    table.addItem(file.name + " -> " + file.readLink(), align, foreground, background, weight);
                else
                    table.addItem(file.name, align, foreground, background, weight);
            }

            // size
            table.addItem(ITKCommon::ByteUtils::byteSmartPrint(file.size, "B"));
            // last modified
            table.addItem(file.lastWriteTime.toString(true).c_str());
            // created at
            table.addItem(file.creationTime.toString(true).c_str());

            table.endRow();
        }

        table.setPrintToOutput(true);
    }
}

int main(int argc, char *argv[])
{
    if (argc == 3 && strcmp(argv[1], "ls") == 0)
        list_dir(argv[2]);
    else
        list_dir(".");
        //list_commands();

    return 0;
}
