#include <gtest/gtest.h>

#include "si_flow_sensor.h"

TEST(SiFlowSensorTest, five_to_33) {
  // Setup
  int result;

  // Act
  result = stupidity(5);

  // Verify
  ASSERT_EQ(result, 33);
}

TEST(SiFlowSensorTest, four_to_33) {
  // Setup
  int result;

  // Act
  result = stupidity(4);

  // Verify
  ASSERT_EQ(result, 33);
}