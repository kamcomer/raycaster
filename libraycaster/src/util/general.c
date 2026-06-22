#include "internal/util/general.h"
#include <ctype.h>
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
