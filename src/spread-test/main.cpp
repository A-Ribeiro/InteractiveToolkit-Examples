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

    int cpus = Platform::Thread::QueryNumberOfSystemThreads();
    Platform::ThreadPool pool(cpus-1);

    {
        struct _chunk_task
        {
            uint64_t start;
            uint64_t end;
        };

        Platform::ObjectQueue<_chunk_task> tasks;
        Platform::Mutex mtx;
        int count = 0;

        printf("\nUINT64_MAX\n\n");
        uint64_t i = 0;
        while (i < UINT64_MAX)
        {
            uint64_t next_i = i + 0xffffffff;
            uint64_t i_to_set = (next_i < i) ? UINT64_MAX : next_i;

            {
                Platform::AutoLock _lock(&mtx);
                count++;
            }
            tasks.enqueue(_chunk_task{i, i_to_set});
            pool.postTask([&]()
                          {
                              auto task = tasks.dequeue(nullptr, true);
                              for (uint64_t i = task.start; i < task.end; i++)
                              {
                                  uint64_t v = SortToolu64::spread(0, UINT64_MAX, i);
                                  if (v != i)
                                  {
                                      printf("error at i=%" PRIu64 " v=%" PRIu64 "\n", i, v);
                                      exit(1);
                                  }
                                  if (MathCore::OP<uint64_t>::is_power_of_two(i))
                                      printf("i=%.16" PRIx64 " v=%.16" PRIx64 "\n", i, v);
                              }
                              {
                                  Platform::AutoLock _lock(&mtx);
                                  count--;
                              } });

            while ((int)tasks.size() > cpus * 10)
                Platform::Sleep::millis(1);

            i = i_to_set;
        }

        while (count > 0)
            Platform::Sleep::millis(1);

        i = UINT64_MAX;
        uint64_t v = SortToolu64::spread(0, UINT64_MAX, i);
        if (v != i)
        {
            printf("error at i=%" PRIu64 " v=%" PRIu64 "\n", i, v);
            return 1;
        }
    }

    {
        struct _chunk_task
        {
            int64_t start;
            int64_t end;
        };

        Platform::ObjectQueue<_chunk_task> tasks;
        Platform::Mutex mtx;
        int count = 0;

        printf("\nINT64_MIN - INT64_MAX\n\n");
        int64_t i = INT64_MIN;
        while (i < INT64_MAX)
        {
            int64_t next_i = i + 0xffffffff;
            int64_t i_to_set = (next_i < i) ? INT64_MAX : next_i;

            {
                Platform::AutoLock _lock(&mtx);
                count++;
            }
            tasks.enqueue(_chunk_task{i, i_to_set});
            pool.postTask([&]()
                          {
                              auto task = tasks.dequeue(nullptr, true);
                              for (int64_t i = task.start; i < task.end; i++)
                              {
                                  int64_t v = SortTooli64::spread(INT64_MIN, INT64_MAX, i);
                                  if (v != i)
                                  {
                                      printf("error at i=%" PRIi64 " v=%" PRIi64 "\n", i, v);
                                      exit(1);
                                  }
                                  if (MathCore::OP<int64_t>::is_power_of_two(i))
                                      printf("i=%.16" PRIx64 " v=%.16" PRIx64 "\n", i, v);
                              }
                              {
                                  Platform::AutoLock _lock(&mtx);
                                  count--;
                              } });

            while ((int)tasks.size() > cpus * 10)
                Platform::Sleep::millis(1);

            i = i_to_set;
        }

        while (count > 0)
            Platform::Sleep::millis(1);

        i = INT64_MAX;
        int64_t v = SortTooli64::spread(INT64_MIN, INT64_MAX, i);
        if (v != i)
        {
            printf("error at i=%" PRIi64 " v=%" PRIi64 "\n", i, v);
            return 1;
        }
    }

    return 0;
}
