#include "unity.h"
#include "sprite.h"
#include <string.h>

void test_sort_sprites_descending(void)
{
  int order[] = {0, 1, 2, 3, 4};
  double dist[] = {1.0, 4.0, 2.0, 3.0, 0.5};

  sort_sprites(order, dist, 5);

  TEST_ASSERT_EQUAL(1, order[0]);
  TEST_ASSERT_EQUAL(3, order[1]);
  TEST_ASSERT_EQUAL(2, order[2]);
  TEST_ASSERT_EQUAL(0, order[3]);
  TEST_ASSERT_EQUAL(4, order[4]);
}

void test_sort_sprites_already_sorted(void)
{
  int order[] = {0, 1, 2};
  double dist[] = {3.0, 2.0, 1.0};

  sort_sprites(order, dist, 3);

  TEST_ASSERT_EQUAL(0, order[0]);
  TEST_ASSERT_EQUAL(1, order[1]);
  TEST_ASSERT_EQUAL(2, order[2]);
}

void test_sort_sprites_single_element(void)
{
  int order[] = {0};
  double dist[] = {1.0};

  sort_sprites(order, dist, 1);

  TEST_ASSERT_EQUAL(0, order[0]);
}

void test_sort_sprites_reverse_sorted(void)
{
  int order[] = {0, 1, 2};
  double dist[] = {1.0, 2.0, 3.0};

  sort_sprites(order, dist, 3);

  TEST_ASSERT_EQUAL(2, order[0]);
  TEST_ASSERT_EQUAL(1, order[1]);
  TEST_ASSERT_EQUAL(0, order[2]);
}

int main(void)
{
  UNITY_BEGIN();
  RUN_TEST(test_sort_sprites_descending);
  RUN_TEST(test_sort_sprites_already_sorted);
  RUN_TEST(test_sort_sprites_single_element);
  RUN_TEST(test_sort_sprites_reverse_sorted);
  UNITY_END();
}
