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
			int mib[2] = { CTL_HW, HW_MEMSIZE };
			uint64_t physical_memory;
			size_t length = sizeof(physical_memory);

			if (sysctl(mib, 2, &physical_memory, &length, NULL, 0) == 0)
				return physical_memory;
			return 0;
#elif defined(_WIN32)
			MEMORYSTATUSEX memStatus;
			memStatus.dwLength = sizeof(memStatus);
			if (GlobalMemoryStatusEx(&memStatus))
				return (uint64_t)memStatus.ullTotalPhys;
			return 0;
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

            vm_size_t vm_page_size;
    
            // Get page size using sysctl
            int mib[2] = { CTL_HW, HW_PAGESIZE };
            size_t len = sizeof(vm_page_size);
            if (sysctl(mib, 2, &vm_page_size, &len, NULL, 0) != 0)
                return 0;

			vm_statistics vm_stats;
			mach_msg_type_number_t count = HOST_VM_INFO_COUNT;
            
            kern_return_t kr = host_statistics(mach_host_self(), HOST_VM_INFO, (host_info_t)&vm_stats, &count);
            if (kr != KERN_SUCCESS)
                return 0;

			vm_size_t free_memory = (vm_stats.free_count + vm_stats.inactive_count) * vm_page_size;
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
	
	namespace ByteUtils
	{
		static ITK_INLINE double byteToKilo(uint64_t byte) {
			return (double)byte / 1024.0;
		}
		static ITK_INLINE double byteToMega(uint64_t byte) {
			return (double)byte / 1048576.0;
		}
		static ITK_INLINE double byteToGiga(uint64_t byte) {
			return (double)byte / 1073741824.0;
		}
		static ITK_INLINE double byteToTera(uint64_t byte) {
			return byteToGiga(byte) / 1024.0;
		}
		static ITK_INLINE double byteToPeta(uint64_t byte) {
			return byteToGiga(byte) / 1048576.0;
		}
		static ITK_INLINE double byteToExtra(uint64_t byte) {
			return byteToGiga(byte) / 1073741824.0;
		}

		static ITK_INLINE std::string byteSmartPrint(uint64_t byte) {
			uint64_t check_zero = byte;

			check_zero >>= 10;
			if (!check_zero)
				return ITKCommon::PrintfToStdString("%.2lf B", (double)byte);
			check_zero >>= 10;
			if (!check_zero)
				return ITKCommon::PrintfToStdString("%.2lf KB", byteToKilo(byte));
			check_zero >>= 10;
			if (!check_zero)
				return ITKCommon::PrintfToStdString("%.2lf MB", byteToMega(byte));
			check_zero >>= 10;
			if (!check_zero)
				return ITKCommon::PrintfToStdString("%.2lf GB", byteToGiga(byte));
			check_zero >>= 10;
			if (!check_zero)
				return ITKCommon::PrintfToStdString("%.2lf TB", byteToTera(byte));
			check_zero >>= 10;
			if (!check_zero)
				return ITKCommon::PrintfToStdString("%.2lf PB", byteToPeta(byte));
			return ITKCommon::PrintfToStdString("%.2lf EB", byteToExtra(byte));
		}


		static ITK_INLINE double bitToKilo(uint64_t byte) {
			return (double)byte / 1000.0;
		}
		static ITK_INLINE double bitToMega(uint64_t byte) {
			return (double)byte / 1000000.0;
		}
		static ITK_INLINE double bitToGiga(uint64_t byte) {
			return (double)byte / 1000000000.0;
		}
		static ITK_INLINE double bitToTera(uint64_t byte) {
			return bitToGiga(byte) / 1000.0;
		}
		static ITK_INLINE double bitToPeta(uint64_t byte) {
			return bitToGiga(byte) / 1000000.0;
		}
		static ITK_INLINE double bitToExtra(uint64_t byte) {
			return bitToGiga(byte) / 1000000000.0;
		}

		static ITK_INLINE std::string bitSmartPrint(uint64_t bit) {
			uint64_t check_zero = bit;

			check_zero /= 1000;
			if (!check_zero)
				return ITKCommon::PrintfToStdString("%.2lf", (double)bit);
			check_zero /= 1000;
			if (!check_zero)
				return ITKCommon::PrintfToStdString("%.2lf Kb", bitToKilo(bit));
			check_zero /= 1000;
			if (!check_zero)
				return ITKCommon::PrintfToStdString("%.2lf Mb", bitToMega(bit));
			check_zero /= 1000;
			if (!check_zero)
				return ITKCommon::PrintfToStdString("%.2lf Gb", bitToGiga(bit));
			check_zero /= 1000;
			if (!check_zero)
				return ITKCommon::PrintfToStdString("%.2lf Tb", bitToTera(bit));
			check_zero /= 1000;
			if (!check_zero)
				return ITKCommon::PrintfToStdString("%.2lf Pb", bitToPeta(bit));
			return ITKCommon::PrintfToStdString("%.2lf Eb", bitToExtra(bit));
		}
	}

}

int main(int argc, char* argv[])
{
	printf("Total RAM: %s\n", ITKCommon::ByteUtils::byteSmartPrint(ITKCommon::Memory::total_ram()).c_str());
	printf("Available RAM: %s\n", ITKCommon::ByteUtils::byteSmartPrint(ITKCommon::Memory::available_ram()).c_str());

	uint64_t bit_rate = 200000000;

	printf("%sps\n", ITKCommon::ByteUtils::byteSmartPrint(bit_rate/8).c_str());
	printf("%sps\n", ITKCommon::ByteUtils::bitSmartPrint(bit_rate).c_str());

	return 0;
}
