#ifndef __DATA_STRUCTS
#define __DATA_STRUCTS

#include <stdlib.h>

#define matrix_get_index(mat, i, j) (mat->cols * j) + i;

#define DEF_MATRIX(type_name, name, type)                                      \
    typedef struct {                                                           \
        type *items;                                                           \
        size_t cols;                                                           \
        size_t rows;                                                           \
    } type_name;                                                               \
    int name##_new(type_name *mat, size_t cols, size_t rows) {                 \
        size_t size = cols * rows;                                             \
        type *buff = calloc(size, sizeof(type));                               \
        if (buff == NULL)                                                      \
            return 1;                                                          \
        mat->items = buff;                                                     \
        mat->cols = cols;                                                      \
        mat->rows = rows;                                                      \
                                                                               \
        return 0;                                                              \
    }                                                                          \
    void name##_set(type_name *mat, size_t col, size_t row, type t) {          \
        size_t idx = matrix_get_index(mat, col, row);                          \
        mat->items[idx] = t;                                                   \
    }                                                                          \
    type name##_get(type_name *mat, size_t col, size_t row) {                  \
        size_t idx = matrix_get_index(mat, col, row);                          \
        return mat->items[idx];                                                \
    }                                                                          \
    void name##_free(type_name *mat) {                                         \
        free(mat->items);                                                      \
        mat->cols = mat->rows = 0;                                             \
    }

#define DECL_ARRAY(type_name, name, type)                                      \
    typedef struct {                                                           \
        type *items;                                                           \
        size_t len;                                                            \
        size_t capacity;                                                       \
    } type_name;                                                               \
    int name##_append(type_name *vect, type item) {                            \
        if (vect->len == vect->capacity) {                                     \
            vect->capacity *= 2;                                               \
            vect->items = realloc(vect->items, sizeof(type) * vect->capacity); \
            if (vect->items == NULL)                                           \
                return 1;                                                      \
        }                                                                      \
        vect->items[vect->len++] = item;                                       \
        return 0;                                                              \
    }                                                                          \
    int name##_init(type_name *vect, size_t cap) {                             \
        vect->items = malloc(sizeof(type) * cap);                              \
        vect->len = 0;                                                         \
        vect->capacity = cap;                                                  \
        return (vect->items == NULL) ? 1 : 0;                                  \
    }                                                                          \
    void name##_free(type_name *vect) {                                        \
        free(vect->items);                                                     \
        vect->capacity = vect->len = 0;                                        \
        vect->items = NULL;                                                    \
    }                                                                          \
    void name##_clear(type_name *vect) { vect->len = 0; }

#endif
