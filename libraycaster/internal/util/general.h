#ifndef GENERAL_H
#define GENERAL_H

#include <stdlib.h>

typedef struct {
  char **strs;
  size_t max_len;
  size_t len;
} StringArray;

void trim_line(char *line);

StringArray *string_array_create(size_t size);
void string_array_destroy(StringArray *a);
StringArray *duplicate_string_array(StringArray *src, size_t dest_size);
#endif // GENERAL_H
