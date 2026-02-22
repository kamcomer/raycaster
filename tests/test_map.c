#include "unity.h"
#include "map.h"
#include <stdlib.h>

void test_load_valid_map(void)
{
  Map map = load_map("../assets/maps/test_valid.txt");
  
  TEST_ASSERT_EQUAL(3, map.width);
  TEST_ASSERT_EQUAL(3, map.height);
  
  TEST_ASSERT_EQUAL(1, map.walls[0][0]);
  TEST_ASSERT_EQUAL(0, map.walls[1][1]);
  TEST_ASSERT_EQUAL(1, map.walls[2][2]);
  
  TEST_ASSERT_EQUAL(2, map.floor[0][0]);
  TEST_ASSERT_EQUAL(1, map.ceil[0][0]);
  
  free_map(map);
}

int main(void)
{
  UNITY_BEGIN();
  RUN_TEST(test_load_valid_map);
  UNITY_END();
}
