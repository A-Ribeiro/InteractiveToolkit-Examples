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
    Platform::ThreadPool threadPool( Platform::Thread::QueryNumberOfSystemThreads() );

    MathRandomExt<Random> mathRandom(Random::Instance());

    printf("generating random points...\n");

    std::vector<PointDistance> rnd_points(0x3ffffff);
    vec2d origin = mathRandom.next<vec2d>();
    for (auto &pt : rnd_points)
    {
        pt.pos = mathRandom.next<vec2d>();
        pt.distance = OP<vec2d>::distance(pt.pos, origin);
    }

    //
    // sort
    //

    uint64_t min = UINT64_MAX;
    uint64_t max = 0;

    // create index array
    std::vector<SortIndexu64> ref_array(rnd_points.size());
    for (uint32_t i=0; i < (uint32_t)ref_array.size(); i++)
    {
        ref_array[i].index = i;
        ref_array[i].toSort = SortToolu64::doubleToInt(rnd_points[i].distance);

        min = MathCore::OP<uint64_t>::minimum(min, ref_array[i].toSort);
        max = MathCore::OP<uint64_t>::maximum(max, ref_array[i].toSort);
    }

    printf("\n SPREAD => min: %" PRIu64 " max: %" PRIu64 "\n\n", min, max);

    // Spread facilitates if you have implemented 
    // static task distribution among all available UINT64_MAX range.
    for(auto& item: ref_array)
        item.toSort = SortToolu64::spread(min, max, item.toSort);

    Platform::Time t;

    printf("[Single-Thread Sorting]\n");

    // Check sorting...
    {
        bool sorted = true;
        for (size_t i=0;i<ref_array.size()-1;i++){
            const auto &refA = ref_array[i];
            const auto &refB = ref_array[i+1];
            sorted = sorted && rnd_points[refA.index].distance <= rnd_points[refB.index].distance;
        }
        printf("    Set sorted check (before): %i\n",sorted);
    }
    
    t.update();
    // sort O(n)
    RadixCountingSortu64::sortIndex(ref_array.data(), (uint32_t)ref_array.size());
    t.update();

    printf("    Time: %f sec\n", t.deltaTime);

    // Check sorting...
    {
        bool sorted = true;
        for (size_t i=0;i<ref_array.size()-1;i++){
            const auto &refA = ref_array[i];
            const auto &refB = ref_array[i+1];
            sorted = sorted && rnd_points[refA.index].distance <= rnd_points[refB.index].distance;
        }
        printf("    Set sorted check (after): %i\n",sorted);
    }

    // create index array
    for (uint32_t i=0; i < (uint32_t)ref_array.size(); i++)
    {
        ref_array[i].index = i;
        ref_array[i].toSort = SortToolu64::doubleToInt(rnd_points[i].distance);

        ref_array[i].toSort = SortToolu64::spread(min, max, ref_array[i].toSort);
    }

    printf("[Multi-Thread Sorting]\n");

    // Check sorting...
    {
        bool sorted = true;
        for (size_t i=0;i<ref_array.size()-1;i++){
            const auto &refA = ref_array[i];
            const auto &refB = ref_array[i+1];
            sorted = sorted && rnd_points[refA.index].distance <= rnd_points[refB.index].distance;
        }
        printf("    Set sorted check (before): %i\n",sorted);
    }

    t.update();
    ParallelRadixCountingSortu64::sortIndex( ref_array.data(), ref_array.size(), &threadPool);
    t.update();

    printf("    Time: %f sec\n", t.deltaTime);

    // Check sorting...
    {
        bool sorted = true;
        for (size_t i=0;i<ref_array.size()-1;i++){
            const auto &refA = ref_array[i];
            const auto &refB = ref_array[i+1];
            sorted = sorted && rnd_points[refA.index].distance <= rnd_points[refB.index].distance;
        }
        printf("    Set sorted check (after): %i\n",sorted);
    }

    return 0;
}
