#pragma once

#include <stddef.h>

typedef void(*darray_free_elem_fn_t)(void *elem);

typedef struct darray_s {
    void *data;
    size_t count;
    size_t capacity;
    size_t elem_size;
    darray_free_elem_fn_t free_elem_fn;
} darray_t;

darray_t *darray_empty (size_t elem_size);
void      darray_free (darray_t *arr);

void      darray_append (darray_t *arr, const void *elem);
void     *darray_elem (darray_t *arr, size_t idx);

#define darray_append_v(arr, value) do { \
    auto _arr = (arr); \
    auto _value = (value); \
    darray_append (_arr, &_value); \
} while (false)

