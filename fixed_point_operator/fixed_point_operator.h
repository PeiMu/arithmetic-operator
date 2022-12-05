//
// Created by pei on 01/12/22.
//

#ifndef QUANTIZATON_TEST_FIXED_POINT_OPERATOR_H
#define QUANTIZATON_TEST_FIXED_POINT_OPERATOR_H

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// precomputed value:
#define Q   7
#define K   (1 << (Q - 1))

#ifdef __cplusplus
extern "C"{
#endif

int16_t upSat16(int32_t x);
int32_t upSat32(int64_t x);
int16_t q_add(int16_t a, int16_t b);
int16_t q_sub(int16_t a, int16_t b);
int16_t q_mul(int16_t a, int16_t b);
int32_t q_mul_32(int32_t a, int32_t b);
int16_t q_div(int16_t a, int16_t b);

#ifdef __cplusplus
}
#endif

#endif //QUANTIZATON_TEST_FIXED_POINT_OPERATOR_H
