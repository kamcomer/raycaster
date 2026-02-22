#ifndef UNITY_H
#define UNITY_H

#include <stdio.h>
#include <setjmp.h>

extern int unity_test_count;
extern int unity_fail_count;
extern jmp_buf unity_abort_frame;

#define UNITY_BEGIN() unity_test_count = 0; unity_fail_count = 0
#define UNITY_END() unity_finish()

#define TEST_ASSERT(condition) \
  do { \
    unity_test_count++; \
    if (!(condition)) { \
      unity_fail_count++; \
      printf("FAIL: %s:%d: %s\n", __FILE__, __LINE__, #condition); \
      longjmp(unity_abort_frame, 1); \
    } \
  } while(0)

#define TEST_ASSERT_EQUAL(expected, actual) \
  do { \
    unity_test_count++; \
    if ((expected) != (actual)) { \
      unity_fail_count++; \
      printf("FAIL: %s:%d: Expected %d, got %d\n", __FILE__, __LINE__, (int)(expected), (int)(actual)); \
      longjmp(unity_abort_frame, 1); \
    } \
  } while(0)

#define TEST_ASSERT_EQUAL_FLOAT(expected, actual) \
  do { \
    unity_test_count++; \
    float _exp = (expected); \
    float _act = (actual); \
    if (_exp != _act) { \
      unity_fail_count++; \
      printf("FAIL: %s:%d: Expected %f, got %f\n", __FILE__, __LINE__, _exp, _act); \
      longjmp(unity_abort_frame, 1); \
    } \
  } while(0)

#define TEST_ASSERT_TRUE(condition) TEST_ASSERT(condition)
#define TEST_ASSERT_FALSE(condition) TEST_ASSERT(!(condition))

#define RUN_TEST(func) \
  do { \
    printf("Running %s...\n", #func); \
    if (setjmp(unity_abort_frame) == 0) { \
      func(); \
    } \
  } while(0)

void unity_finish(void);

#endif
