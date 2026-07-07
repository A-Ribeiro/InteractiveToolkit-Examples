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
    Platform::ThreadPool threadPool(Platform::Thread::QueryNumberOfSystemThreads());

    MathRandomExt<Random> mathRandom(Random::Instance());

    printf("allocating random points memory...\n");

    std::vector<PointDistance> rnd_points(64 * 1000 * 1000); // 64 million points
    std::vector<SortIndexu32> ref_array(rnd_points.size());
    std::vector<SortIndexu32> tmp_array(rnd_points.size());

    // std::vector<PointDistance> rnd_points(0x3ff);
    vec2f origin = mathRandom.next<vec2f>();

    Platform::Semaphore completion_semaphore(0);
    uint64_t element_count = (uint64_t)rnd_points.size();

    uint64_t thread_count = (uint64_t)threadPool.threadCount();
    uint64_t block_count = (element_count + thread_count - 1) / thread_count;

    printf("Initializing random points...\n");
    for (uint64_t i = 0; i < thread_count; i++)
    {
        uint64_t i_start = block_count * i;
        uint64_t i_end = i_start + block_count;
        if (i_end > element_count)
            i_end = element_count;
        threadPool.postTask(
            [&completion_semaphore, i_start, i_end, &rnd_points, origin]()
            {
                ITKCommon::RandomTemplate<uint32_t> _rnd(RandomDefinition<uint32_t>::randomSeed());
                MathRandomExt<ITKCommon::RandomTemplate<uint32_t>> mathRandom(&_rnd);
                for (uint64_t j = i_start; j < i_end; j++)
                {
                    auto &pt = rnd_points[j];
                    pt.pos = mathRandom.next<vec2f>();
                    pt.distance = OP<vec2f>::distance(pt.pos, origin);
                }
                completion_semaphore.release();
            });
    }
    for (uint64_t i = 0; i < thread_count; i++)
        completion_semaphore.blockingAcquire();

    //
    // sort
    //

    // uint64_t min = UINT64_MAX;
    // uint64_t max = 0;

    // create index array
    printf("Setting index array...\n");
    for (uint64_t i = 0; i < thread_count; i++)
    {
        uint64_t i_start = block_count * i;
        uint64_t i_end = i_start + block_count;
        if (i_end > element_count)
            i_end = element_count;
        threadPool.postTask(
            [&completion_semaphore, i_start, i_end, &ref_array, &rnd_points, origin]()
            {
                for (uint64_t j = i_start; j < i_end; j++)
                {
                    auto &pt = ref_array[j];
                    pt.index = (uint32_t)j;
                    pt.toSort = SortToolu32::floatToInt(rnd_points[j].distance);
                }
                completion_semaphore.release();
            });
    }
    for (uint64_t i = 0; i < thread_count; i++)
        completion_semaphore.blockingAcquire();

    // for (uint32_t i = 0; i < (uint32_t)ref_array.size(); i++)
    // {
    //     ref_array[i].index = i;
    //     ref_array[i].toSort = SortToolu64::doubleToInt(rnd_points[i].distance);

    //     min = MathCore::OP<uint64_t>::minimum(min, ref_array[i].toSort);
    //     max = MathCore::OP<uint64_t>::maximum(max, ref_array[i].toSort);
    // }

    // printf("\n SPREAD => min: %" PRIu64 " max: %" PRIu64 "\n\n", min, max);

    // // Spread facilitates if you have implemented
    // // static task distribution among all available UINT64_MAX range.
    // for (auto &item : ref_array)
    //     item.toSort = SortToolu64::spread(min, max, item.toSort);

    Platform::Time t;

    printf("[Single-Thread Sorting]\n");

    // Check sorting...
    {
        bool sorted = true;
        for (size_t i = 0; i < ref_array.size() - 1; i++)
        {
            const auto &refA = ref_array[i];
            const auto &refB = ref_array[i + 1];
            sorted = sorted && rnd_points[refA.index].distance <= rnd_points[refB.index].distance;
        }
        printf("    Set sorted check (before): %i\n", sorted);
    }

    t.update();
    // sort O(n)
    RadixCountingSortu32::sortIndex(ref_array.data(), (uint32_t)ref_array.size(), tmp_array.data());
    t.update();

    printf("    Time: %f sec\n", t.deltaTime);

    // Check sorting...
    {
        bool sorted = true;
        for (size_t i = 0; i < ref_array.size() - 1; i++)
        {
            const auto &refA = ref_array[i];
            const auto &refB = ref_array[i + 1];
            sorted = sorted && rnd_points[refA.index].distance <= rnd_points[refB.index].distance;
        }
        printf("    Set sorted check (after): %i\n", sorted);
    }

    // create index array
    printf("Setting index array...\n");
    for (uint64_t i = 0; i < thread_count; i++)
    {
        uint64_t i_start = block_count * i;
        uint64_t i_end = i_start + block_count;
        if (i_end > element_count)
            i_end = element_count;
        threadPool.postTask(
            [&completion_semaphore, i_start, i_end, &ref_array, &rnd_points, origin]()
            {
                for (uint64_t j = i_start; j < i_end; j++)
                {
                    auto &pt = ref_array[j];
                    pt.index = (uint32_t)j;
                    pt.toSort = SortToolu32::floatToInt(rnd_points[j].distance);
                }
                completion_semaphore.release();
            });
    }
    for (uint64_t i = 0; i < thread_count; i++)
        completion_semaphore.blockingAcquire();
    // for (uint32_t i = 0; i < (uint32_t)ref_array.size(); i++)
    // {
    //     ref_array[i].index = i;
    //     ref_array[i].toSort = SortToolu64::doubleToInt(rnd_points[i].distance);

    //     ref_array[i].toSort = SortToolu64::spread(min, max, ref_array[i].toSort);
    // }

    printf("[Multi-Thread Sorting]\n");

    // Check sorting...
    {
        bool sorted = true;
        for (size_t i = 0; i < ref_array.size() - 1; i++)
        {
            const auto &refA = ref_array[i];
            const auto &refB = ref_array[i + 1];
            sorted = sorted && rnd_points[refA.index].distance <= rnd_points[refB.index].distance;
        }
        printf("    Set sorted check (before): %i\n", sorted);
    }

    t.update();
    ParallelRadixCountingSortu32::sortIndex(ref_array.data(), ref_array.size(), &threadPool, tmp_array.data());
    t.update();

    printf("    Time: %f sec\n", t.deltaTime);

    // Check sorting...
    {
        bool sorted = true;
        for (size_t i = 0; i < ref_array.size() - 1; i++)
        {
            const auto &refA = ref_array[i];
            const auto &refB = ref_array[i + 1];
            sorted = sorted && rnd_points[refA.index].distance <= rnd_points[refB.index].distance;
        }
        printf("    Set sorted check (after): %i\n", sorted);
    }

    return 0;
}
