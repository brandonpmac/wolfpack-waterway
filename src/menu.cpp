/**
 * @file menu.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief functions to control the menu
 * @date 2024-01-30
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 */

#include <Arduino.h>
#include <stdio.h>

#include "control.h"
#include "encoder.h"
#include "lcd.h"
#include "menu.h"

// variables
static int encoder_increment = 5;

display_frame_t frame = DISPLAY_FRAME_INIT;
static char buffer[4][21] = {
    "Target :        mm/s",
    "Current:   ---- mm/s",
    "Pump1: OFF | Max:   ",
    "Pump2: OFF | Min:   ",
};

// function declratations
static void populate_display_frame(display_frame_t frame);

void encoder_task(void) {
  encoder_event_t event = encoder_event_get();

  switch (event) {
  case ENCODER_EVENT_NONE:
    break;

  case ENCODER_EVENT_LEFT:
    if (control_setpoint_get() == 0) {
      break;
    }
    control_setpoint_set(control_setpoint_get() - encoder_increment);
    break;

  case ENCODER_EVENT_RIGHT:
    control_setpoint_set(control_setpoint_get() + encoder_increment);
    break;

  case ENCODER_EVENT_BTN:
    break;

  default:
    Serial.println("unhandled encoder event");
    break;
  }
}

// Populates the buffer with the correct data
static void populate_display_frame(display_frame_t frame) {
  switch (frame) {
  case DISPLAY_FRAME_INIT:
    strcpy(&buffer[0][0], "--------------------");
    strcpy(&buffer[1][0], "      WOLFPACK      ");
    strcpy(&buffer[2][0], "      WATERWAY      ");
    strcpy(&buffer[3][0], "--------------------");
    break;

  case DISPALY_FRAME_HOME:
    strcpy(&buffer[0][0], "--------------------");
    strcpy(&buffer[1][0], "      WOLFPACK      ");
    strcpy(&buffer[2][0], "      WATERWAY      ");
    strcpy(&buffer[3][0], "--------------------");
    break;

  case DISPLAY_FRAME_IDLE:
    int target = control_setpoint_get();
    int current = control_process_variable_get();
    pump_enum_t pump_current_status = pump_status_get();
    static char pump_1_status[4] = {"   "};
    static char pump_2_status[4] = {"   "};
    static char limit_max_status = ' ';
    static char limit_min_status = ' ';

    switch (pump_current_status) {
    case PUMPS_BOTH_ACTIVE:
      strcpy(&pump_1_status[0], "ON ");
      strcpy(&pump_2_status[0], "ON ");
      break;

    case PUMPS_BOTH_OFF:
      strcpy(&pump_1_status[0], "OFF");
      strcpy(&pump_2_status[0], "OFF");
      break;

    case PUMPS_FIRST_ACTIVE:
      strcpy(&pump_1_status[0], "ON ");
      strcpy(&pump_2_status[0], "OFF");
      break;

    case PUMPS_SECOND_ACTIVE:
      strcpy(&pump_1_status[0], "OFF");
      strcpy(&pump_2_status[0], "ON ");
      break;
    }
    // check max limit switch
    if (limit_max_get()) {
      limit_max_status = 'X';
    }
    // check min limit switch
    if (limit_min_get()) {
      limit_min_status = 'X';
    }

    sprintf(&buffer[0][0], "Target :   %04d mm/s", target);
    sprintf(&buffer[1][0], "Current:   %04d mm/s", current);
    sprintf(&buffer[2][0], "Pump1: %s | Max: %c ", pump_1_status,
            limit_max_status);
    sprintf(&buffer[3][0], "Pump2: %s | Min: %c ", pump_2_status,
            limit_min_status);
    break;

  case DISPLAY_FRAME_RUN:
    strcpy(&buffer[0][0], "--------------------");
    strcpy(&buffer[1][0], "      WOLFPACK      ");
    strcpy(&buffer[2][0], "      WATERWAY      ");
    strcpy(&buffer[3][0], "--------------------");
    break;

  case DISPLAY_FRAME_ERROR:
    strcpy(&buffer[0][0], "--------------------");
    strcpy(&buffer[1][0], "      WOLFPACK      ");
    strcpy(&buffer[2][0], "      WATERWAY      ");
    strcpy(&buffer[3][0], "--------------------");
    break;

  default:
    Serial.println("Error: Unhandled display state");
    break;
  }
}

// Updates the frame to print to the lcd screen
void frame_task(void) {
  populate_display_frame(frame);
  lcd.setCursor(0, 0);
  lcd.print(buffer[0]);
  lcd.setCursor(0, 1);
  lcd.print(buffer[1]);
  lcd.setCursor(0, 2);
  lcd.print(buffer[2]);
  lcd.setCursor(0, 3);
  lcd.print(buffer[3]);
}
