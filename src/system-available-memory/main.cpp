#include <InteractiveToolkit/InteractiveToolkit.h>

int main(int argc, char* argv[])
{
	printf("Total RAM: %s\n", ITKCommon::ByteUtils::byteSmartPrint(ITKCommon::Memory::total_ram(), "B").c_str());
	printf("Available RAM: %s\n", ITKCommon::ByteUtils::byteSmartPrint(ITKCommon::Memory::available_ram(), "B").c_str());

	uint64_t bit_rate = 200000000;

	printf("%s\n", ITKCommon::ByteUtils::byteSmartPrint(bit_rate/8, "Bps").c_str());
	printf("%s\n", ITKCommon::ByteUtils::bitSmartPrint(bit_rate, "bps").c_str());

	return 0;
}
