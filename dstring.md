# Dstring - Darray String Specialization

## Table of contents
1. [Introduction](#introduction) 
1. [API](#api)
    + [Creation and Deletion](#creation-and-deletion)
        + [dstr_alloc_empty](#dstr_alloc_empty)
        + [dstr_alloc_cstr](#dstr_alloc_cstr)
        + [dstr_alloc_dstr](#dstr_alloc_dstr)
        + [dstr_alloc_format](#dstr_alloc_format)
        + [dstr_alloc_empty_custom](#dstr_alloc_empty_custom)
        + [dstr_alloc_cstr_custom](#dstr_alloc_cstr_custom)
        + [dstr_alloc_dstr_custom](#dstr_alloc_dstr_custom)
        + [dstr_alloc_format_custom](#dstr_alloc_format_custom)
        + [dstr_free](#dstr_free)
    + [Reassignment](#reassignment)
        + [dstr_reassign_empty](#dstr_reassign_empty)
        + [dstr_reassign_cstr](#dstr_reassign_cstr)
        + [dstr_reassign_dstr](#dstr_reassign_dstr)
        + [dstr_reassign_format](#dstr_reassign_format)
    + [Dstring Data](#dstring-data)
        + [dstr_length](#dstr_length)
    + [Concatination](#concatination)
        + [dstr_concat_char](#dstr_concat_char)
        + [dstr_concat_cstr](#dstr_concat_cstr)
        + [dstr_concat_dstr](#dstr_concat_dstr)
        + [dstr_concat_format](#dstr_concat_format)
    + [Comparison](#comparison)
        + [dstr_cmp](#dstr_cmp)
        + [dstr_cmp_case](#dstr_cmp_case)
    + [Find and Replace Functions](#find-and-replace-functions)
        + [dstr_find](#dstr_find)
        + [dstr_find_case](#dstr_find_case)
        + [dstr_replace_all](#dstr_replace_all)
        + [dstr_replace_all_case](#dstr_replace_all_case)
    + [In-place Transformation Functions](#in-place-transformation-functions)
        + [dstr_transform_lower](#dstr_transform_lower)
        + [dstr_transform_upper](#dstr_transform_upper)
    + [Misc.](#misc)
        + [dstr_getline](#dstr_getline)
        + [dstr_getdelim](#dstr_getdelim)
        + [dstr_trim](#dstr_trim)

## Introduction
Character arrays are by far the most common array type in C. Many functions in the C standard library like `strcmp` and `printf` will work exactly the same with `darray(char)` as built-in cstrings, but some functions such as `strcpy` and `sprintf` will "break" character darrays by desynching the length property of the darray from the actual length of the string. The dstring extension to the darray library was created to prevent these issues. A dstring is written as `darray(char)` and refered to as such in all documentation.

Generally speaking, if you are using dstrings, you should prefer the functions in this library over the C standard library when possible.

## API

### Creation and Deletion

#### dstr_alloc_empty
Allocate a dstring as the empty string `""`.

Returns a pointer to a new dstring on success. `NULL` on allocation failure.
```C
darray(char) dstr_alloc_empty(void);
```

#### dstr_alloc_cstr
Allocate a dstring as copy of cstring `src`. `src` may also be a dstring.

Returns a pointer to a new dstring on success. `NULL` on allocation failure.
```C
darray(char) dstr_alloc_cstr(const char* src);
```

#### dstr_alloc_dstr
Allocate a dstring as a copy of dstring `src`. May be faster than `dstr_alloc_cstr` when copying a large dstring.

Returns a pointer to a new dstring on success. `NULL` on allocation failure.
```C
darray(char) dstr_alloc_dstr(const darray(char) src);
```

#### dstr_alloc_format
Allocate a dstring using `sprintf` style formatting.

Returns a pointer to a new dstring on success. `NULL` on allocation failure.
```C
darray(char) dstr_alloc_format(const char* format, ...);
```

#### dstr_alloc_empty_custom
Allocate a dstring as the empty string `""`. All memory allocation, reallocation, and freeing will be handled using the provided memory management functions for this dstring.

Returns a pointer to a new dstring on success. `NULL` on allocation failure.
```C
darray(char) dstr_alloc_empty_custom(struct da_mem_funcs mem_funcs);
```
See `darray.h` for the definition of `struct da_mem_funcs`.

#### dstr_alloc_cstr_custom
Allocate a dstring as copy of cstring `src`. `src` may also be a dstring. All memory allocation, reallocation, and freeing will be handled using the provided memory management functions for this dstring.

Returns a pointer to a new dstring on success. `NULL` on allocation failure.
```C
darray(char) dstr_alloc_cstr_custom(struct da_mem_funcs mem_funcs, const char* src);
```
See `darray.h` for the definition of `struct da_mem_funcs`.

#### dstr_alloc_dstr_custom
Allocate a dstring as a copy of dstring `src`. May be faster than `dstr_alloc_cstr_custom` when copying a large dstring. All memory allocation, reallocation, and freeing will be handled using the provided memory management functions for this dstring.

Returns a pointer to a new dstring on success. `NULL` on allocation failure.
```C
darray(char) dstr_alloc_dstr_custom(struct da_mem_funcs mem_funcs, const darray(char) src);
```
See `darray.h` for the definition of `struct da_mem_funcs`.

#### dstr_alloc_format_custom
Allocate a dstring using `sprintf` style formatting. All memory allocation, reallocation, and freeing will be handled using the provided memory management functions for this dstring.

Returns a pointer to a new dstring on success. `NULL` on allocation failure.
```C
darray(char) dstr_alloc_format_custom(struct da_mem_funcs mem_funcs, const char* format, ...);
```
See `darray.h` for the definition of `struct da_mem_funcs`.

#### dstr_free
Free a dstring. Equivalent to calling `da_free` on `dstr`.
```C
void dstr_free(darray(char) dstr);
```

----

### Reassignment
These functions can be used to reassign the contents of a dstring. The functions reallocate memory only when necessary to avoid costly syscalls to `malloc` and `free`.

#### dstr_reassign_empty
Reassign the contents of an allocated dstring to the empty string `""`, reallocating memory only when neccesary.

Returns a pointer to the reassigned dstring on success. `NULL` on allocation failure.
```C
darray(char) dstr_reassign_empty(darray(char) allocated_dstr);
```

#### dstr_reassign_cstr
Reassign the contents of an allocated dstring to that of a cstring, reallocating memory only when neccesary.

Returns a pointer to the reassigned dstring on success. `NULL` on allocation failure.
```C
darray(char) dstr_reassign_cstr(darray(char) allocated_dstr, const char* src)
```

#### dstr_reassign_dstr
Reassign the contents of an allocated dstring to that of a dstring, reallocating memory only when neccesary.

Returns a pointer to the reassigned dstring on success. `NULL` on allocation failure.
```C
darray(char) dstr_reassign_dstr(darray(char) allocated_dstr, const darray(char) src)
```

#### dstr_reassign_format
Reassign the contents of an allocated dstring using `sprintf` style formatting, reallocating memory only when neccesary.

Returns a pointer to the reassigned dstring on success. `NULL` on allocation failure.
```C
darray(char) dstr_reassign_format(darray(char) allocated_dstr, const char* format, ...)
```

----

### Dstring Data

#### dstr_length
Returns the length of `dstr` without its null terminator. Equivalent to `da_length(dstr)-1`. O(1) compared to the O(n) `strlen`.
```C
size_t dstr_length(const darray(char) dstr);
```

----

### Concatination

#### dstr_concat_char
Append character `c` to dstring `dest`.

Returns a pointer to the new location of the dstring upon successful function completion. If `dstr_concat_char` returns `NULL`, reallocation failed and `dstr` is left untouched.
```C
darray(char) dstr_concat_char(darray(char) dest, char c);
```
Like `da_concat` references to `dest` may be invalidated across the function call. Use the return value of `dstr_concat_char` as truth for the location of `dest` after function completion.

#### dstr_concat_cstr
Append `src` to dstring `dest`. `src` may also be a dstring.

Returns a pointer to the new location of the dstring upon successful function completion. If `dstr_concat_cstr` returns `NULL`, reallocation failed and `dstr` is left untouched.
```C
darray(char) dstr_concat_cstr(darray(char) dest, const char* src);
```
Like `da_concat` references to `dest` may be invalidated across the function call. Use the return value of `dstr_concat_cstr` as truth for the location of `dest` after function completion.

#### dstr_concat_dstr
Append `src` to dstring `dest`. Faster than `dstr_concat_cstr` for cat-ing a dstring to another dstring.

Returns a pointer to the new location of the dstring upon successful function completion. If `dstr_concat_dstr` returns `NULL`, reallocation failed and `dstr` is left untouched.
```C
darray(char) dstr_concat_dstr(darray(char) dest, const darray(char) src);
```
Like `da_concat` references to `dest` may be invalidated across the function call. Use the return value of `dstr_concat_dstr` as truth for the location of `dest` after function completion.

#### dstr_concat_format
Append a string to dstring `dest` using `sprintf` style formatting.

Returns a pointer to the new location of the dstring upon successful function completion. If `dstr_concat_format` returns `NULL`, reallocation failed and `dstr` is left untouched.
```C
darray(char) dstr_concat_format(darray(char) dest, const char* format, ...);
```


Like `da_concat` references to `dest` may be invalidated across the function call. Use the return value of `dstr_concat_format` as truth for the location of `dest` after function completion.

----

### Comparison

#### dstr_cmp
Comparison function. Currently functionally equivalent to `strcmp`.
```C
int dstr_cmp(const darray(char) s1, const char* s2);
```

#### dstr_cmp_case
Comparison function. Currently functionally equivalent to `strcasecmp`.
```C
int dstr_cmp_case(const darray(char) s1, const char* s2);
```

----

### Find and Replace Functions

#### dstr_find
Returns the index of the first occurrence of `substr` in `dstr` or `-1` if `substr` was not found. Similar to Python's `str.find`.
```C
long dstr_find(darray(char) dstr, const char* substr);
```

#### dstr_find_case
Returns the index of the first case insensitive occurrence of `substr` in `dstr` or `-1` if `substr` was not found. Similar to Python's `str.find`.
```C
long dstr_find_case(darray(char) dstr, const char* substr);
```

#### dstr_replace_all
Replace all occurrences of `substr` in `dstr` with `new_str`.

Returns the new location of `dstr` after function completion. If `dstr_replace_all` returns `NULL` reallocation failed somewhere and `dstr` may be corrupted.
```C
darray(char) dstr_replace_all(darray(char) dstr, const char* substr, const char* new_str);
```

#### dstr_replace_all_case
Replace all occurrences of `substr` (case insensitive) in `dstr` with `new_str`.

Returns the new location of `dstr` after function completion. If `dstr_replace_all_case` returns `NULL` reallocation failed somewhere and `dstr` may be corrupted.
```C
darray(char) dstr_replace_all(darray(char) dstr, const char* substr, const char* new_str);
```

----

### In-place Transformation Functions

#### dstr_transform_lower
Transform `dstr` to lower case in place.
```C
void dstr_transform_lower(darray(char) dstr);
```

#### dstr_transform_upper
Transform `dstr` to upper case in place.
```C
void dstr_transform_upper(darray(char) dstr);
```

----

### Misc.

#### dstr_getline
Read an entire line from `stream` and store it in `dstr`. Equivalent to calling `dstr_getdelim` with `delim == '\n'`.

Returns the new location of `allocated_dstr` after function completion.`NULL` on failure (including end-of-file condition).
```C
darray(char) dstr_getline(darray(char) allocated_dstr, FILE* stream);
```

#### dstr_getdelim
Read characters from `stream` up to but not including `delim` and `dstr`.

Returns the new location of `allocated_dstr` after function completion.`NULL` on failure (including end-of-file condition if `delim != EOF`).
```C
darray(char) dstr_getdelim(darray(char) allocated_dstr, int delim, FILE* stream);
```

#### dstr_trim
Trims leading and trailing whitespace from `dstr`.

Returns the new location of `dstr` after function completion. If `dstr_trim` returns `NULL` reallocation failed and `dstr` is left untouched.
```C
darray(char) dstr_trim(darray(char) dstr);
```
