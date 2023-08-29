#include <InteractiveToolkit/InteractiveToolkit.h>
#include <iostream>

using namespace ITKCommon;
using namespace AlgorithmCore::PatternMatch;


int main(int argc, char *argv[])
{
    Path::setWorkingPath(Path::getExecutablePath(argv[0]));

    printf("edit_distance\n");
    printf("edit_distance(\"Hello World!\",\"Hello World!\"): %i\n", edit_distance("Hello World!","Hello World!"));
    printf("edit_distance(\"Hello World!\",\"Helo World!\"): %i\n", edit_distance("Hello World!","Helo World!"));
    printf("edit_distance(\"Hello World!\",\"Helmo World!\"): %i\n", edit_distance("Hello World!","Helmo World!"));
    printf("edit_distance(\"Hello World!\",\"World\"): %i\n", edit_distance("Hello World!","World"));
    printf("\n");

    printf("array_index_of\n");
    printf("input_txt = \"Some Very Long Text Or Binary Content To Search.\"\n");
    printf("pattern_to_search = \"Binary\"\n");

    const char* input_txt = "Some Very Long Text Or Binary Content To Search.";
    const char* pattern_to_search = "Binary";

    int index = array_index_of((const uint8_t*)input_txt,0,strlen(input_txt),
    (const uint8_t*)pattern_to_search,strlen(pattern_to_search));

    printf("array_index_of: %i\n", index);

    printf("\n");
    


    return 0;
}
