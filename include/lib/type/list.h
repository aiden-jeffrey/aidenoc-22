#ifndef __TYPE_LIST_H__
#define __TYPE_LIST_H__

/*  Generic list implementation for holding a set of pointers to a type
    (has to be consistently handled by the element_match and element_delete
    functions)
*/

typedef struct list_s list_t;

#include <stdbool.h>
#include <stdint.h>

list_t *list_create(
	bool     (*element_match )(const void *a, const void *b),
	void     (*element_delete)(void *element));

void list_delete(list_t *list);

bool     list_contains(const list_t *list, const void *element);
bool     list_is_empty(const list_t *list);
uint32_t list_length  (const list_t *list);

bool list_concat(list_t *self, list_t *consumed);

void  list_clear   (list_t *list);
bool  list_append  (list_t *list, void *element);
void *list_pop_back(list_t *list);
void  list_flip    (list_t *list);

void    *list_get        (const list_t *list, uint32_t index);
int64_t  list_index      (const list_t *list, const void *element);
int64_t  list_index_from (
	const list_t *list, const void *element, int64_t start_index);

bool list_remove   (list_t *list, void *element);
bool list_remove_at(list_t *list, uint32_t index);

bool list_match_ptr(const void *a, const void *b);
bool list_match_str(const void *a, const void *b);

#endif
