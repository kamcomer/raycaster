#include "raycaster/input.h"
#include "internal/input/input_int.h"
#include "unity.h"

#include <stdlib.h>

void setUp(void) {}
void tearDown(void) {}

void test_create_unknown_backend_returns_null(void)
{
  RcInput *in = rc_input_create((RcInputBackend)999);
  TEST_ASSERT_NULL(in);
}

void test_destroy_null_no_crash(void)
{
  rc_input_destroy(NULL);
  TEST_PASS();
}

void test_destroy_does_not_crash_on_missing_impl(void)
{
  RcInput in;
  in.vtbl = NULL;
  in.impl = NULL;
  rc_input_destroy(&in);
  TEST_PASS();
}

int main(void)
{
  UNITY_BEGIN();
  RUN_TEST(test_create_unknown_backend_returns_null);
  RUN_TEST(test_destroy_null_no_crash);
  RUN_TEST(test_destroy_does_not_crash_on_missing_impl);
  return UNITY_END();
}
