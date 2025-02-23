//#include <InteractiveToolkit/InteractiveToolkit.h>
#include "fixed_t.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
	// ITKCommon::Path::setWorkingPath(ITKCommon::Path::getExecutablePath(argv[0]));

	{
		printf(
			" ----------\n"
			"|  TEST A  |\n"
			" ----------\n");

		// 16bits to the integer part -- range: [0,65536]
		// 16bits to the decimal part -- range: [0,65536]
		using fixed = ufixed32_t<16>;

		printf("ufixed32_t<%li>:\n", fixed::FRAC_BITS);
		printf("  int_bits:%li [%li -> %li]\n", fixed::INT_BITS, fixed::INT_RANGE_MIN, fixed::INT_RANGE_MAX);
		printf("  frac_bits:%li [%li -> %li]\n", fixed::FRAC_BITS, fixed::FRAC_RANGE_MIN, fixed::FRAC_RANGE_MAX);

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
			auto result = a + (delta * fixed(i, 0)) / fixed(n_div, 0);

			printf("%i => %f\n", i, result.toFloat());
		}
	}

	{
		printf(
			" ----------\n"
			"|  TEST B  |\n"
			" ----------\n");

		// 11bits to the integer part -- range: [-1024,1023]
		// 21bits to the decimal part -- range: [0,2097151]
		using fixed = fixed32_t<21>;

		printf("fixed32_t<%li>:\n", fixed::FRAC_BITS);
		printf("  int_bits:%li [%li -> %li]\n", fixed::INT_BITS, fixed::INT_RANGE_MIN, fixed::INT_RANGE_MAX);
		printf("  frac_bits:%li [%li -> %li]\n", fixed::FRAC_BITS, fixed::FRAC_RANGE_MIN, fixed::FRAC_RANGE_MAX);

		auto a = fixed(-255, 0);
		auto b = fixed(0, 0);
		const auto lrp = fixed::fromFloat(0.35f);

		printf("a => %f\n", a.toFloat());
		printf("b => %f\n", b.toFloat());
		printf("b - a => %f\n", (b - a).toFloat());
		printf("lrp => %f\n", lrp.toFloat());

		auto result = a + (b - a) * lrp;
		printf("result => %f\n", result.toFloat());

		// round result
		const auto _0_5 = fixed::fromFloat(0.5f);
		result = (result.isNegative()) ? (result - _0_5) : (result + _0_5);
		printf("result => %f\n", result.toFloat());

		printf("result.integer_part() => %i\n", result.signed_integer_part());
		printf("result.fract_part() => %i -> %f\n", result.signed_fract_part(), (float)result.signed_fract_part() / (fixed::FRAC_RANGE_MAX + 1));

		// auto new_from_result = (result.isNegative()) ? -fixed(result.integer_part(), result.fract_part()) : fixed(result.integer_part(), result.fract_part());
		auto new_from_result = fixed(result.signed_integer_part(), result.signed_fract_part());
		
		printf("new_from_result => %f\n", new_from_result.toFloat());
	}

	return 0;
}
