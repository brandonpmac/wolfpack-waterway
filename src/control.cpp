/**
 * @file control.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief main control loop
 * @date 2024-02-08
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 */

#include <Arduino.h>
#include <EEPROM.h>
#include <stdint.h>
#include <stdlib.h>

#include "control.h"
#include "log.h"
#include "menu.h"
#include "si_encoder.h"
#include "si_flow_sensor.h"
#include "si_relay.h"
#include "si_stepper.h"
#include "si_switch.h"
#include "sm.h"

#define TUNNEL_MIN_SPEED (175)
#define TUNNEL_MAX_SPEED (700)
#define TUNNEL_SINGLE_SPEED (400)

// function declarations
static uint32_t flow_rate_calc();
static void write_to_eeprom(int16_t number, eeprom_location_t address);
static int16_t read_from_eeprom(eeprom_location_t address);

// Variables ------------------------------------

// flow sensor variables
static float flow_sensor_1_count = 0;
static float flow_sensor_2_count = 0;
// static uint32_t flow_time = 0;
// static uint32_t flow_correction_factor = 220; // 21.5278208334
static uint8_t flow_sensor_updates = 0;
static bool my_data_record = false;
static uint16_t my_flow_correction =
    read_from_eeprom(EEPROM_LOCATION_FLOW_CORRECTION);

// switch variables
static bool my_sw_limit_max = false;
static bool my_sw_limit_min = false;
static bool my_sw_run = false;
static uint16_t encoder_increment = 5;
static bool my_switch_override = false;

// control variables
static uint32_t my_tunnel_setpoint = 0;
static uint32_t my_tunnel_speed = 0;
static bool my_speed_override = false;
static uint16_t my_override_value = 0;
static uint32_t my_test_start = 0;

// PID Variables
static int16_t my_p_const = read_from_eeprom(EEPROM_LOCATION_P_CONST);
static int16_t my_i_const = read_from_eeprom(EEPROM_LOCATION_I_CONST);
static int32_t i_term = 0;
static int32_t my_max_i = 100;
static int32_t my_min_i = -100;
static int16_t my_d_const = read_from_eeprom(EEPROM_LOCATION_D_CONST);
static int32_t control_motor_speed = 0;
static float my_last_value = 0;

// Pump variables
static bool my_pump_status = true;

// tasks ----------------------------------

/// @brief encoder task stages an increase or decrease in the tunnel target
/// based on the rotation of the encoder
void encoder_task(void) {
  si_encoder_event_t event = si_encoder_event_get();
  switch (event) {
  case ENCODER_EVENT_NONE:
    break;

  case ENCODER_EVENT_LEFT:
    if (tunnel_setpoint_get() == TUNNEL_MIN_SPEED) {
      break;
    }
    tunnel_setpoint_set(tunnel_setpoint_get() - encoder_increment);
    display_notification_send(DISPLAY_NOTIFICATION_TUNNEL_SETPOINT);
    break;

  case ENCODER_EVENT_RIGHT:
    if (tunnel_setpoint_get() == TUNNEL_MAX_SPEED) {
      break;
    }
    tunnel_setpoint_set(tunnel_setpoint_get() + encoder_increment);
    display_notification_send(DISPLAY_NOTIFICATION_TUNNEL_SETPOINT);
    break;

  default:
    LOG_INF("unhandled encoder event");
    break;
  }
}

/// @brief Checks switches to see if any action needs to be taken
void switch_task(void) {
  // checking the min limit switch
  if (si_switch_get(SW_LIMIT_MIN) != my_sw_limit_min) {
    display_notification_send(DISPLAY_NOTIFICATION_SW_LIMIT_MIN);
    my_sw_limit_min = !my_sw_limit_min;
    if (my_sw_limit_min & (control_motor_speed < 0)) {
      si_stepper_speed_set(0);
    }
  }
  // checking the max limit switch
  if (si_switch_get(SW_LIMIT_MAX) != my_sw_limit_max) {
    display_notification_send(DISPLAY_NOTIFICATION_SW_LIMIT_MAX);
    my_sw_limit_max = !my_sw_limit_max;
    if (my_sw_limit_max & (control_motor_speed > 0)) {
      si_stepper_speed_set(0);
    }
  }
  // check if the control switch is flipped
  if (si_switch_get(SW_RUN) != my_sw_run) {
    my_sw_run = !my_sw_run;
  }

  if (my_switch_override) {
    my_sw_run = true;
  }
}

/// @brief flow sensor loop
void flow_sensor_task(void) {
  uint32_t new_tunnel_speed = flow_rate_calc();
  if (my_tunnel_speed != new_tunnel_speed) {
    my_tunnel_speed = new_tunnel_speed;
  }
  if (flow_sensor_updates < 32) {
    flow_sensor_updates = flow_sensor_updates + 1;
  } else {
    display_notification_send(DISPLAY_NOTIFICATION_TUNNEL_SPEED);
    flow_sensor_updates = 0;
  }
}

/// @brief pid loop
void control_task(void) {

  if (my_data_record) {
    LOG_INF("%.8lu, %.4lu, %.4lu", millis() - my_test_start, my_tunnel_setpoint,
            my_tunnel_speed);
  }

  // PID value setup
  uint32_t error = my_tunnel_setpoint - my_tunnel_speed;

  // P Value
  uint32_t p_term = my_p_const * error;

  // I Value
  i_term = i_term + error;
  // Saturation check
  if (i_term > my_max_i) {
    i_term = my_max_i;
  } else if (i_term < my_min_i) {
    i_term = my_min_i;
  }
  i_term *= my_i_const;

  // D value
  uint32_t d_term = (my_tunnel_speed - my_last_value) * my_d_const;
  my_last_value = my_tunnel_speed;

  // PID Calculation
  // control_motor_speed = p_term + i_term - d_term;
  control_motor_speed = p_term + i_term - d_term;

  if ((control_motor_speed > 0) & si_switch_get(SW_LIMIT_MAX)) {
    control_motor_speed = 0;
  }

  if ((control_motor_speed < 0) & si_switch_get(SW_LIMIT_MIN)) {
    control_motor_speed = 0;
  }

  si_stepper_speed_set(control_motor_speed);
}

void pump_task(void) {
  if (my_tunnel_setpoint < TUNNEL_SINGLE_SPEED) {
    if (my_pump_status) {
      my_pump_status = false;
      byte last_pump = EEPROM.read(EEPROM_LOCATION_PUMP);
      if (last_pump == 1) {
        write_to_eeprom(0, EEPROM_LOCATION_PUMP);
        si_relay_set(RELAY_PUMP_1, LOW);
      } else {
        write_to_eeprom(1, EEPROM_LOCATION_PUMP);
        si_relay_set(RELAY_PUMP_2, LOW);
      }
      // my_p_const = read_from_eeprom(EEPROM_LOCATION_P_CONST_SINGLE);
      // my_i_const = read_from_eeprom(EEPROM_LOCATION_I_CONST_SINGLE);
      // my_d_const = read_from_eeprom(EEPROM_LOCATION_D_CONST_SINGLE);
    }
  } else {
    if (!my_pump_status) {
      my_pump_status = true;
      si_relay_set(RELAY_PUMP_1, HIGH);
      si_relay_set(RELAY_PUMP_2, HIGH);
      // my_p_const = read_from_eeprom(EEPROM_LOCATION_P_CONST);
      // my_i_const = read_from_eeprom(EEPROM_LOCATION_I_CONST);
      // my_d_const = read_from_eeprom(EEPROM_LOCATION_D_CONST);
    }
  }
}

// functions ----------------------------

uint16_t tunnel_setpoint_get(void) { return my_tunnel_setpoint; }
uint32_t tunnel_speed_get(void) { return my_tunnel_speed; }

bool sw_limit_max_get(void) { return my_sw_limit_max; }
bool sw_limit_min_get(void) { return my_sw_limit_min; }
bool sw_run_get(void) { return my_sw_run; }

/// @brief sets the tunnel setpoint
/// @param new_setpoint new setpoint vale
void tunnel_setpoint_set(uint16_t new_setpoint) {
  my_tunnel_setpoint = new_setpoint;
  display_notification_send(DISPLAY_NOTIFICATION_TUNNEL_SETPOINT);
}

void data_record_set(bool state, uint32_t test_start) {
  my_data_record = state;
  my_test_start = test_start;
}

int16_t pid_values_get(int value) {
  switch (value) {
  case (0):
    return my_p_const;
    break;

  case (1):
    return my_i_const;
    break;

  case (2):
    return my_d_const;
    break;

  default:
    return 0;
    break;
  }
}

void set_correction_factor(uint16_t value) {
  write_to_eeprom(value, EEPROM_LOCATION_FLOW_CORRECTION);
  my_flow_correction = read_from_eeprom(EEPROM_LOCATION_FLOW_CORRECTION);
  LOG_INF("Flow Correction : %.4u", my_flow_correction);
}

/// @brief calculates the current flow rate of the tunnel
/// @return flowspeed of the tonnel
static uint32_t flow_rate_calc() {
  flow_sensor_1_count = si_flow_sensor_rate_get(FLOW_SENSOR_1);
  flow_sensor_2_count = si_flow_sensor_rate_get(FLOW_SENSOR_2);
  int32_t flow_count;

  flow_count = (flow_sensor_1_count +
                flow_sensor_2_count); // amount of liters since last calc
  uint32_t flow_speed = ((flow_count)*my_flow_correction) / 2;

  if (!my_pump_status) {
    flow_count = flow_sensor_1_count - flow_sensor_2_count;
    flow_speed = (abs(flow_count) * my_flow_correction) / 2;
  }

  // override
  return flow_speed;
}

/// @brief allows the shell script to override the actual tunnel speed for
/// testing purposes
/// @param override bool 1 or 0, 1 activates the override
/// @param new_speed new speed for the tunnel
void tunnel_speed_set(bool override, int32_t new_speed) {
  if (override) {
    my_speed_override = true;
    my_override_value = new_speed;
  } else {
    my_speed_override = false;
  }
}

void set_override(bool value) { my_switch_override = value; }

/// @brief allows the shell script to set the pid values
/// @param pid specified value to change. 0 for p, 1 for i, 2 for d
/// @param new_value value to set the pid value to
void control_set_pid(int pid, uint16_t new_value) {
  switch (pid) {
  case 0:
    write_to_eeprom(new_value, EEPROM_LOCATION_P_CONST);
    break;

  case 1:
    write_to_eeprom(new_value, EEPROM_LOCATION_I_CONST);
    break;

  case 2:
    write_to_eeprom(new_value, EEPROM_LOCATION_D_CONST);
    break;

  case 3:
    write_to_eeprom(new_value, EEPROM_LOCATION_P_CONST_SINGLE);
    break;

  case 4:
    write_to_eeprom(new_value, EEPROM_LOCATION_I_CONST_SINGLE);
    break;

  case 5:
    write_to_eeprom(new_value, EEPROM_LOCATION_D_CONST_SINGLE);
    break;
  }

  my_p_const = read_from_eeprom(EEPROM_LOCATION_P_CONST);
  my_i_const = read_from_eeprom(EEPROM_LOCATION_I_CONST);
  my_d_const = read_from_eeprom(EEPROM_LOCATION_D_CONST);

  if (my_pump_status) {

  } else {
    // my_p_const = read_from_eeprom(EEPROM_LOCATION_P_CONST_SINGLE);
    // my_i_const = read_from_eeprom(EEPROM_LOCATION_I_CONST_SINGLE);
    // my_d_const = read_from_eeprom(EEPROM_LOCATION_D_CONST_SINGLE);
  }
  LOG_INF("P Const: %4u", read_from_eeprom(EEPROM_LOCATION_P_CONST));
  LOG_INF("I Const: %4u", read_from_eeprom(EEPROM_LOCATION_I_CONST));
  LOG_INF("D Const: %4u", read_from_eeprom(EEPROM_LOCATION_D_CONST));
  LOG_INF("P Const Single: %4u",
          read_from_eeprom(EEPROM_LOCATION_P_CONST_SINGLE));
  LOG_INF("I Const Single: %4u",
          read_from_eeprom(EEPROM_LOCATION_I_CONST_SINGLE));
  LOG_INF("D Const Single: %4u",
          read_from_eeprom(EEPROM_LOCATION_D_CONST_SINGLE));
}

static void write_to_eeprom(int16_t number, eeprom_location_t address) {
  byte byte1 = number >> 8;
  byte byte2 = number & 0xFF;

  switch (address) {
  case EEPROM_LOCATION_P_CONST:
    EEPROM.write(0, byte1);
    EEPROM.write(1, byte2);
    break;

  case EEPROM_LOCATION_I_CONST:
    EEPROM.write(2, byte1);
    EEPROM.write(3, byte2);
    break;

  case EEPROM_LOCATION_D_CONST:
    EEPROM.write(4, byte1);
    EEPROM.write(5, byte2);
    break;

  case EEPROM_LOCATION_P_CONST_SINGLE:
    EEPROM.write(6, byte1);
    EEPROM.write(7, byte2);
    break;

  case EEPROM_LOCATION_I_CONST_SINGLE:
    EEPROM.write(8, byte1);
    EEPROM.write(9, byte2);
    break;

  case EEPROM_LOCATION_D_CONST_SINGLE:
    EEPROM.write(10, byte1);
    EEPROM.write(11, byte2);
    break;

  case EEPROM_LOCATION_PUMP:
    EEPROM.write(12, byte1);
    break;

  case EEPROM_LOCATION_FLOW_CORRECTION:
    EEPROM.write(13, byte1);
    EEPROM.write(14, byte2);
    break;

  default:
    LOG_ERR("Unhandled EEPROM address");
    break;
  }
}

static int16_t read_from_eeprom(eeprom_location_t address) {
  byte byte1;
  byte byte2;

  switch (address) {
  case EEPROM_LOCATION_P_CONST:
    byte1 = EEPROM.read(0);
    byte2 = EEPROM.read(1);
    break;

  case EEPROM_LOCATION_I_CONST:
    byte1 = EEPROM.read(2);
    byte2 = EEPROM.read(3);
    break;

  case EEPROM_LOCATION_D_CONST:
    byte1 = EEPROM.read(4);
    byte2 = EEPROM.read(5);
    break;

  case EEPROM_LOCATION_P_CONST_SINGLE:
    byte1 = EEPROM.read(6);
    byte2 = EEPROM.read(7);
    break;

  case EEPROM_LOCATION_I_CONST_SINGLE:
    byte1 = EEPROM.read(8);
    byte2 = EEPROM.read(9);
    break;

  case EEPROM_LOCATION_D_CONST_SINGLE:
    byte1 = EEPROM.read(10);
    byte2 = EEPROM.read(11);
    break;

  case EEPROM_LOCATION_PUMP:
    byte1 = EEPROM.read(12);
    break;

  case EEPROM_LOCATION_FLOW_CORRECTION:
    byte1 = EEPROM.read(13);
    byte2 = EEPROM.read(14);
    break;

  default:
    LOG_ERR("Unhandled EEPROM address");
    break;
  }
  return (byte1 << 8) + byte2;
}

#ifdef TEST_BUILD

#include "../test/mock/mock_si_switch.h"
#include <gtest/gtest.h>

void pump_status_set(pump_enum_t status) { my_pump_status = status; }

// Pump Status Check
TEST(CTRL_PumpStatusCheck, both_above_trigger_both) {
  // Setup
  pump_enum_t result;
  my_sw_run = true;
  my_tunnel_setpoint = PUMP_TRIGGER_BOTH + 1;

  // Act
  result = pump_check(PUMPS_BOTH_ACTIVE);

  // Verify
  ASSERT_EQ(result, PUMPS_BOTH_ACTIVE);
}
TEST(CTRL_PumpStatusCheck, both_below_trigger_both) {
  // Setup
  pump_enum_t result;
  my_sw_run = true;
  my_tunnel_setpoint = PUMP_TRIGGER_BOTH - 1;

  // Act
  result = pump_check(PUMPS_BOTH_ACTIVE);

  // Verify
  ASSERT_EQ(result, PUMPS_BOTH_ACTIVE);
}
TEST(CTRL_PumpStatusCheck, both_to_off) {
  // Setup
  pump_enum_t result;
  my_sw_run = false;

  // Act
  result = pump_check(PUMPS_BOTH_ACTIVE);

  // Verify
  ASSERT_EQ(result, PUMPS_NONE_ACTIVE);
}
TEST(CTRL_PumpStatusCheck, both_below_trigger_off) {
  // Setup
  pump_enum_t result;
  my_sw_run = true;
  my_tunnel_setpoint = PUMP_TRIGGER_OFF;

  // Act
  result = pump_check(PUMPS_BOTH_ACTIVE);

  // Verify
  ASSERT_EQ(result, PUMPS_NONE_ACTIVE);
}
TEST(CTRL_PumpStatusCheck, both_to_first) {
  // Setup
  pump_enum_t result;

  my_sw_run = true;
  my_tunnel_setpoint = PUMP_TRIGGER_SINGLE - 1;
  pump_single_switch = true;

  // Act
  result = pump_check(PUMPS_BOTH_ACTIVE);

  // Verify
  ASSERT_EQ(result, PUMPS_FIRST_ACTIVE);
}
TEST(CTRL_PumpStatusCheck, both_to_second) {
  // Setup
  pump_enum_t result;

  my_sw_run = true;
  my_tunnel_setpoint = PUMP_TRIGGER_SINGLE - 1;
  pump_single_switch = false;

  // Act
  result = pump_check(PUMPS_BOTH_ACTIVE);

  // Verify
  ASSERT_EQ(result, PUMPS_SECOND_ACTIVE);
}
TEST(CTRL_PumpStatusCheck, first_to_both) {
  // Setup
  pump_enum_t result;

  pump_status_set(PUMPS_FIRST_ACTIVE);
  my_sw_run = true;
  my_tunnel_setpoint = PUMP_TRIGGER_BOTH + 1;

  // Act
  result = pump_check(PUMPS_FIRST_ACTIVE);

  // Verify
  ASSERT_EQ(result, PUMPS_BOTH_ACTIVE);
}
TEST(CTRL_PumpStatusCheck, first_to_first) {
  // Setup
  pump_enum_t result;
  my_sw_run = true;
  my_tunnel_setpoint = PUMP_TRIGGER_BOTH - 1;

  // Act
  result = pump_check(PUMPS_FIRST_ACTIVE);

  // Verify
  ASSERT_EQ(result, PUMPS_FIRST_ACTIVE);
}
TEST(CTRL_PumpStatusCheck, first_to_off) {
  // Setup
  pump_enum_t result;
  my_sw_run = false;

  // Act
  result = pump_check(PUMPS_FIRST_ACTIVE);

  // Verify
  ASSERT_EQ(result, PUMPS_NONE_ACTIVE);
}
TEST(CTRL_PumpStatusCheck, off_to_both) {
  // Setup
  pump_enum_t result;
  my_sw_run = true;
  my_tunnel_setpoint = PUMP_TRIGGER_BOTH + 1;

  // Act
  result = pump_check(PUMPS_NONE_ACTIVE);

  // Verify
  ASSERT_EQ(result, PUMPS_BOTH_ACTIVE);
}
TEST(CTRL_PumpStatusCheck, off_to_first) {
  // Setup
  pump_enum_t result;

  my_sw_run = true;
  my_tunnel_setpoint = PUMP_TRIGGER_SINGLE - 1;
  pump_single_switch = true;

  // Act
  result = pump_check(PUMPS_NONE_ACTIVE);

  // Verify
  ASSERT_EQ(result, PUMPS_FIRST_ACTIVE);
}
TEST(CTRL_PumpStatusCheck, off_to_off) {
  // Setup
  pump_enum_t result;
  my_sw_run = false;

  // Act
  result = pump_check(PUMPS_NONE_ACTIVE);

  // Verify
  ASSERT_EQ(result, PUMPS_NONE_ACTIVE);
}
TEST(CTRL_PumpStatusCheck, off_to_second) {
  // Setup
  pump_enum_t result;
  my_sw_run = true;
  my_tunnel_setpoint = PUMP_TRIGGER_SINGLE - 1;
  pump_single_switch = false;

  // Act
  result = pump_check(PUMPS_NONE_ACTIVE);

  // Verify
  ASSERT_EQ(result, PUMPS_SECOND_ACTIVE);
}
TEST(CTRL_PumpStatusCheck, second_to_both) {
  // Setup
  pump_enum_t result;

  pump_status_set(PUMPS_SECOND_ACTIVE);
  my_sw_run = true;
  my_tunnel_setpoint = PUMP_TRIGGER_BOTH + 1;

  // Act
  result = pump_check(PUMPS_SECOND_ACTIVE);

  // Verify
  ASSERT_EQ(result, PUMPS_BOTH_ACTIVE);
}
TEST(CTRL_PumpStatusCheck, second_to_off) {
  // Setup
  pump_enum_t result;

  pump_status_set(PUMPS_SECOND_ACTIVE);
  my_sw_run = false;

  // Act
  result = pump_check(PUMPS_SECOND_ACTIVE);

  // Verify
  ASSERT_EQ(result, PUMPS_NONE_ACTIVE);
}

// SW_MIN tests
TEST(SW_TASK, min_low_high) {
  // Setup
  bool result;
  my_sw_limit_min = false;
  si_switch_mock_set_value(SW_LIMIT_MIN, true);
  // Act
  switch_task();
  result = my_sw_limit_min;

  // Verify
  ASSERT_EQ(result, true);
}
TEST(SW_TASK, min_low_low) {
  // Setup
  bool result;
  my_sw_limit_min = false;
  si_switch_mock_set_value(SW_LIMIT_MIN, false);
  // Act
  switch_task();
  result = my_sw_limit_min;

  // Verify
  ASSERT_EQ(result, false);
}
TEST(SW_TASK, min_high_high) {
  // Setup
  bool result;
  my_sw_limit_min = true;
  si_switch_mock_set_value(SW_LIMIT_MIN, true);
  // Act
  switch_task();
  result = my_sw_limit_min;

  // Verify
  ASSERT_EQ(result, true);
}
TEST(SW_TASK, min_high_low) {
  // Setup
  bool result;
  my_sw_limit_min = true;
  si_switch_mock_set_value(SW_LIMIT_MIN, false);
  // Act
  switch_task();
  result = my_sw_limit_min;

  // Verify
  ASSERT_EQ(result, false);
}

// SW_MAX tests
TEST(SW_TASK, max_low_high) {
  // Setup
  bool result;
  my_sw_limit_max = false;
  si_switch_mock_set_value(SW_LIMIT_MAX, true);
  // Act
  switch_task();
  result = my_sw_limit_max;

  // Verify
  ASSERT_EQ(result, true);
}
TEST(SW_TASK, max_low_low) {
  // Setup
  bool result;
  my_sw_limit_max = false;
  si_switch_mock_set_value(SW_LIMIT_MAX, false);
  // Act
  switch_task();
  result = my_sw_limit_max;

  // Verify
  ASSERT_EQ(result, false);
}
TEST(SW_TASK, max_high_high) {
  // Setup
  bool result;
  my_sw_limit_max = true;
  si_switch_mock_set_value(SW_LIMIT_MAX, true);
  // Act
  switch_task();
  result = my_sw_limit_max;

  // Verify
  ASSERT_EQ(result, true);
}
TEST(SW_TASK, max_high_low) {
  // Setup
  bool result;
  my_sw_limit_max = true;
  si_switch_mock_set_value(SW_LIMIT_MAX, false);
  // Act
  switch_task();
  result = my_sw_limit_max;

  // Verify
  ASSERT_EQ(result, false);
}

// SW_RUN tests
TEST(SW_TASK, run_low_high) {
  // Setup
  bool result;
  my_sw_run = false;
  si_switch_mock_set_value(SW_RUN, true);
  // Act
  switch_task();
  result = my_sw_run;

  // Verify
  ASSERT_EQ(result, true);
}
TEST(SW_TASK, run_low_low) {
  // Setup
  bool result;
  my_sw_limit_min = false;
  si_switch_mock_set_value(SW_RUN, false);
  // Act
  switch_task();
  result = my_sw_run;

  // Verify
  ASSERT_EQ(result, false);
}
TEST(SW_TASK, run_high_high) {
  // Setup
  bool result;
  my_sw_limit_min = true;
  si_switch_mock_set_value(SW_RUN, true);
  // Act
  switch_task();
  result = my_sw_run;

  // Verify
  ASSERT_EQ(result, true);
}
TEST(SW_TASK, run_high_low) {
  // Setup
  bool result;
  my_sw_run = true;
  si_switch_mock_set_value(SW_RUN, false);
  // Act
  switch_task();
  result = my_sw_run;

  // Verify
  ASSERT_EQ(result, false);
}
#endif // TEST_BUILD
