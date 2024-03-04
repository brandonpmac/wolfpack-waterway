/**
 * @file control_test.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief test: control.cpp
 * @date 2024-03-03
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#include <gtest/gtest.h>

#include "control.h"

TEST(SiFlowSensorTest, four_to_33) {
  // Setup
  int result;

  // Act
  result = stupidity(4);

  // Verify
  ASSERT_EQ(result, 33);
}
