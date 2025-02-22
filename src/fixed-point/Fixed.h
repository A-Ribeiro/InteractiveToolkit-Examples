#pragma once

#include <InteractiveToolkit/InteractiveToolkit.h>
#include <cstdint>

template<int frac_bits>
class fixed32_t {
private:
    int32_t value;

    static const int32_t FRACTIONAL_BITS = frac_bits;
    static const int32_t FRACTIONAL_MASK = (1 << FRACTIONAL_BITS) - 1;
    static const int32_t FACTOR = 1 << FRACTIONAL_BITS;

public:
    fixed32_t(int32_t integerPart, int32_t fractionalPart) {
        value = (integerPart << FRACTIONAL_BITS) | (fractionalPart & FRACTIONAL_MASK);
    }

    fixed32_t(int32_t rawValue) : value(rawValue) {}

    fixed32_t operator+(const fixed32_t& other) const {
        return fixed32_t(value + other.value);
    }

    fixed32_t operator-(const fixed32_t& other) const {
        return fixed32_t(value - other.value);
    }

    fixed32_t operator-() const {
        return fixed32_t(-value);
    }

    fixed32_t operator*(const fixed32_t& other) const {
        int64_t result = static_cast<int64_t>(value) * other.value;
        return fixed32_t(static_cast<int32_t>(result >> FRACTIONAL_BITS));
    }

    fixed32_t operator/(const fixed32_t& other) const {
        int64_t result = (static_cast<int64_t>(value) << FRACTIONAL_BITS) / other.value;
        return fixed32_t(static_cast<int32_t>(result));
    }

    double toDouble() const {
        bool sign_negative = (value & 0x80000000) != 0;
        uint32_t uvalue = (sign_negative)?-value:value;
        uint32_t integerPart = uvalue >> FRACTIONAL_BITS;
        uint32_t fractionalPart = uvalue & FRACTIONAL_MASK;
        double number = (double)integerPart + (double)fractionalPart / (double)FACTOR;
        return (sign_negative)?-number:number;
    }

    double toFloat() const {
        bool sign_negative = (value & 0x80000000) != 0;
        uint32_t uvalue = (sign_negative)?-value:value;
        uint32_t integerPart = uvalue >> FRACTIONAL_BITS;
        uint32_t fractionalPart = uvalue & FRACTIONAL_MASK;
        float number = (float)integerPart + (float)fractionalPart / (float)FACTOR;
        return (sign_negative)?-number:number;
    }

    std::string toString() const {
        return ITKCommon::PrintfToStdString("%f",toDouble());
    }
    
    int32_t integer_part() const {
        return (value >> FRACTIONAL_BITS);
    }

    int32_t fract_part() const {
        return (value & 0x80000000) ? (-(-value >> FRACTIONAL_MASK)) : (value >> FRACTIONAL_MASK);
    }
};

template<int frac_bits>
class ufixed32_t {
private:
    uint32_t value;

    static const uint32_t FRACTIONAL_BITS = frac_bits;
    static const uint32_t FRACTIONAL_MASK = (1 << FRACTIONAL_BITS) - 1;
    static const uint32_t FACTOR = 1 << FRACTIONAL_BITS;

public:
    ufixed32_t(uint32_t integerPart, uint32_t fractionalPart) {
        value = (integerPart << FRACTIONAL_BITS) | (fractionalPart & FRACTIONAL_MASK);
    }

    ufixed32_t(uint32_t rawValue) : value(rawValue) {}

    ufixed32_t operator+(const ufixed32_t& other) const {
        return ufixed32_t(value + other.value);
    }

    ufixed32_t operator-(const ufixed32_t& other) const {
        return ufixed32_t(value - other.value);
    }

    ufixed32_t operator-() const {
        return ufixed32_t(-value);
    }

    ufixed32_t operator*(const ufixed32_t& other) const {
        int64_t result = static_cast<int64_t>(value) * other.value;
        return ufixed32_t(static_cast<uint32_t>(result >> FRACTIONAL_BITS));
    }

    ufixed32_t operator/(const ufixed32_t& other) const {
        int64_t result = (static_cast<int64_t>(value) << FRACTIONAL_BITS) / other.value;
        return ufixed32_t(static_cast<uint32_t>(result));
    }

    double toDouble() const {
        uint32_t integerPart = value >> FRACTIONAL_BITS;
        uint32_t fractionalPart = value & FRACTIONAL_MASK;
        double number = (double)integerPart + (double)fractionalPart / (double)FACTOR;
        return number;
    }

    double toFloat() const {
        uint32_t integerPart = value >> FRACTIONAL_BITS;
        uint32_t fractionalPart = value & FRACTIONAL_MASK;
        float number = (float)integerPart + (float)fractionalPart / (float)FACTOR;
        return number;
    }

    std::string toString() const {
        return ITKCommon::PrintfToStdString("%f",toDouble());
    }


    uint32_t integer_part() const {
        return value >> FRACTIONAL_BITS;
    }

    uint32_t fract_part() const {
        return value & FRACTIONAL_MASK;
    }

};

