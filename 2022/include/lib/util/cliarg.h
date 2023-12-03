#ifndef __CLIARG_H__
#define __CLIARG_H__

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum
{
	CLIARG_ARG_TYPE_NONE = 0,
	CLIARG_ARG_TYPE_STRING,
	CLIARG_ARG_TYPE_NUMBER,
} cliarg_arg_type_e;

typedef struct
{
	cliarg_arg_type_e type;
	const char*       name;
	unsigned          min, max;
	bool            (*callback)(void*);
} cliarg_arg_desc_t;

#define CLIARG_ARG_DESC_EMPTY \
	{ CLIARG_ARG_TYPE_NONE, NULL, 0, 0, NULL }

typedef struct
{
	char              flag;
	const char*       flag_long;
	cliarg_arg_type_e param_type;
	const char*       param_name;
	const char*       desc;
	bool              exclusive;
	bool            (*callback)(void*);
} cliarg_flag_desc_t;

#define CLIARG_FLAG_DESC_EMPTY \
	{ '\0', NULL, CLIARG_ARG_TYPE_NONE, NULL, NULL, false, NULL }


void cliarg_print_usage(
	const char* app,
	const cliarg_flag_desc_t* flags,
	const cliarg_arg_desc_t*  args);

bool cliarg_parse(
	int argc, const char* argv[],
	const cliarg_flag_desc_t* flags,
	const cliarg_arg_desc_t*  args);

#endif