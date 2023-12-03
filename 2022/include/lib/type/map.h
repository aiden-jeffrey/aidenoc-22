#ifndef __TYPE_MAP_H__
#define __TYPE_MAP_H__

typedef struct map_s map_t;

#include <stdbool.h>
#include <stdint.h>

#include "lib/type/list.h"

map_t *map_create(
	uint8_t     (*key_hash   )(const void *key),
	bool        (*key_match  )(const void *a, const void *b),
	const void *(*item_key   )(const void *item),
	void        (*item_delete)(void *item));
void map_delete(map_t *map);

void map_clear(map_t *map);

bool  map_add    (map_t *map, void *item);
bool  map_replace(map_t *map, void *item);
bool  map_remove (map_t *map, void *item);
void *map_find   (map_t *map, const void *key);

unsigned map_length(map_t *map);

list_t *map_get_items(map_t *map);

uint8_t map_hash_str (const void *str);
bool    map_match_str(const void *a, const void *b);
uint8_t map_hash_ptr(const void *num);
bool    map_match_ptr(const void *a, const void *b);

const void *map_key_self(const void *item);

#endif
