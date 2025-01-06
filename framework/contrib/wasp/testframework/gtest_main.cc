#include <stdio.h>
#include "gtest/gtest.h"
#if defined _WIN32 || defined __CYGWIN__
  #define TFW_PUBLIC __declspec(dllexport)
  #define TFW_LOCAL
#else
  #define TFW_PUBLIC
  #define TFW_LOCAL __declspec(dllimport)
#endif

int TFW_PUBLIC main(int argc, char **argv) {
  printf("Running main() from testframework/gtest_main.cc\n");
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
