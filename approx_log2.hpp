#pragma once

#include <cstring>
#include <cmath>


/* The general idea behind this algorithm is as follows:
 * we want to decompose x into a natural number and a fractional part
 * using the fact that any x will in some half-open interval [ 2^n, 2^(n+1) )
 * such that 2^-n * x will be in the half-open interval [1, 2) 
 * then log2(x) = n + log2( x' )
 * where x' = 2^-n * x
 * this can be done efficiently for ieee754 floats,
 * given that they store numbers as m * 2^n.
 * After the initial normalization step, we iterate as follows:
 *   square x' until it is in the half-open interval [2,4)
 *   set x_new = x'^(2^k) where k is the number of times we had to square x'
 *   then log2(x_new) = 2^k * log2(x')
 *   thus log2(x') = log2(x_new)/2^k = ( 1 + log2(x_new/2) )/2^k = 2^-k + 2^-k * log2(x_new/2)
 *   set x' = x_new and continue iteration.
 * now collecting all those `k`s from the iterations lets us calculate log2(x):
 * log2(x) = n + 2^(-k1) * (1 + 2^(-k2) * (1 + 2^(-k3) * (1 + ...  )))
 * log2(x) = n + 2^(-k1) + 2^(-k1-k2) + 2^(-k1-k2-k3) + ...
 * 
 * At any iteration where the remaining x' is found to be 1 we can terminate as the logarithmic expansion is finite.
 * The precision of the result depends on the number of iterations after which the algorithm is terminated.
 * The error is bounded as err(m) < 2^(-(k1+k2+k3+...+km))
 * which in turn implies that:
 *  1: we can get a fixed upper bound on the error independent of the input
 *     by setting a fixed number of iterations the worst case will be:
 *     err(m) < 2^(-(1+1+1+...+1)) = 2^(-m)
 *  2: instead of a fixed number of iterations we can look at the current error:
 *     2^(-(k1+k2+k3+...) and decided if it meets our required error bounds and then terminate
 * 
 */


float get_exponent_and_normalize(float& f){
    uint32_t x;
    std::memcpy(&x, &f, sizeof(f));
    uint32_t e = x >> 23;                     //exponent of f
    uint32_t m = x & 0x7fffff;                //mantissa of f
    uint32_t new_exp = 0x3f800000;            //exponent = 0 -> float is in the range [1, 2)
    uint32_t normalized = new_exp | m;        //this is now the binary representation of a normalized 'f'
    std::memcpy(&f, &normalized, sizeof(f));  //copy back to float
    int32_t res = e & 0xff;                   //we remove the sign bit (should be 0 anyway)
    res -= 0x7f;                              //compute the actual exponent (ieee floats have a bias of 127 i.e. 0x7f)
    return static_cast<float>(res);           //return the exponent as a float. note that this is not a bit-reinterpretation but an actual conversion
}

float pow_neg_two(uint32_t m){                //calculates 2^(-m)
    m = 127 - m;
    m = m << 23;
    float f;
    std::memcpy(&f, &m, sizeof(f));
    return f;
}

uint32_t log2_step(float& f){
    uint32_t m = 0;
    while(f < 2.0f){                          //square f until it is in the (2, 4] range
        f = f*f;
        m++;
    }
    f /= 2.0f;                                //re-normalize in the (1, 2] range
    return m;                                 //return the number of squarings we did
}

float approx_log2(float f){
    float e = get_exponent_and_normalize(f); //step 1: normalization
    if(f == 1.0f) return e;                  //f = 2^n where `n` is an integer -> we're done
    float res = e;                           //this will be our result. Initialize it to the exponent
    uint32_t m = 0;
    for(int i = 0; i < 9; i++){              //fixed number of iterations. See description at the top for more information
        m += log2_step(f);
        res += pow_neg_two(m);
        if(f == 1.0f) return res;            //no more digits to find -> terminate
    }
    return res;
}

float approx_ln(float f){
    const float inv_log2_e = 0.6931471805599453f;
    return approx_log2(f) * inv_log2_e;
}

