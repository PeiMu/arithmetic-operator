#include "fixed_point_operator/fixed_point_operator.h"
#include "soft_float_operator/include/soft_float.h"
#include <math.h>
#include <string.h>
#include <time.h>

/*
 * random floating point, [min, max]
 * */
double randomDouble(double min, double max)
{
    double randDbValue = min + 1.0 * rand() / RAND_MAX * (max - min);
    return randDbValue;
}

#if defined(TEST_PRECISION)
#define iteration_num 1
#elif defined(TEST_TIME_CONSUMPTION)
#define iteration_num 100
#else
#define iteration_num 1
#endif

#define array_size 100000

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
        int qr_left = (int)(leftOp[idx]+0.5);
        int qr_right = (int)(rightOp[idx]+0.5);
        int qr_result = qr_left + qr_right;
        /* round constant
         * (int)(12.789) = 13;
         * (int)(15.653) = 16;
         * */
        int qr_x = qr_left + 13;
        int qr_y = qr_right + 16;
        int qr_z = qr_x + qr_y;
        int temp_z = qr_z%100;
        result[idx] = qr_result * temp_z;
    }
}

/*
 * a testsuite for q-format-fixed-point-roundization,
 * this example test code wants to show the performance of computation sensitive program.
 * the magic numbers and operators in this function are random and meaningless.
 * */
void fixed_point_add_test(const double* leftOp, const double* rightOp, double* result) {
    int c1 = (int)round(12.789 * (1 << Q));
    int c2 = (int)round(15.653 * (1 << Q));
    for (size_t idx = 0; idx < array_size; idx++) {
        int leftOpround = (int)round(leftOp[idx] * (1 << Q));
        int rightOpround = (int)round(rightOp[idx] * (1 << Q));
        int resultround = q_add(leftOpround, rightOpround);
        int x = q_add(leftOpround, c1);
        int y = q_add(rightOpround, c2);
        int z = q_add(x, y);
        /*
         * For non-linear operator, change back to floating-point
         * */
        double fp_z = (double)(z >> Q);
        int temp_z = (int)fp_z%100;
        temp_z = temp_z << Q;
        /*
         * When the range analyzer find the range of result might need 32 bits,
         * it changes to upper precision
         * */
        int result_round_32 = q_mul_32(resultround, temp_z);
        result[idx] = (double)result_round_32 / (1<<Q);
    }
}

/*
 * simplified version of fixed_point_add_test function.
 * */
const int c1 = (int)(12.789 * (1 << Q)+0.5);
const int c2 = (int)(15.653 * (1 << Q)+0.5);

void fixed_point_add_test_simplified(const int* leftOp, const int* rightOp, int* result) {
    for (size_t idx = 0; idx < array_size; idx++) {
        int resultround = leftOp[idx] + rightOp[idx];
        int x = leftOp[idx] + c1;
        int y = rightOp[idx] + c2;
        int z = x + y;
        /*
         * For non-linear operator, change back to floating-point
         * */
        int temp_z = (z >> Q)%100;
        temp_z = temp_z << Q;
        /*
         * When the range analyzer find the range of result might need 32 bits,
         * it changes to upper precision
         * */
        result[idx] = (resultround * temp_z + K) >> Q;
    }
}

#define NS_PER_SECOND 1000000000

void sub_timespec(struct timespec t1, struct timespec t2, struct timespec *td)
{
    td->tv_nsec = t2.tv_nsec - t1.tv_nsec;
    td->tv_sec  = t2.tv_sec - t1.tv_sec;
    if (td->tv_sec > 0 && td->tv_nsec < 0)
    {
        td->tv_nsec += NS_PER_SECOND;
        td->tv_sec--;
    }
    else if (td->tv_sec < 0 && td->tv_nsec > 0)
    {
        td->tv_nsec -= NS_PER_SECOND;
        td->tv_sec++;
    }
}

double leftOps[iteration_num][array_size];
double rightOps[iteration_num][array_size];
int fixed_leftOps[iteration_num][array_size];
int fixed_rightOps[iteration_num][array_size];

int main(int argc, char** argv) {
    for (int i = 0; i < iteration_num; i++) {
        for (size_t idx = 0; idx < array_size; idx++) {
            leftOps[i][idx] = randomDouble(0, 63);
            rightOps[i][idx] = randomDouble(0, 63);
#if (defined(TEST_TIME_CONSUMPTION) && defined(BENCHMARK_SUITE_FIXEDPOINT)) || defined(TEST_PRECISION)
            fixed_leftOps[i][idx] = (int) (leftOps[i][idx] * (1 << Q) + 0.5);
            fixed_rightOps[i][idx] = (int) (rightOps[i][idx] * (1 << Q) + 0.5);
#endif
        }
    }
#if defined(TEST_PRECISION)
    double sf_error = 0, qr_error = 0, fp_error = 0;
#endif

#if defined(TEST_TIME_CONSUMPTION)
    struct timespec start_time;
    clock_gettime(CLOCK_REALTIME, &(start_time));
#endif

    for (int i = 0; i < iteration_num; i++) {
#if (defined(TEST_TIME_CONSUMPTION) && defined(BENCHMARK_SUITE_DOUBLE)) || defined(TEST_PRECISION)
        double db_result[array_size];
        double_add_test(leftOps[i], rightOps[i], db_result);
#endif
#if (defined(TEST_TIME_CONSUMPTION) && defined(BENCHMARK_SUITE_SOFT_FLOAT)) || defined(TEST_PRECISION)
        double sf_result[array_size];
        soft_float_add_test(leftOps[i], rightOps[i], sf_result);
#endif
#if (defined(TEST_TIME_CONSUMPTION) && defined(BENCHMARK_SUITE_ROUND)) || defined(TEST_PRECISION)
        double qr_result[array_size];
        round_add_test(leftOps[i], rightOps[i], qr_result);
#endif
#if (defined(TEST_TIME_CONSUMPTION) && defined(BENCHMARK_SUITE_FIXEDPOINT)) || defined(TEST_PRECISION)
        int fixed_fp_result[array_size];
        fixed_point_add_test_simplified(fixed_leftOps[i], fixed_rightOps[i], fixed_fp_result);
#endif

#if defined(TEST_PRECISION)
        double fp_result[array_size];
        for (size_t idx = 0; idx < array_size; idx++) {
            sf_error += db_result[idx] == 0 ? sf_result[idx] : fabs(sf_result[idx] - db_result[idx]) / db_result[idx];
            qr_error += db_result[idx] == 0 ? qr_result[idx] : fabs(qr_result[idx] - db_result[idx]) / db_result[idx];

            fp_result[idx] = (double)fixed_fp_result[idx] / (1<<Q);
            fp_error += db_result[idx] == 0 ? fp_result[idx] : fabs(fp_result[idx] - db_result[idx]) / db_result[idx];
        }
        sf_error /= array_size;
        qr_error /= array_size;
        fp_error /= array_size;
#endif
    }

#if defined(TEST_TIME_CONSUMPTION)
    struct timespec end_time, time_consumption;
    clock_gettime(CLOCK_REALTIME, &(end_time));
    sub_timespec(start_time, end_time, &time_consumption);
    printf("%d.%.9ld\n", (int)time_consumption.tv_sec, time_consumption.tv_nsec);
#endif

#if defined(TEST_PRECISION)
    sf_error = sf_error * 100 / iteration_num;
    qr_error = qr_error * 100 / iteration_num;
    fp_error = fp_error * 100 / iteration_num;
    printf("accumulation error: soft-float: %f%\tround-resolution: %f%\t"
           "fixed-point: %f%\n", sf_error, qr_error, fp_error);
#endif

    return 0;
}
