#include <stdlib.h>
#include <string.h>

#include "lib/type/map.h"

typedef struct map_node_s map_node_t;

struct map_s
{
	uint8_t     (*key_hash   )(const void *key);
	bool        (*key_match  )(const void *a, const void *b);
	const void *(*item_key   )(const void *item);
	void        (*item_delete)(void *item);

	unsigned    length;

	map_node_t *base[256];
};

struct map_node_s
{
	void       *item;
	map_node_t *next;
};


map_t *map_create(
	uint8_t     (*key_hash   )(const void *key),
	bool        (*key_match  )(const void *a, const void *b),
	const void *(*item_key   )(const void *item),
	void        (*item_delete)(void *item))
{
	map_t *map = (map_t *)malloc(sizeof(map_t));
	if (!map) return NULL;

	map->key_hash    = key_hash;
	map->key_match   = key_match;
	map->item_key    = item_key;
	map->item_delete = item_delete;
	map->length      = 0;

	for (unsigned i = 0; i < 256; i++)
		map->base[i] = NULL;

	return map;
}

void map_delete(map_t *map)
{
	if (!map) return;

	map_clear(map);

	free(map);
}


void map_clear(map_t *map)
{
	if (!map) return;

	for (unsigned i = 0; i < 256; i++)
	{
		map_node_t *node = map->base[i];
		while (node)
		{
			map_node_t *next = node->next;
			if (map->item_delete)
				map->item_delete(node->item);
			free(node);
			node = next;
		}
		map->base[i] = NULL;
	}

	map->length = 0;
}


static map_node_t *map__find_node(
	map_t *map, const void *key, uint8_t hash)
{
	if (!map->item_key
		|| !map->key_match)
		return NULL;

	map_node_t *node;
	for (node = map->base[hash]; node; node = node->next)
	{
		const void *nkey = map->item_key(node->item);
		if (!nkey) continue;
		if (map->key_match(key, nkey))
			return node;
	}

	return NULL;
}

static void *map__find(
	map_t *map, const void *key, uint8_t hash)
{
	map_node_t *node = map__find_node(map, key, hash);
	if (!node) return NULL;
	return node->item;
}


static bool map__add(map_t *map, void *item, bool replace)
{
	if (!map || !item
		|| !map->item_key
		|| !map->key_hash)
		return false;

	const void *key = map->item_key(item);
	if (!key) return false;

	uint8_t hash = map->key_hash(key);

	map_node_t *node = map__find_node(map, key, hash);
	if (node)
	{
		if (!replace)
			return false;

		if (map->item_delete)
			map->item_delete(node->item);
		node->item = item;
		return true;
	}

	node = (map_node_t *)malloc(sizeof(map_node_t));
	if (!node) return false;

	node->item = item;
	node->next = map->base[hash];
	map->base[hash] = node;
	map->length++;
	return true;
}


bool map_add(map_t *map, void *item)
{
	return map__add(map, item, false);
}


bool map_replace(map_t *map, void *item)
{
	return map__add(map, item, true);
}


bool map_remove(map_t *map, void *item)
{
	if (!map || !map->item_key
		|| !map->key_match)
		return false;

	const void *key = map->item_key(item);
	if (!key) return false;

	uint8_t hash = map->key_hash(key);

	bool removed = false;
	map_node_t *node, *prev = NULL;
	for (node = map->base[hash]; node; prev = node, node = node->next)
	{
		const void *nkey = map->item_key(node->item);
		if (!nkey) continue;
		if (map->key_match(key, nkey))
		{
			removed = true;
			if (map->item_delete)
				map->item_delete(node->item);

			if (prev)
				prev->next = node->next;
			else
				map->base[hash] = node->next;
			free(node);
			break;
		}
	}

	if (removed)
		map->length--;

	return removed;
}


void *map_find(map_t *map, const void *key)
{
	if (!map || !key
		|| !map->key_hash)
		return NULL;

	uint8_t hash = map->key_hash(key);
	return map__find(map, key, hash);
}


unsigned map_length(map_t *map)
{
	return map ? map->length : 0;
}


list_t *map_get_items(map_t *map)
{
	if (!map) return NULL;

	// TODO: do we want match function here?
	list_t *items_list = list_create(NULL, NULL);

	for (unsigned i = 0; i < 256; i++)
	{
		map_node_t *node = map->base[i];
		while (node)
		{
			map_node_t *next = node->next;
			list_append(items_list, node->item);
			node = next;
		}
	}

	return items_list;
}


uint8_t map_hash_str(const void *str)
{
	char *c_str = (char *)str;
	unsigned long hash = 5381;
	int c;

	while ((c = *c_str++) != '\0')
		hash = ((hash << 5) + hash) + c;

	return (hash % 256);
}


bool map_match_str(const void *a, const void *b)
{
	if (!a || !b)
		return false;

	return (strcmp(a, b) == 0);
}


uint8_t map_hash_ptr(const void *ptr)
{
	uint8_t h = 0;
	uintptr_t p = (uintptr_t)ptr;
	for (unsigned i = 0; i < 8; i++, p >>= 8)
		h += p & 0xFF;
	return h;
}

bool map_match_ptr(const void *a, const void *b)
{
	return (a == b);
}


const void *map_key_self(const void *item)
{
	return item;
}

