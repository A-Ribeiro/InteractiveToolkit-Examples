#include <InteractiveToolkit/InteractiveToolkit.h>
#include <iostream>

using namespace ITKCommon;
using namespace MathCore;

int main(int argc, char *argv[])
{
    Path::setWorkingPath(Path::getExecutablePath(argv[0]));

    float32x4_t input = (float32x4_t){0,1,2,3};
    {
        float32x4_t rc = vshuffle_3120(input);
        printf("vshuffle_3120: %f %f %f %f\n", rc[0], rc[1], rc[2], rc[3]);
    }
    {
        float32x4_t rc = vshuffle_3102(input);
        printf("vshuffle_3102: %f %f %f %f\n", rc[0], rc[1], rc[2], rc[3]);
    }
    {
        float32x4_t rc = vshuffle_0031(input);
        printf("vshuffle_0031: %f %f %f %f\n", rc[0], rc[1], rc[2], rc[3]);
    }
    {
        float32x4_t rc = vshuffle_3110(input);
        printf("vshuffle_3110: %f %f %f %f\n", rc[0], rc[1], rc[2], rc[3]);
    }
    {
        float32x4_t rc = vshuffle_0031(input);
        printf("vshuffle_0031: %f %f %f %f\n", rc[0], rc[1], rc[2], rc[3]);
    }

    return 0;
}
