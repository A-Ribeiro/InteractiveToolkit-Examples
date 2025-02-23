#pragma once

// #include <InteractiveToolkit/InteractiveToolkit.h>
#include <InteractiveToolkit/common.h>
#include <cstdint>
#include <type_traits>

template <typename _T>
struct StoreSpecType
{
};

template <>
struct StoreSpecType<uint32_t>
{
    using _is_signed = std::false_type;

    typedef uint32_t valueT;
    typedef uint32_t u_valueT;

    typedef uint64_t sizeT;
    typedef int64_t rangeT;
    typedef int64_t mul_div_T;

    static constexpr valueT SIGN_MASK = UINT32_C(0x80000000);

    static constexpr int SHIFT_GET_SIGN_MINUS_ONE = 30;
    static constexpr valueT FIRST_BIT_ZERO = UINT32_C(0xfffffffe);

    static constexpr rangeT VALUE_TOTAL_BITS = 32;

    static constexpr rangeT INTEGER_PART_MIN_RANGE(rangeT bits) noexcept
    {
        return 0;
    }
    static constexpr rangeT INTEGER_PART_MAX_RANGE(rangeT bits) noexcept
    {
        return (1 << bits) - 1;
    }
    static constexpr rangeT FRACTIONAL_PART_MIN_RANGE(rangeT bits) noexcept
    {
        return 0;
    }
    static constexpr rangeT FRACTIONAL_PART_MAX_RANGE(rangeT bits) noexcept
    {
        return (1 << bits) - 1;
    }
};

template <>
struct StoreSpecType<int32_t>
{
    using _is_signed = std::true_type;

    typedef int32_t valueT;
    typedef uint32_t u_valueT;

    typedef uint64_t sizeT;
    typedef int64_t rangeT;
    typedef int64_t mul_div_T;

    static constexpr valueT SIGN_MASK = INT32_C(0x80000000);

    static constexpr int SHIFT_GET_SIGN_MINUS_ONE = 30;
    static constexpr valueT FIRST_BIT_ZERO = INT32_C(0xfffffffe);

    static constexpr rangeT VALUE_TOTAL_BITS = 32;

    static constexpr rangeT INTEGER_PART_MIN_RANGE(rangeT bits) noexcept
    {
        return -(1 << (bits - 1));
    }
    static constexpr rangeT INTEGER_PART_MAX_RANGE(rangeT bits) noexcept
    {
        return (1 << (bits - 1)) - 1;
    }
    static constexpr rangeT FRACTIONAL_PART_MIN_RANGE(rangeT bits) noexcept
    {
        return -((1 << bits) - 1);
    }
    static constexpr rangeT FRACTIONAL_PART_MAX_RANGE(rangeT bits) noexcept
    {
        return (1 << bits) - 1;
    }
};

template <typename store_type, int frac_bits>
class fixed_t
{
public:
    using StoreSpec = StoreSpecType<store_type>;

    typedef typename StoreSpec::valueT valueT;
    typedef typename StoreSpec::u_valueT u_valueT;
    typedef typename StoreSpec::sizeT sizeT;
    typedef typename StoreSpec::rangeT rangeT;
    typedef typename StoreSpec::mul_div_T mul_div_T;

private:
        valueT value;

    static const sizeT FRACTIONAL_BITS = frac_bits;
    static const sizeT FRACTIONAL_MASK = (1 << FRACTIONAL_BITS) - 1;
    static const sizeT FACTOR = 1 << FRACTIONAL_BITS;

public:
    static const rangeT INT_BITS = StoreSpec::VALUE_TOTAL_BITS - FRACTIONAL_BITS;
    static const rangeT INT_RANGE_MIN = StoreSpec::INTEGER_PART_MIN_RANGE(INT_BITS);
    static const rangeT INT_RANGE_MAX = StoreSpec::INTEGER_PART_MAX_RANGE(INT_BITS);

    static const rangeT FRAC_BITS = FRACTIONAL_BITS;
    static const rangeT FRAC_RANGE_MIN = StoreSpec::FRACTIONAL_PART_MIN_RANGE(FRAC_BITS);
    static const rangeT FRAC_RANGE_MAX = StoreSpec::FRACTIONAL_PART_MAX_RANGE(FRAC_BITS);

    template <typename T = StoreSpec, typename std::enable_if<T::_is_signed::value, bool>::type = true>
    fixed_t(valueT integerPart, valueT fractionalPart)
    {
        value = (integerPart << FRACTIONAL_BITS) + fractionalPart;
    }

    template <typename T = StoreSpec, typename std::enable_if<!T::_is_signed::value, bool>::type = true>
    fixed_t(valueT integerPart, valueT fractionalPart)
    {
        value = (integerPart << FRACTIONAL_BITS) | (fractionalPart & FRACTIONAL_MASK);
    }

    fixed_t(valueT rawValue) : value(rawValue) {}

    fixed_t operator+(const fixed_t &other) const
    {
        return fixed_t(value + other.value);
    }

    fixed_t operator-(const fixed_t &other) const
    {
        return fixed_t(value - other.value);
    }

    fixed_t operator-() const
    {
        return fixed_t(-value);
    }

    fixed_t operator*(const fixed_t &other) const
    {
        mul_div_T result = mul_div_T(value);
        result *= mul_div_T(other.value);
        result >>= FRACTIONAL_BITS;
        return fixed_t((valueT)result);
    }

    fixed_t operator/(const fixed_t &other) const
    {
        mul_div_T result = mul_div_T(value);
        result <<= FRACTIONAL_BITS;
        result /= mul_div_T(other.value);
        return fixed_t((valueT)result);
    }

    template <typename T = StoreSpec, typename std::enable_if<T::_is_signed::value, bool>::type = true>
    double toDouble() const
    {
        bool sign_negative = (value & StoreSpec::SIGN_MASK) != 0;
        u_valueT uvalue = (sign_negative) ? -value : value;
        u_valueT integerPart = uvalue >> FRACTIONAL_BITS;
        u_valueT fractionalPart = uvalue & FRACTIONAL_MASK;
        double number = (double)integerPart + (double)fractionalPart / (double)FACTOR;
        return (sign_negative) ? -number : number;
    }

    template <typename T = StoreSpec, typename std::enable_if<!T::_is_signed::value, bool>::type = true>
    double toDouble() const
    {
        valueT integerPart = value >> FRACTIONAL_BITS;
        valueT fractionalPart = value & FRACTIONAL_MASK;
        double number = (double)integerPart + (double)fractionalPart / (double)FACTOR;
        return number;
    }

    template <typename T = StoreSpec, typename std::enable_if<T::_is_signed::value, bool>::type = true>
    double toFloat() const
    {
        bool sign_negative = (value & StoreSpec::SIGN_MASK) != 0;
        u_valueT uvalue = (sign_negative) ? -value : value;
        u_valueT integerPart = uvalue >> FRACTIONAL_BITS;
        u_valueT fractionalPart = uvalue & FRACTIONAL_MASK;
        float number = (float)integerPart + (float)fractionalPart / (float)FACTOR;
        return (sign_negative) ? -number : number;
    }

    template <typename T = StoreSpec, typename std::enable_if<!T::_is_signed::value, bool>::type = true>
    double toFloat() const
    {
        valueT integerPart = value >> FRACTIONAL_BITS;
        valueT fractionalPart = value & FRACTIONAL_MASK;
        float number = (float)integerPart + (float)fractionalPart / (float)FACTOR;
        return number;
    }

    std::string toString() const
    {
        return ITKCommon::PrintfToStdString("%f", toDouble());
    }

    template <typename T = StoreSpec, typename std::enable_if<T::_is_signed::value, bool>::type = true>
    valueT signed_integer_part() const
    {
        return (value & StoreSpec::SIGN_MASK) ? (-(-value >> FRACTIONAL_BITS)) : (value >> FRACTIONAL_BITS);
    }
    template <typename T = StoreSpec, typename std::enable_if<T::_is_signed::value, bool>::type = true>
    u_valueT integer_part() const
    {
        return (value & StoreSpec::SIGN_MASK) ? ((-value >> FRACTIONAL_BITS)) : (value >> FRACTIONAL_BITS);
    }
    template <typename T = StoreSpec, typename std::enable_if<T::_is_signed::value, bool>::type = true>
    valueT signed_fract_part() const
    {
        return (value & StoreSpec::SIGN_MASK) ? (-(-value & FRACTIONAL_MASK)) : (value & FRACTIONAL_MASK);
    }
    template <typename T = StoreSpec, typename std::enable_if<T::_is_signed::value, bool>::type = true>
    u_valueT fract_part() const
    {
        return (value & StoreSpec::SIGN_MASK) ? ((-value & FRACTIONAL_MASK)) : (value & FRACTIONAL_MASK);
    }

    template <typename T = StoreSpec, typename std::enable_if<!T::_is_signed::value, bool>::type = true>
    valueT signed_integer_part() const
    {
        return value >> FRACTIONAL_BITS;
    }
    template <typename T = StoreSpec, typename std::enable_if<!T::_is_signed::value, bool>::type = true>
    valueT integer_part() const
    {
        return value >> FRACTIONAL_BITS;
    }
    template <typename T = StoreSpec, typename std::enable_if<!T::_is_signed::value, bool>::type = true>
    valueT signed_fract_part() const
    {
        return value & FRACTIONAL_MASK;
    }
    template <typename T = StoreSpec, typename std::enable_if<!T::_is_signed::value, bool>::type = true>
    valueT fract_part() const
    {
        return value & FRACTIONAL_MASK;
    }

    template <typename T = StoreSpec, typename std::enable_if<T::_is_signed::value, bool>::type = true>
    valueT sign() const
    {
        return ((value >> StoreSpec::SHIFT_GET_SIGN_MINUS_ONE) & StoreSpec::FIRST_BIT_ZERO) + 1;
    }

    template <typename T = StoreSpec, typename std::enable_if<!T::_is_signed::value, bool>::type = true>
    valueT sign() const
    {
        return 1;
    }

    template <typename T = StoreSpec, typename std::enable_if<T::_is_signed::value, bool>::type = true>
    bool isNegative() const
    {
        return (value & StoreSpec::SIGN_MASK) != 0;
    }

    template <typename T = StoreSpec, typename std::enable_if<!T::_is_signed::value, bool>::type = true>
    bool isNegative() const
    {
        return 0;
    }

    template <typename T = StoreSpec, typename std::enable_if<T::_is_signed::value, bool>::type = true>
    static constexpr fixed_t fromFloat(float v) noexcept
    {
        bool sign_negative = v < 0;
        v = (sign_negative) ? -v : v;
        u_valueT integerPart = (u_valueT)v;
        u_valueT fractionalPart = (u_valueT)((v - (float)integerPart) * (float)FACTOR);
        return (sign_negative) ? -fixed_t(integerPart, fractionalPart) : fixed_t(integerPart, fractionalPart);
    }

    template <typename T = StoreSpec, typename std::enable_if<!T::_is_signed::value, bool>::type = true>
    static constexpr fixed_t fromFloat(float v) noexcept
    {
        valueT integerPart = (valueT)v;
        valueT fractionalPart = (valueT)((v - (float)integerPart) * (float)FACTOR);
        return fixed_t(integerPart, fractionalPart);
    }

    template <typename T = StoreSpec, typename std::enable_if<T::_is_signed::value, bool>::type = true>
    static constexpr fixed_t fromDouble(double v) noexcept
    {
        bool sign_negative = v < 0;
        v = (sign_negative) ? -v : v;
        u_valueT integerPart = (u_valueT)v;
        u_valueT fractionalPart = (u_valueT)((v - (double)integerPart) * (double)FACTOR);
        return (sign_negative) ? -fixed_t(integerPart, fractionalPart) : fixed_t(integerPart, fractionalPart);
    }

    template <typename T = StoreSpec, typename std::enable_if<!T::_is_signed::value, bool>::type = true>
    static constexpr fixed_t fromDouble(double v) noexcept
    {
        valueT integerPart = (valueT)v;
        valueT fractionalPart = (valueT)((v - (double)integerPart) * (double)FACTOR);
        return fixed_t(integerPart, fractionalPart);
    }
};

template <int frac_bits>
using ufixed32_t = fixed_t<uint32_t, frac_bits>;

template <int frac_bits>
using fixed32_t = fixed_t<int32_t, frac_bits>;
