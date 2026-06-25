#include "internal/util/general.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

void trim_line(char *line)
{
  // Remove leading whitespace
  char *start = line;
  while (*start && isspace((unsigned char)*start)) {
    start++;
  }

  // Remove trailing whitespace
  char *end = start + strlen(start) - 1;
  while (end > start && isspace((unsigned char)*end)) {
    *end = '\0';
    end--;
  }

  // Shift the trimmed line back to the original buffer
  if (start != line) {
    memmove(line, start, strlen(start) + 1);
  }
}

StringArray *string_array_create(size_t size)
{
  StringArray *a = malloc(sizeof(StringArray));
  if (!a) {
    return NULL;
  }
  a->strs = (char **)malloc(size * sizeof(char *));
  if (!a->strs) {
    free(a);
    return NULL;
  }
  a->len = 0;
  a->max_len = size;
  return a;
}
void string_array_destroy(StringArray *a)
{
  if (!a) {
    return;
  }
  if (a->strs) {
    for (size_t i = 0; i < a->len; i++) {
      free(a->strs[i]);
    }
    free(a->strs);
  }
  free(a);
  return;
}

StringArray *duplicate_string_array(StringArray *src, size_t dest_size)
{
  if (src == NULL || src->strs == NULL || src->len <= 0 || dest_size < src->len) {
    return NULL;
  }

  StringArray *dest = string_array_create(dest_size);
  if (dest == NULL) {
    return NULL;
  }

  for (size_t i = 0; i < src->len; i++) {
    if (src->strs[i] == NULL) {
      dest->strs[i] = NULL;
      continue;
    }

    dest->strs[i] = (char *)malloc((strlen(src->strs[i]) + 1) * sizeof(char));
    if (dest->strs[i] == NULL) {
      for (size_t j = 0; j < dest_size; j++) {
        free(dest->strs[j]);
      }
      free(dest);
      return NULL;
    }

    strcpy(dest->strs[i], src->strs[i]);
  }
  return dest;
}
