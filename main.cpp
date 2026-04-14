#include <iostream>
#include "float_types.hpp"
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <fmt/std.h>
#include <fmt/os.h>

/* Mari kita membuat f8, uf8, f16, dan uf16, yaitu 8-bit & 16-bit floating point, signed & unsigned  */

int main() {
    std::cout << "=== Floating Point Types Demo ===\n\n";
    
    // f8: 8-bit signed floating point
    std::cout << "--- f8 (8-bit signed) ---\n";
    std::cout << "Format: 1 sign bit, 4 exponent bits, 3 mantissa bits\n";
    f8 a(3.5f);
    f8 b(-2.25f);
    std::cout << fmt::format("a = {}, b = {}\n", static_cast<float>(a), static_cast<float>(b));
    std::cout << fmt::format("a + b = {}\n", static_cast<float>(a + b));
    std::cout << fmt::format("a * b = {}\n", static_cast<float>(a * b));
    std::cout << fmt::format("a / b = {}\n", static_cast<float>(a / b));
    std::cout << fmt::format("-a = {}\n", static_cast<float>(-a));
    std::cout << fmt::format("sizeof(f8) = {} bytes\n\n", sizeof(f8));
    
    // uf8: 8-bit unsigned floating point
    std::cout << "--- uf8 (8-bit unsigned) ---\n";
    std::cout << "Format: 0 sign bits, 5 exponent bits, 3 mantissa bits\n";
    uf8 c(10.0f);
    uf8 d(2.5f);
    std::cout << fmt::format("c = {}, d = {}\n", static_cast<float>(c), static_cast<float>(d));
    std::cout << fmt::format("c + d = {}\n", static_cast<float>(c + d));
    std::cout << fmt::format("c * d = {}\n", static_cast<float>(c * d));
    std::cout << fmt::format("c / d = {}\n", static_cast<float>(c / d));
    std::cout << fmt::format("sizeof(uf8) = {} bytes\n\n", sizeof(uf8));
    
    // f16: 16-bit signed floating point (IEEE 754 half precision)
    std::cout << "--- f16 (16-bit signed, IEEE 754 half) ---\n";
    std::cout << "Format: 1 sign bit, 5 exponent bits, 10 mantissa bits\n";
    f16 e(123.456f);
    f16 f(-67.89f);
    std::cout << fmt::format("e = {}, f = {}\n", static_cast<float>(e), static_cast<float>(f));
    std::cout << fmt::format("e + f = {}\n", static_cast<float>(e + f));
    std::cout << fmt::format("e * f = {}\n", static_cast<float>(e * f));
    std::cout << fmt::format("e / f = {}\n", static_cast<float>(e / f));
    std::cout << fmt::format("-e = {}\n", static_cast<float>(-e));
    std::cout << fmt::format("sizeof(f16) = {} bytes\n\n", sizeof(f16));
    
    // uf16: 16-bit unsigned floating point
    std::cout << "--- uf16 (16-bit unsigned) ---\n";
    std::cout << "Format: 0 sign bits, 6 exponent bits, 10 mantissa bits\n";
    uf16 g(1000.0f);
    uf16 h(3.14159f);
    std::cout << fmt::format("g = {}, h = {}\n", static_cast<float>(g), static_cast<float>(h));
    std::cout << fmt::format("g + h = {}\n", static_cast<float>(g + h));
    std::cout << fmt::format("g * h = {}\n", static_cast<float>(g * h));
    std::cout << fmt::format("g / h = {}\n", static_cast<float>(g / h));
    std::cout << fmt::format("sizeof(uf16) = {} bytes\n\n", sizeof(uf16));
    
    // Range demonstrations
    std::cout << "--- Value Ranges ---\n";
    std::cout << fmt::format("f8: min positive = {}, max = {}\n", 
                             static_cast<float>(static_cast<f8>(0.0078125f)), static_cast<float>(static_cast<f8>(480.0f)));
    std::cout << fmt::format("uf8: min positive = {}, max = {}\n", 
                             static_cast<float>(static_cast<uf8>(0.000061035f)), static_cast<float>(static_cast<uf8>(50000.0f)));
    std::cout << fmt::format("f16: min positive = {}, max = {}\n", 
                             static_cast<float>(static_cast<f16>(6.104e-5f)), static_cast<float>(static_cast<f16>(65504.0f)));
    std::cout << fmt::format("uf16: min positive = {}, max = {}\n", 
                             static_cast<float>(static_cast<uf16>(2.9e-10f)), static_cast<float>(static_cast<uf16>(1e9f)));
    
    return 0;
}