//
// Created by pei on 01/12/22.
//
// https://stackoverflow.com/questions/12697523/performing-bit-division-without-arithmetic-operators/12699549#12699549

#include "bit_operator.h"

/** This function adds the two arguments using bitwise operators. Your
* implementation should not use arithmetic operators except for loop
* control. Integers are 32 bits long.  This function prints a message
* saying "Overflow occurred\n" if a two's complement overflow occurs
* during the addition process. The sum is returned as the value of
* the function.
*/
int bit_add(int x,int y){

    int i;

    char sum;
    char car_in=0;
    char car_out;
    char a,b;

    unsigned int mask=0x00000001;
    int result=0;

    for(i=0;i<32;i++){

        a=(x&mask)!=0;
        b=(y&mask)!=0;
        car_out=(car_in & (a|b)) |(a&b);
        sum=a^b^car_in;

        if(sum) {
            result|=mask;
        }

        if(i!=31) {
            car_in=car_out;
        } else {
            if(car_in!=car_out) {
                printf("Overflow occurred\n");
            }
        }

        mask<<=1;
    }

    return result;
}

// subtracts two integers by finding the complement
// of "y", adding 1, and using the bit_add() function
// to add "-y" and "x"
int bit_sub(int x, int y){

    return bit_add(x, bit_add(~y, 1));
}

unsigned int bit_mul(unsigned int x, unsigned int y)
{
    int total = 0;
    int i;

    /* if the i-th bit is non-zero, add 'x' to total */
    /* Multiple total by 2 each step */
    for(i = 32 ; i >= 0 ; i--)
    {
        total <<= 1;
        if( (y & (1 << i)) >> i )
        {
            total = bit_add(total, x);
        }
    }

    return total;
}

unsigned int bit_div(unsigned int dividend, unsigned int divisor)
{
    int i, quotient = 0, remainder = 0;

    if(divisor == 0) { printf("div by zero\n"); return 0; }

    for(i = 31 ; i >= 0 ; i--)
    {
        quotient <<= 1;
        remainder <<= 1;
        remainder |= (dividend & (1 << i)) >> i;

        if(remainder >= divisor)
        {
            remainder = bit_sub(remainder, divisor);
            quotient |= 1;
        }
    }

    return quotient;
}
