#ifndef _DARRAY_H_
#define _DARRAY_H_

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/* DARRAY MEMORY LAYOUT
 * ====================
 * +----------+---------+---------+-----+------------------+
 * | metadata | elem[0] | elem[1] | ... | elem[capacity-1] |
 * +----------+---------+---------+-----+------------------+
 *            ^
 *            Handle to the darray points to the first
 *            element of the array.
 *
 * METADATA
 * ========
 *  size_t : sizeof contained element
 *  size_t : length of the darray
 *  size_t : capacity of the darray
 *  void*  : tmp pointer for use in array functions
 */

#define DA_SIZEOF_ELEM_OFFSET 0
#define DA_LENGTH_OFFSET      (1*sizeof(size_t))
#define DA_CAPACITY_OFFSET    (2*sizeof(size_t))
#define DA_TMP_PTR_OFFSET     (3*sizeof(size_t))
#define DA_HANDLE_OFFSET      (3*sizeof(size_t) + 1*sizeof(void*))

#define DA_HEAD_FROM_HANDLE(darr_h) \
    (((char*)(darr_h)) - DA_HANDLE_OFFSET)
#define DA_P_SIZEOF_ELEM_FROM_HANDLE(darr_h) \
    ((size_t*)(DA_HEAD_FROM_HANDLE(darr_h) + DA_SIZEOF_ELEM_OFFSET))
#define DA_P_LENGTH_FROM_HANDLE(darr_h) \
    ((size_t*)(DA_HEAD_FROM_HANDLE(darr_h) + DA_LENGTH_OFFSET))
#define DA_P_CAPACITY_FROM_HANDLE(darr_h) \
    ((size_t*)(DA_HEAD_FROM_HANDLE(darr_h) + DA_CAPACITY_OFFSET))
#define DA_P_TMP_PTR_FROM_HANDLE(darr_h) \
    ((void**)(DA_HEAD_FROM_HANDLE(darr_h) + DA_TMP_PTR_OFFSET))

#define DA_CAPACITY_FACTOR 1.3
#define DA_NEW_CAPACITY_FROM_LENGTH(length) \
    (length < 10 ? 10 : (length*DA_CAPACITY_FACTOR))

/**@function
 * @brief Allocate an darray of `nelem` elements each of size `size`.
 *
 * @param nelem : Initial number of elements in the darray.
 * @param size : Size of each element.
 *
 * @return Pointer to a new darray.
 */
static inline void* da_alloc(size_t nelem, size_t size);

/**@function
 * @brief Free a darray.
 *
 * @param darr : Target darray to be freed.
 */
static inline void da_free(void* darr);

/**@function
 * @brief Returns the number of elements in the darray.
 *
 * @param darr : Target darray.
 * @return Number of elements in the darray.
 */
static inline size_t da_length(void* darr);

/**@function
 * @brief Returns the maximum number of elements the darray hand hold without
    resizing.
 *
 * @param darr : Target darray.
 * @return Total number of allocated elements in the darray.
 */
static inline size_t da_capacity(void* darr);

/**@function
 * @brief Returns the size of contained elements in the darray.
 *
 * @param darr : Target darray.
 * @return sizeof elements in the darray.
 */
static inline size_t da_sizeof_elem(void* darr);

/**@function
 * @brief Change the length of the array to nelem. Data for elements with index
    >= nelem may be lost.
 *
 * @param darr : Target darray. Upon function completion, darr may or may not
    point to its previous location, potentially breaking references to the darr.
 * @return Pointer to the new location of the darray upon successful function
    completion. If da_resize returns NULL, allocation failed and darr is left
    untouched.
 *
 * @note Affects the length attribute of the darray.
 */
static inline void* da_resize(void* darr, size_t nelem);

/**@function
 * @brief Ensure there is at least `nelem` valid elements available beyond the
    current length of the darray without requiring resizing.
 *
 * @param darr : Target darray. Upon function completion, darr may or may not
    point to its previous location, potentially breaking references to the darr.
 * @return Pointer to the new location of the darray upon successful function
    completion. If da_reserve returns NULL, allocation failed and darr is left
    untouched.
 *
 * @note Does not affect the length attribute of the darray.
 */
static inline void* da_reserve(void* darr, size_t nelem);

/**@macro
 * @brief Add a value to the back of darr.
 *
 * @param darr : lvalue pointing to the target darray.
 * @param value : Value to be pushed onto the array.
 *
 * @return Pointer to the new location of the darray upon successful macro
    completion. If da_push returns NULL, allocation failed and darr is left
    untouched.
 *
 * @note Affects the length of the darray.
 * @note Due to the macro implementation of da_push there is no need to
    reassign the darray unless you are checking for macro failure. Since the
    common case for dynamic arrays is to push back a lot of values, assignment
    could waste a lot of clock cycles, so if you are confident memory allocation
    will not fail, or if you know you have enough space reserved, feel free to
    push without assigning the result of da_push back to darr.
 */
#define /* void* */da_push(/* void* */darr, /* ARRAY TYPE */value) \
    _da_push(darr, value)

/**@macro
 * @brief Remove a value to the back of darr and return it.
 *
 * @param darr : lvalue pointing to the target darray.
 *
 * @return Value popped off of the darray.
 *
 * @note Affects the length of the darray.
 * @note da_pop will never reallocate memory, so popping is always
    allocation-safe.
 */
#define /* ARRAY TYPE */da_pop(/* void* */darr) \
    _da_pop(darr)

////////////////////////////////////////////////////////////////////////////////
// DEFINITIONS
////////////////////////////////////////////////////////////////////////////////

static inline void* da_alloc(size_t nelem, size_t size)
{
    size_t capacity = DA_NEW_CAPACITY_FROM_LENGTH(nelem);
    void* mem = malloc(capacity*size + DA_HANDLE_OFFSET);
    if (mem == NULL)
    {
        return mem;
    }
    (*(size_t*)(mem + DA_SIZEOF_ELEM_OFFSET)) = size;
    (*(size_t*)(mem + DA_LENGTH_OFFSET))      = nelem;
    (*(size_t*)(mem + DA_CAPACITY_OFFSET))    = capacity;
    return mem + DA_HANDLE_OFFSET;
}

static inline void da_free(void* darr)
{
    free(DA_HEAD_FROM_HANDLE(darr));
}

static inline size_t da_length(void* darr)
{
    return *DA_P_LENGTH_FROM_HANDLE(darr);
}

static inline size_t da_capacity(void* darr)
{
    return *DA_P_CAPACITY_FROM_HANDLE(darr);
}

static inline size_t da_sizeof_elem(void* darr)
{
    return *DA_P_SIZEOF_ELEM_FROM_HANDLE(darr);
}

static inline void* da_resize(void* darr, size_t nelem)
{
    size_t new_capacity = DA_NEW_CAPACITY_FROM_LENGTH(nelem);
    void* ptr = realloc(DA_HEAD_FROM_HANDLE(darr),
        new_capacity*da_sizeof_elem(darr) + DA_HANDLE_OFFSET);
    if (ptr == NULL)
    {
        return NULL;
    }
    darr = ptr;
    *((size_t*)(ptr + DA_CAPACITY_OFFSET)) = new_capacity;
    *((size_t*)(ptr + DA_LENGTH_OFFSET))   = nelem;
    return ptr + DA_HANDLE_OFFSET;
}

static inline void* da_reserve(void* darr, size_t nelem)
{
    size_t curr_capacity = da_capacity(darr);
    size_t curr_length = da_length(darr);
    size_t min_capacity = curr_length + nelem;
    if (curr_capacity >= min_capacity)
    {
        return darr;
    }
    size_t new_capacity = DA_NEW_CAPACITY_FROM_LENGTH(min_capacity);
    void* ptr = realloc(DA_HEAD_FROM_HANDLE(darr),
        new_capacity*da_sizeof_elem(darr) + DA_HANDLE_OFFSET);
    if (ptr == NULL)
    {
        return NULL;
    }
    *((size_t*)(ptr + DA_CAPACITY_OFFSET)) = new_capacity;
    return ptr + DA_HANDLE_OFFSET;
}

#define /* void* */_da_push(/* void* */darr, /* ARRAY TYPE */value)            \
                    /* ABANDON HOPE ALL YE WHO ENTER HERE */                   \
(                                                                              \
/* if darr.length < darr.capacity */                                           \
(da_length(darr) < da_capacity(darr)) ?                                        \
(                                                                              \
    /* darr[darr.length++] = value */                                          \
    ((darr)[(*DA_P_LENGTH_FROM_HANDLE(darr))++] = value)                       \
    , /* then */                                                               \
    /* return darr */                                                          \
    (darr)                                                                     \
)                                                                              \
: /* else */                                                                   \
(                                                                              \
    ( /* "then" paren(s) */                                                    \
    /* darr.tmp = new_darr */                                                  \
    (*DA_P_TMP_PTR_FROM_HANDLE(darr) = da_alloc(                               \
        da_length(darr),                                                       \
        *DA_P_SIZEOF_ELEM_FROM_HANDLE(darr)))                                  \
    ), /* then */                                                              \
    /* if darr.tmp == NULL (i.e. if new_darr alloc failed) */                  \
    (*DA_P_TMP_PTR_FROM_HANDLE(darr) == NULL) ?                                \
        /* return NULL */                                                      \
        (NULL)                                                                 \
    : /* else */                                                               \
    (                                                                          \
        ((((( /* "then" paren(s) */                                            \
        /* darr.tmp.tmp = darr (i.e. new_darr.tmp = orig_darr) */              \
        (*DA_P_TMP_PTR_FROM_HANDLE(*DA_P_TMP_PTR_FROM_HANDLE(darr)) = (darr))  \
        ), /* then */                                                          \
        /* darr = darr.tmp (i.e. darr = new_darr) */                           \
        ((darr) = *DA_P_TMP_PTR_FROM_HANDLE(darr))                             \
        ), /* then */                                                          \
        /* darr[0:darr.length-1] = darr.tmp[0:darr.length-1] */                \
        memcpy(                                                                \
            (darr), /* dest == new_arr */                                      \
            (*DA_P_TMP_PTR_FROM_HANDLE(darr)), /* src == dest.tmp */           \
            (*DA_P_LENGTH_FROM_HANDLE(darr)) * /* n == darr.length*darr.elsz */\
                (*DA_P_SIZEOF_ELEM_FROM_HANDLE(darr)))                         \
        ), /* then */                                                          \
        /* darr[darr.length++] = value */                                      \
        ((darr)[(*DA_P_LENGTH_FROM_HANDLE(darr))++] = value)                   \
        ), /* then */                                                          \
        /* free darr.tmp (i.e. free orig_darr) */                              \
        free(DA_HEAD_FROM_HANDLE(*DA_P_TMP_PTR_FROM_HANDLE(darr)))             \
        ), /* then */                                                          \
        /* return darr (i.e. return new_darr) */                               \
        (darr)                                                                 \
    )                                                                          \
)                                                                              \
)

#define /* ARRAY TYPE */_da_pop(/* void* */darr)                               \
(                                                                              \
    (darr)[--(*DA_P_LENGTH_FROM_HANDLE(darr))]                                 \
)

#endif // !_DARRAY_H_
