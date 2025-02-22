#include <InteractiveToolkit/InteractiveToolkit.h>
#include "Fixed.h"
int main(int argc, char *argv[])
{
	ITKCommon::Path::setWorkingPath(ITKCommon::Path::getExecutablePath(argv[0]));

	using fixed = ufixed32_t<16>;

	auto a = fixed(0, 0);
	auto b = fixed(100, 0);
	auto delta = b - a;

	int n_div = 10;

	for (int i = 0; i <= n_div; i++)
	{
		// method 1: not so precise
		// auto lrp = fixed(i, 0) / fixed(n_div, 0);
		// auto result = a + delta * lrp;
		// method 2: better precision for integers
		auto result = a + ( delta * fixed(i, 0) ) / fixed(n_div, 0);

		printf("%i => %f\n", i, result.toFloat());
	}

	return 0;
}
