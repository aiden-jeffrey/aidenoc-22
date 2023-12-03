#define _XOPEN_SOURCE 700
#define _GNU_SOURCE
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "lib/util/filepath.h"

static const char *filepath__file(const char *filepath)
{
	if (!filepath) return NULL;
	const char *nfilepath;
	for (nfilepath = strchr(filepath, '/'); nfilepath;
		nfilepath = strchr(&nfilepath[1], '/'))
		filepath = &nfilepath[1];
	return filepath;
}

static const char *filepath__file_ext(const char *filepath)
{
	if (!filepath || (filepath[0] == '\0'))
		return NULL;
	const char *next, *ext = NULL;
	for (next = strchr(&filepath[1], '.'); next;
		next = strchr(&next[1], '.'))
		ext = &next[1];
	return ext;
}



char *filepath_optimize(const char *filepath)
{
	if (!filepath || (filepath[0] == '\0'))
		return NULL;

	if (strcmp(filepath, ".") == 0)
		return strdup(filepath);

	unsigned c_count = 1;
	const char *src;
	for (src = filepath; *src != '\0'; src++)
		c_count += (*src == '/' ? 1 : 0);

	const char *c[c_count];
	unsigned c_len[c_count];

	c[0] = filepath;

	unsigned ci = 0;
	unsigned cl = 0;
	for (src = filepath; *src != '\0'; src++)
	{
		if (*src == '/')
		{
			c_len[ci] = cl;
			c[++ci] = &src[1];
			cl = 0;
		}
		else
		{
			cl++;
		}
	}
	c_len[ci] = cl;

	const char *nc[c_count];
	unsigned nc_len[c_count];
	unsigned nc_count = 0;

	for (ci = 0; ci < c_count; ci++)
	{
		if ((c_len[ci] == 0)
			&& (nc_count != 0)
			&& ((ci + 1) != c_count))
			continue;

		if ((c_len[ci] == 1)
			&& (c[ci][0] == '.'))
			continue;

		if ((c_len[ci] == 2)
			&& (strncmp(c[ci], "..", 2) == 0)
			&& (nc_count > 0))
		{
			bool was_root = (nc_count == 1) && (nc_len[0] == 0);
			if (!was_root)
				nc_count--;
			continue;
		}

		nc[nc_count] = c[ci];
		nc_len[nc_count] = c_len[ci];
		nc_count++;
	}

	unsigned nc_total = nc_count;
	for (ci = 0; ci < nc_count; ci++)
		nc_total += nc_len[ci];

	char *nfilepath = (char *)malloc(nc_total);
	if (!nfilepath) return NULL;

	for (ci = 0, cl = 0; ci < nc_count; ci++)
	{
		if (ci > 0)
			nfilepath[cl++] = '/';
		if (nc_len[ci] > 0)
		{
			memcpy(&nfilepath[cl], nc[ci], nc_len[ci]);
			cl += nc_len[ci];
		}
	}
	nfilepath[nc_total - 1] = '\0';

	return nfilepath;
}

char *filepath_resolve(const char *filepath)
{
	if (!filepath) return NULL;

	if (filepath[0] == '/')
		return filepath_optimize(filepath);

	long clen = pathconf(".", _PC_PATH_MAX);
	if (clen < 0) return NULL;
	size_t plen = strlen(filepath);

	char rfilepath[clen + plen + 2];
	if (!getcwd(rfilepath, clen))
		return NULL;
	clen = strlen(rfilepath);
	if (clen == 0) return NULL;

	if (rfilepath[clen - 1] != '/')
		rfilepath[clen++] = '/';
	memcpy(&rfilepath[clen], filepath, (plen + 1));

	return filepath_optimize(rfilepath);
}



char *filepath_dir(const char *filepath)
{
	if (!filepath) return NULL;

	const char *end = filepath__file(filepath);
	uintptr_t dlen = ((uintptr_t)end - (uintptr_t)filepath);
	char *ret = (char *)malloc(dlen + 1);
	if (!ret) return NULL;
	memcpy(ret, filepath, dlen);
	ret[dlen] = '\0';
	return ret;
}

char *filepath_file(const char *filepath)
{
	filepath = filepath__file(filepath);
	if (!filepath) return NULL;
	return strdup(filepath);
}

char *filepath_file_name(const char *filepath)
{
	filepath = filepath__file(filepath);
	if (!filepath) return NULL;

	const char *end
		= filepath__file_ext(filepath);
	if (!end) return strdup(filepath);

	size_t plen = ((uintptr_t)--end - (uintptr_t)filepath);
	char *ret = (char *)malloc(plen + 1);
	if (!ret) return NULL;
	memcpy(ret, filepath, plen);
	ret[plen] = '\0';
	return ret;
}

char *filepath_file_ext(const char *filepath)
{
	filepath = filepath__file(filepath);
	filepath = filepath__file_ext(filepath);
	if (!filepath) return NULL;
	return strdup(filepath);
}


bool filepath_file_is_dir(const char *path)
{
	if (!path) return false;

	struct stat sb;

	return (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode));
}

bool filepath_file_is_file(const char *path)
{
	if (!path) return false;

	struct stat sb;

	return (stat(path, &sb) == 0 && S_ISREG(sb.st_mode));
}


bool filepath_file_has_ext(const char *filepath, const char *ext)
{
	filepath = filepath__file(filepath);
	const char *pext = filepath__file_ext(filepath);
	if (!pext) return false;

	size_t elen = strlen(ext);
	size_t plen = strlen(pext);

	return ((elen == plen)
		&& (strncasecmp(ext, pext, elen) == 0));
}


bool filepath_file_endswith(const char *path, const char *end)
{
	if (!path || !end) return false;

	char *path_end = filepath_file_name(path);
	bool endswith = (strcmp(path_end, end) == 0);
	free(path_end);
	return endswith;
}



char *filepath_relative(const char *base, const char *filepath)
{
	if (!filepath || (filepath[0] == '\0'))
		return NULL;
	if (!base || (base[0] == '\0')
		|| (filepath[0] == '/'))
		return filepath_optimize(filepath);

	uintptr_t base_len
		= ((uintptr_t)filepath__file(base) - (uintptr_t)base);
	if (base_len == 0)
		return filepath_optimize(filepath);

	uintptr_t filepath_len = strlen(filepath);
	char full[base_len + filepath_len + 1];
	memcpy(full, base, base_len);
	memcpy(&full[base_len], filepath, filepath_len);
	full[base_len + filepath_len] = '\0';

	return filepath_optimize(full);
}


char *filepath_append(const char *base, const char *tail)
{
	if (!base || !tail) return NULL;

	uintptr_t base_len = strlen(base);
	uintptr_t tail_len = strlen(tail);
	char full[base_len + tail_len + 2];

	memcpy(full, base, base_len);

	if (base[base_len - 1] != '/')
		full[base_len++] = '/';

	memcpy(&full[base_len], tail, tail_len);
	full[base_len + tail_len] = '\0';

	return filepath_optimize(full);
}


static bool filepath__match(const void *a, const void *b)
{
	if (!a || !b) return false;

	return (strcmp(a, b) == 0);
}


static void filepath__delete(void *a)
{
	if (!a) return;

	return free(a);
}


list_t *filepath_list_contents(const char *path, bool just_name)
{
	if (!path) return NULL;

	list_t *list = list_create(filepath__match, filepath__delete);
	if (!list) return NULL;

	DIR *dp;
	dp = opendir(path);

	if (dp)
	{
		struct dirent *ep;
		while ((ep = readdir(dp)))
		{
			if ((strcmp(ep->d_name, ".") != 0) && (strcmp(ep->d_name, "..") != 0))
			{
				char fname[strlen(path) + strlen(ep->d_name) + 2];
				fname[0] = '\0';
				if (!just_name)
				{
					strcat(fname, path);
					strcat(fname, "/");
				}
				strcat(fname, ep->d_name);

				char *fopt = filepath_optimize(fname);
				if (!list_append(list, fopt))
				{
					// TODO: Handle error in some way.
					free(fopt);
				}
			}
		}

		closedir(dp);
	}

	return list;
}


char *filepath_from_name(
	const char *base,
	const char *class,
	const char *name,
	const char *ext)
{
	if (!base || !class || !name)
		return NULL;

	size_t len[4] =
	{
		strlen(base), strlen(class), strlen(name), ext ? strlen(ext) : 0
	};

	char *path = (char *)malloc(len[0] + len[1] + 1 + len[2] + len[3] + 1);
	if (!path) return NULL;

	unsigned offset = 0;

	memcpy(&path[offset], base, len[0]);
	offset += len[0];

	memcpy(&path[offset], class, len[1]);
	offset += len[1];

	path[offset++] = '/';

	for (size_t i = 0; i < len[2]; i++, offset++)
	{
		char c = name[i];
		if (c == '.') c = '/';
		path[offset] = c;
	}

	if (ext)
	{
		memcpy(&path[offset], ext, len[3]);
		offset += len[3];
	}
	path[offset] = '\0';

	return path;
}
