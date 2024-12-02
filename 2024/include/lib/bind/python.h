#ifndef __BIND_PYTHON_H__
#define __BIND_PYTHON_H__

#include <stdbool.h>
#include <stdlib.h>

bool run_python(
	const char *directory,
	const char *python_filename,
	const char *fname,
	unsigned    part,
	const char *input_filename);

#endif
