#include <stdio.h>

#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "lib/util/math.h"

#include "days.h"

#define ROUND_MAX 4096

typedef enum {
	ROCK     = 0,
	PAPER    = 1,
	SCISSORS = 2,
	COUNT,
} shape_e;

static const char *shape_strings[] =
{
	"ROCK",
	"PAPER",
	"SCISSORS",
};

static bool sum(const unsigned *round, size_t round_cnt);
static unsigned score_round(shape_e other, shape_e yours);

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

	unsigned round[ROUND_MAX] = {0};

	bool success = true;
	size_t round_cnt = 0;
	size_t line_cnt = 0;

	shape_e other = COUNT;
	shape_e yours = COUNT;

	for (const char *string = buff; success; string++)
	{
		if (round_cnt >= ROUND_MAX)
		{
			fprintf(stderr, "Error: too many rounds!\n");
			success = false;
			break;
		}

		switch (*string)
		{
			case ('A'):
			case ('B'):
			case ('C'):
				other = *string - 'A';
				break;

			case ('X'):
			case ('Y'):
			case ('Z'):
				if (other == COUNT)
				{
					fprintf(stderr, "Error: missing other\n");
					success = false;
					break;
				}
				if (part == 0)
				{
					yours = *string - 'X';
				}
				else
				{
					// delta = 0 (draw) 1 (win) -1 (loss)
					int delta = ((int)(*string - 'X') - 1);
					yours = mod((int)other + delta, COUNT);
				}
				break;

			case ('\n'):
			case ('\0'):
				line_cnt++;
				if ((other == COUNT) || (yours == COUNT))
				{
					fprintf(stderr, "malformed line at %lu\n", line_cnt);
					success = false;
				}
				else
				{
					unsigned score = score_round(other, yours);
					fprintf(stdout, "score (%lu): %u\n", round_cnt, score);
					round[round_cnt++] = score;
				}
				other = COUNT;
				yours = COUNT;
				break;

			case (' '):
				break;

			default:
				fprintf(stderr, "unexpected char %c on line %lu\n", *string, line_cnt);
				break;;
		}

		if (*string == '\0') break;
	}

	close(fd);

	return success && sum(round, round_cnt);
}

static bool sum(const unsigned *round, size_t round_cnt)
{
	if (!round || (round_cnt == 0)) return false;

	unsigned total = 0;
	for (size_t i = 0; i < round_cnt; i++)
	{
		total += round[i];
	}

	fprintf(stdout, "total score is %u\n", total);

	return true;
}

static unsigned score_round(shape_e other, shape_e yours)
{
	unsigned score = yours + 1;

	int result = mod((int)other - (int)yours, COUNT);

	printf("%s v %s ", shape_strings[other], shape_strings[yours]);

	switch (result)
	{
		case (0):
			printf("draw ");
			score += 3;
			break;

		case (1):
			printf("loss ");
			break;

		case (2):
			printf("win ");
			score += 6;
			break;

		default:
			fprintf(stderr, "Error: bad char\n");
			break;
	}

	return score;
}
