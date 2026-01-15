#include <InteractiveToolkit/InteractiveToolkit.h>
#include <InteractiveToolkit-Extension/InteractiveToolkit-Extension.h>

#include <iostream>

int main(int argc, char *argv[])
{
    ITKCommon::Path::setWorkingPath(ITKCommon::Path::getExecutablePath(argv[0]));

    int w = 32;
    int h = 64;

    ITKCommon::Matrix<uint8_t> image(w * 3, h);
    //    image.clear(255);

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            image[y][x * 3 + 0] = (x % 2) ? 255 : 0;
            image[y][x * 3 + 1] = (y * 255) / h;
            image[y][x * 3 + 2] = (x % 2) ? 255 : 0;
        }
    }

    printf("Writing PNG\n");

    ITKExtension::Image::PNG::writePNG("output.png", w, h, 3, (char *)image.array);

    {
        int output_size;
        auto buff = ITKExtension::Image::PNG::writePNGToMemory(&output_size, w, h, 3, (char *)image.array);
        auto file = ITKCommon::FileSystem::File::fopen("output_from_memory.png", "wb");
        if (file)
        {
            fwrite(buff, sizeof(char), output_size, file);
            ITKCommon::FileSystem::File::fclose(file);
        }
        ITKExtension::Image::PNG::closePNG(buff);
    }

    printf("Writing JPG\n");

    ITKExtension::Image::JPG::writeJPG("output.jpg", w, h, 3, (char *)image.array);

    {
        int output_size;
        auto buff = ITKExtension::Image::JPG::writeJPGToMemory(&output_size, w, h, 3, (char *)image.array);
        auto file = ITKCommon::FileSystem::File::fopen("output_from_memory.jpg", "wb");
        if (file)
        {
            fwrite(buff, sizeof(char), output_size, file);
            ITKCommon::FileSystem::File::fclose(file);
        }
        ITKExtension::Image::JPG::closeJPG(buff);
    }

    printf("Done\n");

    return 0;
}
