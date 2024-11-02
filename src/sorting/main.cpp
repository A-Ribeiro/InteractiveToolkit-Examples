#include <InteractiveToolkit/InteractiveToolkit.h>
#include <iostream>

using namespace ITKCommon;
using namespace MathCore;
using namespace AlgorithmCore::Sorting;

struct PointDistance
{
    vec2d pos;
    double distance;
};

int main(int argc, char *argv[])
{
    Path::setWorkingPath(Path::getExecutablePath(argv[0]));

    MathRandomExt<Random> mathRandom(Random::Instance());

    std::vector<PointDistance> rnd_points(Random::Instance()->getRange(10, 20));
    vec2d origin = mathRandom.next<vec2d>();
    printf("Input:\n");
    printf("origin x: %.3f y: %.3f dst: %.3f\n", origin.x, origin.y, 0.0f);
    for (auto &pt : rnd_points)
    {
        pt.pos = mathRandom.next<vec2d>();
        pt.distance = OP<vec2d>::distance(pt.pos, origin);
        if (Random::Instance()->getRange(0, 20) > 10)
            pt.distance *= -1;
        printf(" x: %.3f y: %.3f dst: %.3f\n", pt.pos.x, pt.pos.y, pt.distance);
    }
    printf("\n");

    //
    // sort
    //

    // create index array
    std::vector<SortIndexi64> ref_array(rnd_points.size());

    printf("checking number conversion:\n");

    for (uint64_t i=0; i < (uint64_t)ref_array.size(); i++)
    {
        ref_array[i].index = i;
        ref_array[i].toSort = SortTooli64::doubleToInt(rnd_points[i].distance);

        printf("%.6f -> 0x%.16" PRIx64 " -> %.6f\n", rnd_points[i].distance, ref_array[i].toSort, SortTooli64::intToDouble(ref_array[i].toSort));
    }

    // sort O(n)
    RadixCountingSorti64::sortIndex(ref_array.data(), (uint32_t)ref_array.size());

    // draw sorted without move source array elements
    printf("Draw sorted content\nWithout move elements from array:\n");
    printf("origin x: %.3f y: %.3f dst: %.3f\n", origin.x, origin.y, 0.0f);
    for (const auto &ref : ref_array){
        const auto &pt = rnd_points[ref.index];
        printf(" x: %.3f y: %.3f dst: %.3f\n", pt.pos.x, pt.pos.y, pt.distance);
    }
    printf("\n");

    // replace content
    SortTooli64::inplaceReplace(ref_array.data(), rnd_points.data(), (uint32_t)ref_array.size());

    // show result
    printf("Draw sorted content\nElements from array were moved:\n");
    printf("origin x: %.3f y: %.3f dst: %.3f\n", origin.x, origin.y, 0.0f);
    for (const auto &pt : rnd_points)
        printf(" x: %.3f y: %.3f dst: %.3f\n", pt.pos.x, pt.pos.y, pt.distance);

    return 0;
}
