#include <stdio.h>
#include <InteractiveToolkit/MathCore/MathCore.h>

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
		using fixed = MathCore::ufixed32_t<16>;

		printf("ufixed32_t<%i>:\n", fixed::FRAC_BITS);
		printf("  int_bits:%i [%" PRIi64 " -> %" PRIi64 "]\n", fixed::INT_BITS, fixed::INT_RANGE_MIN, fixed::INT_RANGE_MAX);
		printf("  frac_bits:%i [%" PRIi64 " -> %" PRIi64 "]\n", fixed::FRAC_BITS, fixed::FRAC_RANGE_MIN, fixed::FRAC_RANGE_MAX);

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

			printf("%i => %f\n", i, result.toDouble());
		}
	}

	{
		printf(
			" ----------\n"
			"|  TEST B  |\n"
			" ----------\n");

		// 11bits to the integer part -- range: [-1024,1023]
		// 21bits to the decimal part -- range: [0,2097151]
		using fixed = MathCore::fixed32_t<21>;

		printf("fixed32_t<%i>:\n", fixed::FRAC_BITS);
		printf("  int_bits:%i [%" PRIi64 " -> %" PRIi64 "]\n", fixed::INT_BITS, fixed::INT_RANGE_MIN, fixed::INT_RANGE_MAX);
		printf("  frac_bits:%i [%" PRIi64 " -> %" PRIi64 "]\n", fixed::FRAC_BITS, fixed::FRAC_RANGE_MIN, fixed::FRAC_RANGE_MAX);

		auto a = fixed(-255, 0);
		auto b = fixed(0, 0);
		const auto lrp = fixed::fromDouble(0.35);

		printf("a => %f\n", a.toDouble());
		printf("b => %f\n", b.toDouble());
		printf("b - a => %f\n", (b - a).toDouble());
		printf("lrp => %f\n", lrp.toDouble());

		auto result = a + (b - a) * lrp;
		printf("result => %f\n", result.toDouble());

		// round result
		const auto _0_5 = fixed::fromDouble(0.5);
		result = (result.isNegative()) ? (result - _0_5) : (result + _0_5);
		printf("result => %f\n", result.toDouble());

		printf("result.integer_part() => %i\n", result.signed_integer_part());
		printf("result.fract_part() => %i -> %f\n", result.signed_fract_part(), (double)result.signed_fract_part() / (fixed::FRAC_RANGE_MAX + 1));

		// auto new_from_result = (result.isNegative()) ? -fixed(result.integer_part(), result.fract_part()) : fixed(result.integer_part(), result.fract_part());
		auto new_from_result = fixed(result.signed_integer_part(), result.signed_fract_part());

		printf("new_from_result => %f\n", new_from_result.toDouble());
	}

	{
		printf(
			" ----------\n"
			"|  TEST C  |\n"
			" ----------\n");

		// vec2 example
		using fixed = MathCore::fixed32_t<21>;
		using vec2T = MathCore::vec2<fixed>;

		auto print = std::function<void(const vec2T&)>([]( const vec2T&v ){
			printf("( ");
			for(auto item: v.array)
				printf("%f ", item.toDouble());
			printf(")\n");
		});

		auto result = (vec2T(512) >> 1) + 10 - vec2T(fixed::fromDouble(0.5));
		print(result);
		result >>= 1;
		print(result);

		printf("result == (132.75, 132.75) => ( %i )\n", (result == vec2T(fixed::fromDouble(132.75))) ? 1 : 0);
		printf("result != (132.75, 132.75) => ( %i )\n", (result != vec2T(fixed::fromDouble(132.75))) ? 1 : 0);

	}

	{
		printf(
			" ----------\n"
			"|  TEST D  |\n"
			" ----------\n");

		using fixed = MathCore::ufixed32_t<16>;
		using vec4T = MathCore::vec4ufixed32<16>;

		auto print = std::function<void(const vec4T&)>([]( const vec4T&v ){
			printf("( ");
			for(auto item: v.array)
				printf("%f ", item.toDouble());
			printf(")\n");
		});

		auto result = vec4T(1,2,3,4);
		printf("vec4T(1,2,3,4) ");print(result);
		result >>= 2;
		printf("result >>= 2 ");print(result);
		result *= vec4T(2,4,8,16);
		printf("result *= vec4T(2,4,8,16) ");print(result);
		result += fixed::fromDouble(0.5);
		printf("result += 0.5 ");print(result);
		result = ~result;
		printf("result = ~result ");print(result);
		result /= vec4T(2,4,8,16);
		printf("result /= vec4T(2,4,8,16) ");print(result);

	}

	return 0;
}
