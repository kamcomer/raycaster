#include "raycaster/camera.h"
#include "raycaster/types.h"
#include "unity.h"

#include <math.h>

#define EPSILON 1e-6f

void setUp(void) {}
void tearDown(void) {}

void test_create_defaults(void)
{
  RcCamera *cam = rc_camera_create(800, 600);
  TEST_ASSERT_NOT_NULL(cam);
  TEST_ASSERT_EQUAL_INT(800, cam->width);
  TEST_ASSERT_EQUAL_INT(600, cam->height);
  TEST_ASSERT_EQUAL_FLOAT(60.0f, cam->fov);
  TEST_ASSERT_EQUAL_DOUBLE(-1.0, cam->dir.x);
  TEST_ASSERT_EQUAL_DOUBLE(0.0, cam->dir.y);
  TEST_ASSERT_EQUAL_DOUBLE(0.0, cam->plane.x);

  double expected_plane_y = -1.0 * tan(60.0 * RC_DEG_TO_RAD / 2.0);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, expected_plane_y, cam->plane.y);

  rc_camera_destroy(cam);
}

void test_destroy_null(void)
{
  rc_camera_destroy(NULL);
  TEST_PASS();
}

void test_set_position(void)
{
  RcCamera *cam = rc_camera_create(800, 600);
  rc_camera_set_position(cam, 10.5, 20.5);
  TEST_ASSERT_EQUAL_DOUBLE(10.5, cam->pos.x);
  TEST_ASSERT_EQUAL_DOUBLE(20.5, cam->pos.y);
  rc_camera_destroy(cam);
}

void test_set_direction(void)
{
  RcCamera *cam = rc_camera_create(800, 600);
  rc_camera_set_direction(cam, 0.0, 1.0);
  TEST_ASSERT_EQUAL_DOUBLE(0.0, cam->dir.x);
  TEST_ASSERT_EQUAL_DOUBLE(1.0, cam->dir.y);

  double fov_rad = 60.0 * RC_DEG_TO_RAD / 2.0;
  double expected_plane_x = -1.0 * tan(fov_rad);
  double expected_plane_y = 0.0 * tan(fov_rad);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, expected_plane_x, cam->plane.x);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, expected_plane_y, cam->plane.y);
  rc_camera_destroy(cam);
}

void test_set_fov(void)
{
  RcCamera *cam = rc_camera_create(800, 600);
  rc_camera_set_fov(cam, 90.0f);
  TEST_ASSERT_EQUAL_FLOAT(90.0f, cam->fov);

  double fov_rad = 90.0 * RC_DEG_TO_RAD / 2.0;
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 0.0, cam->plane.x);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, -1.0 * tan(fov_rad), cam->plane.y);
  rc_camera_destroy(cam);
}

void test_rotate_ninety(void)
{
  RcCamera *cam = rc_camera_create(800, 600);
  rc_camera_rotate(cam, RC_PI_2);

  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 0.0, cam->dir.x);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, -1.0, cam->dir.y);

  double fov_rad = 60.0 * RC_DEG_TO_RAD / 2.0;
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 1.0 * tan(fov_rad), cam->plane.x);

  rc_camera_destroy(cam);
}

void test_rotate_one_eighty(void)
{
  RcCamera *cam = rc_camera_create(800, 600);
  rc_camera_rotate(cam, RC_PI);

  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 1.0, cam->dir.x);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 0.0, cam->dir.y);
  rc_camera_destroy(cam);
}

void test_rotate_then_set_fov(void)
{
  RcCamera *cam = rc_camera_create(800, 600);
  rc_camera_rotate(cam, RC_PI_2);
  rc_camera_set_fov(cam, 90.0f);

  TEST_ASSERT_EQUAL_FLOAT(90.0f, cam->fov);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 0.0, cam->dir.x);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, -1.0, cam->dir.y);

  double fov_rad = 90.0 * RC_DEG_TO_RAD / 2.0;
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 1.0 * tan(fov_rad), cam->plane.x);

  rc_camera_destroy(cam);
}

void test_get_view_dimensions(void)
{
  RcCamera *cam = rc_camera_create(1024, 768);
  uint32_t w, h;
  rc_camera_get_view_dimensions(cam, &w, &h);
  TEST_ASSERT_EQUAL_INT(1024, w);
  TEST_ASSERT_EQUAL_INT(768, h);
  rc_camera_destroy(cam);
}

void test_rotate_negative_ninety(void)
{
  RcCamera *cam = rc_camera_create(800, 600);
  rc_camera_rotate(cam, -RC_PI_2);

  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 0.0, cam->dir.x);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 1.0, cam->dir.y);
  rc_camera_destroy(cam);
}

int main(void)
{
  UNITY_BEGIN();
  RUN_TEST(test_create_defaults);
  RUN_TEST(test_destroy_null);
  RUN_TEST(test_set_position);
  RUN_TEST(test_set_direction);
  RUN_TEST(test_set_fov);
  RUN_TEST(test_rotate_ninety);
  RUN_TEST(test_rotate_one_eighty);
  RUN_TEST(test_rotate_then_set_fov);
  RUN_TEST(test_get_view_dimensions);
  RUN_TEST(test_rotate_negative_ninety);
  return UNITY_END();
}
