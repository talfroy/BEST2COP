#include "../include/my_printf.h"
#include <string.h>

int print_seglist (FILE* stream, const struct printf_info *info, const void *const *args)
{
    const struct segment_list *sl;

    char *buffer;
    char tmp[128];
    int len = 0;

    sl = *((const struct segment_list**) args[0]);
    buffer = calloc(1024, sizeof(char));

    strcpy(buffer, "[");
    len++;
    for (int i = 0 ; i < sl->size ; i++) {
        memset(tmp, 0, 128);
        if (i < sl->size - 1) {
            len += sprintf(tmp, " %d ,", sl->seg[i]);
        } else {
            len += sprintf(tmp, " %d ", sl->seg[i]);
        }
        strcat(buffer, tmp);
    }

    strcat(buffer, "]");
    len++;

    if (len == -1) {
        return -1;
    }

    len = fprintf(stream, "%*s", (info->left ? -info->width : info->width), buffer);

    free(buffer);
    return len;
}



int print_seglist_arginfo (const struct printf_info *info, size_t n, int *argtypes)
{
    UNUSED(info);
    if (n > 0) {
        argtypes[0] = PA_POINTER;
    }

    return 1;
}