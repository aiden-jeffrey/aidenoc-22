#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "days.h"

#define MAX_DIM 256

static bool part_1(char grid[MAX_DIM][MAX_DIM], size_t width, size_t height);
static bool part_2(
	char grid   [MAX_DIM][MAX_DIM],
	char scratch[MAX_DIM][MAX_DIM],
	size_t width,
	size_t height);

bool day_4(unsigned part, const char *input_filename)
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

	char grid   [MAX_DIM][MAX_DIM] = { {'\0'} };
	char scratch[MAX_DIM][MAX_DIM] = { {'\0'} };
	size_t width  = 0;
	size_t height = 0;
	size_t row_i  = 0;
	size_t col_i  = 0;

	bool success = true;
	const char *string = buff;

	for (; *string != '\0' && success; string++)
	{
		char ch = *string;
		if (ch == '\n')
		{
			size_t n_width = col_i + 1;
			if (row_i > 0)
			{
				// check width same
				if (n_width > width || n_width > MAX_DIM)
				{
					fprintf(stderr, "bad width %lu %lu %lu\n", row_i, col_i, width);
					return false;
				}
			}
			width = n_width;

			row_i++;
			height++;
			col_i = 0;
		}
		else
		{
			grid[col_i][row_i] = ch;
			scratch[col_i][row_i] = '0';
			col_i++;
		}
	}
	height++;

	// for (uint y = 0; y < height; y++)
	// {
	// 	for (uint x = 0; x < width; x++)
	// 	{
	// 		fprintf(stdout, "%c", grid[x][y]);
	// 	}
	// 	fprintf(stdout, "\n");
	// }
	// fprintf(stdout, "\n");

	return (part == 0)
		? part_1(grid, width, height)
		: part_2(grid, scratch, width, height);
}

static int strcmp(const char *str, const char *find, int offset)
{
	const char *str_p  = str + offset;
	const char *find_p = find;

	// uint find_len = 0;
	// for (const char *fl = find; (*fl) != '\0'; fl++, find_len++);

	for (;; str_p++)
	{
		if ((*find_p) == '\0')
		{
			// found
			int ret = str_p - str;
			if (ret < 0) fprintf(stderr, "bad strcmp\n");
			return ret;
		}

		if ((*str_p) == (*find_p))
		{
			find_p++;
		}
		else if ((*str_p) == (*find))
		{
			find_p = find + 1;
		}
		else
		{
			find_p = find;
		}

		if (*str_p == '\0') break;
	}
	return -1;
}

static uint strcnt(const char *test, const char *find)
{
	uint cnt = 0;

	const char *str_p  = test;
	const char *find_p = find;

	for (;; str_p++)
	{
		if ((*find_p) == '\0')
		{
			// found
			cnt++;
		}

		if ((*str_p) == (*find_p))
		{
			find_p++;
		}
		else if ((*str_p) == (*find))
		{
			find_p = find + 1;
		}
		else
		{
			find_p = find;
		}

		if (*str_p == '\0') break;
	}

	return cnt;
}

static bool part_1(char grid[MAX_DIM][MAX_DIM], size_t width, size_t height)
{
	const char *xmas[] = {
		"XMAS",
		"SAMX",
	};

	char test_arr[MAX_DIM] = {'\0'};
	char *test = test_arr;

	uint count = 0;
	for (uint xmas_i = 0; xmas_i < 2; xmas_i++)
	{
		const char *to_find = xmas[xmas_i];
		for (uint y = 0; y < height; y++)
		{
			for (uint x = 0; x < width; x++)
			{
				*(test++) = grid[x][y];
			}
			*(test) = '\0';
			test = test_arr;

			uint c = strcnt(test, to_find);
			count += c;
		}

		for (uint x = 0; x < width; x++)
		{
			for (uint y = 0; y < height; y++)
			{
				*(test++) = grid[x][y];
			}

			*(test) = '\0';
			test = test_arr;

			uint c = strcnt(test, to_find);
			count += c;
		}

		for (
			int col = 0, row = height - 1;
			(row >= 0) && (col < (int)width);
		)
		{
			for (
				int x = col, y = row;
				(x < (int)width) && (y < (int)height);
				x++, y++
			)
			{
				*(test++) = grid[x][y];
			}

			*(test) = '\0';
			test = test_arr;
			uint c = strcnt(test, to_find);
			count += c;

			if (row > 0) row--;
			else col++;
		}

		for (
			int col = 0, row = 0;
			(col < (int)width) && (row < (int)height);
		)
		{
			for (
				int x = col, y = row;
				(x < (int)width) && (y >= 0);
				x++, y--
			)
			{
				*(test++) = grid[x][y];
			}

			*(test) = '\0';
			test = test_arr;
			uint c = strcnt(test, to_find);
			count += c;

			if (row < (int)height - 1) row++;
			else col++;
		}
	}

	fprintf(stdout, "count: %u\n", count);

	return true;
}

static bool part_2(
	char grid   [MAX_DIM][MAX_DIM],
	char scratch[MAX_DIM][MAX_DIM],
	size_t width,
	size_t height)
{
	uint count = 0;
	const char *mas[] = {
		"MAS",
		"SAM",
	};

	char test_arr[MAX_DIM] = {'\0'};
	char *test = test_arr;

	for (uint xmas_i = 0; xmas_i < 2; xmas_i++)
	{
		const char *to_find = mas[xmas_i];
		for (
			int col = 0, row = height - 1;
			(row >= 0) && (col < (int)width);
		)
		{
			for (
				int x = col, y = row;
				(x < (int)width) && (y < (int)height);
				x++, y++
			)
			{
				*(test++) = grid[x][y];
			}

			*(test) = '\0';
			test = test_arr;

			for (int i = strcmp(test, to_find, 0); i > 0; i = strcmp(test, to_find, i))
			{
				int c = i - 2;
				scratch[col + c][row + c]++;
			}

			if (row > 0) row--;
			else col++;
		}

		for (
			int col = 0, row = 0;
			(col < (int)width) && (row < (int)height);
		)
		{
			for (
				int x = col, y = row;
				(x < (int)width) && (y >= 0);
				x++, y--
			)
			{
				*(test++) = grid[x][y];
			}

			*(test) = '\0';
			test = test_arr;


			for (int i = strcmp(test, to_find, 0); i > 0; i = strcmp(test, to_find, i))
			{
				int c = i - 2;
				scratch[col + c][row - c]++;
			}

			if (row < (int)height - 1) row++;
			else col++;
		}
	}

	for (uint y = 0; y < height; y++)
	{
		for (uint x = 0; x < width; x++)
		{
			if (scratch[x][y] == '2') count++;
		}
	}
	fprintf(stdout, "count: %u\n", count);

	return true;
}
