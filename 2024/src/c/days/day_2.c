#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "lib/type/list.h"
#include "lib/util/parse.h"

#include "days.h"

#define REPORTS_MAX 1024

bool safe_skipped(const list_t *report);
bool safe        (const list_t *report, bool skip, uint32_t skip_i);

static bool part_1(list_t **lists, uint32_t list_cnt);
static bool part_2(list_t **lists, uint32_t list_cnt);

bool day_2(unsigned part, const char *input_filename)
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

	list_t  *reports[REPORTS_MAX] = {0};
	uint32_t report_i             = 0;
	bool     success              = true;

	const char *string = buff;
	while (*string != '\0' && success)
	{
		if (report_i + 1 > REPORTS_MAX)
		{
			fprintf(stderr, "too many reports\n");
			return false;
		}

		if (!reports[report_i])
		{
			reports[report_i] = list_create(NULL, NULL);
			if (!reports[report_i])
			{
				fprintf(stderr, "bad list create\n");
				success = false;
			}
		}

		int line_item;
		if (!parse_int(&string, &line_item))
		{
			fprintf(stderr, "can't parse int %c\n", *string);
			success = false;
			break;
		}

		int *element = malloc(sizeof(int));
		*element = line_item;

		if (!list_append(reports[report_i], element))
		{
			fprintf(stderr, "bad list append %p %u\n", reports[report_i], report_i);
			success = false;
		}

		if (*string == '\n')
		{
			report_i++;
			string++;
		}

		while (*string == ' ') string++;
	}

	fprintf(stdout, "made %u reports\n", report_i + 1);
	close(fd);

	return success
		? (part == 0)
			? part_1(reports, report_i + 1)
			: part_2(reports, report_i + 1)
		: false;
}

static bool part_1(list_t **reports, uint32_t list_cnt)
{
	bool success  = true;
	uint num_safe = 0;

	for (uint i = 0; i < list_cnt; i++)
	{
		if (!reports[i])
		{
			fprintf(stderr, "missing report\n");
			return false;
		}

		if (safe(reports[i], 0, false)) num_safe++;

	}
	fprintf(stdout, "num_safe %u\n", num_safe);

	return success;
}

static bool part_2(list_t **reports, uint32_t list_cnt)
{
	bool success    = true;
	uint num_safe   = 0;
	uint num_unsafe = 0;

	for (uint i = 0; i < list_cnt; i++)
	{
		if (!reports[i])
		{
			fprintf(stderr, "missing report\n");
			return false;
		}

		if (safe_skipped(reports[i])) num_safe++;
		else num_unsafe++;
	}
	fprintf(stdout, "num_safe %u\n", num_safe);
	fprintf(stdout, "num_unsafe %u\n", num_unsafe);

	return success;
}

bool safe_skipped(const list_t *report)
{
	if (safe(report, false, 0))
	{
		return true;
	}

	for (uint i = 0; i < list_length(report); i++)
	{
		if (safe(report, true, i))
		{
			return true;
		}
	}
	return false;
}

bool safe(const list_t *report, bool skip, uint32_t skip_i)
{
	if (!report) return false;

	int  prev_sign = 0;
	int  prev      = 0;
	bool set_prev  = false;


	for (uint32_t i = 0; i < list_length(report); i++)
	{
		if (skip && (skip_i == i)) continue;

		int *current = list_get(report, i);
		if (!current)
		{
			fprintf(stderr, "bad element");
			return false;
		}

		if (set_prev)
		{
			int delta = *current - prev;
			int abs_delta = abs(delta);
			if (abs_delta < 1 || abs_delta > 3) return false;

			int sign = delta < 0 ? -1 : 1;
			if (prev_sign != 0 && sign != prev_sign) return false;

			prev_sign = sign;
		}

		prev = *current;
		set_prev = true;
	}

	return true;
}
