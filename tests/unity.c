#include "unity.h"
#include <stdlib.h>

int unity_test_count = 0;
int unity_fail_count = 0;
jmp_buf unity_abort_frame;

void unity_finish(void)
{
  printf("\n===========================\n");
  printf("Tests: %d, Failures: %d\n", unity_test_count, unity_fail_count);
  if (unity_fail_count == 0) {
    printf("All tests passed!\n");
  }
  exit(unity_fail_count > 0 ? 1 : 0);
}
