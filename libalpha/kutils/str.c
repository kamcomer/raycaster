#include "str.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

void trim_line(char *line) {
    char *start = line;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }

    char *end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }

    if (start != line) {
        memmove(line, start, strlen(start) + 1);
    }
}

static int string_array_resize(StringArray *sa, uint32_t size) {
    char **tmp = realloc(sa->items, size * sizeof(char *));
    if (!tmp)
        return -1;
    sa->items = tmp;
    sa->capacity = size;
    return 0;
}

int string_array_init(StringArray *sa, size_t size) {
    if (!sa)
        return -1;

    sa->items = (char **)malloc(size * sizeof(char *));
    if (!sa->items)
        return -1;
    sa->len = 0;
    sa->capacity = size;
    return 0;
}

void string_array_destroy(StringArray *sa) {
    if (sa->items) {
        for (size_t i = 0; i < sa->len; i++) {
            free(sa->items[i]);
        }
        free(sa->items);
        sa->items = NULL;
    }
    return;
}

int string_array_push(StringArray *sa, const char *s) {
    if (!sa->items)
        return -1;

    if (sa->len == sa->capacity) {
        if (string_array_resize(sa, sa->capacity * 2) != 0)
            return -1;
    }

    sa->items[sa->len] = malloc(strlen(s) + 1);

    if (!sa->items[sa->len])
        return -1;

    strcpy(sa->items[sa->len], s);
    sa->len++;
    return 0;
}

int duplicate_string_array(StringArray *src, StringArray *dest,
                           size_t dest_size) {
    if (src == NULL || dest == NULL || src->items == NULL || src->len <= 0 ||
        dest_size < src->len) {
        return -1;
    }

    for (size_t i = 0; i < src->len; i++) {
        if (src->items[i] == NULL) {
            dest->items[i] = NULL;
            continue;
        }

        dest->items[i] =
            (char *)malloc((strlen(src->items[i]) + 1) * sizeof(char));
        if (dest->items[i] == NULL) {
            for (size_t j = 0; j < dest_size; j++) {
                free(dest->items[j]);
            }
            return -1;
        }

        strcpy(dest->items[i], src->items[i]);
        dest->len++;
    }
    return 0;
}
