#include "lib/util/math.h"

int int_pow(int base, int exp)
{
    int result = 1;
    for (;;)
    {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        if (!exp)
            break;
        base *= base;
    }

    return result;
}

int mod(int a, int b)
{
	int result = a % b;
	if (result < 0) result += b;

	return result;
}
