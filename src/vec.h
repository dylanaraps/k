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

#include <assert.h> /* for assert */
#include <stddef.h> /* for size_t */
#include <stdlib.h> /* for malloc/realloc/free */

/**
 * @brief vec_iter - Iterate over a vector.
 * @param vec - the vector
 * @return void
 */
#define vec_iter(p) for (size_t i = 0; i < vec_size(p); ++i)

/**
 * @brief vec_set_capacity - For internal use, sets the capacity variable of
                             the vector
 * @param vec - the vector
 * @param size - the new capacity to set
 * @return void
 */
#define vec_set_capacity(vec, size)         \
    do {                                    \
        if (vec) {                          \
            ((size_t *)(vec))[-1] = (size); \
        }                                   \
    } while (0)

/**
 * @brief vec_set_size - For internal use, sets the size variable of the vector
 * @param vec - the vector
 * @param size - the new capacity to set
 * @return void
 */
#define vec_set_size(vec, size)             \
    do {                                    \
        if (vec) {                          \
            ((size_t *)(vec))[-2] = (size); \
        }                                   \
    } while (0)

/**
 * @brief vec_capacity - gets the current capacity of the vector
 * @param vec - the vector
 * @return the capacity as a size_t
 */
#define vec_capacity(vec) ((vec) ? ((size_t *)(vec))[-1] : (size_t)0)

/**
 * @brief vec_size - gets the current size of the vector
 * @param vec - the vector
 * @return the size as a size_t
 */
#define vec_size(vec) ((vec) ? ((size_t *)(vec))[-2] : (size_t)0)

/**
 * @brief vec_empty - returns non-zero if the vector is empty
 * @param vec - the vector
 * @return non-zero if empty, zero if non-empty
 */
#define vec_empty(vec) (vec_size(vec) == 0)

/**
 * @brief vec_grow - For internal use, ensures that the vector is at least
                     <count> elements big
 * @param vec - the vector
 * @param size - the new capacity to set
 * @return void
 */
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

/**
 * @brief vec_pop_back - removes the last element from the vector
 * @param vec - the vector
 * @return void
 */
#define vec_pop_back(vec)                       \
    do {                                        \
        vec_set_size((vec), vec_size(vec) - 1); \
    } while (0)

/**
 * @brief vec_erase - removes the element at index i from the vector
 * @param vec - the vector
 * @param i - index of element to remove
 * @return void
 */
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

/**
 * @brief vec_free - frees all memory associated with the vector
 * @param vec - the vector
 * @return void
 */
#define vec_free(vec)                        \
    do {                                         \
        if (vec) {                               \
            size_t *p1 = &((size_t *)(vec))[-2]; \
            free(p1);                            \
        }                                        \
    } while (0)

/**
 * @brief vec_begin - returns an iterator to first element of the vector
 * @param vec - the vector
 * @return a pointer to the first element (or NULL)
 */
#define vec_begin(vec) (vec)

/**
 * @brief vec_end - returns an iterator to one past the last element
 * @param vec - the vector
 * @return a pointer to one past the last element (or NULL)
 */
#define vec_end(vec) ((vec) ? &((vec)[vec_size(vec)]) : NULL)

/**
 * @brief vec_push_back - adds an element to the end of the vector
 * @param vec - the vector
 * @param value - the value to add
 * @return void
 */
#define vec_push_back(vec, value)                            \
	do {                                                     \
		size_t __cap = vec_capacity(vec);                    \
		if (__cap <= vec_size(vec)) {                        \
			vec_grow((vec), !__cap ? __cap + 1 : __cap * 2); \
		}                                                    \
		vec[vec_size(vec)] = (value);                        \
		vec_set_size((vec), vec_size(vec) + 1);              \
	} while (0)

#endif /* VEC_H_ */
