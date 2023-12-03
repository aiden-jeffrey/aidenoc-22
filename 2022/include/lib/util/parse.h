#ifndef __PARSE_H__
#define __PARSE_H__

#include <stdbool.h>

bool parse_number(const char **string, double *out);
bool parse_pass_char(const char **string, unsigned *cnt, char ch);

#endif
