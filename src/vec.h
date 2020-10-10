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

#ifndef KISS_VEC_H_
#define KISS_VEC_H_

#include <assert.h>
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

#define vec_grow(vec, count)                                                  \
    do {                                                                      \
        const size_t cv_sz = (count) * sizeof(*(vec)) + (sizeof(size_t) * 2); \
        if (!(vec)) {                                                         \
            size_t *cv_p = malloc(cv_sz);                                     \
            assert(cv_p);                                                     \
            (vec) = (void *)(&cv_p[2]);                                       \
            vec_set_capacity((vec), (count));                                 \
            vec_set_size((vec), 0);                                           \
        } else {                                                              \
            size_t *cv_p1 = &((size_t *)(vec))[-2];                           \
            size_t *cv_p2 = realloc(cv_p1, (cv_sz));                          \
            assert(cv_p2);                                                    \
            (vec) = (void *)(&cv_p2[2]);                                      \
            vec_set_capacity((vec), (count));                                 \
        }                                                                     \
    } while (0)

#define vec_pop_back(vec)                       \
    do {                                        \
        vec_set_size((vec), vec_size(vec) - 1); \
    } while (0)

#define vec_erase(vec, i)                                      \
    do {                                                       \
        if (vec) {                                             \
            const size_t cv_sz = vec_size(vec);                \
            if ((i) < cv_sz) {                                 \
                vec_set_size((vec), cv_sz - 1);                \
                size_t cv_x;                                   \
                for (cv_x = (i); cv_x < (cv_sz - 1); ++cv_x) { \
                    (vec)[cv_x] = (vec)[cv_x + 1];             \
                }                                              \
            }                                                  \
        }                                                      \
    } while (0)

#define vec_free(vec)                            \
    do {                                         \
        if (vec) {                               \
            size_t *p1 = &((size_t *)(vec))[-2]; \
            free(p1);                            \
        }                                        \
    } while (0)

#define vec_push(vec, value)                                    \
    do {                                                        \
        size_t cv_cap = vec_capacity(vec);                      \
        if (cv_cap <= vec_size(vec)) {                          \
            vec_grow((vec), !cv_cap ? cv_cap + 1 : cv_cap * 2); \
        }                                                       \
        vec[vec_size(vec)] = (value);                           \
        vec_set_size((vec), vec_size(vec) + 1);                 \
    } while (0)

#endif
