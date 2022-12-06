#include "fixed_point_operator/fixed_point_operator.h"
#include "soft_float_operator/include/soft_float.h"
#include <math.h>
#include <string.h>

/*
 * random floating point, [min, max]
 * */
double
randomDouble(double min, double max)
{
    double randDbValue = min + 1.0 * rand() / RAND_MAX * (max - min);
    return randDbValue;
}

#if defined(TEST_PRECISION)
int iteration_num = 1;
#elif defined(TEST_TIME_CONSUMPTION)
int iteration_num = 1000000;
#else
int iteration_num = 1;
#endif

#define array_size 5

/*
 * a common case testsuite,
 * this example test code wants to show the performance of computation sensitive program.
 * the magic numbers and operators in this function are random and meaningless.
 * */
void double_add_test(const double* leftOp, const double* rightOp, double* result) {
    for (size_t idx = 0; idx < array_size; idx++) {
        result[idx] = leftOp[idx] + rightOp[idx];
        double x = leftOp[idx] + 12.789;
        double y = rightOp[idx] + 15.653;
        double z = x + y;
        result[idx] = result[idx] * ((int)z%100);
    }
}

/*
 * It's just another version of soft-float arithmetic
 * */
void soft_float_add_test(const double* leftOp, const double* rightOp, double* result) {
    float64 soft_c1 = 0, soft_c2 = 0;
    double c1 = 12.789, c2 = 15.653;
    memcpy(&soft_c1, &c1, sizeof(double));
    memcpy(&soft_c2, &c2, sizeof(double));
    for (size_t idx = 0; idx < array_size; idx++) {
        float64 soft_left = 0;
        float64 soft_right = 0;
        memcpy(&soft_left, &leftOp[idx], sizeof(double));
        memcpy(&soft_right, &rightOp[idx], sizeof(double));
        float64 soft_res = float64_add(soft_left, soft_right);
        float64 soft_x = float64_add(soft_left, soft_c1);
        float64 soft_y = float64_add(soft_right, soft_c2);
        float64 soft_z = float64_add(soft_x, soft_y);
        double z = 0;
        memcpy(&z, &soft_z, sizeof(float64));
        memcpy(&result[idx], &soft_res, sizeof(double));
        result[idx] = result[idx] * ((int)z%100);
    }
}

/*
 * a round testsuite for the "double_add_test"
 * they have same computation pattern.
 * */
void round_add_test(const double* leftOp, const double* rightOp, double* result) {
    for (size_t idx = 0; idx < array_size; idx++) {
        int qr_left = (int)round(leftOp[idx]);
        int qr_right = (int)round(rightOp[idx]);
        int qr_result = qr_left + qr_right;
        /* the resolution of 'double' is 0.98, so
         * (int)(12.789/0.98) = 13;
         * (int)(15.653/0.98) = 16;
         * */
        int qr_x = qr_left + 13;
        int qr_y = qr_right + 16;
        int qr_z = qr_x + qr_y;
        double z = qr_z;
        int temp_z = (int)z%100;
        result[idx] = qr_result * temp_z;
    }
}

/*
 * a testsuite for q-format-fixed-point-roundization,
 * this example test code wants to show the performance of computation sensitive program.
 * the magic numbers and operators in this function are random and meaningless.
 * */
void fixed_point_add_test(const double* leftOp, const double* rightOp, double* result) {
    int16_t c1 = (int16_t)round(12.789 * (1 << Q));
    int16_t c2 = (int16_t)round(15.653 * (1 << Q));
    for (size_t idx = 0; idx < array_size; idx++) {
        int16_t leftOpround = (int16_t)round(leftOp[idx] * (1 << Q));
        int16_t rightOpround = (int16_t)round(rightOp[idx] * (1 << Q));
        int16_t resultround = q_add(leftOpround, rightOpround);
        int16_t x = q_add(leftOpround, c1);
        int16_t y = q_add(rightOpround, c2);
        int16_t z = q_add(x, y);
        /*
         * For non-linear operator, change back to floating-point
         * */
        double fp_z = (double)(z >> Q);
        int16_t temp_z = (int16_t)fp_z%100;
        temp_z = temp_z << Q;
        /*
         * When the range analyzer find the range of result might need 32 bits,
         * it changes to upper precision
         * */
        int32_t resultround_32 = q_mul_32(resultround, temp_z);
        result[idx] = (double)resultround_32 / (1<<Q);
    }
}

/*
 * simplified version of fixed_point_add_test function.
 * */
void fixed_point_add_test_simplified(const double* leftOp, const double* rightOp, double* result) {
    const int16_t c1 = (int16_t)(12.789 * (1 << Q)+0.5);
    const int16_t c2 = (int16_t)(15.653 * (1 << Q)+0.5);
    for (size_t idx = 0; idx < array_size; idx++) {
        int16_t leftOpround = (int16_t)(leftOp[idx] * (1 << Q)+0.5);
        int16_t rightOpround = (int16_t)(rightOp[idx] * (1 << Q)+0.5);
        int16_t resultround = leftOpround + rightOpround;
        int16_t x = leftOpround + c1;
        int16_t y = rightOpround + c2;
        int16_t z = x + y;
        /*
         * For non-linear operator, change back to floating-point
         * */
        int16_t temp_z = (z >> Q)%100;
        temp_z = temp_z << Q;
        /*
         * When the range analyzer find the range of result might need 32 bits,
         * it changes to upper precision
         * */
        int32_t resultround_32 = (resultround * temp_z + K) >> Q;
        result[idx] = (double)resultround_32 / (1<<Q);
    }
}

int main(int argc, char** argv) {

    for (int i = 0; i < iteration_num; i++)
    {
        double leftOps[array_size];
        double rightOps[array_size];
        for (size_t idx = 0; idx < array_size; idx++) {
            leftOps[idx] = randomDouble(0, 63);
            rightOps[idx] = randomDouble(0, 63);
        }
#if (defined(TEST_TIME_CONSUMPTION) && defined(BENCHMARK_SUITE_DOUBLE)) || defined(TEST_PRECISION)
        double db_result[array_size];
        double_add_test(leftOps, rightOps, db_result);
#endif
#if (defined(TEST_TIME_CONSUMPTION) && defined(BENCHMARK_SUITE_SOFT_FLOAT)) || defined(TEST_PRECISION)
        double sf_result[array_size];
        soft_float_add_test(leftOps, rightOps, sf_result);
#endif
#if (defined(TEST_TIME_CONSUMPTION) && defined(BENCHMARK_SUITE_ROUND)) || defined(TEST_PRECISION)
        double qr_result[array_size];
        round_add_test(leftOps, rightOps, qr_result);
#endif
#if (defined(TEST_TIME_CONSUMPTION) && defined(BENCHMARK_SUITE_FIXEDPOINT)) || defined(TEST_PRECISION)
        double fp_result[array_size];
        fixed_point_add_test_simplified(leftOps, rightOps, fp_result);
#endif

#if defined(TEST_PRECISION)
        double sf_error = 0, qr_error = 0, fp_error = 0;
        for (size_t idx = 0; idx < array_size; idx++) {
            sf_error += fabs(sf_result[idx] - db_result[idx]) / db_result[idx];
            qr_error += fabs(qr_result[idx] - db_result[idx]) / db_result[idx];
            fp_error += fabs(fp_result[idx] - db_result[idx]) / db_result[idx];
        }
        sf_error = sf_error / array_size * 100;
        qr_error = qr_error / array_size * 100;
        fp_error = fp_error / array_size * 100;
        printf("accumulation error: soft-float: %f%\tround-resolution: %f%\tfixed-point: %f%\n",
               sf_error, qr_error, fp_error);
#endif
    }
    return 0;
}
