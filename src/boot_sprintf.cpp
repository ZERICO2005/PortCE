#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <ti/sprintf.h>

int boot_vsprintf(char *__restrict buffer, const char *__restrict format, va_list args) {
    return vsprintf(buffer, format, args);
}

int boot_sprintf(char *__restrict buffer, const char *__restrict format, ...) {
    va_list args;
    va_start(args, format);
    const int ret = boot_vsprintf(buffer, format, args);
    va_end(args);
    return ret;
}

int boot_vsnprintf(char *__restrict buffer, size_t count, const char *__restrict format, va_list args) {
    va_list args_copy;
    va_copy(args_copy, args);
    int str_len = vsnprintf(nullptr, 0, format, args_copy);
    va_end(args_copy);

    if (buffer == nullptr || count == 0) {
        return str_len;
    }
    if (static_cast<size_t>(str_len) >= count || str_len < 0) {
        // won't fit or invalid formatting
        *buffer = '\0';
        return str_len;
    }
    return vsnprintf(buffer, count, format, args);
}

int boot_snprintf(char *__restrict buffer, size_t count, const char *__restrict format, ...) {
    va_list args;
    va_start(args, format);
    const int ret = boot_vsnprintf(buffer, count, format, args);
    va_end(args);
    return ret;
}

int boot_vasprintf(char **__restrict p_buffer, const char *__restrict format, va_list args) {
    int ret = -1;
    *p_buffer = nullptr;
    va_list args_copy;
    va_copy(args_copy, args);
    int str_len = vsnprintf(nullptr, 0, format, args_copy);
    va_end(args_copy);

    if (str_len < 0) {
        // formatting error
        return str_len;
    }
    size_t buffer_size = (size_t)str_len + 1;
    *p_buffer = static_cast<char*>(malloc(buffer_size));
    if (*p_buffer != nullptr) {
        va_copy(args_copy, args);
        ret = vsnprintf(*p_buffer, buffer_size, format, args_copy);
        va_end(args_copy);
    }
    return ret;
}

int boot_asprintf(char **__restrict p_str, const char *__restrict format, ...) {
    va_list args;
    va_start(args, format);
    const int ret = boot_vasprintf(p_str, format, args);
    va_end(args);
    return ret;
}
