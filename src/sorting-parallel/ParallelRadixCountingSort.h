#pragma once

#include <InteractiveToolkit/InteractiveToolkit.h>
#include "ThreadPool.h"
#include <InteractiveToolkit/Platform/Core/ObjectBuffer.h>

namespace AlgorithmCore
{

    namespace Sorting
    {
        class ParallelRadixCountingSort
        {
            struct TaskParameter
            {

                AlgorithmCore::Sorting::SortIndexu *data;
                size_t count;

                Platform::Semaphore *completion_semaphore;

                AlgorithmCore::Sorting::SortIndexu *tmp_buffer;
            };

        public:
            static void sort(AlgorithmCore::Sorting::SortIndexu *data,
                             const size_t &count,
                             Platform::ThreadPool *threadpool,
                             int64_t bucket_count = -1)
            {

                //
                if (bucket_count == -1)
                    bucket_count = threadpool->threadCount();

                uint32_t counting[bucket_count];
                uint32_t offset[bucket_count];
                memset(counting, 0, sizeof(uint32_t) * bucket_count);

                Platform::ObjectBuffer buffer;
                buffer.setSize(sizeof(AlgorithmCore::Sorting::SortIndexu) * count);
                AlgorithmCore::Sorting::SortIndexu *aux = ((AlgorithmCore::Sorting::SortIndexu *)buffer.data);

                // 1st step: counting elements based on bucket_counting...
                {

                    int64_t min = 0;
                    int64_t max = UINT32_MAX;

                    // for (int i = 0; i < count; i++)
                    // {
                    //     const AlgorithmCore::Sorting::SortIndexu &element32 = data[i];
                    //     min = MathCore::OP<int64_t>::minimum(min, element32.toSort);
                    //     max = MathCore::OP<int64_t>::maximum(max, element32.toSort);
                    // }

                    // count
                    for (int i = 0; i < count; i++)
                    {
                        const AlgorithmCore::Sorting::SortIndexu &element32 = data[i];
                        int64_t element64 = (int64_t)element32.toSort - min;
                        uint32_t index = (element64 * bucket_count) >> 32;
                        counting[index]++;
                    }

                    // compute offset
                    uint32_t acc = counting[0];
                    counting[0] = 0;
                    for (uint32_t j = 1; j < bucket_count; j++)
                    {
                        uint32_t tmp = counting[j];
                        counting[j] = acc;
                        acc += tmp;
                    }

                    memcpy(offset, counting, sizeof(uint32_t) * bucket_count);

                    // place elements in the output array
                    for (uint32_t i = 0; i < count; i++)
                    {
                        const AlgorithmCore::Sorting::SortIndexu &element32 = data[i];
                        int64_t element64 = (int64_t)element32.toSort - min;
                        uint32_t index = (element64 * bucket_count) >> 32;
                        uint32_t out_index = counting[index];
                        counting[index]++;
                        aux[out_index] = element32;
                    }

                    memcpy(data, aux, sizeof(AlgorithmCore::Sorting::SortIndexu) * count);
                }

                // post thread task
                Platform::Semaphore completion_semaphore(0);
                for (int i = 0; i < bucket_count; i++)
                {
                    int32_t element_count = counting[i] - offset[i];
                    if (element_count == 0)
                    {
                        completion_semaphore.release();
                        continue;
                    }
                    TaskParameter task;

                    task.data = data + offset[i];
                    task.tmp_buffer = aux + offset[i];
                    task.count = element_count;
                    task.completion_semaphore = &completion_semaphore;

                    threadpool->postTask([task]()
                                         {
                                        AlgorithmCore::Sorting::RadixCountingSortu::sortIndex(task.data, task.count, task.tmp_buffer);
                                        task.completion_semaphore->release(); 
                                        });
                }

                for (int i = 0; i < bucket_count; i++)
                    completion_semaphore.blockingAcquire();
            }
        };

    }
}