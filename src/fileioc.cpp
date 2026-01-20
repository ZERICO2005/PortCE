#include <fileioc.h>
#include <cstdint>

static void *util_alloc_string_equ(
    size_t count,
    size_t len,
    void *(*malloc_routine)(size_t)
) {
    if (count > UINT16_MAX) {
        // prevents overflow
        return nullptr;
    }
    uint16_t* ptr = reinterpret_cast<uint16_t*>((*malloc_routine)(len + 2));
    if (ptr == nullptr) {
        return nullptr;
    }
    *ptr = static_cast<uint16_t>(count);
    return reinterpret_cast<void*>(ptr);
}

static void *util_alloc_var(
    size_t count,
    size_t len,
    void *(*malloc_routine)(size_t)
) {
    return util_alloc_string_equ(count, len * 9, malloc_routine);
}

string_t *ti_AllocString(size_t len, void *(*malloc_routine)(size_t)) {
    return reinterpret_cast<string_t*>(
        util_alloc_string_equ(len, len, malloc_routine)
    );
}

equ_t *ti_AllocEqu(size_t len, void *(*malloc_routine)(size_t)) {
    return reinterpret_cast<equ_t*>(
        util_alloc_string_equ(len, len, malloc_routine)
    );
}

list_t *ti_AllocList(size_t dim, void *(*malloc_routine)(size_t)) {
    return reinterpret_cast<list_t*>(
        util_alloc_var(dim, dim, malloc_routine)
    );
}

matrix_t *ti_AllocMatrix(uint8_t rows, uint8_t cols, void *(*malloc_routine)(size_t)) {
    return reinterpret_cast<matrix_t*>(
        util_alloc_var(static_cast<size_t>((rows << 8) | cols), rows * cols, malloc_routine)
    );
}

cplx_list_t *ti_AllocCplxList(size_t dim, void *(*malloc_routine)(size_t)) {
    return reinterpret_cast<cplx_list_t*>(
        util_alloc_var(dim, dim * 2, malloc_routine)
    );
}
