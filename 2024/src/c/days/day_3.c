#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "lib/util/parse.h"

#include "days.h"

typedef enum {
	MUL   = 0,
	DO    = 1,
	DONT  = 2,
	COUNT,
} instr_e;

static const char *instr_strings[] =
{
	"mul(",
	"do(",
	"don't(",
};

static bool parse(const char *string, instr_e max_oper);

static bool part_1(const char *string);
static bool part_2(const char *string);

bool day_3(unsigned part, const char *input_filename)
{
	if (!input_filename)
	{
		fprintf(stderr, "Error: no input file supplied\n");
		return false;
	}
	fprintf(stdout, "Running part %u with %s...\n", part, input_filename);

	// read file
	int fd = open(input_filename, O_RDONLY);
	if (fd < 0) return false;

	struct stat st;
	if (fstat(fd, &st) != 0)
		return false;

	char buff[st.st_size + 1];
	if (read(fd, buff, st.st_size) != st.st_size)
		return false;
	buff[st.st_size] = '\0';

	return (part == 0)
		? part_1(buff)
		: part_2(buff);
}

static bool parse(const char *string, instr_e max_oper)
{
	bool    success     = true;
	instr_e current     = COUNT;
	size_t  instr_str_i = 0;
	int     total       = 0;
	bool    enabled     = true;

	for (; *string != '\0' && success; string++)
	{
		char ch = *string;
		if (current == COUNT)
		{
			for (size_t i = 0; i <= max_oper; i++)
			{
				char start = instr_strings[i][0];
				if (start == ch)
				{
					current = i;
					instr_str_i++;
					break;
				}
			}
		}
		else
		{
			if (instr_str_i == 0)
			{
				fprintf(stderr, "bad index\n");
				return false;
			}

			char comp = instr_strings[current][instr_str_i];
			if (comp == '\0')
			{
				switch (current)
				{
					case MUL:
					{
						int lh = 0;
						if (parse_int(&string, &lh))
						{
							if (*string == ',')
							{
								string++;
								int rh = 0;
								if (parse_int(&string, &rh))
								{
									if (*string == ')')
									{
										int result = lh * rh;
										if (enabled) total += result;
									}
								}
							}
						}
						break;
					}
					case DO:
					case DONT:
					{
						if (ch == ')') enabled = current == DO;
						break;
					}
					default:
					{
						fprintf(stderr, "bad operation %u\n", current);
						break;
					}
				}
			}

			if (comp == ch)
			{
				instr_str_i++;
			}
			else if (current == DO && ch == instr_strings[DONT][instr_str_i])
			{
				// hack
				instr_str_i++;
				current = DONT;
			}
			else
			{
				instr_str_i = 0;
				current = COUNT;
			}
		}
	}

	fprintf(stdout, "total: %i\n", total);
	return success;
}

static bool part_1(const char *string)
{
	return parse(string, MUL);
}

static bool part_2(const char *string)
{
	return parse(string, DONT);
}
