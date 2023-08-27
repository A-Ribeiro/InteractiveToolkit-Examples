#include <InteractiveToolkit/InteractiveToolkit.h>
#include <iostream>

using namespace ITKCommon;
using namespace MathCore;
using namespace AlgorithmCore::Sorting;

struct PointDistance
{
    vec2f pos;
    float distance;
};

int main(int argc, char *argv[])
{
    Path::setWorkingPath(Path::getExecutablePath(argv[0]));

    MathRandomExt<Random> mathRandom(Random::Instance());

    std::vector<PointDistance> rnd_points(Random::Instance()->getRange(10, 20));
    vec2f origin = mathRandom.next<vec2f>();
    printf("Input:\n");
    printf("origin x: %.3f y: %.3f dst: %.3f\n", origin.x, origin.y, 0.0f);
    for (auto &pt : rnd_points)
    {
        pt.pos = mathRandom.next<vec2f>();
        pt.distance = OP<vec2f>::distance(pt.pos, origin);
        printf(" x: %.3f y: %.3f dst: %.3f\n", pt.pos.x, pt.pos.y, pt.distance);
    }
    printf("\n");

    //
    // sort
    //

    // create index array
    std::vector<SortIndexu> ref_array(rnd_points.size());
    for (size_t i; i < ref_array.size(); i++)
    {
        ref_array[i].index = i;
        ref_array[i].toSort = SortToolu::floatToInt(rnd_points[i].distance);
    }

    // sort O(n)
    RadixCountingSortu::sortIndex(ref_array.data(), ref_array.size());

    // draw sorted without move source array elements
    printf("Draw sorted content\nWithout move elements from array:\n");
    printf("origin x: %.3f y: %.3f dst: %.3f\n", origin.x, origin.y, 0.0f);
    for (const auto &ref : ref_array){
        const auto &pt = rnd_points[ref.index];
        printf(" x: %.3f y: %.3f dst: %.3f\n", pt.pos.x, pt.pos.y, pt.distance);
    }
    printf("\n");

    // replace content
    SortToolu::inplaceReplace(ref_array.data(), rnd_points.data(), ref_array.size());

    // show result
    printf("Draw sorted content\nElements from array were moved:\n");
    printf("origin x: %.3f y: %.3f dst: %.3f\n", origin.x, origin.y, 0.0f);
    for (const auto &pt : rnd_points)
        printf(" x: %.3f y: %.3f dst: %.3f\n", pt.pos.x, pt.pos.y, pt.distance);

    return 0;
}
