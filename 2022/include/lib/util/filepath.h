#ifndef __PATH_H__
#define __PATH_H__

#include <stdbool.h>

#include "lib/type/list.h"

char *filepath_optimize(const char *path);
char *filepath_resolve(const char *path);
char *filepath_relative(const char *base, const char *path);
char *filepath_append  (const char *base, const char *tail);

char *filepath_dir(const char *path);
char *filepath_file(const char *path);
char *filepath_file_name(const char *path);
char *filepath_file_ext(const char *path);

bool  filepath_file_is_dir  (const char *path);
bool  filepath_file_is_file (const char *path);
bool  filepath_file_has_ext (const char *path, const char *ext);
bool  filepath_file_endswith(const char *path, const char *end);

list_t *filepath_list_contents(const char *path, bool just_name);

char *filepath_from_name(
	const char *base,
	const char *class,
	const char *name,
	const char *ext);

#endif
