#include "lib/bind/python.h"

#include "days.h"

bool day_4(unsigned part, const char *input_filename)
{
	return run_python(
		"src/days/day_4/",
		"day_4",
		"day_4",
		part,
		input_filename);
}
