#include "utilities.h"

// calculate binomial coefficient
double util::binomial (const unsigned &n, const unsigned &k)
{
	double result = 1;
	unsigned int tmp = k;

	if ( tmp > n - tmp )
		tmp = n - tmp;

	for (unsigned i = 0; i < tmp; ++i)
	{
		result *= (n - i);
		result /= (i + 1);
	}

	return result;
}

// calculate factorial recursively
double util::factorial (unsigned n)
{
	return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}
