#include "fixed_point_operator/fixed_point_operator.h"

int main() {
    // arithmetic of 1.505016722 and 1.59765625
    double a = 1.5;
    double b = 1.6;
    auto a_quant = (int16_t)(a * (1 << Q));
    auto b_quant = (int16_t)(b * (1 << Q));
    int16_t res = q_add(a_quant, b_quant);
    printf("add result: %f\n", (float)res/(1<<Q));
    res = q_sub(a_quant, b_quant);
    printf("sub result: %f\n", (float)res/(1<<Q));
    res = q_mul(a_quant, b_quant);
    printf("mul result: %f\n", (float)res/(1<<Q));
    res = q_div(a_quant, b_quant);
    printf("div result: %f\n", (float)res/(1<<Q));
    return 0;
}
