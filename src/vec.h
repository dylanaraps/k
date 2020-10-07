/* Original sources: https://github.com/eteran/c-vector

The MIT License (MIT)

Copyright (c) 2015 Evan Teran

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef VEC_H_
#define VEC_H_

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#define vec_capacity(vec) ((vec) ? ((size_t *)(vec))[-1] : (size_t)0)
#define vec_size(vec) ((vec) ? ((size_t *)(vec))[-2] : (size_t)0)
#define vec_empty(vec) (vec_size(vec) == 0)
#define vec_begin(vec) (vec)
#define vec_end(vec) ((vec) ? &((vec)[vec_size(vec)]) : NULL)

#define vec_set_capacity(vec, size)         \
    do {                                    \
        if (vec) {                          \
            ((size_t *)(vec))[-1] = (size); \
        }                                   \
    } while (0)

#define vec_set_size(vec, size)             \
    do {                                    \
        if (vec) {                          \
            ((size_t *)(vec))[-2] = (size); \
        }                                   \
    } while (0)

#define vec_grow(vec, count)                                                 \
    do {                                                                     \
        const size_t __sz = (count) * sizeof(*(vec)) + (sizeof(size_t) * 2); \
        if (!(vec)) {                                                        \
            size_t *__p = malloc(__sz);                                      \
            assert(__p);                                                     \
            (vec) = (void *)(&__p[2]);                                       \
            vec_set_capacity((vec), (count));                                \
            vec_set_size((vec), 0);                                          \
        } else {                                                             \
            size_t *__p1 = &((size_t *)(vec))[-2];                           \
            size_t *__p2 = realloc(__p1, (__sz));                            \
            assert(__p2);                                                    \
            (vec) = (void *)(&__p2[2]);                                      \
            vec_set_capacity((vec), (count));                                \
        }                                                                    \
    } while (0)

#define vec_pop_back(vec)                       \
    do {                                        \
        vec_set_size((vec), vec_size(vec) - 1); \
    } while (0)

#define vec_erase(vec, i)                                  \
    do {                                                   \
        if (vec) {                                         \
            const size_t __sz = vec_size(vec);             \
            if ((i) < __sz) {                              \
                vec_set_size((vec), __sz - 1);             \
                size_t __x;                                \
                for (__x = (i); __x < (__sz - 1); ++__x) { \
                    (vec)[__x] = (vec)[__x + 1];           \
                }                                          \
            }                                              \
        }                                                  \
    } while (0)

#define vec_free(vec)                            \
    do {                                         \
        if (vec) {                               \
            size_t *p1 = &((size_t *)(vec))[-2]; \
            free(p1);                            \
        }                                        \
    } while (0)

#define vec_add(vec, value)                                  \
    do {                                                     \
        size_t __cap = vec_capacity(vec);                    \
        if (__cap <= vec_size(vec)) {                        \
            vec_grow((vec), !__cap ? __cap + 1 : __cap * 2); \
        }                                                    \
        vec[vec_size(vec)] = (value);                        \
        vec_set_size((vec), vec_size(vec) + 1);              \
    } while (0)

#endif
