#include <InteractiveToolkit/InteractiveToolkit.h>
#include <iostream>

#if defined(__linux__)
#include <sys/sysinfo.h>
#elif defined(__APPLE__)
#include <mach/mach_init.h>
#include <mach/mach_host.h>
#include <mach/mach_port.h>
#include <mach/vm_statistics.h>
#include <sys/sysctl.h>
#elif defined(_WIN32)
#endif

namespace ITKCommon
{
    namespace Memory
    {

        static ITK_INLINE uint64_t total_ram()
        {
#if defined(__linux__)
            struct sysinfo si;
            if (sysinfo(&si) == 0)
                return (uint64_t)si.totalram;
            return 0;
#elif defined(__APPLE__)
            int mib[2] = {CTL_HW, HW_MEMSIZE};
            uint64_t physical_memory;
            size_t length = sizeof(physical_memory);

            if (sysctl(mib, 2, &physical_memory, &length, NULL, 0) == 0)
                return physical_memory;
            return 0;
#elif defined(_WIN32)
            SYSTEM_INFO systemInfo;
            GetSystemInfo(&systemInfo);
            return (uint64_t)systemInfo.ullTotalPhysicalMemory;
#endif
        }

        static ITK_INLINE uint64_t available_ram()
        {
#if defined(__linux__)
            struct sysinfo si;
            if (sysinfo(&si) == 0)
                return (uint64_t)si.freeram;
            return 0;
#elif defined(__APPLE__)
            vm_size_t page_size;
            vm_statistics vm_stats;
            mach_msg_type_number_t count = HOST_VM_INFO_COUNT;

            host_page_size(mach_host_self(), &page_size);
            host_statistics(mach_host_self(), HOST_VM_INFO, (host_info_t)&vm_stats, &count);

            vm_size_t free_memory = vm_stats.free_count * page_size;
            return (uint64_t)free_memory;
#elif defined(_WIN32)
            MEMORYSTATUSEX memStatus;
            memStatus.dwLength = sizeof(memStatus);
            if (GlobalMemoryStatusEx(&memStatus))
                return (uint64_t)memStatus.ullAvailPhys;
            return 0;
#endif
        }
    }
}

int main(int argc, char *argv[])
{
    printf("Total RAM: %" PRIu64 " bytes\n", ITKCommon::Memory::total_ram());
    printf("Available RAM: %" PRIu64 " bytes\n", ITKCommon::Memory::available_ram());

    return 0;
}
