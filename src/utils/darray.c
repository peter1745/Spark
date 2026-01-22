#include "darray.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

static void
darray_realloc (darray_t *arr)
{
    arr->data = reallocarray (arr->data, arr->capacity, arr->elem_size);
    // FIXME: Zero out new memory?
}

darray_t *
darray_empty (size_t elem_size)
{
    darray_t *arr = calloc (1, sizeof (darray_t));
    arr->capacity = 10;
    arr->elem_size = elem_size;
    darray_realloc (arr);
    return arr;
}

void
darray_free (darray_t *arr)
{
    if (arr->free_elem_fn) {
        for (size_t i = 0; i < arr->count; ++i) {
            arr->free_elem_fn (darray_elem (arr, i));
        }
    }

    free (arr);
}

void
darray_append (darray_t *arr, const void *elem)
{
    if (arr->count + 1 >= arr->capacity) {
        arr->capacity += arr->capacity / 2;
        darray_realloc (arr);
    }

    memcpy ((uint8_t *)arr->data + arr->count * arr->elem_size, elem, arr->elem_size);
    ++arr->count;
}

void *
darray_elem (darray_t *arr, size_t idx)
{
    assert (idx < arr->count);
    return (uint8_t *)arr->data + idx * arr->elem_size;
}

