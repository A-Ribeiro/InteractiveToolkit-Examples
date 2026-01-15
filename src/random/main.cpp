#include <InteractiveToolkit/InteractiveToolkit.h>
#include <iostream>

using namespace ITKCommon;
using namespace MathCore;

int main(int argc, char *argv[])
{
    Path::setWorkingPath(Path::getExecutablePath(argv[0]));

    //
    // 32bit generator
    //
    Random32 rnd32(Random32::TypeDefinition::randomSeed());
    printf("32bit generator\n");
    printf("rnd32.getRange(10,15): %i\n", rnd32.getRange(10, 15));
    printf("rnd32.next01<float>(): %f\n", rnd32.next01<float>());
    printf("rnd32.next01<double>(): %.15lf\n", rnd32.next01<double>());
    printf("\n");

    //
    // 64bit generator
    //
    Random64 rnd64(Random64::TypeDefinition::randomSeed());
    printf("64bit generator\n");
    printf("rnd64.getRange(10,15): %i\n", rnd64.getRange(10, 15));
    printf("rnd64.next01<float>(): %f\n", rnd64.next01<float>());
    printf("rnd64.next01<double>(): %.15lf\n", rnd64.next01<double>());
    printf("\n");

    //
    // MathCore extension
    //
    printf("MathCore extension\n");
    MathRandomExt<Random> mathRnd(&rnd32);
    {
        vec4f v = mathRnd.next<vec4f>();
        printf("mathRnd.next<vec4f>(): %f %f %f %f\n", v.x, v.y, v.z, v.w);
    }
    {
        vec4f v = mathRnd.nextDirection<vec4f>(true);
        printf("mathRnd.nextDirection<vec4f>(homogeneous=true): %f %f %f %f\n", v.x, v.y, v.z, v.w);
    }
    {
        vec2i v = mathRnd.nextRange<vec2i>(vec2i(10,200),vec2i(20,210));
        printf("mathRnd.nextRange<vec2i>(vec2i(10,200),vec2i(20,210)): %i %i\n", v.x, v.y);
    }
    printf("\n");

    return 0;
}
