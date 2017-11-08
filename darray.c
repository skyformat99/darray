#include "darray.h"

////////////////////////////////// DARRAY CORE /////////////////////////////////
static inline void _da_memswap(void* p1, void* p2, size_t sz)
{
    char tmp, *a = p1, *b = p2;
    for (size_t i = 0; i < sz; ++i)
    {
        tmp = a[i];
        a[i] = b[i];
        b[i] = tmp;
    }
}

void* da_alloc(size_t nelem, size_t size)
{
    size_t capacity = DA_NEW_CAPACITY_FROM_LENGTH(nelem);
    struct _darray* darr = malloc(sizeof(struct _darray) + capacity*size);
    if (darr == NULL)
        return darr;
    darr->_mem_funcs = DA_DEFAULT_MEM_FUNCS;
    darr->_elemsz = size;
    darr->_length = nelem;
    darr->_capacity = capacity;
    return darr->_data;
}

void* da_alloc_exact(size_t nelem, size_t size)
{
    struct _darray* darr = malloc(sizeof(struct _darray) + nelem*size);
    if (darr == NULL)
        return darr;
    darr->_mem_funcs = DA_DEFAULT_MEM_FUNCS;
    darr->_elemsz = size;
    darr->_length = nelem;
    darr->_capacity = nelem;
    return darr->_data;
}

void* da_alloc_custom(struct da_mem_funcs mem_funcs, size_t nelem, size_t size)
{
    size_t capacity = DA_NEW_CAPACITY_FROM_LENGTH(nelem);
    struct _darray* darr = mem_funcs.alloc_f(sizeof(struct _darray) + capacity*size);
    if (darr == NULL)
        return darr;
    darr->_mem_funcs = mem_funcs;
    darr->_elemsz = size;
    darr->_length = nelem;
    darr->_capacity = capacity;
    return darr->_data;
}

void* da_alloc_exact_custom(struct da_mem_funcs mem_funcs, size_t nelem,
    size_t size)
{
    struct _darray* darr = mem_funcs.alloc_f(sizeof(struct _darray) + nelem*size);
    if (darr == NULL)
        return darr;
    darr->_mem_funcs = mem_funcs;
    darr->_elemsz = size;
    darr->_length = nelem;
    darr->_capacity = nelem;
    return darr->_data;
}

void da_free(void* darr)
{
    ((struct _darray*)DA_P_HEAD_FROM_HANDLE(darr))->
        _mem_funcs.free_f(DA_P_HEAD_FROM_HANDLE(darr));
}

size_t da_length(const void* darr)
{
    return *DA_P_LENGTH_FROM_HANDLE(darr);
}

size_t da_capacity(const void* darr)
{
    return *DA_P_CAPACITY_FROM_HANDLE(darr);
}

size_t da_sizeof_elem(const void* darr)
{
    return *DA_P_SIZEOF_ELEM_FROM_HANDLE(darr);
}

void* da_resize(void* darr, size_t nelem)
{
    size_t new_capacity = DA_NEW_CAPACITY_FROM_LENGTH(nelem);
    size_t new_arr_size =
        sizeof(struct _darray) + new_capacity*da_sizeof_elem(darr);
    struct _darray* ptr = ((struct _darray*)DA_P_HEAD_FROM_HANDLE(darr))->
        _mem_funcs.realloc_f(DA_P_HEAD_FROM_HANDLE(darr), new_arr_size);
    if (ptr == NULL)
        return NULL;
    ptr->_length = nelem;
    ptr->_capacity = new_capacity;
    return ptr->_data;
}

void* da_resize_exact(void* darr, size_t nelem)
{
    size_t new_arr_size = sizeof(struct _darray) + nelem*da_sizeof_elem(darr);
    struct _darray* ptr = ((struct _darray*)DA_P_HEAD_FROM_HANDLE(darr))->
        _mem_funcs.realloc_f(DA_P_HEAD_FROM_HANDLE(darr), new_arr_size);
    if (ptr == NULL)
        return NULL;
    ptr->_length = nelem;
    ptr->_capacity = nelem;
    return ptr->_data;
}

void* da_reserve(void* darr, size_t nelem)
{
    size_t min_capacity = da_length(darr) + nelem;
    if (da_capacity(darr) >= min_capacity)
        return darr;
    size_t new_capacity = DA_NEW_CAPACITY_FROM_LENGTH(min_capacity);
    size_t new_arr_size =
        sizeof(struct _darray) + new_capacity*da_sizeof_elem(darr);
    struct _darray* ptr = ((struct _darray*)DA_P_HEAD_FROM_HANDLE(darr))->
        _mem_funcs.realloc_f(DA_P_HEAD_FROM_HANDLE(darr), new_arr_size);
    if (ptr == NULL)
        return NULL;
    ptr->_capacity = new_capacity;
    return ptr->_data;
}

void* da_insert_arr(void* darr, size_t index, const void* src, size_t nelem)
{
    darr = da_reserve(darr, nelem);
    if (darr == NULL)
        return NULL;
    memmove(
        darr + da_sizeof_elem(darr)*(index+nelem),
        darr + da_sizeof_elem(darr)*index,
        da_sizeof_elem(darr)*(da_length(darr)-index)
    );
    memcpy(
        darr + da_sizeof_elem(darr)*index,
        src,
        da_sizeof_elem(darr)*nelem
    );
    *DA_P_LENGTH_FROM_HANDLE(darr) += nelem;
    return darr;
}

void da_remove_arr(void* darr, size_t index, size_t nelem)
{
    memmove(
        darr + da_sizeof_elem(darr)*index,
        darr + da_sizeof_elem(darr)*(index+nelem),
        da_sizeof_elem(darr)*(da_length(darr)-index-nelem)
    );
    *DA_P_LENGTH_FROM_HANDLE(darr) -= nelem;
}

void da_swap(void* darr, size_t index_a, size_t index_b)
{
    size_t size = da_sizeof_elem(darr);
    _da_memswap(
        ((char*)darr) + (index_a*size),
        ((char*)darr) + (index_b*size),
        size
    );
}

void* da_concat(void* dest, const void* src, size_t nelem)
{
    size_t offset = da_length(dest)*da_sizeof_elem(dest);
    dest = da_reserve(dest, nelem);
    if (dest == NULL)
        return NULL;
    memcpy((char*)dest+offset, src, nelem*da_sizeof_elem(dest));
    *DA_P_LENGTH_FROM_HANDLE(dest) += nelem;
    return dest;
}

/////////////////////////////////// DSTRING ////////////////////////////////////
darray(char) dstr_alloc_empty(void)
{
    char* dstr = da_alloc(1, sizeof(char));
    if (dstr == NULL)
        return NULL;
    dstr[0] = '\0';
    return dstr;
}

darray(char) dstr_alloc_cstr(const char* src)
{
    size_t src_len_with_nullterm = strlen(src)+1;
    char* dstr = da_alloc(src_len_with_nullterm, sizeof(char));
    if (dstr == NULL)
        return NULL;
    memcpy(dstr, src, src_len_with_nullterm);
    return dstr;
}

darray(char) dstr_alloc_dstr(const darray(char) src)
{
    size_t src_len_with_nullterm = da_length(src);
    char* dstr = da_alloc(src_len_with_nullterm, sizeof(char));
    if (dstr == NULL)
        return NULL;
    memcpy(dstr, src, src_len_with_nullterm);
    return dstr;
}

darray(char) dstr_alloc_format(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    va_list copy;
    va_copy(copy, args);
    size_t size = vsnprintf(NULL, 0, format, copy) + 1 /* +1 for '\0' */;
    va_end(copy);

    char* dstr = da_alloc(size, sizeof(char));
    if (dstr == NULL)
        return NULL;
    vsprintf(dstr, format, args);

    va_end(args);
    return dstr;
}

darray(char) dstr_alloc_empty_custom(struct da_mem_funcs mem_funcs)
{
    char* dstr = da_alloc_custom(mem_funcs, 1, sizeof(char));
    if (dstr == NULL)
        return NULL;
    dstr[0] = '\0';
    return dstr;
}

darray(char) dstr_alloc_cstr_custom(struct da_mem_funcs mem_funcs,
    const char* src)
{
    size_t src_len_with_nullterm = strlen(src)+1;
    char* dstr = da_alloc_custom(mem_funcs, src_len_with_nullterm, sizeof(char));
    if (dstr == NULL)
        return NULL;
    memcpy(dstr, src, src_len_with_nullterm);
    return dstr;
}

darray(char) dstr_alloc_dstr_custom(struct da_mem_funcs mem_funcs,
    const darray(char) src)
{
    size_t src_len_with_nullterm = da_length(src);
    char* dstr = da_alloc_custom(mem_funcs, src_len_with_nullterm, sizeof(char));
    if (dstr == NULL)
        return NULL;
    memcpy(dstr, src, src_len_with_nullterm);
    return dstr;
}

darray(char) dstr_alloc_format_custom(struct da_mem_funcs mem_funcs,
    const char* format, ...)
{
    va_list args;
    va_start(args, format);

    va_list copy;
    va_copy(copy, args);
    size_t size = vsnprintf(NULL, 0, format, copy) + 1 /* +1 for '\0' */;
    va_end(copy);

    char* dstr = da_alloc_custom(mem_funcs, size, sizeof(char));
    if (dstr == NULL)
        return NULL;
    vsprintf(dstr, format, args);

    va_end(args);
    return dstr;
}

void dstr_free(darray(char) dstr)
{
    da_free(dstr);
}

darray(char) dstr_reassign_empty(darray(char) allocated_dstr)
{
    *DA_P_LENGTH_FROM_HANDLE(allocated_dstr) = 0;
    allocated_dstr = da_concat(allocated_dstr, "", 1);
    return allocated_dstr;
}

darray(char) dstr_reassign_cstr(darray(char) allocated_dstr,
    const char* src)
{
    *DA_P_LENGTH_FROM_HANDLE(allocated_dstr) = 0;
    allocated_dstr = da_concat(allocated_dstr, src, strlen(src)+1);
    return allocated_dstr;
}

darray(char) dstr_reassign_dstr(darray(char) allocated_dstr,
    const darray(char) src)
{
    *DA_P_LENGTH_FROM_HANDLE(allocated_dstr) = 0;
    allocated_dstr = da_concat(allocated_dstr, src, da_length(src));
    return allocated_dstr;
}

darray(char) dstr_reassign_format(darray(char) allocated_dstr,
    const char* format, ...)
{
    va_list args;
    va_start(args, format);
    
    va_list copy;
    va_copy(copy, args);
    size_t size = vsnprintf(NULL, 0, format, copy) + 1 /* +1 for '\0' */;
    va_end(copy);

    *DA_P_LENGTH_FROM_HANDLE(allocated_dstr) = 0;
    allocated_dstr = da_reserve(allocated_dstr, size);
    if (allocated_dstr == NULL)
        return NULL;
    vsprintf(allocated_dstr, format, args);
    *DA_P_LENGTH_FROM_HANDLE(allocated_dstr) = size;

    va_end(args);
    return allocated_dstr;
}

size_t dstr_length(const darray(char) dstr)
{
    // dstrings always have a null terminator so this should never underflow.
    return da_length(dstr)-1;
}

darray(char) dstr_concat_char(darray(char) dest, char c)
{
    return dest = da_insert(dest, da_length(dest)-1, c);
}

darray(char) dstr_concat_cstr(darray(char) dest, const char* src)
{
    size_t dest_strlen = dstr_length(dest);
    size_t src_strlen = strlen(src);
    dest = da_reserve(dest, src_strlen);
    if (dest == NULL)
        return NULL;
    memcpy(dest+dest_strlen, src, src_strlen+1);
    *DA_P_LENGTH_FROM_HANDLE(dest) += src_strlen;
    return dest;
}

darray(char) dstr_concat_dstr(darray(char) dest, const darray(char) src)
{
    size_t dest_strlen = dstr_length(dest);
    size_t src_strlen = dstr_length(src);
    dest = da_reserve(dest, src_strlen);
    if (dest == NULL)
        return NULL;
    memcpy(dest+dest_strlen, src, src_strlen+1);
    *DA_P_LENGTH_FROM_HANDLE(dest) += src_strlen;
    return dest;
}

darray(char) dstr_concat_format(darray(char) dest, const char* format, ...)
{
    size_t dest_strlen = dstr_length(dest);

    va_list args;
    va_start(args, format);

    va_list copy;
    va_copy(copy, args);
    size_t formatted_strlen = vsnprintf(NULL, 0, format, copy);
    va_end(copy);

    dest = da_reserve(dest, formatted_strlen);
    if (dest == NULL)
        return NULL;
    vsprintf(dest+dest_strlen, format, args);
    *DA_P_LENGTH_FROM_HANDLE(dest) += formatted_strlen;

    va_end(args);
    return dest;
}

int dstr_cmp(const darray(char) s1, const char* s2)
{
    while (*s1 == *s2 && *s1 != '\0')
    {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

int dstr_cmp_case(const darray(char) s1, const char* s2)
{
    while (tolower(*s1) == tolower(*s2) && *s1 != '\0')
    {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

long dstr_find(darray(char) dstr, const char* substr)
{
    char* loc = strstr(dstr, substr);
    if (loc == NULL)
        return -1;
    return loc - dstr;
}

static const char* _da_strcasestr(const char* haystack, const char* needle)
{
    const char* currh = haystack;
    const char* currn = needle;
    while (1)
    {
        if (*currn == '\0')
            return haystack;
        else if (*currh == '\0')
            return NULL;

        if (tolower(*currh) != tolower(*currn))
        {
            haystack = ++currh;
            currn = needle;
        }
        else
        {
            currh++;
            currn++;
        }
    }
}

long dstr_find_case(darray(char) dstr, const char* substr)
{
    const char* loc = _da_strcasestr(dstr, substr);
    if (loc == NULL)
        return -1;
    return loc - dstr;
}

darray(char) dstr_replace_all(darray(char) dstr, const char* substr,
    const char* new_str)
{
    size_t substr_len = strlen(substr);
    size_t new_str_len = strlen(new_str);
    long loc;
    while ((loc = dstr_find(dstr, substr)) != -1)
    {
        da_remove_arr(dstr, loc, substr_len);
        if ((dstr = da_insert_arr(dstr, loc, new_str, new_str_len)) == NULL)
            return NULL;
    }
    return dstr;
}

darray(char) dstr_replace_all_case(darray(char) dstr, const char* substr,
    const char* new_str)
{
    size_t substr_len = strlen(substr);
    size_t new_str_len = strlen(new_str);
    long loc;
    while ((loc = dstr_find_case(dstr, substr)) != -1)
    {
        da_remove_arr(dstr, loc, substr_len);
        if ((dstr = da_insert_arr(dstr, loc, new_str, new_str_len)) == NULL)
            return NULL;
    }
    return dstr;
}

void dstr_transform_lower(darray(char) dstr)
{
    da_foreach(dstr, c)
        *c = tolower(*c);
}

void dstr_transform_upper(darray(char) dstr)
{
    da_foreach(dstr, c)
        *c = toupper(*c);
}

darray(char) dstr_getline(darray(char) allocated_dstr, FILE* stream)
{
    return dstr_getdelim(allocated_dstr, '\n', stream);
}

darray(char) dstr_getdelim(darray(char) allocated_dstr, int delim, FILE* stream)
{
    allocated_dstr = dstr_reassign_empty(allocated_dstr);
    if (allocated_dstr == NULL)
        return NULL;
    int c;
    while ((c = fgetc(stream)) != delim)
    {
        if (delim != EOF && c == EOF)
            return NULL;
        allocated_dstr = dstr_concat_char(allocated_dstr, c);
        if (allocated_dstr == NULL)
            return NULL;
    }
    return allocated_dstr;
}

darray(char) dstr_trim(darray(char) dstr)
{
    size_t n;
    char* tmp;

    // Trim leading whitespace.
    n = 0;
    tmp = dstr;
    while (isspace(*tmp++))
        ++n;
    da_remove_arr(dstr, 0, n);

    // Trim trailing whitespace.
    n = 0;
    tmp = dstr + dstr_length(dstr) - 1;
    while (isspace(*tmp--))
        n++;
    da_remove_arr(dstr, dstr_length(dstr)-n, n);

    return dstr;
}
