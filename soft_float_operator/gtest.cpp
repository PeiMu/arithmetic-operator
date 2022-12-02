//
// Created by pei on 02/12/22.
//

#include <cfloat>
#include <cmath>
#include <cstring>
#include <gtest/gtest.h>
#include "include/soft_float.h"

class SoftFloatOpTest {
public:
    SoftFloatOpTest(double a, double b) {
        memcpy(&a_soft_float_, &a, sizeof(double));
        memcpy(&b_soft_float_, &b, sizeof(double));
    }
    ~SoftFloatOpTest() = default;

    float64 a_soft_float_ = 0;
    float64 b_soft_float_ = 0;
};

TEST(SoftFloatOpTest, SoftFloatAdd) {
    double a = 468.31, b = 294.74;
    SoftFloatOpTest tester(a, b);
    float64 res = float64_add(tester.a_soft_float_, tester.b_soft_float_);
    double db_res;
    memcpy(&db_res, &res, sizeof(double));
    double real_res = a + b;
    EXPECT_LE(fabs(db_res-real_res), DBL_MIN);
    EXPECT_STRNE("SoftFloatAdd", "SUCCESS");
}

TEST(SoftFloatOpTest, SoftFloatSub) {
    double a = 468.31, b = 294.74;
    SoftFloatOpTest tester(a, -b);
    float64 res = float64_add(tester.a_soft_float_, tester.b_soft_float_);
    double db_res;
    memcpy(&db_res, &res, sizeof(double));
    double real_res = a - b;
    EXPECT_LE(fabs(db_res-real_res), DBL_MIN);
    EXPECT_STRNE("SoftFloatSub", "SUCCESS");
}

TEST(SoftFloatOpTest, SoftFloatMul) {
    double a = 468.31, b = 294.74;
    SoftFloatOpTest tester(a, b);
    float64 res = float64_mul(tester.a_soft_float_, tester.b_soft_float_);
    double db_res;
    memcpy(&db_res, &res, sizeof(double));
    double real_res = a * b;
    EXPECT_LE(fabs(db_res-real_res), DBL_MIN);
    EXPECT_STRNE("SoftFloatMul", "SUCCESS");
}

TEST(SoftFloatOpTest, SoftFloatDiv) {
    double a = 468.31, b = 294.74;
    SoftFloatOpTest tester(a, b);
    float64 res = float64_div(tester.a_soft_float_, tester.b_soft_float_);
    double db_res;
    memcpy(&db_res, &res, sizeof(double));
    double real_res = a / b;
    EXPECT_LE(fabs(db_res-real_res), DBL_MIN);
    EXPECT_STRNE("SoftFloatDiv", "SUCCESS");
}
