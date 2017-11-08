#if __unix__ || __APPLE__ || __MACH__
#   define _EMU_ENABLE_COLOR_
#endif
#include <EMUtest.h>
#include "../darray.h"

#define INITIAL_NUM_ELEMS 5
#define RESIZE_NUM_ELEMS 100

#define EMPTY_STR ""
#define TEST_STR0 "foobar"
#define TEST_STR1 "Darray is the best library!"
#define TEST_FILE "./resources/testfile"

int cust_counter;
void* custom_malloc(size_t size)
{
    cust_counter++;
    return malloc(size);
}

void* custom_realloc(void* ptr, size_t size)
{
    cust_counter++;
    return realloc(ptr, size);
}

void custom_free(void* ptr)
{
    cust_counter++;
    free(ptr);
}

struct da_mem_funcs custom_mem_funcs = {
    .alloc_f=custom_malloc,
    .realloc_f=custom_realloc,
    .free_f=custom_free
};

EMU_TEST(da_length)
{
    struct _darray dastruct;

    dastruct = (struct _darray){._length=7};
    EMU_EXPECT_EQ_UINT(da_length(dastruct._data), 7);

    dastruct = (struct _darray){._length=42};
    EMU_EXPECT_EQ_UINT(da_length(dastruct._data), 42);

    EMU_END_TEST();
}

EMU_TEST(da_capacity)
{
    struct _darray dastruct;

    dastruct = (struct _darray){._capacity=7};
    EMU_EXPECT_EQ_UINT(da_capacity(dastruct._data), 7);

    dastruct = (struct _darray){._capacity=42};
    EMU_EXPECT_EQ_UINT(da_capacity(dastruct._data), 42);

    EMU_END_TEST();
}

EMU_TEST(da_sizeof_elem)
{
    struct _darray dastruct;

    dastruct = (struct _darray){._elemsz=4};
    EMU_EXPECT_EQ_UINT(da_sizeof_elem(dastruct._data), 4);

    dastruct = (struct _darray){._elemsz=32};
    EMU_EXPECT_EQ_UINT(da_sizeof_elem(dastruct._data), 32);

    EMU_END_TEST();
}

EMU_TEST(da_alloc__and__da_free)
{
    int* da = da_alloc(INITIAL_NUM_ELEMS, sizeof(int));
    EMU_REQUIRE_NOT_NULL(da);
    EMU_REQUIRE_EQ_UINT(da_length(da), INITIAL_NUM_ELEMS);
    EMU_REQUIRE_GE_UINT(da_capacity(da), INITIAL_NUM_ELEMS);
    EMU_REQUIRE_EQ_UINT(da_sizeof_elem(da), sizeof(int));

    for (size_t i = 0; i < INITIAL_NUM_ELEMS; ++i) // writiable without crashing
    {
        da[i] = i;
    }

    da_free(da);
    EMU_END_TEST();
}

EMU_TEST(da_alloc_exact__and__da_free)
{
    int* da = da_alloc_exact(INITIAL_NUM_ELEMS, sizeof(int));
    EMU_REQUIRE_NOT_NULL(da);
    EMU_REQUIRE_EQ_UINT(da_length(da), INITIAL_NUM_ELEMS);
    EMU_REQUIRE_EQ_UINT(da_capacity(da), INITIAL_NUM_ELEMS);
    EMU_REQUIRE_EQ_UINT(da_sizeof_elem(da), sizeof(int));

    for (size_t i = 0; i < INITIAL_NUM_ELEMS; ++i) // writiable without crashing
    {
        da[i] = i;
    }

    da_free(da);
    EMU_END_TEST();
}

EMU_TEST(da_alloc_custom__and__da_free)
{
    cust_counter = 0;
    int* da = da_alloc_custom(custom_mem_funcs, INITIAL_NUM_ELEMS, sizeof(int));
    EMU_REQUIRE_EQ_INT(cust_counter, 1);
    EMU_REQUIRE_NOT_NULL(da);
    EMU_REQUIRE_EQ_UINT(da_length(da), INITIAL_NUM_ELEMS);
    EMU_REQUIRE_GE_UINT(da_capacity(da), INITIAL_NUM_ELEMS);
    EMU_REQUIRE_EQ_UINT(da_sizeof_elem(da), sizeof(int));

    for (size_t i = 0; i < INITIAL_NUM_ELEMS; ++i) // writiable without crashing
    {
        da[i] = i;
    }

    da_free(da);
    EMU_REQUIRE_EQ(cust_counter, 2);
    EMU_END_TEST();
}

EMU_TEST(da_alloc_exact_custom__and__da_free)
{
    cust_counter = 0;
    int* da = da_alloc_exact_custom(custom_mem_funcs, INITIAL_NUM_ELEMS, sizeof(int));
    EMU_REQUIRE_EQ(cust_counter, 1);
    EMU_REQUIRE_NOT_NULL(da);
    EMU_REQUIRE_EQ_UINT(da_length(da), INITIAL_NUM_ELEMS);
    EMU_REQUIRE_EQ_UINT(da_capacity(da), INITIAL_NUM_ELEMS);
    EMU_REQUIRE_EQ_UINT(da_sizeof_elem(da), sizeof(int));

    for (size_t i = 0; i < INITIAL_NUM_ELEMS; ++i) // writiable without crashing
    {
        da[i] = i;
    }

    da_free(da);
    EMU_REQUIRE_EQ(cust_counter, 2);
    EMU_END_TEST();
}

EMU_GROUP(darray_alloc_and_free_functions)
{
    EMU_ADD(da_alloc__and__da_free);
    EMU_ADD(da_alloc_exact__and__da_free);
    EMU_ADD(da_alloc_custom__and__da_free);
    EMU_ADD(da_alloc_exact_custom__and__da_free);
    EMU_END_GROUP();
}

EMU_TEST(da_resize)
{
    int* da = da_alloc(INITIAL_NUM_ELEMS, sizeof(int));
    for (size_t i = 0; i < INITIAL_NUM_ELEMS; ++i)
    {
        da[i] = i;
    }

    da = da_resize(da, RESIZE_NUM_ELEMS);
    EMU_REQUIRE_NOT_NULL(da);
    EMU_EXPECT_EQ_UINT(da_length(da), RESIZE_NUM_ELEMS);
    EMU_EXPECT_GE_UINT(da_capacity(da), RESIZE_NUM_ELEMS);
    for (size_t i = 0; i < INITIAL_NUM_ELEMS; ++i)
    {
        EMU_EXPECT_EQ_INT(da[i], (int)i);
    }

    da_free(da);
    EMU_END_TEST();
}

EMU_TEST(da_resize_exact)
{
    int* da = da_alloc(INITIAL_NUM_ELEMS, sizeof(int));
    for (size_t i = 0; i < INITIAL_NUM_ELEMS; ++i)
    {
        da[i] = i;
    }

    da = da_resize_exact(da, RESIZE_NUM_ELEMS);
    EMU_REQUIRE_NOT_NULL(da);
    EMU_REQUIRE_EQ_UINT(da_length(da), RESIZE_NUM_ELEMS);
    EMU_REQUIRE_EQ_UINT(da_capacity(da), RESIZE_NUM_ELEMS);
    for (size_t i = 0; i < INITIAL_NUM_ELEMS; ++i)
    {
        EMU_EXPECT_EQ_INT(da[i], (int)i);
    }

    da_free(da);
    EMU_END_TEST();
}

EMU_TEST(da_resize_with_custom_memory_management)
{
    cust_counter = 0;
    int* da = da_alloc_custom(custom_mem_funcs, INITIAL_NUM_ELEMS, sizeof(int));
    EMU_REQUIRE_EQ_INT(cust_counter, 1);
    for (size_t i = 0; i < INITIAL_NUM_ELEMS; ++i)
    {
        da[i] = i;
    }

    da = da_resize(da, RESIZE_NUM_ELEMS);
    EMU_REQUIRE_EQ_INT(cust_counter, 2);
    EMU_REQUIRE_NOT_NULL(da);
    EMU_EXPECT_EQ_UINT(da_length(da), RESIZE_NUM_ELEMS);
    EMU_EXPECT_GE_UINT(da_capacity(da), RESIZE_NUM_ELEMS);
    for (size_t i = 0; i < INITIAL_NUM_ELEMS; ++i)
    {
        EMU_EXPECT_EQ_INT(da[i], (int)i);
    }

    da_free(da);
    EMU_REQUIRE_EQ_INT(cust_counter, 3);
    EMU_END_TEST();
}

EMU_TEST(da_reserve)
{
    int* da = da_alloc(1, sizeof(int));

    da = da_reserve(da, 5000);
    EMU_REQUIRE_NOT_NULL(da);
    EMU_REQUIRE_GE_UINT(da_capacity(da)-da_length(da), 5000);

    // Multiple calls to reserve with the same reserve nelem should only cause
    // the darray to change the first time.
    int* da2 = da_reserve(da, 50);
    EMU_REQUIRE_NOT_NULL(da);
    EMU_EXPECT_EQ(da2, da);

    da_free(da);
    EMU_END_TEST();
}

EMU_TEST(da_push)
{
    const int max_index = 15;
    int* da = da_alloc(0, sizeof(int));

    for (int i = 0; i <= max_index; ++i)
    {
        da_push(da, i);
        EMU_REQUIRE_NOT_NULL(da);
    }
    EMU_REQUIRE_EQ_UINT(da_length(da), max_index+1);
    EMU_REQUIRE_GE_UINT(da_capacity(da),da_length(da));
    for (int i = 0; i <= max_index; ++i)
    {
        EMU_EXPECT_EQ_INT(da[i], i);
    }

    da_free(da);
    EMU_END_TEST();
}

EMU_TEST(da_pop)
{
    int* da = da_alloc(2, sizeof(int));
    da[0] = 3;
    da[1] = 5;

    EMU_EXPECT_EQ_INT(da_pop(da), 5);
    EMU_REQUIRE_NOT_NULL(da);
    EMU_REQUIRE_EQ_UINT(da_length(da), 1);

    EMU_EXPECT_EQ_INT(da_pop(da), 3);
    EMU_REQUIRE_NOT_NULL(da);
    EMU_REQUIRE_EQ_UINT(da_length(da), 0);

    da_free(da);
    EMU_END_TEST();
}

EMU_TEST(da_insert__basic)
{
    int* da = da_alloc(2, sizeof(int));
    da[0] = 3;
    da[1] = 5;

    da = da_insert(da, 0, 7);
    EMU_REQUIRE_NOT_NULL(da);
    EMU_REQUIRE_EQ_UINT(da_length(da), 3);
    EMU_EXPECT_EQ_INT(da[0], 7);
    EMU_EXPECT_EQ_INT(da[1], 3);
    EMU_EXPECT_EQ_INT(da[2], 5);

    da = da_insert(da, 1, 9);
    EMU_REQUIRE_NOT_NULL(da);
    EMU_REQUIRE_EQ_UINT(da_length(da), 4);
    EMU_EXPECT_EQ_INT(da[0], 7);
    EMU_EXPECT_EQ_INT(da[1], 9);
    EMU_EXPECT_EQ_INT(da[2], 3);
    EMU_EXPECT_EQ_INT(da[3], 5);

    da_free(da);
    EMU_END_TEST();
}

EMU_TEST(da_insert__mimic_push_front)
{
    const int max_index = 15;
    int* da = da_alloc(0, sizeof(int));

    for (int i = max_index; i >= 0; --i)
    {
        da = da_insert(da, 0, i);
        EMU_REQUIRE_NOT_NULL(da);
    }
    EMU_REQUIRE_EQ_UINT(da_length(da), max_index+1);
    for (int i = max_index; i >= 0; --i)
    {
        EMU_EXPECT_EQ_INT(da[i], i);
    }

    da_free(da);
    EMU_END_TEST();
}

EMU_GROUP(da_insert)
{
    EMU_ADD(da_insert__basic);
    EMU_ADD(da_insert__mimic_push_front);
    EMU_END_GROUP();
}

EMU_TEST(da_insert_arr)
{
    int* da = da_alloc(2, sizeof(int));
    da[0] = 3;
    da[1] = 5;

    int A[] = {7, 9, 11};
    da = da_insert_arr(da, 0, A, 3);
    EMU_REQUIRE_NOT_NULL(da);
    EMU_REQUIRE_EQ_UINT(da_length(da), 5);
    EMU_EXPECT_EQ_INT(da[0], 7);
    EMU_EXPECT_EQ_INT(da[1], 9);
    EMU_EXPECT_EQ_INT(da[2], 11);
    EMU_EXPECT_EQ_INT(da[3], 3);
    EMU_EXPECT_EQ_INT(da[4], 5);

    int B[] = {13, 15};
    da = da_insert_arr(da, 1, B, 2);
    EMU_REQUIRE_NOT_NULL(da);
    EMU_REQUIRE_EQ_UINT(da_length(da), 7);
    EMU_EXPECT_EQ_INT(da[0], 7);
    EMU_EXPECT_EQ_INT(da[1], 13);
    EMU_EXPECT_EQ_INT(da[2], 15);
    EMU_EXPECT_EQ_INT(da[3], 9);
    EMU_EXPECT_EQ_INT(da[4], 11);
    EMU_EXPECT_EQ_INT(da[5], 3);
    EMU_EXPECT_EQ_INT(da[6], 5);

    da = da_insert_arr(da, 0, NULL, 0);
    EMU_REQUIRE_NOT_NULL(da);
    EMU_REQUIRE_EQ_UINT(da_length(da), 7);

    da_free(da);
    EMU_END_TEST();
}

EMU_TEST(da_remove)
{
    int* da1 = da_alloc(4, sizeof(int));
    da1[0] = 3;
    da1[1] = 5;
    da1[2] = 7;
    da1[3] = 9;

    // remove from middle
    EMU_EXPECT_EQ_INT(da_remove(da1, 1), 5);
    EMU_REQUIRE_NOT_NULL(da1);
    EMU_EXPECT_EQ_UINT(da_length(da1), 3);

    // remove from front
    EMU_EXPECT_EQ_INT(da_remove(da1, 0), 3);
    EMU_REQUIRE_NOT_NULL(da1);
    EMU_EXPECT_EQ_UINT(da_length(da1), 2);

    // remove from back
    EMU_EXPECT_EQ_INT(da_remove(da1, 1), 9);
    EMU_REQUIRE_NOT_NULL(da1);
    EMU_EXPECT_EQ_UINT(da_length(da1), 1);

    int* da2 = da_alloc(4, sizeof(int));
    da_remove(da2, 1); // remove from middle
    da_remove(da2, 0); // remove from front
    da_remove(da2, 1); // remove from back

    da_free(da1);
    da_free(da2);
    EMU_END_TEST();
}

EMU_TEST(da_remove_arr)
{
    int* da = da_alloc(6, sizeof(int));
    for (size_t i = 0; i < da_length(da); ++i)
        da[i] = i;

    da_remove_arr(da, 2, 3);
    EMU_REQUIRE_NOT_NULL(da);
    EMU_REQUIRE_EQ_UINT(da_length(da), 3);
    EMU_EXPECT_EQ_INT(da[0], 0);
    EMU_EXPECT_EQ_INT(da[1], 1);
    EMU_EXPECT_EQ_INT(da[2], 5);

    da_remove_arr(da, 0, 0);
    EMU_REQUIRE_NOT_NULL(da);
    EMU_REQUIRE_EQ_UINT(da_length(da), 3);

    EMU_END_TEST();
}

EMU_TEST(da_swap)
{
    int* da = da_alloc(INITIAL_NUM_ELEMS, sizeof(int));

    da[3] = 12;
    da[5] = 99;

    da_swap(da, 3, 5);
    EMU_REQUIRE_EQ_INT(da[3], 99);
    EMU_REQUIRE_EQ_INT(da[5], 12);

    da_swap(da, 3, 5);
    EMU_REQUIRE_EQ_INT(da[3], 12);
    EMU_REQUIRE_EQ_INT(da[5], 99);

    // swap element with itself
    da_swap(da, 3, 3);
    EMU_REQUIRE_EQ_INT(da[3], 12);
    EMU_REQUIRE_EQ_INT(da[5], 99);

    da_free(da);
    EMU_END_TEST();
}

EMU_TEST(da_concat__darray_cat)
{
    int* src = da_alloc(2, sizeof(int));
    src[0] = 3;
    src[1] = 4;

    int* dest = da_alloc(3, sizeof(int));
    dest[0] = 0;
    dest[1] = 1;
    dest[2] = 2;

    dest = da_concat(dest, src, 2);
    EMU_REQUIRE_NOT_NULL(dest);
    EMU_REQUIRE_NOT_NULL(src);
    EMU_REQUIRE_EQ_UINT(da_length(dest), 5);
    for (size_t i = 0; i < 5; ++i)
    {
        EMU_EXPECT_EQ_INT(dest[i], i);
    }

    da_free(src);
    da_free(dest);
    EMU_END_TEST();
}

EMU_TEST(da_concat__array_cat)
{
    int src[] = {3, 4};

    int* dest = da_alloc(3, sizeof(int));
    dest[0] = 0;
    dest[1] = 1;
    dest[2] = 2;

    dest = da_concat(dest, src, 2);
    EMU_REQUIRE_NOT_NULL(dest);
    EMU_REQUIRE_NOT_NULL(src);
    EMU_REQUIRE_EQ_UINT(da_length(dest), 5);
    for (size_t i = 0; i < 5; ++i)
    {
        EMU_EXPECT_EQ_INT(dest[i], i);
    }

    da_free(dest);
    EMU_END_TEST();
}

EMU_TEST(da_concat__cstring_cat)
{
    char* src = "World!";

    char* dest = da_alloc(strlen("Hello "), sizeof(char));
    memcpy(dest, "Hello ", strlen("Hello "));

    dest = da_concat(dest, src, strlen(src)+1);
    EMU_PRINT_INDENT(); printf("%s\n", dest);
    EMU_EXPECT_STREQ(dest, "Hello World!");
    EMU_EXPECT_EQ_UINT(da_length(dest), strlen("Hello World!")+1);

    da_pop(dest); // remove null terminator

    char another[] = " Another one!";
    dest = da_concat(dest, another, strlen(another)+1);
    EMU_PRINT_INDENT(); printf("%s\n", dest);
    EMU_EXPECT_STREQ(dest, "Hello World! Another one!");
    EMU_EXPECT_EQ_UINT(da_length(dest), strlen("Hello World! Another one!")+1);

    da_free(dest);
    EMU_END_TEST();
}

EMU_GROUP(da_concat)
{
    EMU_ADD(da_concat__darray_cat);
    EMU_ADD(da_concat__array_cat);
    EMU_ADD(da_concat__cstring_cat);
    EMU_END_GROUP();
}

EMU_TEST(da_fill__const_value)
{
    int* da = da_alloc(INITIAL_NUM_ELEMS, sizeof(int));

    // prefill with different values
    for (size_t i = 0; i < da_length(da); ++i)
    {
        da[i] = i;
    }

    da_fill(da, 12 + 3);
    for (size_t i = 0; i < da_length(da); ++i)
    {
        EMU_EXPECT_EQ_INT(da[i], 15);
    }

    da_free(da);
    EMU_END_TEST();
}

EMU_TEST(da_fill__rand_value)
{
    int* da = da_alloc(INITIAL_NUM_ELEMS, sizeof(int));

    // prefill with different values
    for (size_t i = 0; i < da_length(da); ++i)
    {
        da[i] = i;
    }

    da_fill(da, rand());
    for (size_t i = 1; i < da_length(da); ++i)
    {
        EMU_EXPECT_EQ_INT(da[i], da[i-1]);
    }

    da_free(da);
    EMU_END_TEST();
}

EMU_GROUP(da_fill)
{
    EMU_ADD(da_fill__const_value);
    EMU_ADD(da_fill__rand_value);
    EMU_END_GROUP();
}

EMU_TEST(da_foreach__iterates_through_all_elements)
{
    int* da = da_alloc(INITIAL_NUM_ELEMS, sizeof(int));

    for (size_t i = 0; i < da_length(da); ++i){da[i] = i;}
    da_foreach(da, iter)
    {
        *iter += 1;
    }
    for (size_t i = 0; i < da_length(da); ++i)
    {
        EMU_EXPECT_EQ_INT(da[i], i + 1);
    }

    da_free(da);
    EMU_END_TEST();
}

EMU_TEST(da_foreach__iterates_forward)
{
    int* da = da_alloc(INITIAL_NUM_ELEMS, sizeof(int));

    for (size_t i = 0; i < da_length(da); ++i){da[i] = i;}
    int last = -1;
    da_foreach(da, iter)
    {
        EMU_EXPECT_GE_INT(*iter, last);
        last = *iter;
    }

    da_free(da);
    EMU_END_TEST();
}

EMU_TEST(da_foreach__iterates_once_per_element)
{
    int* da = da_alloc(INITIAL_NUM_ELEMS, sizeof(int));

    unsigned counter = 0;
    da_foreach(da, iter)
    {
        counter += 1;
    }
    EMU_EXPECT_EQ_UINT(counter, INITIAL_NUM_ELEMS);

    da_free(da);
    EMU_END_TEST();
}

EMU_TEST(da_foreach__nested_darrays)
{
    int** nested = da_alloc_exact(INITIAL_NUM_ELEMS, sizeof(int*));
    da_foreach(nested, iter)
    {
        *iter = da_alloc(INITIAL_NUM_ELEMS, sizeof(int*));
    }

    unsigned counter = 0;
    da_foreach(nested, row)
    {
        da_foreach(*row, col)
        {
            counter += 1;
        }
    }
    EMU_EXPECT_EQ_UINT(counter, INITIAL_NUM_ELEMS*INITIAL_NUM_ELEMS);

    da_foreach(nested, iter)
    {
        da_free(*iter);
    }
    da_free(nested);
    EMU_END_TEST();
}

EMU_GROUP(da_foreach)
{
    EMU_ADD(da_foreach__iterates_through_all_elements);
    EMU_ADD(da_foreach__iterates_forward);
    EMU_ADD(da_foreach__iterates_once_per_element);
    EMU_ADD(da_foreach__nested_darrays);
    EMU_END_GROUP();
}

EMU_TEST(container_style_type)
{
    int* da = da_alloc(INITIAL_NUM_ELEMS, sizeof(int));
    darray(int) da2 = da; // check for no compiler warnings

    da_free(da2);
    EMU_END_TEST();
}

EMU_GROUP(darray_functions)
{
    EMU_ADD(da_length);
    EMU_ADD(da_capacity);
    EMU_ADD(da_sizeof_elem);
    EMU_ADD(darray_alloc_and_free_functions);
    EMU_ADD(da_resize);
    EMU_ADD(da_resize_exact);
    EMU_ADD(da_resize_with_custom_memory_management);
    EMU_ADD(da_reserve);
    EMU_ADD(da_push);
    EMU_ADD(da_pop);
    EMU_ADD(da_insert);
    EMU_ADD(da_insert_arr);
    EMU_ADD(da_remove);
    EMU_ADD(da_remove_arr);
    EMU_ADD(da_swap);
    EMU_ADD(da_concat);
    EMU_ADD(da_fill);
    EMU_ADD(da_foreach);
    EMU_ADD(container_style_type);
    EMU_END_GROUP();
}

EMU_TEST(dstr_alloc_empty__and__dstr_free);
{
    char* dstr = dstr_alloc_empty();
    EMU_REQUIRE_NOT_NULL(dstr);
    EMU_REQUIRE_GE_UINT(da_capacity(dstr), da_length(dstr));
    EMU_REQUIRE_EQ_UINT(strlen(dstr), 0);
    EMU_REQUIRE_EQ_UINT(da_length(dstr), strlen(dstr)+1);
    EMU_REQUIRE_STREQ(dstr, EMPTY_STR);
    dstr_free(dstr);
    EMU_END_TEST();
}

EMU_TEST(dstr_alloc_empty_custom__and__dstr_free)
{
    cust_counter = 0;
    char* dstr = dstr_alloc_empty_custom(custom_mem_funcs);
    EMU_REQUIRE_NOT_NULL(dstr);
    EMU_REQUIRE_GE_UINT(da_capacity(dstr), da_length(dstr));
    EMU_REQUIRE_EQ_UINT(strlen(dstr), 0);
    EMU_REQUIRE_EQ_UINT(da_length(dstr), strlen(dstr)+1);
    EMU_REQUIRE_STREQ(dstr, EMPTY_STR);
    dstr_free(dstr);
    EMU_REQUIRE_EQ(cust_counter, 2);
    EMU_END_TEST();
}

EMU_TEST(dstr_alloc_cstr__and__dstr_free);
{
    char* dstr = dstr_alloc_cstr(EMPTY_STR);
    EMU_REQUIRE_NOT_NULL(dstr);
    EMU_REQUIRE_GE_UINT(da_capacity(dstr), da_length(dstr));
    EMU_REQUIRE_EQ_UINT(strlen(dstr), 0);
    EMU_REQUIRE_EQ_UINT(da_length(dstr), strlen(dstr)+1);
    EMU_REQUIRE_STREQ(dstr, EMPTY_STR);
    dstr_free(dstr);

    dstr = dstr_alloc_cstr(TEST_STR0);
    EMU_REQUIRE_NOT_NULL(dstr);
    EMU_REQUIRE_GE_UINT(da_capacity(dstr), da_length(dstr));
    EMU_REQUIRE_EQ_UINT(strlen(dstr), strlen(TEST_STR0));
    EMU_REQUIRE_EQ_UINT(da_length(dstr), strlen(dstr)+1);
    EMU_REQUIRE_STREQ(dstr, TEST_STR0);
    dstr_free(dstr);

    dstr = dstr_alloc_cstr(TEST_STR1);
    EMU_REQUIRE_NOT_NULL(dstr);
    EMU_REQUIRE_GE_UINT(da_capacity(dstr), da_length(dstr));
    EMU_REQUIRE_EQ_UINT(strlen(dstr), strlen(TEST_STR1));
    EMU_REQUIRE_EQ_UINT(da_length(dstr), strlen(dstr)+1);
    EMU_REQUIRE_STREQ(dstr, TEST_STR1);
    dstr_free(dstr);

    EMU_END_TEST();
}

EMU_TEST(dstr_alloc_cstr_custom__and__dstr_free)
{
    cust_counter = 0;

    char* dstr = dstr_alloc_cstr_custom(custom_mem_funcs, EMPTY_STR);
    EMU_REQUIRE_NOT_NULL(dstr);
    EMU_REQUIRE_GE_UINT(da_capacity(dstr), da_length(dstr));
    EMU_REQUIRE_EQ_UINT(strlen(dstr), 0);
    EMU_REQUIRE_EQ_UINT(da_length(dstr), strlen(dstr)+1);
    EMU_REQUIRE_STREQ(dstr, EMPTY_STR);
    dstr_free(dstr);
    EMU_REQUIRE_EQ(cust_counter, 2);

    dstr = dstr_alloc_cstr_custom(custom_mem_funcs, TEST_STR0);
    EMU_REQUIRE_NOT_NULL(dstr);
    EMU_REQUIRE_GE_UINT(da_capacity(dstr), da_length(dstr));
    EMU_REQUIRE_EQ_UINT(strlen(dstr), strlen(TEST_STR0));
    EMU_REQUIRE_EQ_UINT(da_length(dstr), strlen(dstr)+1);
    EMU_REQUIRE_STREQ(dstr, TEST_STR0);
    dstr_free(dstr);
    EMU_REQUIRE_EQ(cust_counter, 4);

    dstr = dstr_alloc_cstr_custom(custom_mem_funcs, TEST_STR1);
    EMU_REQUIRE_NOT_NULL(dstr);
    EMU_REQUIRE_GE_UINT(da_capacity(dstr), da_length(dstr));
    EMU_REQUIRE_EQ_UINT(strlen(dstr), strlen(TEST_STR1));
    EMU_REQUIRE_EQ_UINT(da_length(dstr), strlen(dstr)+1);
    EMU_REQUIRE_STREQ(dstr, TEST_STR1);
    dstr_free(dstr);
    EMU_REQUIRE_EQ(cust_counter, 6);

    EMU_END_TEST();
}

EMU_TEST(dstr_alloc_dstr__and__dstr_free);
{
    char* src = dstr_alloc_cstr(EMPTY_STR);
    char* dest = dstr_alloc_dstr(src);
    EMU_REQUIRE_NOT_NULL(dest);
    EMU_REQUIRE_GE_UINT(da_capacity(dest), da_length(dest));
    EMU_REQUIRE_EQ_UINT(strlen(dest), 0);
    EMU_REQUIRE_EQ_UINT(da_length(dest), strlen(dest)+1);
    EMU_REQUIRE_STREQ(dest, EMPTY_STR);
    dstr_free(src);
    dstr_free(dest);

    src = dstr_alloc_cstr(TEST_STR0);
    dest = dstr_alloc_dstr(src);
    EMU_REQUIRE_NOT_NULL(dest);
    EMU_REQUIRE_GE_UINT(da_capacity(dest), da_length(dest));
    EMU_REQUIRE_EQ_UINT(strlen(dest), strlen(TEST_STR0));
    EMU_REQUIRE_EQ_UINT(da_length(dest), strlen(dest)+1);
    EMU_REQUIRE_STREQ(dest, TEST_STR0);
    dstr_free(src);
    dstr_free(dest);

    src = dstr_alloc_cstr(TEST_STR1);
    dest = dstr_alloc_dstr(src);
    EMU_REQUIRE_NOT_NULL(dest);
    EMU_REQUIRE_GE_UINT(da_capacity(dest), da_length(dest));
    EMU_REQUIRE_EQ_UINT(strlen(dest), strlen(TEST_STR1));
    EMU_REQUIRE_EQ_UINT(da_length(dest), strlen(dest)+1);
    EMU_REQUIRE_STREQ(dest, TEST_STR1);
    dstr_free(src);
    dstr_free(dest);

    EMU_END_TEST();
}

EMU_TEST(dstr_alloc_dstr_custom__and__dstr_free)
{
    cust_counter = 0;

    char* src = dstr_alloc_cstr(EMPTY_STR);
    char* dest = dstr_alloc_dstr_custom(custom_mem_funcs, src);
    EMU_REQUIRE_NOT_NULL(dest);
    EMU_REQUIRE_GE_UINT(da_capacity(dest), da_length(dest));
    EMU_REQUIRE_EQ_UINT(strlen(dest), 0);
    EMU_REQUIRE_EQ_UINT(da_length(dest), strlen(dest)+1);
    EMU_REQUIRE_STREQ(dest, EMPTY_STR);
    dstr_free(src);
    dstr_free(dest);
    EMU_REQUIRE_EQ(cust_counter, 2);

    src = dstr_alloc_cstr(TEST_STR0);
    dest = dstr_alloc_dstr_custom(custom_mem_funcs, src);
    EMU_REQUIRE_NOT_NULL(dest);
    EMU_REQUIRE_GE_UINT(da_capacity(dest), da_length(dest));
    EMU_REQUIRE_EQ_UINT(strlen(dest), strlen(TEST_STR0));
    EMU_REQUIRE_EQ_UINT(da_length(dest), strlen(dest)+1);
    EMU_REQUIRE_STREQ(dest, TEST_STR0);
    dstr_free(src);
    dstr_free(dest);
    EMU_REQUIRE_EQ(cust_counter, 4);

    src = dstr_alloc_cstr(TEST_STR1);
    dest = dstr_alloc_dstr_custom(custom_mem_funcs, src);
    EMU_REQUIRE_NOT_NULL(dest);
    EMU_REQUIRE_GE_UINT(da_capacity(dest), da_length(dest));
    EMU_REQUIRE_EQ_UINT(strlen(dest), strlen(TEST_STR1));
    EMU_REQUIRE_EQ_UINT(da_length(dest), strlen(dest)+1);
    EMU_REQUIRE_STREQ(dest, TEST_STR1);
    dstr_free(src);
    dstr_free(dest);
    EMU_REQUIRE_EQ(cust_counter, 6);

    EMU_END_TEST();
}

EMU_TEST(dstr_alloc_format__and__dstr_free)
{
    char* dstr = dstr_alloc_format("%d %s", 5, "foo");
    EMU_REQUIRE_NOT_NULL(dstr);
    EMU_REQUIRE_GE_UINT(da_capacity(dstr), da_length(dstr));
    EMU_REQUIRE_EQ_UINT(strlen(dstr), 5);
    EMU_REQUIRE_EQ_UINT(da_length(dstr), strlen(dstr)+1);
    EMU_REQUIRE_STREQ(dstr, "5 foo");
    dstr_free(dstr);
    EMU_END_TEST();
}

EMU_TEST(dstr_alloc_format_custom__and__dstr_free)
{
    cust_counter = 0;
    char* dstr = dstr_alloc_format_custom(custom_mem_funcs, "%d %s", 5, "foo");
    EMU_REQUIRE_NOT_NULL(dstr);
    EMU_REQUIRE_GE_UINT(da_capacity(dstr), da_length(dstr));
    EMU_REQUIRE_EQ_UINT(strlen(dstr), 5);
    EMU_REQUIRE_EQ_UINT(da_length(dstr), strlen(dstr)+1);
    EMU_REQUIRE_STREQ(dstr, "5 foo");
    dstr_free(dstr);
    EMU_REQUIRE_EQ(cust_counter, 2);
    EMU_END_TEST();
}

EMU_GROUP(dstring_alloc_and_free_functions)
{
    EMU_ADD(dstr_alloc_empty__and__dstr_free);
    EMU_ADD(dstr_alloc_empty_custom__and__dstr_free);
    EMU_ADD(dstr_alloc_cstr__and__dstr_free);
    EMU_ADD(dstr_alloc_cstr_custom__and__dstr_free);
    EMU_ADD(dstr_alloc_dstr__and__dstr_free);
    EMU_ADD(dstr_alloc_dstr_custom__and__dstr_free);
    EMU_ADD(dstr_alloc_format__and__dstr_free);
    EMU_ADD(dstr_alloc_format_custom__and__dstr_free);
    EMU_END_GROUP();
}

#define FIRST_STR   "string"
#define SHORTER_STR "str"
#define LONGER_STR  "longer string"

EMU_TEST(dstr_reassign_empty)
{
    char* dstr = dstr_alloc_cstr(FIRST_STR);
    dstr = dstr_reassign_empty(dstr);
    EMU_REQUIRE_NOT_NULL(dstr);
    EMU_REQUIRE_GE_UINT(da_capacity(dstr), da_length(dstr));
    EMU_REQUIRE_EQ_UINT(strlen(dstr), 0);
    EMU_REQUIRE_EQ_UINT(da_length(dstr), strlen(dstr)+1);
    EMU_REQUIRE_STREQ(dstr, EMPTY_STR);
    dstr_free(dstr);
    EMU_END_TEST();
}

EMU_TEST(dstr_reassign_cstr)
{
    char* dstr = dstr_alloc_cstr(FIRST_STR);

    dstr = dstr_reassign_cstr(dstr, SHORTER_STR);
    EMU_REQUIRE_NOT_NULL(dstr);
    EMU_REQUIRE_GE_UINT(da_capacity(dstr), da_length(dstr));
    EMU_REQUIRE_EQ_UINT(strlen(dstr), strlen(SHORTER_STR));
    EMU_REQUIRE_EQ_UINT(da_length(dstr), strlen(dstr)+1);
    EMU_REQUIRE_STREQ(dstr, SHORTER_STR);

    dstr = dstr_reassign_cstr(dstr, LONGER_STR);
    EMU_REQUIRE_NOT_NULL(dstr);
    EMU_REQUIRE_GE_UINT(da_capacity(dstr), da_length(dstr));
    EMU_REQUIRE_EQ_UINT(strlen(dstr), strlen(LONGER_STR));
    EMU_REQUIRE_EQ_UINT(da_length(dstr), strlen(dstr)+1);
    EMU_REQUIRE_STREQ(dstr, LONGER_STR);

    dstr_free(dstr);
    EMU_END_TEST();
}

EMU_TEST(dstr_reassign_dstr)
{
    char* dstr = dstr_alloc_cstr(FIRST_STR);
    char* src;

    src = dstr_alloc_cstr(SHORTER_STR);
    dstr = dstr_reassign_dstr(dstr, src);
    EMU_REQUIRE_NOT_NULL(dstr);
    EMU_REQUIRE_GE_UINT(da_capacity(dstr), da_length(dstr));
    EMU_REQUIRE_EQ_UINT(strlen(dstr), strlen(SHORTER_STR));
    EMU_REQUIRE_EQ_UINT(da_length(dstr), strlen(dstr)+1);
    EMU_REQUIRE_STREQ(dstr, SHORTER_STR);
    dstr_free(src);

    src = dstr_alloc_cstr(LONGER_STR);
    dstr = dstr_reassign_dstr(dstr, src);
    EMU_REQUIRE_NOT_NULL(dstr);
    EMU_REQUIRE_GE_UINT(da_capacity(dstr), da_length(dstr));
    EMU_REQUIRE_EQ_UINT(strlen(dstr), strlen(LONGER_STR));
    EMU_REQUIRE_EQ_UINT(da_length(dstr), strlen(dstr)+1);
    EMU_REQUIRE_STREQ(dstr, LONGER_STR);
    dstr_free(src);

    dstr_free(dstr);
    EMU_END_TEST();
}

EMU_TEST(dstr_reassign_format)
{
    char* dstr = dstr_alloc_cstr(FIRST_STR);
    dstr = dstr_reassign_format(dstr, "%d %s", 5, "foo");
    EMU_REQUIRE_NOT_NULL(dstr);
    EMU_REQUIRE_GE_UINT(da_capacity(dstr), da_length(dstr));
    EMU_REQUIRE_EQ_UINT(strlen(dstr), 5);
    EMU_REQUIRE_EQ_UINT(da_length(dstr), strlen(dstr)+1);
    dstr_free(dstr);
    EMU_END_TEST();
}

EMU_GROUP(dstring_reassignment_functions)
{
    EMU_ADD(dstr_reassign_empty);
    EMU_ADD(dstr_reassign_cstr);
    EMU_ADD(dstr_reassign_dstr);
    EMU_ADD(dstr_reassign_format);
    EMU_END_GROUP();
}

EMU_TEST(dstr_length)
{
    char* dstr = dstr_alloc_empty();
    EMU_EXPECT_EQ_UINT(dstr_length(dstr), 0);
    dstr_free(dstr);
    dstr = dstr_alloc_cstr(TEST_STR0);
    EMU_EXPECT_EQ_UINT(dstr_length(dstr), strlen(TEST_STR0));
    EMU_EXPECT_EQ_UINT(dstr_length(dstr), da_length(dstr)-1);
    dstr_free(dstr);
    EMU_END_TEST();
}

EMU_TEST(dstr_concat_char)
{
    char* dstr = dstr_alloc_cstr(TEST_STR0);

    dstr = dstr_concat_char(dstr, 'A');
    EMU_REQUIRE_NOT_NULL(dstr);
    EMU_REQUIRE_EQ_UINT(strlen(dstr), strlen(TEST_STR0)+1);
    EMU_REQUIRE_EQ_UINT(dstr_length(dstr), strlen(TEST_STR0)+1);
    char cmpstr[100] = TEST_STR0;
    strcat(cmpstr, "A");
    EMU_REQUIRE_STREQ(dstr, cmpstr);

    dstr = dstr_concat_char(dstr, 'B');
    EMU_REQUIRE_NOT_NULL(dstr);
    EMU_REQUIRE_EQ_UINT(strlen(dstr), strlen(TEST_STR0)+2);
    EMU_REQUIRE_EQ_UINT(dstr_length(dstr), strlen(TEST_STR0)+2);
    strcat(cmpstr, "B");
    EMU_REQUIRE_STREQ(dstr, cmpstr);

    dstr_free(dstr);
    EMU_END_TEST();
}

EMU_TEST(dstr_concat_cstr)
{
    char* dstr = dstr_alloc_cstr(TEST_STR0);

    dstr = dstr_concat_cstr(dstr, TEST_STR1);
    EMU_REQUIRE_NOT_NULL(dstr);
    EMU_REQUIRE_EQ_UINT(strlen(dstr), strlen(TEST_STR0 TEST_STR1));
    EMU_REQUIRE_EQ_UINT(dstr_length(dstr), strlen(TEST_STR0 TEST_STR1));
    EMU_REQUIRE_STREQ(dstr, TEST_STR0 TEST_STR1);

    dstr = dstr_concat_cstr(dstr, TEST_STR1);
    EMU_REQUIRE_NOT_NULL(dstr);
    EMU_REQUIRE_EQ_UINT(strlen(dstr), strlen(TEST_STR0 TEST_STR1 TEST_STR1));
    EMU_REQUIRE_EQ_UINT(dstr_length(dstr), strlen(TEST_STR0 TEST_STR1 TEST_STR1));
    EMU_REQUIRE_STREQ(dstr, TEST_STR0 TEST_STR1 TEST_STR1);

    dstr_free(dstr);
    EMU_END_TEST();
}

EMU_TEST(dstr_concat_dstr)
{
    char* dest = dstr_alloc_cstr(TEST_STR0);
    char* src = dstr_alloc_cstr(TEST_STR1);

    dest = dstr_concat_dstr(dest, src);
    EMU_REQUIRE_NOT_NULL(dest);
    EMU_REQUIRE_EQ_UINT(strlen(dest), strlen(TEST_STR0 TEST_STR1));
    EMU_REQUIRE_EQ_UINT(dstr_length(dest), strlen(TEST_STR0 TEST_STR1));
    EMU_REQUIRE_STREQ(dest, TEST_STR0 TEST_STR1);

    dest = dstr_concat_dstr(dest, src);
    EMU_REQUIRE_NOT_NULL(dest);
    EMU_REQUIRE_EQ_UINT(strlen(dest), strlen(TEST_STR0 TEST_STR1 TEST_STR1));
    EMU_REQUIRE_EQ_UINT(dstr_length(dest), strlen(TEST_STR0 TEST_STR1 TEST_STR1));
    EMU_REQUIRE_STREQ(dest, TEST_STR0 TEST_STR1 TEST_STR1);

    dstr_free(dest);
    dstr_free(src);
    EMU_END_TEST();
}

EMU_TEST(dstr_concat_format)
{
    char* dstr = dstr_alloc_cstr(TEST_STR0);

    dstr = dstr_concat_format(dstr, "%sA", TEST_STR1);
    EMU_REQUIRE_NOT_NULL(dstr);
    EMU_REQUIRE_EQ_UINT(strlen(dstr), strlen(TEST_STR0 TEST_STR1 "A"));
    EMU_REQUIRE_EQ_UINT(dstr_length(dstr), strlen(TEST_STR0 TEST_STR1 "A"));
    EMU_REQUIRE_STREQ(dstr, TEST_STR0 TEST_STR1 "A");

    dstr_free(dstr);
    EMU_END_TEST();
}

EMU_GROUP(dstr_concat_functions)
{
    EMU_ADD(dstr_concat_char);
    EMU_ADD(dstr_concat_cstr);
    EMU_ADD(dstr_concat_dstr);
    EMU_ADD(dstr_concat_format);
    EMU_END_GROUP();
}

EMU_TEST(dstr_cmp)
{
    char A[] = "ABCD A";
    char B[] = "ABCD B";
    EMU_EXPECT_EQ_INT(dstr_cmp(A, B), -1);
    EMU_EXPECT_EQ_INT(dstr_cmp(A, A), -0);
    EMU_EXPECT_EQ_INT(dstr_cmp(B, A), 1);
    EMU_END_TEST();
}

EMU_TEST(dstr_cmp_case)
{
    char A[] = "ABCD A";
    char B[] = "ABCD B";
    EMU_EXPECT_EQ_INT(dstr_cmp_case(A, B), -1);
    EMU_EXPECT_EQ_INT(dstr_cmp_case(A, A), -0);
    EMU_EXPECT_EQ_INT(dstr_cmp_case(B, A), 1);

    char C[] = "some string";
    char D[] = "SOME STRING";
    EMU_EXPECT_EQ_INT(dstr_cmp_case(C, D), 0);

    char E[] = "sOmE sTrInG";
    char F[] = "SoMe StRiNg";
    EMU_EXPECT_EQ_INT(dstr_cmp_case(E, F), 0);

    EMU_END_TEST();
}

EMU_GROUP(dstr_cmp_functions)
{
    EMU_ADD(dstr_cmp);
    EMU_ADD(dstr_cmp_case);
    EMU_END_GROUP();
}

EMU_TEST(dstr_find)
{
    darray(char) dstr = dstr_alloc_cstr("Hello, World!");
    EMU_EXPECT_EQ_INT(dstr_find(dstr, "Hello"), 0);
    EMU_EXPECT_EQ_INT(dstr_find(dstr, "hello"), -1);
    EMU_EXPECT_EQ_INT(dstr_find(dstr, "World!"), 7);
    dstr_free(dstr);
    EMU_END_TEST();
}

EMU_TEST(dstr_find_case)
{
    darray(char) dstr = dstr_alloc_cstr("Hello, World!");
    EMU_EXPECT_EQ_INT(dstr_find_case(dstr, "Hello"), 0);
    EMU_EXPECT_EQ_INT(dstr_find_case(dstr, "hello"), 0);
    EMU_EXPECT_EQ_INT(dstr_find_case(dstr, "World!"), 7);
    EMU_EXPECT_EQ_INT(dstr_find_case(dstr, "WoRlD!"), 7);
    dstr_free(dstr);
    EMU_END_TEST();
}

EMU_GROUP(dstr_find_functions)
{
    EMU_ADD(dstr_find);
    EMU_ADD(dstr_find_case);
    EMU_END_GROUP();
}

EMU_TEST(dstr_replace_all)
{
    char some_str[] = "Hello, World! Hello again.";
    darray(char) dstr = dstr_alloc_cstr(some_str);
    dstr = dstr_replace_all(dstr, "world", "foo");
    EMU_EXPECT_STREQ(dstr, some_str);
    dstr = dstr_replace_all(dstr, "Hello", "foo");
    EMU_EXPECT_STREQ(dstr, "foo, World! foo again.");
    dstr_free(dstr);
    EMU_END_TEST();
}

EMU_TEST(dstr_replace_all_case)
{
    char some_str[] = "Hello, World! Hello again.";
    darray(char) dstr = dstr_alloc_cstr(some_str);
    dstr = dstr_replace_all_case(dstr, "world", "foo");
    EMU_EXPECT_STREQ(dstr, "Hello, foo! Hello again.");
    dstr = dstr_replace_all_case(dstr, "Hello", "foo");
    EMU_EXPECT_STREQ(dstr, "foo, foo! foo again.");
    dstr_free(dstr);
    EMU_END_TEST();
}

EMU_GROUP(dstr_replace_functions)
{
    EMU_ADD(dstr_replace_all);
    EMU_ADD(dstr_replace_all_case);
    EMU_END_GROUP();
}

EMU_TEST(dstr_transform_lower)
{
    char* dstr = dstr_alloc_cstr("ALL UPPER");
    dstr_transform_lower(dstr);
    EMU_EXPECT_STREQ(dstr, "all upper");
    dstr_free(dstr);

    dstr = dstr_alloc_cstr("mIXeD CaSE123");
    dstr_transform_lower(dstr);
    EMU_EXPECT_STREQ(dstr, "mixed case123");
    dstr_free(dstr);

    EMU_END_TEST();
}

EMU_TEST(dstr_transform_upper)
{
    char* dstr = dstr_alloc_cstr("all lower");
    dstr_transform_upper(dstr);
    EMU_EXPECT_STREQ(dstr, "ALL LOWER");
    dstr_free(dstr);

    dstr = dstr_alloc_cstr("mIXeD CaSE123");
    dstr_transform_upper(dstr);
    EMU_EXPECT_STREQ(dstr, "MIXED CASE123");
    dstr_free(dstr);

    EMU_END_TEST();
}

EMU_GROUP(dstr_transform_functions)
{
    EMU_ADD(dstr_transform_lower);
    EMU_ADD(dstr_transform_upper);
    EMU_END_GROUP();
}

EMU_TEST(dstr_getline)
{
    FILE* fp = fopen(TEST_FILE, "r");
    EMU_REQUIRE_NOT_NULL(fp);
    char* dstr = dstr_alloc_empty();

    dstr = dstr_getline(dstr, fp);
    EMU_REQUIRE_NOT_NULL(dstr);
    EMU_REQUIRE_STREQ(dstr, "first line");

    dstr = dstr_getline(dstr, fp);
    EMU_REQUIRE_NOT_NULL(dstr);
    EMU_REQUIRE_STREQ(dstr, "another line");

    // No '\n' means dstr_getline should hit EOF and error out.
    dstr = dstr_getline(dstr, fp);
    EMU_EXPECT_NULL(dstr);

    fclose(fp);
    EMU_END_TEST();
}

EMU_TEST(dstr_getdelim)
{
    FILE* fp = fopen(TEST_FILE, "r");
    EMU_REQUIRE_NOT_NULL(fp);
    char* dstr = dstr_alloc_empty();

    dstr = dstr_getdelim(dstr, '\n', fp);
    EMU_REQUIRE_NOT_NULL(dstr);
    EMU_REQUIRE_STREQ(dstr, "first line");

    dstr = dstr_getdelim(dstr, ' ', fp);
    EMU_REQUIRE_NOT_NULL(dstr);
    EMU_REQUIRE_STREQ(dstr, "another");

    dstr = dstr_getdelim(dstr, '\n', fp);
    EMU_REQUIRE_NOT_NULL(dstr);
    EMU_REQUIRE_STREQ(dstr, "line");

    dstr = dstr_getdelim(dstr, EOF, fp);
    EMU_REQUIRE_NOT_NULL(dstr);
    EMU_REQUIRE_STREQ(dstr, "the last line has no newline character");

    fclose(fp);
    EMU_END_TEST();
}

EMU_TEST(dstr_trim)
{
    char* dstr = dstr_alloc_cstr(" \t\n\v\f\rfoo \t\n\v\f\r");
    dstr = dstr_trim(dstr);
    EMU_EXPECT_STREQ(dstr, "foo");
    EMU_END_TEST();
}

EMU_GROUP(dstring_functions)
{
    EMU_ADD(dstring_alloc_and_free_functions);
    EMU_ADD(dstring_reassignment_functions);
    EMU_ADD(dstr_length);
    EMU_ADD(dstr_concat_functions);
    EMU_ADD(dstr_cmp_functions);
    EMU_ADD(dstr_find_functions);
    EMU_ADD(dstr_replace_functions);
    EMU_ADD(dstr_transform_functions);
    EMU_ADD(dstr_getline);
    EMU_ADD(dstr_getdelim);
    EMU_ADD(dstr_trim);
    EMU_END_GROUP();
}

struct foo
{
    int a;
    char b;
    double c;
};

EMU_TEST(struct_type)
{
    struct foo* da = da_alloc(2, sizeof(struct foo));
    EMU_REQUIRE_NOT_NULL(da);

    struct foo some_struct = {.a=3, .b='y', .c=3.14159};
    da[0] = some_struct;
    // da_push(da, (struct foo){.a=4, .b='z', .c=4.14159});
    // ^^^ this will cause a compile time error since the commas in the
    // declaration get parsed as separate arguments to the push macro.
    da = da_push(da, some_struct);

    struct foo bar = da_remove(da, 0);
    EMU_EXPECT_EQ(some_struct.a, bar.a);
    EMU_EXPECT_EQ(some_struct.b, bar.b);
    EMU_EXPECT_EQ(some_struct.c, bar.c);

    da_free(da);
    EMU_END_TEST();
}

EMU_GROUP(testing_with_additional_types)
{
    EMU_ADD(struct_type);
    EMU_END_GROUP();
}

EMU_GROUP(all_tests)
{
    EMU_ADD(darray_functions);
    EMU_ADD(dstring_functions);
    EMU_ADD(testing_with_additional_types);
    EMU_END_GROUP();
}

int main(void)
{
    time_t seed = time(NULL);
    printf("seed: %ld\n", seed);
    srand(seed);
    return EMU_RUN(all_tests);
}
