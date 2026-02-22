#include "unity.h"
#include "vector.h"
#include <math.h>

void test_set_vector(void)
{
  Vector v = set_vector(3.0, 4.0);
  TEST_ASSERT_EQUAL(3.0, v.x);
  TEST_ASSERT_EQUAL(4.0, v.y);
  TEST_ASSERT_EQUAL(5.0, v.mag);
}

void test_set_vector_magnitude(void)
{
  Vector v = set_vector(3.0, 4.0);
  set_vector_magnitude(&v, 10.0);
  TEST_ASSERT_EQUAL(10.0, v.mag);
}

void test_rotate_vector(void)
{
  Vector v = set_vector(1.0, 0.0);
  rotate_vector(&v, M_PI / 2);
  TEST_ASSERT_TRUE(fabs(v.x) < 0.0001);
  TEST_ASSERT_EQUAL_FLOAT(1.0, v.y);
}

void test_normalize_vector(void)
{
  Vector v = set_vector(3.0, 4.0);
  normalize_vector(&v);
  TEST_ASSERT_EQUAL(1.0, v.mag);
}

void test_translate_vector(void)
{
  Vector v = set_vector(1.0, 1.0);
  Vector origin = set_point(2.0, 3.0);
  translate_vector(&v, origin);
  TEST_ASSERT_EQUAL(3.0, v.x);
  TEST_ASSERT_EQUAL(4.0, v.y);
}

int main(void)
{
  UNITY_BEGIN();
  RUN_TEST(test_set_vector);
  RUN_TEST(test_set_vector_magnitude);
  RUN_TEST(test_rotate_vector);
  RUN_TEST(test_normalize_vector);
  RUN_TEST(test_translate_vector);
  UNITY_END();
}
