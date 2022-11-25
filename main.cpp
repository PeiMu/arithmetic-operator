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
    /* OR compare most significant bits i.e. if (((temp >> 31) & 1) == ((b >> 15) & 1)) */
    if ((temp >= 0 && b >= 0) || (temp < 0 && b < 0)) {
        temp += b / 2;    /* OR shift 1 bit i.e. temp += (b >> 1); */
    } else {
        temp -= b / 2;    /* OR shift 1 bit i.e. temp -= (b >> 1); */
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
    printf("add result: %f\n", (float)res/(1<<8));
    res = q_sub(a_quant, b_quant);
    printf("sub result: %f\n", (float)res/(1<<8));
    res = q_mul(a_quant, b_quant);
    printf("mul result: %f\n", (float)res/(1<<8));
    res = q_div(a_quant, b_quant);
    printf("div result: %f\n", (float)res/(1<<8));
    return 0;
}
