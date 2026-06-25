#include "raycaster/actor.h"
#include "raycaster/level.h"
#include "raycaster/types.h"
#include "unity.h"

static int g_update_called;

static void test_update(RcActor *a, RcLevel *world, RcInput *input, float dt)
{
  (void)a;
  (void)world;
  (void)input;
  (void)dt;
  g_update_called = 1;
}

static RcVector test_get_position(RcActor *a)
{
  RcVector v = {a->pos.x + 1, a->pos.y + 1, 0, 0};
  return v;
}

static uint32_t test_get_texture(RcActor *a)
{
  (void)a;
  return 42;
}

void setUp(void)
{
  g_update_called = 0;
}
void tearDown(void) {}

void test_null_actor_update_no_crash(void)
{
  rc_actor_update(NULL, NULL, NULL, 0.0f);
  TEST_PASS();
}

void test_null_actor_get_position_zero(void)
{
  RcVector v = rc_actor_get_position(NULL);
  TEST_ASSERT_EQUAL_DOUBLE(0.0, v.x);
  TEST_ASSERT_EQUAL_DOUBLE(0.0, v.y);
}

void test_null_actor_get_direction_zero(void)
{
  RcVector v = rc_actor_get_direction(NULL);
  TEST_ASSERT_EQUAL_DOUBLE(0.0, v.x);
  TEST_ASSERT_EQUAL_DOUBLE(0.0, v.y);
}

void test_null_actor_get_texture_zero(void)
{
  uint32_t t = rc_actor_get_texture(NULL);
  TEST_ASSERT_EQUAL_INT(0, t);
}

void test_null_actor_destroy_no_crash(void)
{
  rc_actor_destroy(NULL);
  TEST_PASS();
}

void test_actor_with_null_vtbl_update(void)
{
  RcActor a;
  a.vtbl = NULL;
  rc_actor_update(&a, NULL, NULL, 0.0f);
  TEST_PASS();
}

void test_actor_with_null_vtbl_get_position(void)
{
  RcActor a;
  a.vtbl = NULL;
  RcVector v = rc_actor_get_position(&a);
  TEST_ASSERT_EQUAL_DOUBLE(0.0, v.x);
  TEST_ASSERT_EQUAL_DOUBLE(0.0, v.y);
}

void test_actor_with_null_vtbl_get_texture(void)
{
  RcActor a;
  a.vtbl = NULL;
  TEST_ASSERT_EQUAL_INT(0, rc_actor_get_texture(&a));
}

void test_actor_with_null_vtbl_destroy(void)
{
  RcActor a;
  a.vtbl = NULL;
  rc_actor_destroy(&a);
  TEST_PASS();
}

void test_actor_dispatch_update(void)
{
  RcActorVtbl vtbl = {0};
  vtbl.update = test_update;

  RcActor a;
  a.vtbl = &vtbl;
  a.pos.x = 0;
  a.pos.y = 0;

  rc_actor_update(&a, NULL, NULL, 1.0f);
  TEST_ASSERT_EQUAL_INT(1, g_update_called);
}

void test_actor_dispatch_get_position(void)
{
  RcActorVtbl vtbl = {0};
  vtbl.get_position = test_get_position;

  RcActor a;
  a.vtbl = &vtbl;
  a.pos.x = 5.0;
  a.pos.y = 10.0;

  RcVector v = rc_actor_get_position(&a);
  TEST_ASSERT_EQUAL_DOUBLE(6.0, v.x);
  TEST_ASSERT_EQUAL_DOUBLE(11.0, v.y);
}

void test_actor_dispatch_get_texture(void)
{
  RcActorVtbl vtbl = {0};
  vtbl.get_texture = test_get_texture;

  RcActor a;
  a.vtbl = &vtbl;

  TEST_ASSERT_EQUAL_INT(42, rc_actor_get_texture(&a));
}

int main(void)
{
  UNITY_BEGIN();
  RUN_TEST(test_null_actor_update_no_crash);
  RUN_TEST(test_null_actor_get_position_zero);
  RUN_TEST(test_null_actor_get_direction_zero);
  RUN_TEST(test_null_actor_get_texture_zero);
  RUN_TEST(test_null_actor_destroy_no_crash);
  RUN_TEST(test_actor_with_null_vtbl_update);
  RUN_TEST(test_actor_with_null_vtbl_get_position);
  RUN_TEST(test_actor_with_null_vtbl_get_texture);
  RUN_TEST(test_actor_with_null_vtbl_destroy);
  RUN_TEST(test_actor_dispatch_update);
  RUN_TEST(test_actor_dispatch_get_position);
  RUN_TEST(test_actor_dispatch_get_texture);
  return UNITY_END();
}
