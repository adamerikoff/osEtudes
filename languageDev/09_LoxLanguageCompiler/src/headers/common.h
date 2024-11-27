#ifndef CLOX_COMMON_H
#define CLOX_COMMON_H

#include "settings.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <inttypes.h>

#define DEBUG_TRACE_EXECUTION
#define DEBUG_PRINT_CODE

#define UNUSED(x) (void)(x)

#define ERROR_GUARD(expr) \
    do {\
        if ((expr) == -1) {\
            return -1;\
        }\
    } while (false)

#ifdef _WIN32
#define PLATFORM_WINDOWS
#endif

#if defined(unix) || defined(__unix__) || defined(__unix) || \
    (defined(__APPLE__) && defined(__MACH__))
# define PLATFORM_UNIX
#endif

#ifdef PLATFORM_WINDOWS
#define FORMAT_SIZE_T "Iu"
#else
#define FORMAT_SIZE_T "zu"
#endif

#define UINT8_COUNT (UINT8_MAX + 1)

#endif
