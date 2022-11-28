#include <iostream>

// precomputed value:
#define Q   8
#define K   (1 << (Q - 1))

// saturate to range of int16_t
int16_t sat16(int32_t x)
{
    if (x > INT16_MAX) return INT16_MAX;
    else if (x < INT16_MIN) return INT16_MIN;
    else return (int16_t)x;
}

int16_t q_add_sat(int16_t a, int16_t b)
{
    int32_t tmp;

    tmp = (int32_t)a + (int32_t)b;

    return sat16(tmp);
}

int16_t q_sub(int16_t a, int16_t b)
{
    return a - b;
}

int16_t q_mul(int16_t a, int16_t b)
{
    int16_t result;
    int32_t temp;

    temp = (int32_t)a * (int32_t)b; // result type is operand's type
    // Rounding; mid-values are rounded up
    /*
     * Rounding is possible by adding a 'rounding addend' of
     * half of the scaling factor before shifting;
     *
     * The proof:
     * Res = round(x/y) = (int)(x/y +/- 0.5) = (int)((x +/- y/2)/y)
     * let y = 2^Q,
     * Res = (int)((x +/- 2^(Q-1))/2^Q) = (x +/- 2^(Q-1)) >> Q
     * */
    if ((temp >> 31))
        temp -= K;
    else
        temp += K;
    // Correct by dividing by base and saturate result
    result = sat16(temp >> Q);

    return result;
}

int16_t q_div(int16_t a, int16_t b)
{
    /* pre-multiply by the base (Upscale to Q16 so that the result will be in Q8 format) */
    int32_t temp = (int32_t)a << Q;
    /* Rounding: mid-values are rounded up (down for negative values). */
    /*
     * The proof:
     * Res = round(a/b)
     * Because trunc negative remove the fraction, e.g. (int)(-1.833) = -1
     * if a and b have the same sign bit, Res = (int)(a/b + 0.5) = floor((a+b/2)/b)
     * else Res = (int)(a/b - 0.5) = floor((a-b/2)/b)
     * */
    if ((temp >> 31) ^ (b >> 15)) {
        temp -= (b >> 1);
    } else {
        temp += (b >> 1);
    }
    return (int16_t)(temp / b);
}

int main() {
    // arithmetic of 1.505016722 and 1.59765625
    double a = 1.5;
    double b = 1.6;
    auto a_quant = (int16_t)(a * (1 << Q));
    auto b_quant = (int16_t)(b * (1 << Q));
    int16_t res = q_add_sat(a_quant, b_quant);
    printf("add result: %f\n", (float)res/(1<<Q));
    res = q_sub(a_quant, b_quant);
    printf("sub result: %f\n", (float)res/(1<<Q));
    res = q_mul(a_quant, b_quant);
    printf("mul result: %f\n", (float)res/(1<<Q));
    res = q_div(a_quant, b_quant);
    printf("div result: %f\n", (float)res/(1<<Q));
    return 0;
}
