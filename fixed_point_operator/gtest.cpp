//
// Created by pei on 01/12/22.
//

#include <gtest/gtest.h>
#include <cmath>
#include "fixed_point_operator.h"

class FixedPointOpTest {
public:
    double a_ = 1.5;
    double b_ = 1.6;
    int16_t a_quant = (int16_t)(a_ * (1 << Q));
    int16_t b_quant = (int16_t)(b_ * (1 << Q));
};

TEST(FixedPointOpTest, FixedPointAdd) {
    FixedPointOpTest tester;
    int16_t res = q_add(tester.a_quant, tester.b_quant);
    double quant_res = (float)res/(1<<Q);
    double real_res = tester.a_ + tester.b_;
    EXPECT_LE(fabs(quant_res-real_res), 1.0f/(1<<Q));
    EXPECT_STRNE("QuantizationCorrectly", "SUCCESS");
}

TEST(FixedPointOpTest, FixedPointSub) {
    FixedPointOpTest tester;
    int16_t res = q_sub(tester.a_quant, tester.b_quant);
    double quant_res = (float)res/(1<<Q);
    double real_res = tester.a_ - tester.b_;
    EXPECT_LE(fabs(quant_res-real_res), 1.0f/(1<<Q));
    EXPECT_STRNE("QuantizationCorrectly", "SUCCESS");
}

TEST(FixedPointOpTest, FixedPointMul) {
    FixedPointOpTest tester;
    int16_t res = q_mul(tester.a_quant, tester.b_quant);
    double quant_res = (float)res/(1<<Q);
    double real_res = tester.a_ * tester.b_;
    EXPECT_LE(fabs(quant_res-real_res), 1.0f/(1<<Q));
    EXPECT_STRNE("QuantizationCorrectly", "SUCCESS");
}

TEST(FixedPointOpTest, FixedPointDiv) {
    FixedPointOpTest tester;
    int16_t res = q_div(tester.a_quant, tester.b_quant);
    double quant_res = (float)res/(1<<Q);
    double real_res = tester.a_ / tester.b_;
    EXPECT_LE(fabs(quant_res-real_res), 1.0f/(1<<Q));
    EXPECT_STRNE("QuantizationCorrectly", "SUCCESS");
}