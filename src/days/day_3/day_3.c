#include "lib/bind/python.h"

#include "days.h"

bool day_3(unsigned part, const char *input_filename)
{
	return run_python(
		"src/days/day_3/",
		"day_3",
		"day_3",
		part,
		input_filename);
}
