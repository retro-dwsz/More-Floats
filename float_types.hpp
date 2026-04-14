#ifndef FLOAT_TYPES_HPP
#define FLOAT_TYPES_HPP

#include <cstdint>
#include <cmath>
#include <cstring>
#include <limits>
#include <type_traits>
#include <bit>

// ============================================================================
// 8-bit Floating Point Types
// ============================================================================

// f8: 8-bit signed floating point (1 sign, 4 exponent, 3 mantissa)
// Similar to FP8 E4M3 format
struct f8 {
    uint8_t data;
    
    static constexpr int SIGN_BITS = 1;
    static constexpr int EXP_BITS = 4;
    static constexpr int MANTISSA_BITS = 3;
    static constexpr int EXP_BIAS = 7;
    
    constexpr f8() : data(0) {}
    constexpr explicit f8(float value) : data(float_to_f8(value)) {}
    constexpr explicit f8(double value) : data(float_to_f8(static_cast<float>(value))) {}
    constexpr explicit f8(int value) : data(float_to_f8(static_cast<float>(value))) {}
    
    constexpr operator float() const { return f8_to_float(data); }
    constexpr operator double() const { return static_cast<double>(f8_to_float(data)); }
    
    // Assignment operators
    constexpr f8& operator=(float value) { data = float_to_f8(value); return *this; }
    constexpr f8& operator=(double value) { data = float_to_f8(static_cast<float>(value)); return *this; }
    constexpr f8& operator=(int value) { data = float_to_f8(static_cast<float>(value)); return *this; }
    
    // Unary operators
    constexpr f8 operator-() const { f8 result; result.data = data ^ 0x80; return result; }
    constexpr f8 operator+() const { return *this; }
    
    // Arithmetic operators
    constexpr f8 operator+(const f8& other) const { return f8(static_cast<float>(*this) + static_cast<float>(other)); }
    constexpr f8 operator-(const f8& other) const { return f8(static_cast<float>(*this) - static_cast<float>(other)); }
    constexpr f8 operator*(const f8& other) const { return f8(static_cast<float>(*this) * static_cast<float>(other)); }
    constexpr f8 operator/(const f8& other) const { return f8(static_cast<float>(*this) / static_cast<float>(other)); }
    
    // Compound assignment operators
    constexpr f8& operator+=(const f8& other) { data = float_to_f8(static_cast<float>(*this) + static_cast<float>(other)); return *this; }
    constexpr f8& operator-=(const f8& other) { data = float_to_f8(static_cast<float>(*this) - static_cast<float>(other)); return *this; }
    constexpr f8& operator*=(const f8& other) { data = float_to_f8(static_cast<float>(*this) * static_cast<float>(other)); return *this; }
    constexpr f8& operator/=(const f8& other) { data = float_to_f8(static_cast<float>(*this) / static_cast<float>(other)); return *this; }
    
    // Comparison operators
    constexpr bool operator==(const f8& other) const { return data == other.data; }
    constexpr bool operator!=(const f8& other) const { return data != other.data; }
    constexpr bool operator<(const f8& other) const { return static_cast<float>(*this) < static_cast<float>(other); }
    constexpr bool operator<=(const f8& other) const { return static_cast<float>(*this) <= static_cast<float>(other); }
    constexpr bool operator>(const f8& other) const { return static_cast<float>(*this) > static_cast<float>(other); }
    constexpr bool operator>=(const f8& other) const { return static_cast<float>(*this) >= static_cast<float>(other); }
    
private:
    static constexpr uint8_t float_to_f8(float value) {
        if (std::isnan(value)) return 0x7C;
        if (value == 0.0f) return 0x00;
        if (value < 0.0f) return 0x80 | float_to_f8(-value);
        if (value > 510.0f) return 0x7F; // Overflow to infinity
        if (value < 0.0078125f) return 0x00; // Underflow to zero
        
        uint32_t bits;
        std::memcpy(&bits, &value, sizeof(float));
        
        int exp = ((bits >> 23) & 0xFF) - 127;
        int mantissa = bits & 0x7FFFFF;
        
        // Adjust for f8 format
        exp += EXP_BIAS;
        
        if (exp <= 0) {
            // Subnormal or zero
            if (exp < -3) return 0x00;
            mantissa = (mantissa | 0x800000) >> (4 - exp);
            return (mantissa + 4) >> 3;
        }
        
        if (exp >= 0xF) {
            // Infinity
            return 0x7F;
        }
        
        // Round mantissa to 3 bits
        mantissa = (mantissa + (1 << (23 - MANTISSA_BITS - 1))) >> (23 - MANTISSA_BITS);
        
        if (mantissa >= (1 << MANTISSA_BITS)) {
            exp++;
            mantissa = 0;
        }
        
        if (exp >= 0xF) {
            return 0x7F;
        }
        
        return static_cast<uint8_t>((exp << MANTISSA_BITS) | mantissa);
    }
    
    static constexpr float f8_to_float(uint8_t value) {
        if (value == 0x00) return 0.0f;
        if (value == 0x80) return -0.0f;
        if ((value & 0x7F) == 0x7F) {
            return (value & 0x80) ? -std::numeric_limits<float>::infinity() : std::numeric_limits<float>::infinity();
        }
        
        int sign = (value & 0x80) ? -1 : 1;
        int exp = (value >> MANTISSA_BITS) & 0xF;
        int mantissa = value & 0x7;
        
        if (exp == 0) {
            // Subnormal
            return sign * std::ldexp(static_cast<float>(mantissa), 1 - EXP_BIAS - MANTISSA_BITS);
        }
        
        // Normal number
        float mantissa_float = 1.0f + static_cast<float>(mantissa) / (1 << MANTISSA_BITS);
        return sign * std::ldexp(mantissa_float, exp - EXP_BIAS);
    }
};

// uf8: 8-bit unsigned floating point (0 sign, 5 exponent, 3 mantissa)
struct uf8 {
    uint8_t data;
    
    static constexpr int SIGN_BITS = 0;
    static constexpr int EXP_BITS = 5;
    static constexpr int MANTISSA_BITS = 3;
    static constexpr int EXP_BIAS = 15;
    
    constexpr uf8() : data(0) {}
    constexpr explicit uf8(float value) : data(float_to_uf8(value)) {}
    constexpr explicit uf8(double value) : data(float_to_uf8(static_cast<float>(value))) {}
    constexpr explicit uf8(int value) : data(float_to_uf8(static_cast<float>(value))) {}
    
    constexpr operator float() const { return uf8_to_float(data); }
    constexpr operator double() const { return static_cast<double>(uf8_to_float(data)); }
    
    // Assignment operators
    constexpr uf8& operator=(float value) { data = float_to_uf8(value); return *this; }
    constexpr uf8& operator=(double value) { data = float_to_uf8(static_cast<float>(value)); return *this; }
    constexpr uf8& operator=(int value) { data = float_to_uf8(static_cast<float>(value)); return *this; }
    
    // Unary operators
    constexpr uf8 operator+() const { return *this; }
    
    // Arithmetic operators
    constexpr uf8 operator+(const uf8& other) const { return uf8(static_cast<float>(*this) + static_cast<float>(other)); }
    constexpr uf8 operator-(const uf8& other) const { return uf8(static_cast<float>(*this) - static_cast<float>(other)); }
    constexpr uf8 operator*(const uf8& other) const { return uf8(static_cast<float>(*this) * static_cast<float>(other)); }
    constexpr uf8 operator/(const uf8& other) const { return uf8(static_cast<float>(*this) / static_cast<float>(other)); }
    
    // Compound assignment operators
    constexpr uf8& operator+=(const uf8& other) { data = float_to_uf8(static_cast<float>(*this) + static_cast<float>(other)); return *this; }
    constexpr uf8& operator-=(const uf8& other) { data = float_to_uf8(static_cast<float>(*this) - static_cast<float>(other)); return *this; }
    constexpr uf8& operator*=(const uf8& other) { data = float_to_uf8(static_cast<float>(*this) * static_cast<float>(other)); return *this; }
    constexpr uf8& operator/=(const uf8& other) { data = float_to_uf8(static_cast<float>(*this) / static_cast<float>(other)); return *this; }
    
    // Comparison operators
    constexpr bool operator==(const uf8& other) const { return data == other.data; }
    constexpr bool operator!=(const uf8& other) const { return data != other.data; }
    constexpr bool operator<(const uf8& other) const { return static_cast<float>(*this) < static_cast<float>(other); }
    constexpr bool operator<=(const uf8& other) const { return static_cast<float>(*this) <= static_cast<float>(other); }
    constexpr bool operator>(const uf8& other) const { return static_cast<float>(*this) > static_cast<float>(other); }
    constexpr bool operator>=(const uf8& other) const { return static_cast<float>(*this) >= static_cast<float>(other); }
    
private:
    static constexpr uint8_t float_to_uf8(float value) {
        if (std::isnan(value)) return 0xFF;
        if (value <= 0.0f) return 0x00;
        if (value > 57344.0f) return 0xFE; // Overflow to infinity
        if (value < 0.000061035f) return 0x00; // Underflow to zero
        
        uint32_t bits;
        std::memcpy(&bits, &value, sizeof(float));
        
        int exp = ((bits >> 23) & 0xFF) - 127;
        int mantissa = bits & 0x7FFFFF;
        
        // Adjust for uf8 format
        exp += EXP_BIAS;
        
        if (exp <= 0) {
            // Subnormal or zero
            if (exp < -3) return 0x00;
            mantissa = (mantissa | 0x800000) >> (4 - exp);
            return (mantissa + 4) >> 3;
        }
        
        if (exp >= 0x1F) {
            // Infinity
            return 0xFE;
        }
        
        // Round mantissa to 3 bits
        mantissa = (mantissa + (1 << (23 - MANTISSA_BITS - 1))) >> (23 - MANTISSA_BITS);
        
        if (mantissa >= (1 << MANTISSA_BITS)) {
            exp++;
            mantissa = 0;
        }
        
        if (exp >= 0x1F) {
            return 0xFE;
        }
        
        return static_cast<uint8_t>((exp << MANTISSA_BITS) | mantissa);
    }
    
    static constexpr float uf8_to_float(uint8_t value) {
        if (value == 0x00) return 0.0f;
        if (value == 0xFE) return std::numeric_limits<float>::infinity();
        if (value == 0xFF) return std::numeric_limits<float>::quiet_NaN();
        
        int exp = (value >> MANTISSA_BITS) & 0x1F;
        int mantissa = value & 0x7;
        
        if (exp == 0) {
            // Subnormal
            return std::ldexp(static_cast<float>(mantissa), 1 - EXP_BIAS - MANTISSA_BITS);
        }
        
        // Normal number
        float mantissa_float = 1.0f + static_cast<float>(mantissa) / (1 << MANTISSA_BITS);
        return std::ldexp(mantissa_float, exp - EXP_BIAS);
    }
};

// ============================================================================
// 16-bit Floating Point Types
// ============================================================================

// f16: 16-bit signed floating point (IEEE 754 half precision)
// Standard format: 1 sign, 5 exponent, 10 mantissa
struct f16 {
    uint16_t data;
    
    static constexpr int SIGN_BITS = 1;
    static constexpr int EXP_BITS = 5;
    static constexpr int MANTISSA_BITS = 10;
    static constexpr int EXP_BIAS = 15;
    
    constexpr f16() : data(0) {}
    constexpr explicit f16(float value) : data(float_to_f16(value)) {}
    constexpr explicit f16(double value) : data(float_to_f16(static_cast<float>(value))) {}
    constexpr explicit f16(int value) : data(float_to_f16(static_cast<float>(value))) {}
    
    constexpr operator float() const { return f16_to_float(data); }
    constexpr operator double() const { return static_cast<double>(f16_to_float(data)); }
    
    // Assignment operators
    constexpr f16& operator=(float value) { data = float_to_f16(value); return *this; }
    constexpr f16& operator=(double value) { data = float_to_f16(static_cast<float>(value)); return *this; }
    constexpr f16& operator=(int value) { data = float_to_f16(static_cast<float>(value)); return *this; }
    
    // Unary operators
    constexpr f16 operator-() const { f16 result; result.data = data ^ 0x8000; return result; }
    constexpr f16 operator+() const { return *this; }
    
    // Arithmetic operators
    constexpr f16 operator+(const f16& other) const { return f16(static_cast<float>(*this) + static_cast<float>(other)); }
    constexpr f16 operator-(const f16& other) const { return f16(static_cast<float>(*this) - static_cast<float>(other)); }
    constexpr f16 operator*(const f16& other) const { return f16(static_cast<float>(*this) * static_cast<float>(other)); }
    constexpr f16 operator/(const f16& other) const { return f16(static_cast<float>(*this) / static_cast<float>(other)); }
    
    // Compound assignment operators
    constexpr f16& operator+=(const f16& other) { data = float_to_f16(static_cast<float>(*this) + static_cast<float>(other)); return *this; }
    constexpr f16& operator-=(const f16& other) { data = float_to_f16(static_cast<float>(*this) - static_cast<float>(other)); return *this; }
    constexpr f16& operator*=(const f16& other) { data = float_to_f16(static_cast<float>(*this) * static_cast<float>(other)); return *this; }
    constexpr f16& operator/=(const f16& other) { data = float_to_f16(static_cast<float>(*this) / static_cast<float>(other)); return *this; }
    
    // Comparison operators
    constexpr bool operator==(const f16& other) const { return data == other.data; }
    constexpr bool operator!=(const f16& other) const { return data != other.data; }
    constexpr bool operator<(const f16& other) const { return static_cast<float>(*this) < static_cast<float>(other); }
    constexpr bool operator<=(const f16& other) const { return static_cast<float>(*this) <= static_cast<float>(other); }
    constexpr bool operator>(const f16& other) const { return static_cast<float>(*this) > static_cast<float>(other); }
    constexpr bool operator>=(const f16& other) const { return static_cast<float>(*this) >= static_cast<float>(other); }
    
private:
    static constexpr uint16_t float_to_f16(float value) {
        if (std::isnan(value)) return 0x7FFF;
        if (value == 0.0f) return 0x0000;
        if (value < 0.0f) return 0x8000 | float_to_f16(-value);
        if (value > 65504.0f) return 0x7C00; // Overflow to infinity
        if (value < 5.96e-8f) return 0x0000; // Underflow to zero
        
        uint32_t bits;
        std::memcpy(&bits, &value, sizeof(float));
        
        int exp = ((bits >> 23) & 0xFF) - 127;
        int mantissa = bits & 0x7FFFFF;
        
        // Adjust for f16 format
        exp += EXP_BIAS;
        
        if (exp <= 0) {
            // Subnormal or zero
            if (exp < -10) return 0x0000;
            mantissa = (mantissa | 0x800000) >> (1 - exp);
            mantissa = (mantissa + (1 << (23 - MANTISSA_BITS - 1))) >> (23 - MANTISSA_BITS);
            return static_cast<uint16_t>(mantissa);
        }
        
        if (exp >= 0x1F) {
            // Infinity
            return 0x7C00;
        }
        
        // Round mantissa to 10 bits
        mantissa = (mantissa + (1 << (23 - MANTISSA_BITS - 1))) >> (23 - MANTISSA_BITS);
        
        if (mantissa >= (1 << MANTISSA_BITS)) {
            exp++;
            mantissa = 0;
        }
        
        if (exp >= 0x1F) {
            return 0x7C00;
        }
        
        return static_cast<uint16_t>((exp << MANTISSA_BITS) | mantissa);
    }
    
    static constexpr float f16_to_float(uint16_t value) {
        if (value == 0x0000) return 0.0f;
        if (value == 0x8000) return -0.0f;
        if ((value & 0x7FFF) == 0x7C00) {
            return (value & 0x8000) ? -std::numeric_limits<float>::infinity() : std::numeric_limits<float>::infinity();
        }
        if ((value & 0x7FFF) > 0x7C00) {
            return std::numeric_limits<float>::quiet_NaN();
        }
        
        int sign = (value & 0x8000) ? -1 : 1;
        int exp = (value >> MANTISSA_BITS) & 0x1F;
        int mantissa = value & 0x3FF;
        
        if (exp == 0) {
            // Subnormal
            return sign * std::ldexp(static_cast<float>(mantissa), 1 - EXP_BIAS - MANTISSA_BITS);
        }
        
        // Normal number
        float mantissa_float = 1.0f + static_cast<float>(mantissa) / (1 << MANTISSA_BITS);
        return sign * std::ldexp(mantissa_float, exp - EXP_BIAS);
    }
};

// uf16: 16-bit unsigned floating point (0 sign, 6 exponent, 10 mantissa)
struct uf16 {
    uint16_t data;
    
    static constexpr int SIGN_BITS = 0;
    static constexpr int EXP_BITS = 6;
    static constexpr int MANTISSA_BITS = 10;
    static constexpr int EXP_BIAS = 31;
    
    constexpr uf16() : data(0) {}
    constexpr explicit uf16(float value) : data(float_to_uf16(value)) {}
    constexpr explicit uf16(double value) : data(float_to_uf16(static_cast<float>(value))) {}
    constexpr explicit uf16(int value) : data(float_to_uf16(static_cast<float>(value))) {}
    
    constexpr operator float() const { return uf16_to_float(data); }
    constexpr operator double() const { return static_cast<double>(uf16_to_float(data)); }
    
    // Assignment operators
    constexpr uf16& operator=(float value) { data = float_to_uf16(value); return *this; }
    constexpr uf16& operator=(double value) { data = float_to_uf16(static_cast<float>(value)); return *this; }
    constexpr uf16& operator=(int value) { data = float_to_uf16(static_cast<float>(value)); return *this; }
    
    // Unary operators
    constexpr uf16 operator+() const { return *this; }
    
    // Arithmetic operators
    constexpr uf16 operator+(const uf16& other) const { return uf16(static_cast<float>(*this) + static_cast<float>(other)); }
    constexpr uf16 operator-(const uf16& other) const { return uf16(static_cast<float>(*this) - static_cast<float>(other)); }
    constexpr uf16 operator*(const uf16& other) const { return uf16(static_cast<float>(*this) * static_cast<float>(other)); }
    constexpr uf16 operator/(const uf16& other) const { return uf16(static_cast<float>(*this) / static_cast<float>(other)); }
    
    // Compound assignment operators
    constexpr uf16& operator+=(const uf16& other) { data = float_to_uf16(static_cast<float>(*this) + static_cast<float>(other)); return *this; }
    constexpr uf16& operator-=(const uf16& other) { data = float_to_uf16(static_cast<float>(*this) - static_cast<float>(other)); return *this; }
    constexpr uf16& operator*=(const uf16& other) { data = float_to_uf16(static_cast<float>(*this) * static_cast<float>(other)); return *this; }
    constexpr uf16& operator/=(const uf16& other) { data = float_to_uf16(static_cast<float>(*this) / static_cast<float>(other)); return *this; }
    
    // Comparison operators
    constexpr bool operator==(const uf16& other) const { return data == other.data; }
    constexpr bool operator!=(const uf16& other) const { return data != other.data; }
    constexpr bool operator<(const uf16& other) const { return static_cast<float>(*this) < static_cast<float>(other); }
    constexpr bool operator<=(const uf16& other) const { return static_cast<float>(*this) <= static_cast<float>(other); }
    constexpr bool operator>(const uf16& other) const { return static_cast<float>(*this) > static_cast<float>(other); }
    constexpr bool operator>=(const uf16& other) const { return static_cast<float>(*this) >= static_cast<float>(other); }
    
private:
    static constexpr uint16_t float_to_uf16(float value) {
        if (std::isnan(value)) return 0xFFFF;
        if (value <= 0.0f) return 0x0000;
        if (value > 2.147e9f) return 0xFC00; // Overflow to infinity
        if (value < 2.9e-10f) return 0x0000; // Underflow to zero
        
        uint32_t bits;
        std::memcpy(&bits, &value, sizeof(float));
        
        int exp = ((bits >> 23) & 0xFF) - 127;
        int mantissa = bits & 0x7FFFFF;
        
        // Adjust for uf16 format
        exp += EXP_BIAS;
        
        if (exp <= 0) {
            // Subnormal or zero
            if (exp < -10) return 0x0000;
            mantissa = (mantissa | 0x800000) >> (1 - exp);
            mantissa = (mantissa + (1 << (23 - MANTISSA_BITS - 1))) >> (23 - MANTISSA_BITS);
            return static_cast<uint16_t>(mantissa);
        }
        
        if (exp >= 0x3F) {
            // Infinity
            return 0xFC00;
        }
        
        // Round mantissa to 10 bits
        mantissa = (mantissa + (1 << (23 - MANTISSA_BITS - 1))) >> (23 - MANTISSA_BITS);
        
        if (mantissa >= (1 << MANTISSA_BITS)) {
            exp++;
            mantissa = 0;
        }
        
        if (exp >= 0x3F) {
            return 0xFC00;
        }
        
        return static_cast<uint16_t>((exp << MANTISSA_BITS) | mantissa);
    }
    
    static constexpr float uf16_to_float(uint16_t value) {
        if (value == 0x0000) return 0.0f;
        if (value == 0xFC00) return std::numeric_limits<float>::infinity();
        if (value == 0xFFFF) return std::numeric_limits<float>::quiet_NaN();
        
        int exp = (value >> MANTISSA_BITS) & 0x3F;
        int mantissa = value & 0x3FF;
        
        if (exp == 0) {
            // Subnormal
            return std::ldexp(static_cast<float>(mantissa), 1 - EXP_BIAS - MANTISSA_BITS);
        }
        
        // Normal number
        float mantissa_float = 1.0f + static_cast<float>(mantissa) / (1 << MANTISSA_BITS);
        return std::ldexp(mantissa_float, exp - EXP_BIAS);
    }
};

#endif // FLOAT_TYPES_HPP
