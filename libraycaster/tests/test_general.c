#include "internal/util/general.h"
#include "unity.h"

#include <string.h>

void setUp(void) {}
void tearDown(void) {}

void test_trim_line_noop(void)
{
  char buf[] = "hello";
  trim_line(buf);
  TEST_ASSERT_EQUAL_STRING("hello", buf);
}

void test_trim_line_leading(void)
{
  char buf[] = "  hello";
  trim_line(buf);
  TEST_ASSERT_EQUAL_STRING("hello", buf);
}

void test_trim_line_trailing(void)
{
  char buf[] = "hello  ";
  trim_line(buf);
  TEST_ASSERT_EQUAL_STRING("hello", buf);
}

void test_trim_line_both(void)
{
  char buf[] = "  hello  ";
  trim_line(buf);
  TEST_ASSERT_EQUAL_STRING("hello", buf);
}

void test_trim_line_all_spaces(void)
{
  char buf[] = "   ";
  trim_line(buf);
  TEST_ASSERT_EQUAL_STRING("", buf);
}

void test_trim_line_empty(void)
{
  char buf[] = "";
  trim_line(buf);
  TEST_ASSERT_EQUAL_STRING("", buf);
}

void test_trim_line_tabs(void)
{
  char buf[] = "\thello\n";
  trim_line(buf);
  TEST_ASSERT_EQUAL_STRING("hello", buf);
}

void test_string_array_init(void)
{
  StringArray sa;
  int ret = string_array_init(&sa, 5);
  TEST_ASSERT_EQUAL_INT(0, ret);
  TEST_ASSERT_EQUAL_INT(5, sa.capacity);
  TEST_ASSERT_EQUAL_INT(0, sa.len);
  TEST_ASSERT_NOT_NULL(sa.strs);
  string_array_destroy(&sa);
}

void test_string_array_init_null(void)
{
  int ret = string_array_init(NULL, 5);
  TEST_ASSERT_EQUAL_INT(-1, ret);
}

void test_string_array_push(void)
{
  StringArray sa;
  string_array_init(&sa, 3);

  TEST_ASSERT_EQUAL_INT(0, string_array_push(&sa, "foo"));
  TEST_ASSERT_EQUAL_INT(1, sa.len);
  TEST_ASSERT_EQUAL_STRING("foo", sa.strs[0]);

  TEST_ASSERT_EQUAL_INT(0, string_array_push(&sa, "bar"));
  TEST_ASSERT_EQUAL_INT(2, sa.len);
  TEST_ASSERT_EQUAL_STRING("bar", sa.strs[1]);

  string_array_destroy(&sa);
}

void test_string_array_push_resize(void)
{
  StringArray sa;
  string_array_init(&sa, 2);

  string_array_push(&sa, "a");
  string_array_push(&sa, "b");
  TEST_ASSERT_EQUAL_INT(2, sa.capacity);

  TEST_ASSERT_EQUAL_INT(0, string_array_push(&sa, "c"));
  TEST_ASSERT_EQUAL_INT(4, sa.capacity);
  TEST_ASSERT_EQUAL_INT(3, sa.len);
  TEST_ASSERT_EQUAL_STRING("c", sa.strs[2]);

  string_array_destroy(&sa);
}

void test_string_array_push_multiple_resize(void)
{
  StringArray sa;
  string_array_init(&sa, 1);

  for (int i = 0; i < 10; i++) {
    char buf[8];
    sprintf(buf, "n%d", i);
    TEST_ASSERT_EQUAL_INT(0, string_array_push(&sa, buf));
  }
  TEST_ASSERT_EQUAL_INT(10, sa.len);
  TEST_ASSERT_EQUAL_INT(16, sa.capacity);
  TEST_ASSERT_EQUAL_STRING("n0", sa.strs[0]);
  TEST_ASSERT_EQUAL_STRING("n9", sa.strs[9]);

  string_array_destroy(&sa);
}

void test_string_array_push_into_uninit_fails(void)
{
  StringArray sa;
  memset(&sa, 0, sizeof(sa));
  int ret = string_array_push(&sa, "test");
  TEST_ASSERT_EQUAL_INT(-1, ret);
}

void test_string_array_destroy_empty(void)
{
  StringArray sa;
  string_array_init(&sa, 5);
  string_array_destroy(&sa);
  TEST_ASSERT_NULL(sa.strs);
}

void test_string_array_destroy_zeroed(void)
{
  StringArray sa;
  memset(&sa, 0, sizeof(sa));
  string_array_destroy(&sa);
  TEST_PASS();
}

void test_duplicate_string_array(void)
{
  StringArray src, dest;
  string_array_init(&src, 3);
  string_array_push(&src, "hello");
  string_array_push(&src, "world");
  string_array_init(&dest, 3);

  int ret = duplicate_string_array(&src, &dest, 3);
  TEST_ASSERT_EQUAL_INT(0, ret);
  TEST_ASSERT_EQUAL_INT(2, dest.len);
  TEST_ASSERT_EQUAL_STRING("hello", dest.strs[0]);
  TEST_ASSERT_EQUAL_STRING("world", dest.strs[1]);

  string_array_destroy(&src);
  string_array_destroy(&dest);
}

void test_duplicate_string_array_null_src(void)
{
  StringArray dest;
  string_array_init(&dest, 3);
  int ret = duplicate_string_array(NULL, &dest, 3);
  TEST_ASSERT_EQUAL_INT(-1, ret);
  string_array_destroy(&dest);
}

void test_duplicate_string_array_null_dest(void)
{
  StringArray src;
  string_array_init(&src, 3);
  int ret = duplicate_string_array(&src, NULL, 3);
  TEST_ASSERT_EQUAL_INT(-1, ret);
  string_array_destroy(&src);
}

void test_duplicate_string_array_small_dest(void)
{
  StringArray src, dest;
  string_array_init(&src, 5);
  string_array_push(&src, "a");
  string_array_push(&src, "b");
  string_array_push(&src, "c");
  string_array_init(&dest, 2);

  int ret = duplicate_string_array(&src, &dest, 2);
  TEST_ASSERT_EQUAL_INT(-1, ret);

  string_array_destroy(&src);
  string_array_destroy(&dest);
}

int main(void)
{
  UNITY_BEGIN();
  RUN_TEST(test_trim_line_noop);
  RUN_TEST(test_trim_line_leading);
  RUN_TEST(test_trim_line_trailing);
  RUN_TEST(test_trim_line_both);
  RUN_TEST(test_trim_line_all_spaces);
  RUN_TEST(test_trim_line_empty);
  RUN_TEST(test_trim_line_tabs);
  RUN_TEST(test_string_array_init);
  RUN_TEST(test_string_array_init_null);
  RUN_TEST(test_string_array_push);
  RUN_TEST(test_string_array_push_resize);
  RUN_TEST(test_string_array_push_multiple_resize);
  RUN_TEST(test_string_array_push_into_uninit_fails);
  RUN_TEST(test_string_array_destroy_empty);
  RUN_TEST(test_string_array_destroy_zeroed);
  RUN_TEST(test_duplicate_string_array);
  RUN_TEST(test_duplicate_string_array_null_src);
  RUN_TEST(test_duplicate_string_array_null_dest);
  RUN_TEST(test_duplicate_string_array_small_dest);
  return UNITY_END();
}
