#include <iostream>
#include "approx_log2.hpp"

int main(){
    float x = 1.2345f;
    float cmath_log2_result = std::log2(x);
    float approx_log2_result = approx_log2(x);
    float error = std::abs(cmath_log2_result - approx_log2_result);
    std::cout << "cmath: " << cmath_log2_result << " ";
    std::cout << "approx_log2: " << approx_log2_result << " ";
    std::cout << "error: " << error << std::endl;
}
