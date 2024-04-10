/**
 * @file menu.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief functions to control the menu
 * @date 2024-01-30
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 */

// includes
#include <stdio.h>

#include "control.h"
#include "log.h"
#include "menu.h"
#include "si_encoder.h"
#include "si_lcd.h"
#include "si_stepper.h"

// variables
static bool update_display[4] = {false, false, false, false};
static display_frame_t frame = DISPLAY_FRAME_INIT;
static char buffer[4][21] = {
    "Target :        mm/s",
    "Current:   ---- mm/s",
    "Max:     | Min:     ",
    "                    ",
};

static uint16_t my_tunnel_setpoint = 0;
static uint32_t my_tunnel_speed = 0;
static bool my_sw_limit_min = false;
static bool my_sw_limit_max = false;

// functions
void frame_set(display_frame_t new_frame) {
  frame = new_frame;
  switch (frame) {
  case DISPLAY_FRAME_RUN:
    si_lcd_write(LCD_LINE_1, buffer[0]);
    si_lcd_write(LCD_LINE_2, buffer[1]);
    si_lcd_write(LCD_LINE_3, buffer[2]);
    si_lcd_write(LCD_LINE_4, buffer[3]);
    break;

  case DISPLAY_FRAME_INIT:
  case DISPLAY_FRAME_HOME:
    LOG_INF("INIT FRAME")
    si_lcd_write(LCD_LINE_1, "--------------------");
    si_lcd_write(LCD_LINE_2, "      WOLFPACK      ");
    si_lcd_write(LCD_LINE_3, "      WATERWAY      ");
    si_lcd_write(LCD_LINE_4, "--------------------");
    break;

  case DISPLAY_FRAME_ERROR:
    si_lcd_write(LCD_LINE_1, "--------------------");
    si_lcd_write(LCD_LINE_2, "       ERROR        ");
    si_lcd_write(LCD_LINE_3, "   RESTART SYSTEM   ");
    si_lcd_write(LCD_LINE_4, "--------------------");
    break;

  case DISPLAY_FRAME_IDLE:
    si_lcd_write(LCD_LINE_1, "--------------------");
    si_lcd_write(LCD_LINE_2, "        IDLE        ");
    si_lcd_write(LCD_LINE_3, "                    ");
    si_lcd_write(LCD_LINE_4, "--------------------");
    break;

  case DISPLAY_FRAME_PRIME_FIRST:
    si_lcd_write(LCD_LINE_1, "--------------------");
    si_lcd_write(LCD_LINE_2, "   Priming Pump 1   ");
    si_lcd_write(LCD_LINE_3, "                    ");
    si_lcd_write(LCD_LINE_4, "--------------------");
    break;

  case DISPLAY_FRAME_PRIME_SECOND:
    si_lcd_write(LCD_LINE_1, "--------------------");
    si_lcd_write(LCD_LINE_2, "   Priming Pump 2   ");
    si_lcd_write(LCD_LINE_3, "                    ");
    si_lcd_write(LCD_LINE_4, "--------------------");
    break;

  case DISPLAY_FRAME_SHUTDOWN_FIRST:
    si_lcd_write(LCD_LINE_1, "--------------------");
    si_lcd_write(LCD_LINE_2, "   Shutdown Pump 2  ");
    si_lcd_write(LCD_LINE_3, "                    ");
    si_lcd_write(LCD_LINE_4, "--------------------");
    break;

  case DISPLAY_FRAME_SHUTDOWN_SECOND:
    si_lcd_write(LCD_LINE_1, "--------------------");
    si_lcd_write(LCD_LINE_2, "   Shutdown Pump 1  ");
    si_lcd_write(LCD_LINE_3, "                    ");
    si_lcd_write(LCD_LINE_4, "--------------------");
    break;

  default:
    LOG_ERR("Unhandled Frame");
    break;
  }
}

/// @brief task which updates the menu. Only updates in new data was sent.
void frame_task() {

  if (update_display[0]) {
    my_tunnel_setpoint = tunnel_setpoint_get();
    snprintf(buffer[0], 21, "Target :   %04d mm/s", my_tunnel_setpoint);
    si_lcd_write(LCD_LINE_1, buffer[0]);
  }

  if (update_display[1]) {
    my_tunnel_speed = tunnel_speed_get();
    snprintf(buffer[1], 21, "Current:   %04d mm/s", my_tunnel_speed);
    si_lcd_write(LCD_LINE_2, buffer[1]);
  }

  if (update_display[2]) {
    if (my_sw_limit_min) {
      if (my_sw_limit_max) {
        snprintf(buffer[2], 21, "Max:     | Min:     ");
      } else {
        snprintf(buffer[2], 21, "Max:  X  | Min:     ");
      }
    } else {
      if (my_sw_limit_max) {
        snprintf(buffer[2], 21, "Max:     | Min:  X  ");
      } else {
        snprintf(buffer[2], 21, "Max:  X  | Min:  X  ");
      }
    }
    si_lcd_write(LCD_LINE_3, buffer[2]);
  }

  // if (update_display[3]) {
  //   if (my_relay_pump_2) {
  //     if (my_sw_limit_min) {
  //       snprintf(buffer[3], 21, "Pump2: ON  | Min:   ");
  //     } else {
  //       snprintf(buffer[3], 21, "Pump2: ON  | Min: X ");
  //     }
  //   } else {
  //     if (my_sw_limit_min) {
  //       snprintf(buffer[3], 21, "Pump2: OFF | Min:   ");
  //     } else {
  //       snprintf(buffer[3], 21, "Pump2: OFF | Min: X ");
  //     }
  //   }
  si_lcd_write(LCD_LINE_4, buffer[3]);
}

/// @brief allows other files to send update notifications to the menu
/// @param notification specifies which notification was sent
void display_notification_send(display_notification_t notification) {
  switch (notification) {
  case DISPLAY_NOTIFICATION_TUNNEL_SETPOINT:
    my_tunnel_setpoint = tunnel_setpoint_get();
    update_display[0] = true;
    break;

  case DISPLAY_NOTIFICATION_TUNNEL_SPEED:
    my_tunnel_speed = tunnel_speed_get();
    update_display[1] = true;
    break;

  case DISPLAY_NOTIFICATION_SW_LIMIT_MIN:
    my_sw_limit_min = sw_limit_min_get();
    update_display[3] = true;
    break;

  case DISPLAY_NOTIFICATION_SW_LIMIT_MAX:
    my_sw_limit_max = sw_limit_max_get();
    update_display[2] = true;
    break;

  default:

    break;
  }
}

#ifdef TEST_BUILD

#include "../test/mock/mock_si_encoder.h"
#include "../test/mock/mock_si_lcd.h"
#include <gtest/gtest.h>
#include <string.h>

TEST(MENU, display_line_get_check) {
  // Setup
  si_lcd_line_t line_to_check = LCD_LINE_1;
  const char *expected_str = "abcdefghaijklmnopqrs";
  int result;
  // Act
  result = strcmp(si_lcd_mock_display_line_get(line_to_check), expected_str);
  // Verify
  ASSERT_EQ(result, 0);
}

// Tests for the init menu
TEST(MENU, Frame_Set_init1) {
  // Setup
  int result;
  frame_set(DISPLAY_FRAME_INIT);
  // Act
  result = strncmp(si_lcd_mock_display_line_get(LCD_LINE_1),
                   "--------------------", 21);
  // Verify
  ASSERT_EQ(result, 0);
}
TEST(MENU, Frame_Set_init2) {
  // Setup
  int result;
  frame_set(DISPLAY_FRAME_INIT);
  // Act
  result = strncmp(si_lcd_mock_display_line_get(LCD_LINE_2),
                   "      WOLFPACK      ", 21);
  // Verify
  ASSERT_EQ(result, 0);
}
TEST(MENU, Frame_Set_init3) {
  // Setup
  int result;
  frame_set(DISPLAY_FRAME_INIT);
  // Act
  result = strncmp(si_lcd_mock_display_line_get(LCD_LINE_3),
                   "      WATERWAY      ", 21);
  // Verify
  ASSERT_EQ(result, 0);
}
TEST(MENU, Frame_Set_init4) {
  // Setup
  int result;
  frame_set(DISPLAY_FRAME_INIT);
  // Act
  result = strncmp(si_lcd_mock_display_line_get(LCD_LINE_4),
                   "--------------------", 21);
  // Verify
  ASSERT_EQ(result, 0);
}

// Tests for the home menu
TEST(MENU, Frame_Set_home1) {
  // Setup
  int result;
  frame_set(DISPLAY_FRAME_HOME);
  // Act
  result = strncmp(si_lcd_mock_display_line_get(LCD_LINE_1),
                   "--------------------", 21);
  // Verify
  ASSERT_EQ(result, 0);
}
TEST(MENU, Frame_Set_home2) {
  // Setup
  int result;
  frame_set(DISPLAY_FRAME_HOME);
  // Act
  result = strncmp(si_lcd_mock_display_line_get(LCD_LINE_2),
                   "      WOLFPACK      ", 21);
  // Verify
  ASSERT_EQ(result, 0);
}
TEST(MENU, Frame_Set_home3) {
  // Setup
  int result;
  frame_set(DISPLAY_FRAME_HOME);
  // Act
  result = strncmp(si_lcd_mock_display_line_get(LCD_LINE_3),
                   "      WATERWAY      ", 21);
  // Verify
  ASSERT_EQ(result, 0);
}
TEST(MENU, Frame_Set_home4) {
  // Setup
  int result;
  frame_set(DISPLAY_FRAME_HOME);
  // Act
  result = strncmp(si_lcd_mock_display_line_get(LCD_LINE_4),
                   "--------------------", 21);
  // Verify
  ASSERT_EQ(result, 0);
}

// Tests for the error menu
TEST(MENU, Frame_Set_error1) {
  // Setup
  int result;
  frame_set(DISPLAY_FRAME_ERROR);
  // Act
  result = strncmp(si_lcd_mock_display_line_get(LCD_LINE_1),
                   "--------------------", 21);
  // Verify
  ASSERT_EQ(result, 0);
}
TEST(MENU, Frame_Set_error2) {
  // Setup
  int result;
  frame_set(DISPLAY_FRAME_ERROR);
  // Act
  result = strncmp(si_lcd_mock_display_line_get(LCD_LINE_2),
                   "       ERROR        ", 21);
  // Verify
  ASSERT_EQ(result, 0);
}
TEST(MENU, Frame_Set_error3) {
  // Setup
  int result;
  frame_set(DISPLAY_FRAME_ERROR);
  // Act
  result = strncmp(si_lcd_mock_display_line_get(LCD_LINE_3),
                   "   RESTART SYSTEM   ", 21);
  // Verify
  ASSERT_EQ(result, 0);
}
TEST(MENU, Frame_Set_error4) {
  // Setup
  int result;
  frame_set(DISPLAY_FRAME_ERROR);
  // Act
  result = strncmp(si_lcd_mock_display_line_get(LCD_LINE_4),
                   "--------------------", 21);
  // Verify
  ASSERT_EQ(result, 0);
}

#endif
