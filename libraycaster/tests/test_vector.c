#include "raycaster/vector.h"
#include "raycaster/types.h"
#include "unity.h"

#define EPSILON 1e-9

void setUp(void) {}
void tearDown(void) {}

void test_create_computes_mag_and_angle(void)
{
  RcVector v = rc_vector_create(3.0, 4.0);
  TEST_ASSERT_EQUAL_DOUBLE(3.0, v.x);
  TEST_ASSERT_EQUAL_DOUBLE(4.0, v.y);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 5.0, v.mag);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, atan2(4.0, 3.0), v.angle);
}

void test_create_negative(void)
{
  RcVector v = rc_vector_create(-1.0, -1.0);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 1.4142135623730951, v.mag);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 3.9269908169872414, v.angle);
}

void test_from_point_sets_zero_mag_angle(void)
{
  RcVector v = rc_vector_from_point(5.0, 10.0);
  TEST_ASSERT_EQUAL_DOUBLE(5.0, v.x);
  TEST_ASSERT_EQUAL_DOUBLE(10.0, v.y);
  TEST_ASSERT_EQUAL_DOUBLE(0.0, v.mag);
  TEST_ASSERT_EQUAL_DOUBLE(0.0, v.angle);
}

void test_set_magnitude_scales_vector(void)
{
  RcVector v = rc_vector_create(1.0, 0.0);
  rc_vector_set_magnitude(&v, 5.0);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 5.0, v.mag);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 5.0, v.x);
  TEST_ASSERT_EQUAL_DOUBLE(0.0, v.y);
}

void test_set_magnitude_zero_vector(void)
{
  RcVector v = rc_vector_from_point(0.0, 0.0);
  rc_vector_set_magnitude(&v, 3.0);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 3.0, v.mag);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 3.0, v.x);
}

void test_translate(void)
{
  RcVector v = rc_vector_create(1.0, 2.0);
  RcVector origin = rc_vector_from_point(10.0, 20.0);
  rc_vector_translate(&v, origin);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 11.0, v.x);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 22.0, v.y);
}

void test_calc_mag_via_create(void)
{
  RcVector v = rc_vector_create(3.0, 4.0);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 5.0, v.mag);
}

void test_calc_angle_via_create(void)
{
  RcVector v = rc_vector_create(1.0, 0.0);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 0.0, v.angle);

  v = rc_vector_create(0.0, 1.0);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, RC_PI_2, v.angle);

  v = rc_vector_create(-1.0, 0.0);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, RC_PI, v.angle);

  v = rc_vector_create(0.0, -1.0);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 3.0 * RC_PI_2, v.angle);
}

void test_normalize(void)
{
  RcVector v = rc_vector_create(0.0, 5.0);
  rc_vector_normalize(&v);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 1.0, v.mag);
  TEST_ASSERT_EQUAL_DOUBLE(0.0, v.x);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 1.0, v.y);
}

void test_normalize_zero_vector(void)
{
  RcVector v = rc_vector_from_point(0.0, 0.0);
  rc_vector_normalize(&v);
  TEST_ASSERT_EQUAL_DOUBLE(0.0, v.mag);
}

void test_rotate_quarter(void)
{
  RcVector v = rc_vector_create(1.0, 0.0);
  rc_vector_rotate(&v, RC_PI_2);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 0.0, v.x);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 1.0, v.y);
}

void test_rotate_half(void)
{
  RcVector v = rc_vector_create(1.0, 0.0);
  rc_vector_rotate(&v, RC_PI);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, -1.0, v.x);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 0.0, v.y);
}

void test_rotate_negative(void)
{
  RcVector v = rc_vector_create(1.0, 0.0);
  rc_vector_rotate(&v, -RC_PI_2);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 0.0, v.x);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, -1.0, v.y);
}

void test_rotate_full_cycle(void)
{
  RcVector v = rc_vector_create(3.0, 4.0);
  rc_vector_rotate(&v, 2.0 * RC_PI);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 3.0, v.x);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 4.0, v.y);
}

void test_dot_perpendicular(void)
{
  RcVector a = rc_vector_create(1.0, 0.0);
  RcVector b = rc_vector_create(0.0, 1.0);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 0.0, rc_vector_dot(a, b));
}

void test_dot_parallel(void)
{
  RcVector a = rc_vector_create(2.0, 0.0);
  RcVector b = rc_vector_create(3.0, 0.0);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 6.0, rc_vector_dot(a, b));
}

void test_dot_opposite(void)
{
  RcVector a = rc_vector_create(1.0, 0.0);
  RcVector b = rc_vector_create(-1.0, 0.0);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, -1.0, rc_vector_dot(a, b));
}

void test_print_does_not_crash(void)
{
  RcVector v = rc_vector_create(1.0, 2.0);
  rc_vector_print(v);
  TEST_PASS();
}

int main(void)
{
  UNITY_BEGIN();
  RUN_TEST(test_create_computes_mag_and_angle);
  RUN_TEST(test_create_negative);
  RUN_TEST(test_from_point_sets_zero_mag_angle);
  RUN_TEST(test_set_magnitude_scales_vector);
  RUN_TEST(test_set_magnitude_zero_vector);
  RUN_TEST(test_translate);
  RUN_TEST(test_calc_mag_via_create);
  RUN_TEST(test_calc_angle_via_create);
  RUN_TEST(test_normalize);
  RUN_TEST(test_normalize_zero_vector);
  RUN_TEST(test_rotate_quarter);
  RUN_TEST(test_rotate_half);
  RUN_TEST(test_rotate_negative);
  RUN_TEST(test_rotate_full_cycle);
  RUN_TEST(test_dot_perpendicular);
  RUN_TEST(test_dot_parallel);
  RUN_TEST(test_dot_opposite);
  RUN_TEST(test_print_does_not_crash);
  return UNITY_END();
}
