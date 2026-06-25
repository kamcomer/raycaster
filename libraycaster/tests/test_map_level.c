#include "internal/level/map_level_int.h"
#include "raycaster/raycaster.h"
#include "unity.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *VAL_MAP = "[MAP]\n"
                             "3 2\n"
                             "1 2 3\n"
                             "4 5 6\n"
                             "[CEIL]\n"
                             "7 8 9\n"
                             "0 1 2\n"
                             "[FLOOR]\n"
                             "3 4 5\n"
                             "6 7 8\n"
                             "[TEXTURES]\n"
                             "wall1.png\n"
                             "wall2.png\n"
                             "[SPRITES]\n"
                             "1.5 2.5 0\n"
                             "10.0 20.0 1\n";

static const char *MIN_MAP = "[MAP]\n"
                             "2 1\n"
                             "9 9\n";

static const char *TEX_MAP = "[MAP]\n"
                             "1 1\n"
                             "0\n"
                             "[CEIL]\n"
                             "0\n"
                             "[FLOOR]\n"
                             "0\n"
                             "[TEXTURES]\n"
                             "tex_a.png\n"
                             "tex_b.png\n"
                             "tex_c.png\n";

static const char *SPR_MAP = "[MAP]\n"
                             "1 1\n"
                             "0\n"
                             "[CEIL]\n"
                             "0\n"
                             "[FLOOR]\n"
                             "0\n"
                             "[SPRITE_TYPES]\n"
                             "1: type_a.png\n"
                             "2: type_b.png\n"
                             "3: type_c.png\n"
                             "4: type_d.png\n"
                             "5: type_e.png\n"
                             "6: type_f.png\n"
                             "7: type_g.png\n"
                             "[SPRITES]\n"
                             "1.0 2.0 3\n"
                             "4.0 5.0 6\n";

static void write_temp(const char *content, const char *path)
{
  FILE *f = fopen(path, "w");
  TEST_ASSERT_NOT_NULL(f);
  fputs(content, f);
  fclose(f);
}

void setUp(void) {}

void tearDown(void) {}

void test_destroy_null(void) { rc_level_destroy(NULL); }

void test_create_empty(void)
{
  RcLevel *w = rc_level_create_empty(10, 20);
  TEST_ASSERT_NOT_NULL(w);
  TEST_ASSERT_EQUAL_INT(10, rc_level_get_width(w));
  TEST_ASSERT_EQUAL_INT(20, rc_level_get_height(w));
  for (int y = 0; y < 20; y++)
    for (int x = 0; x < 10; x++) {
      TEST_ASSERT_EQUAL_INT(0, rc_level_get_wall(w, x, y));
      TEST_ASSERT_EQUAL_INT(0, rc_level_get_floor(w, x, y));
      TEST_ASSERT_EQUAL_INT(0, rc_level_get_ceil(w, x, y));
    }
  rc_level_destroy(w);
}

void test_unit_size_default(void)
{
  RcLevel *w = rc_level_create_empty(2, 2);
  TEST_ASSERT_EQUAL_INT(20, rc_level_get_unit_size(w));
  rc_level_destroy(w);
}

void test_parse_valid(void)
{
  write_temp(VAL_MAP, "test_valid.txt");
  RcLevel *w = rc_level_load_from_file("test_valid.txt");
  TEST_ASSERT_NOT_NULL(w);
  TEST_ASSERT_EQUAL_INT(3, rc_level_get_width(w));
  TEST_ASSERT_EQUAL_INT(2, rc_level_get_height(w));

  TEST_ASSERT_EQUAL_INT(1, rc_level_get_wall(w, 0, 0));
  TEST_ASSERT_EQUAL_INT(2, rc_level_get_wall(w, 1, 0));
  TEST_ASSERT_EQUAL_INT(3, rc_level_get_wall(w, 2, 0));
  TEST_ASSERT_EQUAL_INT(4, rc_level_get_wall(w, 0, 1));
  TEST_ASSERT_EQUAL_INT(5, rc_level_get_wall(w, 1, 1));
  TEST_ASSERT_EQUAL_INT(6, rc_level_get_wall(w, 2, 1));

  TEST_ASSERT_EQUAL_INT(7, rc_level_get_ceil(w, 0, 0));
  TEST_ASSERT_EQUAL_INT(8, rc_level_get_ceil(w, 1, 0));
  TEST_ASSERT_EQUAL_INT(9, rc_level_get_ceil(w, 2, 0));
  TEST_ASSERT_EQUAL_INT(0, rc_level_get_ceil(w, 0, 1));
  TEST_ASSERT_EQUAL_INT(1, rc_level_get_ceil(w, 1, 1));
  TEST_ASSERT_EQUAL_INT(2, rc_level_get_ceil(w, 2, 1));

  TEST_ASSERT_EQUAL_INT(3, rc_level_get_floor(w, 0, 0));
  TEST_ASSERT_EQUAL_INT(4, rc_level_get_floor(w, 1, 0));
  TEST_ASSERT_EQUAL_INT(5, rc_level_get_floor(w, 2, 0));
  TEST_ASSERT_EQUAL_INT(6, rc_level_get_floor(w, 0, 1));
  TEST_ASSERT_EQUAL_INT(7, rc_level_get_floor(w, 1, 1));
  TEST_ASSERT_EQUAL_INT(8, rc_level_get_floor(w, 2, 1));

  rc_level_destroy(w);
  remove("test_valid.txt");
}

void test_parse_minimal(void)
{
  write_temp(MIN_MAP, "test_minimal.txt");
  RcLevel *w = rc_level_load_from_file("test_minimal.txt");
  TEST_ASSERT_NOT_NULL(w);
  TEST_ASSERT_EQUAL_INT(2, rc_level_get_width(w));
  TEST_ASSERT_EQUAL_INT(1, rc_level_get_height(w));
  TEST_ASSERT_EQUAL_INT(9, rc_level_get_wall(w, 0, 0));
  TEST_ASSERT_EQUAL_INT(9, rc_level_get_wall(w, 1, 0));
  rc_level_destroy(w);
  remove("test_minimal.txt");
}

void test_parse_file_not_found(void)
{
  RcLevel *w = rc_level_load_from_file("nonexistent.txt");
  TEST_ASSERT_NULL(w);
}

void test_bounds_check(void)
{
  write_temp(MIN_MAP, "test_bounds.txt");
  RcLevel *w = rc_level_load_from_file("test_bounds.txt");
  TEST_ASSERT_NOT_NULL(w);
  TEST_ASSERT_EQUAL_INT(0, rc_level_get_wall(w, -1, 0));
  TEST_ASSERT_EQUAL_INT(0, rc_level_get_wall(w, 0, -1));
  TEST_ASSERT_EQUAL_INT(0, rc_level_get_wall(w, 99, 0));
  TEST_ASSERT_EQUAL_INT(0, rc_level_get_wall(w, 0, 99));
  rc_level_destroy(w);
  remove("test_bounds.txt");
}

void test_parse_sprites(void)
{
  write_temp(SPR_MAP, "test_sprites.txt");
  RcLevel *w = rc_level_load_from_file("test_sprites.txt");
  TEST_ASSERT_NOT_NULL(w);

  RcSprite *sprites;
  uint32_t count;
  rc_level_get_sprites(w, &sprites, &count);
  TEST_ASSERT_EQUAL_INT(2, count);
  TEST_ASSERT_NOT_NULL(sprites);

  TEST_ASSERT_EQUAL_DOUBLE(1.0, sprites[0].pos.x);
  TEST_ASSERT_EQUAL_DOUBLE(2.0, sprites[0].pos.y);
  TEST_ASSERT_EQUAL_INT(3, sprites[0].texture_id);
  TEST_ASSERT_FALSE(sprites[0].is_dynamic);

  TEST_ASSERT_EQUAL_DOUBLE(4.0, sprites[1].pos.x);
  TEST_ASSERT_EQUAL_DOUBLE(5.0, sprites[1].pos.y);
  TEST_ASSERT_EQUAL_INT(6, sprites[1].texture_id);

  MapLevelData *data = (MapLevelData *)w->impl;
  TEST_ASSERT_EQUAL_INT(7, data->sprite_types.len);
  TEST_ASSERT_NOT_NULL(data->sprite_types.items);
  TEST_ASSERT_EQUAL_STRING("type_a.png", data->sprite_types.items[0].path);
  TEST_ASSERT_EQUAL_STRING("type_g.png", data->sprite_types.items[6].path);

  rc_level_destroy(w);
  remove("test_sprites.txt");
}

void test_parse_textures(void)
{
  write_temp(TEX_MAP, "test_tex.txt");
  RcLevel *w = rc_level_load_from_file("test_tex.txt");
  TEST_ASSERT_NOT_NULL(w);

  MapLevelData *data = (MapLevelData *)w->impl;
  TEST_ASSERT_EQUAL_INT(3, data->tex_paths.len);
  TEST_ASSERT_NOT_NULL(data->tex_paths.strs[0]);
  TEST_ASSERT_NOT_NULL(data->tex_paths.strs[1]);
  TEST_ASSERT_NOT_NULL(data->tex_paths.strs[2]);
  TEST_ASSERT_EQUAL_STRING("tex_a.png", data->tex_paths.strs[0]);
  TEST_ASSERT_EQUAL_STRING("tex_b.png", data->tex_paths.strs[1]);
  TEST_ASSERT_EQUAL_STRING("tex_c.png", data->tex_paths.strs[2]);

  rc_level_destroy(w);
  remove("test_tex.txt");
}

void test_vtable_dispatch(void)
{
  write_temp(VAL_MAP, "test_vtbl.txt");
  RcLevel *w = rc_level_load_from_file("test_vtbl.txt");
  TEST_ASSERT_NOT_NULL(w);

  TEST_ASSERT_EQUAL_INT(3, rc_level_get_width(w));
  TEST_ASSERT_EQUAL_INT(2, rc_level_get_height(w));
  TEST_ASSERT_EQUAL_INT(1, rc_level_get_wall(w, 0, 0));
  TEST_ASSERT_EQUAL_INT(7, rc_level_get_ceil(w, 0, 0));
  TEST_ASSERT_EQUAL_INT(3, rc_level_get_floor(w, 0, 0));

  rc_level_destroy(w);
  remove("test_vtbl.txt");
}

int main(void)
{
  UNITY_BEGIN();
  RUN_TEST(test_destroy_null);
  RUN_TEST(test_create_empty);
  RUN_TEST(test_unit_size_default);
  RUN_TEST(test_parse_valid);
  RUN_TEST(test_parse_minimal);
  RUN_TEST(test_parse_file_not_found);
  RUN_TEST(test_bounds_check);
  RUN_TEST(test_parse_sprites);
  RUN_TEST(test_parse_textures);
  RUN_TEST(test_vtable_dispatch);
  return UNITY_END();
}
