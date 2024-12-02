#include <stdlib.h>
#include <stdio.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "lib/type/list.h"
#include "lib/util/parse.h"

#include "days.h"

#define LISTS_MAX 32

static bool less_than_int(const void *a, const void *b);

static bool part_1(list_t **lists, uint32_t list_cnt);
static bool part_2(list_t **lists, uint32_t list_cnt);

bool day_1(unsigned part, const char *input_filename)
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

	uint32_t list_cnt = 0;
	list_t *lists[LISTS_MAX] = {0};
	uint32_t list_i   = 0;
	bool     success  = true;

	const char *string = buff;
	while (*string != '\0' && success)
	{
		int line_item;

		if (!parse_int(&string, &line_item))
		{
			fprintf(stderr, "can't parse int %c\n", *string);
			success = false;
			break;
		}

		if (list_cnt <= list_i)
		{
			lists[list_i] = list_create(NULL, NULL);
			if (!lists[list_i])
			{
				fprintf(stderr, "bad list create\n");
				success = false;
			}
			else
			{
				list_cnt++;
			}
		}

		int *element = malloc(sizeof(int));
		*element = line_item;
		if (!list_append(lists[list_i], element))
		{
			fprintf(stderr, "bad list append %p %u %u\n", lists[list_i], list_cnt, list_i);
			success = false;
		}

		if (*string == '\n')
		{
			list_i = 0;
			string++;
		}
		if (*string == ' ')
		{
			list_i++;
		}

		while (*string == ' ') string++;
	}

	fprintf(stdout, "made %u lists\n", list_cnt);
	close(fd);

	return success
		? (part == 0)
			? part_1(lists, list_cnt)
			: part_2(lists, list_cnt)
		: false;
}

static bool part_1(list_t **lists, uint32_t list_cnt)
{
	for (uint32_t i = 0; i < list_cnt; i++) list_sort(lists[i], less_than_int);

	bool success = true;
	int total = 0;
	if (list_cnt >= 2)
	{
		for (uint i = 0; i < list_length(lists[0]); i++)
		{
			int *elem_0 = list_get(lists[0], i);
			int *elem_1 = list_get(lists[1], i);

			if (!elem_0 || !elem_1)
			{
				fprintf(stderr, "bad element\n");
				success = false;
				break;
			}

			int dist = abs(*elem_1 - *elem_0);
			total += dist;
		}
	}
	fprintf(stdout, "diff total %u\n", total);

	return success;
}

static bool part_2(list_t **lists, uint32_t list_cnt)
{
	bool success = true;
	int total = 0;
	if (list_cnt >= 2)
	{
		for (uint i = 0; (i < list_length(lists[0])) && success; i++)
		{
			int *elem_0 = list_get(lists[0], i);
			uint elem_cnt = 0;

			for (uint j = 0; j < list_length(lists[1]); j++)
			{
				int *elem_1 = list_get(lists[1], j);
				if (!elem_0 || !elem_1)
				{
					fprintf(stderr, "bad element\n");
					success = false;
					break;
				}

				if (*elem_0 == *elem_1) elem_cnt++;
			}

			int score = elem_cnt * (*elem_0);

			total += score;
		}
	}
	else
	{
		success = false;
	}

	fprintf(stdout, "total %u\n", total);

	return success;
}

bool less_than_int(const void *a, const void *b)
{
	return *((int*)a) < *((int*)b);
}