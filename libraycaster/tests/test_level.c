#include "internal/level/level_int.h"
#include "raycaster/level.h"
#include "unity.h"

#include <stdlib.h>

static uint32_t test_width(RcLevel *w) { (void)w; return 10; }
static uint32_t test_height(RcLevel *w) { (void)w; return 20; }
static uint32_t test_wall(RcLevel *w, int x, int y) { (void)w; (void)x; (void)y; return 5; }
static uint32_t test_floor(RcLevel *w, int x, int y) { (void)w; (void)x; (void)y; return 3; }
static uint32_t test_ceil(RcLevel *w, int x, int y) { (void)w; (void)x; (void)y; return 7; }
static uint32_t test_unit_size(RcLevel *w) { (void)w; return 32; }
static void test_sprites(RcLevel *w, RcSprite **out, uint32_t *count)
{
  (void)w; *out = NULL; *count = 0;
}
static void test_update(RcLevel *w, float dt) { (void)w; (void)dt; }
static void test_destroy(RcLevel *w) { free(w); }

static RcLevel *create_test_level(void)
{
  static RcLevelVtbl vtbl = {
    .width = test_width,
    .height = test_height,
    .wall = test_wall,
    .floor = test_floor,
    .ceil = test_ceil,
    .unit_size = test_unit_size,
    .sprites = test_sprites,
    .update = test_update,
    .destroy = test_destroy,
  };

  RcLevel *w = calloc(1, sizeof(RcLevel));
  w->vtbl = &vtbl;
  w->impl = NULL;
  return w;
}

void setUp(void) {}
void tearDown(void) {}

void test_destroy_null(void)
{
  rc_level_destroy(NULL);
  TEST_PASS();
}

void test_get_width(void)
{
  RcLevel *w = create_test_level();
  TEST_ASSERT_EQUAL_INT(10, rc_level_get_width(w));
  rc_level_destroy(w);
}

void test_get_height(void)
{
  RcLevel *w = create_test_level();
  TEST_ASSERT_EQUAL_INT(20, rc_level_get_height(w));
  rc_level_destroy(w);
}

void test_get_wall(void)
{
  RcLevel *w = create_test_level();
  TEST_ASSERT_EQUAL_INT(5, rc_level_get_wall(w, 0, 0));
  rc_level_destroy(w);
}

void test_get_floor(void)
{
  RcLevel *w = create_test_level();
  TEST_ASSERT_EQUAL_INT(3, rc_level_get_floor(w, 0, 0));
  rc_level_destroy(w);
}

void test_get_ceil(void)
{
  RcLevel *w = create_test_level();
  TEST_ASSERT_EQUAL_INT(7, rc_level_get_ceil(w, 0, 0));
  rc_level_destroy(w);
}

void test_get_unit_size(void)
{
  RcLevel *w = create_test_level();
  TEST_ASSERT_EQUAL_INT(32, rc_level_get_unit_size(w));
  rc_level_destroy(w);
}

void test_get_sprites(void)
{
  RcLevel *w = create_test_level();
  RcSprite *sprites;
  uint32_t count;
  rc_level_get_sprites(w, &sprites, &count);
  TEST_ASSERT_EQUAL_INT(0, count);
  TEST_ASSERT_NULL(sprites);
  rc_level_destroy(w);
}

void test_update_no_crash(void)
{
  RcLevel *w = create_test_level();
  rc_level_update(w, 0.016f);
  TEST_PASS();
  rc_level_destroy(w);
}

int main(void)
{
  UNITY_BEGIN();
  RUN_TEST(test_destroy_null);
  RUN_TEST(test_get_width);
  RUN_TEST(test_get_height);
  RUN_TEST(test_get_wall);
  RUN_TEST(test_get_floor);
  RUN_TEST(test_get_ceil);
  RUN_TEST(test_get_unit_size);
  RUN_TEST(test_get_sprites);
  RUN_TEST(test_update_no_crash);
  return UNITY_END();
}
