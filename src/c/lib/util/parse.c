#include "lib/util/math.h"
#include "lib/util/parse.h"

bool parse_number(const char **string, double *out)
{
	if (!string || !*string || !out)
		return false;

	const char *tstr = *string;
	int sign = 1;
	if ((*tstr < '0') || (*tstr > '9'))
	{
		if (*tstr == '-')
		{
			sign = -1;
			tstr++;
		}
		else
			return false;
	}

	bool seen_dot = false;
	while (true)
	{
		double   temp = 0;
		unsigned cnt  = 0;

		for (; ((*tstr >= '0') && (*tstr <= '9')); tstr++, cnt++)
			temp = (temp * 10) + (*tstr - '0');

		if (!seen_dot)
			*out = sign * temp;
		else
			*out += (sign * temp) / int_pow(10, cnt);

		if (*tstr == '.')
		{
			seen_dot = true;
			tstr++;
		}
		else
		{
			break;
		}
	}

	*string = tstr;
	return true;
}

unsigned parse_count_char(const char **string, char ch)
{
	if (!string || !*string)
		return 0;

	unsigned cnt = 0;
	for (; **string == ch; cnt++, (*string)++);

	return cnt;
}
