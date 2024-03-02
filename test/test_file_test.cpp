#include <gtest/gtest.h>

#include "test_file.h"

TEST(TestFileTest, five_to_33) {
  // Setup
  int result;

  // Act
  result = stupidity(5);

  // Verify
  ASSERT_EQ(result, 33);
}

TEST(TestFileTest, four_to_33) {
  // Setup
  int result;

  // Act
  result = stupidity(4);

  // Verify
  ASSERT_EQ(result, 33);
}
