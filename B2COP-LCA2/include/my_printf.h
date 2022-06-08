#ifndef __MY_PRINTF__
#define __MY_PRINTF__

#include "Dict.h"
#include "base.h"
#include <printf.h>
#include "Segment_list.h"

extern int print_seglist (FILE* stream, const struct printf_info *info, const void *const *args);

extern int print_seglist_arginfo (const struct printf_info *info, size_t n, int *argtypes);

#endif