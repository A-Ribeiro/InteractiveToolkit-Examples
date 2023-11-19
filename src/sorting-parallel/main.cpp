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
    Platform::ThreadPool threadPool( Platform::Thread::QueryNumberOfSystemThreads() );

    MathRandomExt<Random> mathRandom(Random::Instance());

    printf("generating random points...\n");

    std::vector<PointDistance> rnd_points(50000000);
    vec2f origin = mathRandom.next<vec2f>();
    for (auto &pt : rnd_points)
    {
        pt.pos = mathRandom.next<vec2f>();
        pt.distance = OP<vec2f>::distance(pt.pos, origin);
    }

    //
    // sort
    //

    printf("[Single-Thread Sorting]\n");

    // create index array
    std::vector<SortIndexu> ref_array(rnd_points.size());
    for (uint32_t i=0; i < (uint32_t)ref_array.size(); i++)
    {
        ref_array[i].index = i;
        ref_array[i].toSort = SortToolu::floatToInt(rnd_points[i].distance);
    }

    Platform::Time t;

    // sort O(n)
    t.update();
    RadixCountingSortu::sortIndex(ref_array.data(), (uint32_t)ref_array.size());
    t.update();

    printf("    Time: %f sec\n", t.deltaTime);

    printf("[Multi-Thread Sorting]\n");

    // create index array
    for (uint32_t i=0; i < (uint32_t)ref_array.size(); i++)
    {
        ref_array[i].index = i;
        ref_array[i].toSort = SortToolu::floatToInt(rnd_points[i].distance);
    }

    t.update();
    ParallelRadixCountingSortu::sortIndex( ref_array.data(), ref_array.size(), &threadPool);
    t.update();

    printf("    Time: %f sec\n", t.deltaTime);

    // Check sorting...
    bool sorted = true;
    for (size_t i=0;i<ref_array.size()-1;i++){
        const auto &refA = ref_array[i];
        const auto &refB = ref_array[i+1];
        sorted = sorted && rnd_points[refA.index].distance <= rnd_points[refB.index].distance;
    }
    printf("Final set sorted: %i\n",sorted);

    return 0;
}
