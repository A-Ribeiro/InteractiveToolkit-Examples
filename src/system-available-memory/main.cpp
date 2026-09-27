#include <InteractiveToolkit/InteractiveToolkit.h>

int main(int argc, char *argv[])
{
    printf("Total RAM: %s\n", ITKCommon::ByteUtils::byteSmartPrint(ITKCommon::Memory::total_ram(), "B").c_str());
    printf("Available RAM: %s\n", ITKCommon::ByteUtils::byteSmartPrint(ITKCommon::Memory::available_ram(), "B").c_str());

    uint64_t bit_rate = 200000000;

    printf("%s\n", ITKCommon::ByteUtils::byteSmartPrint(bit_rate / 8, "Bps").c_str());
    printf("%s\n", ITKCommon::ByteUtils::bitSmartPrint(bit_rate, "bps").c_str());

    using namespace MathCore;

    typedef vec2<float, SIMD_TYPE::NONE> vec;
    vec a(0, 5), b(-4.33f, -2.5f), c(4.33f, -2.5f);
    bool v = OP<vec>::is_origin_inside_circle(a-vec(0, 5), b-vec(0,5), c-vec(0,5));
    printf("%i\n", (int)v);

    return 0;
}
