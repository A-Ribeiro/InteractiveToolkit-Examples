#include <InteractiveToolkit/InteractiveToolkit.h>
#include <iostream>

using namespace ITKCommon;
using namespace MathCore;
using namespace AlgorithmCore::Sorting;

int main(int argc, char *argv[])
{

    // spread test
    {
        printf("\nUINT32_MAX\n\n");
        for (uint32_t i = 0; i < UINT32_MAX; i++)
        {
            uint32_t v = SortToolu32::spread(0, UINT32_MAX, i);
            if (v != i)
            {
                printf("error at i=%" PRIu32 " v=%" PRIu32 "\n", i, v);
                return 1;
            }
            if (MathCore::OP<uint32_t>::is_power_of_two(i))
            {
                printf("i=%.8" PRIx32 " v=%.8" PRIx32 "\n", i, v);
            }
        }

        uint32_t i = UINT32_MAX;
        uint32_t v = SortToolu32::spread(0, UINT32_MAX, i);
        if (v != i)
        {
            printf("error at i=%" PRIu32 " v=%" PRIu32 "\n", i, v);
            return 1;
        }
    }

    {
        printf("\nINT32_MIN - INT32_MAX\n\n");
        for (int32_t i = INT32_MIN; i < INT32_MAX; i++)
        {
            int32_t v = SortTooli32::spread(INT32_MIN, INT32_MAX, i);
            if (v != i)
            {
                printf("error at i=%" PRIi32 " v=%" PRIi32 "\n", i, v);
                return 1;
            }
            if (MathCore::OP<int32_t>::is_power_of_two(i))
            {
                printf("i=%.8" PRIx32 " v=%.8" PRIx32 "\n", i, v);
            }
        }

        int32_t i = INT32_MAX;
        int32_t v = SortTooli32::spread(INT32_MIN, INT32_MAX, i);
        if (v != i)
        {
            printf("error at i=%" PRIi32 " v=%" PRIi32 "\n", i, v);
            return 1;
        }
    }

    {
        printf("\nUINT64_MAX\n\n");
        for (uint64_t i = 0; i < UINT64_MAX; i++)
        {
            uint64_t v = SortToolu64::spread(0, UINT64_MAX, i);
            if (v != i)
            {
                printf("error at i=%" PRIu64 " v=%" PRIu64 "\n", i, v);
                return 1;
            }
            if (MathCore::OP<uint64_t>::is_power_of_two(i))
            {
                printf("i=%.16" PRIx64 " v=%.16" PRIx64 "\n", i, v);
            }
        }

        uint64_t i = UINT64_MAX;
        uint64_t v = SortToolu64::spread(0, UINT64_MAX, i);
        if (v != i)
        {
            printf("error at i=%" PRIu64 " v=%" PRIu64 "\n", i, v);
            return 1;
        }
    }

    {
        printf("\nINT64_MIN - INT64_MAX\n\n");
        for (int64_t i = INT64_MIN; i < INT64_MAX; i++)
        {
            int64_t v = SortTooli64::spread(INT64_MIN, INT64_MAX, i);
            if (v != i)
            {
                printf("error at i=%" PRIi64 " v=%" PRIi64 "\n", i, v);
                return 1;
            }
            if (MathCore::OP<int64_t>::is_power_of_two(i))
            {
                printf("i=%.16" PRIx64 " v=%.16" PRIx64 "\n", i, v);
            }
        }

        int64_t i = INT64_MAX;
        int64_t v = SortTooli64::spread(INT64_MIN, INT64_MAX, i);
        if (v != i)
        {
            printf("error at i=%" PRIi64 " v=%" PRIi64 "\n", i, v);
            return 1;
        }
    }

    return 0;
}
