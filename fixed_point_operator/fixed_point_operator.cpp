//
// Created by pei on 01/12/22.
//

#include "fixed_point_operator.h"

// saturate to range of int16_t
int16_t upSat16(int32_t x)
{
    if (x > INT16_MAX || x < INT16_MIN)
        /*
         * In the real code, it changes to a higher bit number
         * */
        assert(false && "overflow!!!");
    else
        return (int16_t)x;
}

// saturate to range of int32_t
int32_t upSat32(int64_t x)
{
    if (x > INT32_MAX || x < INT32_MIN)
        /*
         * In the real code, it changes to a higher bit number
         * */
        assert(false && "overflow!!!");
    else
        return (int32_t)x;
}

int16_t q_add(int16_t a, int16_t b)
{
    int32_t tmp;

    tmp = (int32_t)a + (int32_t)b;

    return upSat16(tmp);
}

int16_t q_sub(int16_t a, int16_t b)
{
    return (int16_t)(a - b);
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
    result = upSat16(temp >> Q);

    return result;
}

int32_t q_mul_32(int32_t a, int32_t b)
{
    int32_t result;
    int64_t temp;

    temp = (int64_t)a * (int64_t)b; // result type is operand's type
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
    if ((temp >> 63))
        temp -= K;
    else
        temp += K;
    // Correct by dividing by base and saturate result
    result = upSat32(temp >> Q);

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
