#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/type/list.h"
#include "lib/type/map.h"
#include "lib/util/cliarg.h"
#include "lib/util/filepath.h"

#include "days.h"

typedef bool (*day_cb)(unsigned, const char *);

static const day_cb days[] =
{
	day_1,
	day_2,
};

static struct
{
	const char   *name;
	const char   *input_filename;

	const day_cb *days;
	unsigned      num_days;

	bool          success;
} globals = {
	.name           = NULL,
	.input_filename = NULL,
	.days           = days,
	.num_days       = sizeof(days) / sizeof(days[0]),
	.success        = false,
};

static bool run_day(const uint64_t *day_num)
{
	if (!day_num) return false;

	const day_cb day = (*day_num - 1 < globals.num_days)
		? globals.days[*day_num - 1]
		: NULL;
	if (!day)
	{
		const char *warning = (*day_num == 0)
			? "Error: there's no %luth of December silly!\n"
			: (*day_num > 25)
				? "Error: christmas overflow\n"
				: "Error: day %lu isn't done yet!\n";
		fprintf(stderr, warning, *day_num);
		globals.success = false;

		fprintf(stderr, "num days: %u\n", globals.num_days);
	}
	else
	{
		globals.success = day(0, globals.input_filename) && day(1, globals.input_filename);
	}

	return true;
}

static const cliarg_arg_desc_t args[] =
{
	{ CLIARG_ARG_TYPE_NUMBER, "day", 1, 1, (void *)run_day },
	CLIARG_ARG_DESC_EMPTY
};

static __attribute__ ((noreturn)) bool flag_help(const void *param);
static bool flag_input(const void *param);

static const cliarg_flag_desc_t flags[] =
{
	{ 'h', "help" , CLIARG_ARG_TYPE_NONE, NULL, "Print usage" , true, (void *)flag_help },
	{ 'i', "input" , CLIARG_ARG_TYPE_STRING, NULL, "Input file" , true, (void *)flag_input },
	CLIARG_FLAG_DESC_EMPTY
};

static __attribute__ ((noreturn)) bool flag_help(const void *param)
{
	(void)param;

	printf("Aiden Jeffrey AOC 2022\n");
	cliarg_print_usage(globals.name, flags, args);
	exit(EXIT_SUCCESS);
}

static bool flag_input(const void *param)
{
	globals.input_filename = param;
	return true;
}

int main(int argc, char *argv[])
{
	globals.name = (const char*)argv[0];

	if ((argc < 2) || !cliarg_parse(argc, (const char**)argv, flags, args))
	{
		cliarg_print_usage(globals.name, flags, args);
		return EXIT_FAILURE;
	}

	return globals.success ? EXIT_SUCCESS : EXIT_FAILURE;
}
