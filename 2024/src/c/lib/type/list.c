#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/type/list.h"

#define LIST_INITIAL_CAP 16

struct list_s
{
	uint32_t   length;
	uint32_t   capacity;
	void     **elements;
	bool     (*element_match )(const void *a, const void *b);
	void     (*element_delete)(void *element);
};


static void list__shift(list_t *list, int64_t start_index);


list_t *list_create(
	bool     (*element_match )(const void *a, const void *b),
	void     (*element_delete)(void *element))
{
	list_t *list;

	list = (list_t *)malloc(sizeof(list_t));
	if (!list) return NULL;

	list->length         = 0;
	list->capacity       = LIST_INITIAL_CAP;
	list->element_match  = element_match;
	list->element_delete = element_delete;

	list->elements = (void **)malloc(sizeof(void *) * LIST_INITIAL_CAP);
	if (!list->elements)
	{
		list_delete(list);
		return NULL;
	}
	return list;
}


void list_delete(list_t *list)
{
	if (!list) return;

	if (list->element_delete)
	{
		for (unsigned i = 0; i< list->length; i++)
			list->element_delete(list->elements[i]);
	}

	free(list->elements);
	free(list);
}


void list_clear(list_t *list)
{
	if (!list) return;

	if (list->element_delete)
	{
		for (unsigned i = 0; i < list->length; i++)
			list->element_delete(list->elements[i]);
	}
	list->length = 0;
}


bool list_append(list_t *list, void *element)
{
	if (!list || ((list->length + 1) == 0)) return false;

	if (list->length >= list->capacity)
	{
		// expand the elements array
		list->capacity *= 2;

		void **nelements = (void **)realloc(list->elements,
			(sizeof(void *) * list->capacity));
		if (!nelements) return false;
		list->elements = nelements;
	}

	list->elements[list->length++] = element;
	return true;
}


void *list_get(const list_t *list, uint32_t index) {
	if (!list || index >= list->length)
		return NULL;

	return list->elements[index];
}


void *list_pop_back(list_t *list)
{
	if (!list || list_is_empty(list))
		return NULL;

	return list->elements[--(list->length)];
}


void list_flip(list_t *list)
{
	if (!list) return;

	for (unsigned i = 0; i < (list->length / 2); i++)
	{
		void *swap = list->elements[list->length - 1 - i];
		list->elements[list->length - 1 - i] = list->elements[i];
		list->elements[i] = swap;
	}
}


bool list_remove(list_t *list, void *element)
{
	int64_t i = list_index(list, element);

	if (i == -1) return false;

	list__shift(list, i);
	list->length--;
	return true;
}


bool list_remove_at(list_t *list, uint32_t index)
{
	if (list->length <= index)
		return false;

	list__shift(list, index);
	list->length--;
	return true;
}


int64_t list_index(const list_t *list, const void *element)
{
	return list_index_from(list, element, 0);
}


int64_t list_index_from(
	const list_t *list, const void *element, int64_t start_index)
{
	/* No way to compare */
	if (!list->element_match ||
		 list->length < (uint32_t) start_index)
		return -1;

	for (uint32_t i = start_index; i < list->length; i++)
	{
		if (list->element_match(list->elements[i], element))
			return i;
	}
	return -1;
}


bool list_contains(const list_t *list, const void *element)
{
	return (list_index(list, element) != -1);
}


bool list_is_empty(const list_t *list)
{
	return list ? (list->length == 0) : true;
}


uint32_t list_length(const list_t *list)
{
	return list ? list->length : 0;
}


bool list_concat(list_t *self, list_t *consumed)
{
	if (!self || !consumed) return false;

	bool grown = false;
	while ((self->length + consumed->length) >= self->capacity)
	{
		self->capacity *= 2;
		grown = true;
	}

	if (grown)
	{
		void **nelements = (void **)realloc(self->elements,
			(sizeof(void *) * self->capacity));
		if (!nelements) return false;
		self->elements = nelements;
	}

	for (unsigned i = 0; i < consumed->length; i++)
		self->elements[i + self->length] = consumed->elements[i];

	self->length += consumed->length;

	if (consumed->element_delete)
		consumed->element_delete = NULL;

	list_delete(consumed);

	return true;
}

static void list__swap(list_t *list, uint32_t i_a, uint32_t i_b) {
	void *t = list->elements[i_a];
	list->elements[i_a] = list->elements[i_b];
	list->elements[i_b] = t;
}

static uint32_t list__sort_partition(
	list_t *list,
	bool (*less_than)(const void *a, const void *b),
	int32_t i_low,
	int32_t i_high)
{
	// Choose the pivot
	void *pivot = list_get(list, i_high);
	if (!pivot)
	{
		fprintf(stderr, "missing pivot in list %u\n", i_high);
		return 0;
	}

	// Index of smaller element and indicates
	// the right position of pivot found so far
	int i = i_low - 1;

	// Traverse arr[low..high] and move all smaller
	// elements to the left side. Elements from low to
	// i are smaller after every iteration
	for (int32_t j = i_low; j <= i_high - 1; j++)
	{
		void *elem = list_get(list, j);
		if (!elem)
		{
			fprintf(stderr, "missing element in list\n");
			continue;
		}
		if (less_than(elem, pivot))
		{
			i++;
			list__swap(list, i, j);
		}
	}

	// Move pivot after smaller elements and
	// return its position
	list__swap(list, i + 1, i_high);
	return i + 1;
}

static void list__quick_sort(
	list_t *list,
	bool (*less_than)(const void *a, const void *b),
	int32_t i_low,
	int32_t i_high)
{
	if (i_low < i_high)
	{
		uint32_t i_p = list__sort_partition(list, less_than, i_low, i_high);

		list__quick_sort(list, less_than, i_low, i_p - 1);
		list__quick_sort(list, less_than, i_p + 1, i_high);
	}
}

void list_sort(list_t *list, bool (*less_than)(const void *a, const void *b))
{
	list__quick_sort(list, less_than, 0, list_length(list) - 1);
}


static void list__shift(list_t *list, int64_t start_index)
{
	if (start_index < 0) return;

	for (unsigned i = start_index; i < list->length; i++)
	{
		if (i == (list->length - 1))
			list->elements[i] = NULL;
		else
			list->elements[i] = list->elements[i + 1];
	}
}


bool list_match_ptr(const void *a, const void *b)
{
	return ((uintptr_t)a == (uintptr_t)b);
}

bool list_match_str(const void *a, const void *b)
{
	if (!a || !b) return false;
	return (strcmp(a, b) == 0);
}
