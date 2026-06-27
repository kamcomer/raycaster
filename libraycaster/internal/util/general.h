#ifndef GENERAL_H
#define GENERAL_H

#include <stdlib.h>

typedef struct {
  char **items;
  size_t capacity;
  size_t len;
} StringArray;

void trim_line(char *line);

int string_array_init(StringArray *sa, size_t size);
int string_array_push(StringArray *sa, const char *s);
void string_array_destroy(StringArray *sa);
int duplicate_string_array(StringArray *src, StringArray *dest, size_t dest_size);
#endif // GENERAL_H
