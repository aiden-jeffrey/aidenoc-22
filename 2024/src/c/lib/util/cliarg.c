#include <lib/util/cliarg.h>
#include <lib/util/filepath.h>

#include <string.h>
#include <stdio.h>
#include <errno.h>



static bool cliarg__flag_desc_empty(cliarg_flag_desc_t flag)
{
	return ((flag.flag == '\0')
		&& (flag.flag_long == NULL));
}

static bool cliarg__arg_desc_empty(cliarg_arg_desc_t arg)
{
	return ((arg.type == CLIARG_ARG_TYPE_NONE)
		|| !arg.name);
}

static int cliarg__print_arg(
	cliarg_arg_desc_t arg)
{
	int ret = printf(" ");

	if (arg.min == 0)
		ret += printf("[");

	ret += printf("%s", arg.name);

	if (arg.min > 1)
	{
		unsigned i;
		for (i = 1; i < arg.max; i++)
			ret += printf(" %s", arg.name);
	}

	if (((arg.max > arg.min) && (arg.max > 1))
		|| (arg.max == 0))
		ret += printf("..");

	if (arg.min == 0)
		ret += printf("]");

	return ret;
}


void cliarg_print_usage(
	const char* app,
	const cliarg_flag_desc_t* flags,
	const cliarg_arg_desc_t*  args)
{
	if (!app)
		return;

	char* app_name = filepath_file(app);
	if (!app_name) app_name = (char*)app;

	printf("Usage: %s", app_name);

	bool has_flags = (flags
		&& !cliarg__flag_desc_empty(flags[0]));
	if (has_flags) printf(" [options]");

	if (args)
	{
		unsigned i;
		for (i = 0; !cliarg__arg_desc_empty(args[i]); i++)
		{
			cliarg__print_arg(args[i]);
		}
	}

	printf("\n");

	if (has_flags)
	{
		printf("Options:\n");

		unsigned i;
		for (i = 0; !cliarg__flag_desc_empty(flags[i]); i++)
		{
			int col = printf("  ");

			if (flags[i].flag != '\0')
				col += printf("-%c", flags[i].flag);
			else
				col += printf("  ");

			if (flags[i].flag_long)
				col += printf(" --%s", flags[i].flag_long);

			if (flags[i].param_type != CLIARG_ARG_TYPE_NONE)
				col += printf(" <%s>", flags[i].param_name);

			if (flags[i].desc)
			{
				if (col >= 32)
				{
					printf("\n");
					col = 0;
				}

				while (col < 32)
					col += printf(" ");

				printf("%s", flags[i].desc);
			}

			printf("\n");
		}
	}

	if (app_name != app)
		free(app_name);
}

static bool cliarg__parse_callback(
	cliarg_arg_type_e type,
	const char* value,
	bool (*callback)(void*))
{
	if (!callback)
		return false;

	uint64_t number;
	switch (type)
	{
		case CLIARG_ARG_TYPE_NONE:
		case CLIARG_ARG_TYPE_STRING:
			break;

		case CLIARG_ARG_TYPE_NUMBER:
		{
			char* endptr = NULL;
			int errno_stack = errno;
			errno = 0;
			number = strtoull(value, &endptr, 0);
			bool success = (errno == 0);
			errno = errno_stack;
			if (!success || !endptr
				|| (endptr[0] != '\0'))
			{
				fprintf(stderr,
					"Error: Failed to parse numeric argument '%s'\n", value);
				return false;
			}
		} break;

		default:
			break;
	}

	switch (type)
	{
		case CLIARG_ARG_TYPE_NONE:
			return callback(NULL);

		case CLIARG_ARG_TYPE_STRING:
			return callback((void*)value);

		case CLIARG_ARG_TYPE_NUMBER:
			return callback((void*)&number);

		default:
			break;
	}

	return false;
}


bool cliarg_parse(
	int argc, const char* argv[],
	const cliarg_flag_desc_t* flags,
	const cliarg_arg_desc_t*  args)
{
	if ((argc < 1) || !argv)
		return false;

	unsigned    nargc = 0;
	const char* nargv[argc];

	unsigned fc = 0;
	const cliarg_flag_desc_t* fd[argc];
	const char* fa[argc];

	unsigned i = 1;
	while (i < (unsigned)argc)
	{
		if (flags && (argv[i][0] == '-')
			&& (argv[i][1] != '\0'))
		{
			const char* flag_name = &argv[i++][1];
			unsigned    flag_len;
			const cliarg_flag_desc_t* flag = NULL;
			const char* flag_arg = NULL;

			if ((flag_name[1] == '\0')
				|| (flag_name[1] == '='))
			{
				flag_len = 1;

				unsigned f;
				for (f = 0; !cliarg__flag_desc_empty(flags[f]); f++)
				{
					if (flags[f].flag != flag_name[0])
						continue;
					if (flag_name[1] == '=')
						flag_arg = &flag_name[2];
					flag = &flags[f];
					break;
				}
			}
			else
			{
				if (flag_name[0] == '-')
					flag_name++;
				for (flag_len = 0; (flag_name[flag_len] != '=')
					&& (flag_name[flag_len] != '\0'); flag_len++);

				unsigned f;
				for (f = 0; !cliarg__flag_desc_empty(flags[f]); f++)
				{
					if (strncmp(flag_name, flags[f].flag_long, flag_len) != 0)
						continue;
					if (flag_name[flag_len] == '=')
						flag_arg = &flag_name[flag_len];
					else if (flag_name[flag_len] != '\0')
						continue;
					flag = &flags[f];
					break;
				}
			}

			if (!flag)
			{
				fprintf(stderr,
					"Error: Unrecognized flag '%.*s'\n",
					flag_len, flag_name);
				return false;
			}

			if (flag->exclusive)
			{
				unsigned j;
				for (j = 0; j < fc; j++)
				{
					if (fd[j] == flag)
					{
						fprintf(stderr,
							"Error: Duplicate flag '%.*s'\n",
							flag_len, flag_name);
						return false;
					}
				}
			}

			fd[fc] = flag;
			fa[fc] = NULL;
			if (flag->param_type != CLIARG_ARG_TYPE_NONE)
			{
				if (!flag_arg)
				{
					if (i >= (unsigned)argc)
					{
						fprintf(stderr,
							"Error: Expected parameter after flag '%.*s'\n",
							flag_len, flag_name);
						return false;
					}
					flag_arg = argv[i++];
				}
				fa[fc] = flag_arg;
			}
			else if (flag_arg)
			{
				fprintf(stderr,
					"Error: Unexpected parameter in flag '%.*s'\n",
					flag_len, flag_name);
				return false;
			}
			fc++;
		}
		else if (!args)
		{
			fprintf(stderr,
				"Error: Unexpected argument '%s'\n", argv[i]);
			return false;
		}
		else
		{
			nargv[nargc++] = argv[i++];
		}
	}

	for (i = 0; i < fc; i++)
	{
		if (!cliarg__parse_callback(
			fd[i]->param_type, fa[i],
			fd[i]->callback))
			return false;
	}

	unsigned min = 0;
	for (i = 0; !cliarg__arg_desc_empty(args[i]); i++)
		min += args[i].min;

	if (nargc < min)
	{
		fprintf(stderr,
			"Error: Not enough arguments\n");
		return false;
	}

	unsigned variable = (nargc - min);
	unsigned a = 0;
	for (i = 0; !cliarg__arg_desc_empty(args[i]); i++)
	{
		if (!args[i].callback)
			return NULL;

		unsigned count = args[i].min;
		if ((args[i].max == 0)
			|| (args[i].max > (args[i].min + variable)))
		{
			count += variable;
			variable = 0;
		}
		else
		{
			unsigned d = (args[i].max - args[i].min);
			count    += d;
			variable -= d;
		}

		unsigned k;
		for (k = 0; k < count; k++, a++)
		{
			if (!cliarg__parse_callback(
				args[i].type, nargv[a],
				args[i].callback))
				return false;
		}
	}

	if (a < nargc)
	{
		fprintf(stderr,
			"Error: Too many arguments\n");
		return false;
	}

	return true;
}
