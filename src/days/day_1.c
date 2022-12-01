#include <stdio.h>

#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "lib/util/parse.h"

#include "days.h"

#define ELVES_MAX 1024

static bool part_1(const unsigned *elves, size_t elf_cnt);
static bool part_2(unsigned *elves, size_t elf_cnt);

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

	unsigned elves[ELVES_MAX];

	bool success = true;
	const char *string = buff;
	size_t elf_cnt = 0;
	size_t line_cnt = 0;
	for (; *string != '\0'; elf_cnt++, string++)
	{
		if (elf_cnt >= ELVES_MAX)
		{
			fprintf(stderr, "Error: too many elves!\n");
			success = false;
			break;
		}

		double line_item;
		unsigned total = 0;

		for (; parse_number(&string, &line_item); total += line_item, line_cnt++)
		{
			if (*string == '\n')
			{
				string++;
				line_cnt++;
			}
			else if (*string != '\0')
			{
				fprintf(stderr, "Error: unexpected token %c at line %lu\n", *string, line_cnt);
				success = false;
				break;
			}
		}
		elves[elf_cnt] = total;
	}

	close(fd);

	return success
		? (part == 0)
			? part_1(elves, elf_cnt)
			: part_2(elves, elf_cnt)
		: false;
}

static bool part_1(const unsigned *elves, size_t elf_cnt)
{
	if (!elves || (elf_cnt == 0)) return false;

	unsigned max_total = 0;
	for (size_t i = 0; i < elf_cnt; i++)
	{
		unsigned elf_total = elves[i];
		if (elf_total > max_total) max_total = elf_total;
	}

	fprintf(stdout, "max calories is %u\n", max_total);

	return true;
}

static bool part_2(unsigned *elves, size_t elf_cnt)
{
	size_t top_cnt = 3;
	if (!elves || (elf_cnt < top_cnt)) return false;

	// sort top_cnt
	for (size_t step = 0; step < top_cnt; step++)
	{
		size_t curr = step;
		for (size_t i = step + 1; i < elf_cnt; i++)
		{
			if (elves[i] > elves[curr])
				curr = i;
		}

		unsigned temp = elves[curr];
		elves[curr] = elves[step];
		elves[step] = temp;
	}

	// sum top_cnt
	unsigned total = 0;
	for (size_t i = 0; i < top_cnt; i++)
		total += elves[i];

	fprintf(stdout, "top %lu elves have %u\n", top_cnt, total);

	return true;
}
