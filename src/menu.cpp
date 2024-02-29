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
#include "stepper.h"

// variables
static int encoder_increment = 5;

static int my_target = 0;
static int my_current = 0;
static bool my_max_limit = false;
static bool my_min_limit = false;
static pump_enum_t my_pump_status = PUMPS_BOTH_OFF;
static bool update_display[4] = {false, false, false, false};

String pump_1_status = "OFF";
String pump_2_status = "OFF";
char limit_max_status = ' ';
char limit_min_status = ' ';

display_frame_t frame = DISPLAY_FRAME_INIT;
static char buffer[4][21] = {
    "Target :        mm/s",
    "Current:   ---- mm/s",
    "Pump1: OFF | Max:   ",
    "Pump2: OFF | Min:   ",
};

// function declratations
static void populate_display_frame(display_frame_t frame);

void frame_set(display_frame_t new_frame) {
  frame = new_frame;
  Serial.println(frame);
  update_display[0] = true;
  update_display[1] = true;
  update_display[2] = true;
  update_display[3] = true;
}

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
    if (control_setpoint_get() == 1000) {
      break;
    }
    control_setpoint_set(control_setpoint_get() + encoder_increment);
    break;

  case ENCODER_EVENT_BTN:
    Serial.println("BTN PRESS");
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

  case DISPLAY_FRAME_RUN:
    strcpy(&buffer[0][0], "--------------------");
    strcpy(&buffer[1][0], "      WOLFPACK      ");
    strcpy(&buffer[2][0], "      WATERWAY      ");
    strcpy(&buffer[3][0], "--------------------");
    break;

  case DISPLAY_FRAME_ERROR:
    strcpy(&buffer[0][0], "--------------------");
    strcpy(&buffer[1][0], "       ERROR        ");
    strcpy(&buffer[2][0], "   RESTART SYSTEM   ");
    strcpy(&buffer[3][0], "--------------------");
    break;

  default:
    Serial.println("Error: Unhandled display state");
    break;

  case DISPLAY_FRAME_IDLE:
    int new_target = control_setpoint_get();
    int new_current = control_process_variable_get();
    bool new_max_limit = limit_max_get();
    bool new_min_limit = limit_min_get();
    pump_enum_t new_pump_status = pump_status_get();

    if (new_target != my_target) {
      update_display[0] = true;
      my_target = new_target;
    }

    if (new_current != my_current) {
      update_display[1] = true;
      my_current = new_current;
    }

    if (new_max_limit != my_max_limit) {
      update_display[2] = true;
      my_max_limit = new_max_limit;
      if (my_max_limit) {
        limit_max_status = 'X';
      } else {
        limit_max_status = ' ';
      }
    }

    if (new_min_limit != my_min_limit) {
      update_display[3] = true;
      my_min_limit = new_min_limit;
      if (my_min_limit) {
        limit_min_status = 'X';
      } else {
        limit_min_status = ' ';
      }
    }

    if (new_pump_status != my_pump_status) {
      update_display[2] = true;
      update_display[3] = true;
      my_pump_status = new_pump_status;

      switch (my_pump_status) {
      case PUMPS_BOTH_ACTIVE:
        pump_1_status = "ON ";
        pump_2_status = "ON ";
        break;

      case PUMPS_BOTH_OFF:
        pump_1_status = "OFF";
        pump_2_status = "OFF";
        break;

      case PUMPS_FIRST_ACTIVE:
        pump_1_status = "ON ";
        pump_2_status = "OFF";
        break;

      case PUMPS_SECOND_ACTIVE:
        pump_1_status = "OFF";
        pump_2_status = "ON ";
        break;
      }
    }

    if (update_display[0]) {
      sprintf(&buffer[0][0], "Target :   %04d mm/s", my_target);
    }

    if (update_display[1]) {
      sprintf(&buffer[1][0], "Current:   %04d mm/s", my_current);
    }

    if (update_display[2]) {
      sprintf(&buffer[2][0], "Pump1: %s | Max: %c ", pump_1_status.c_str(),
              limit_max_status);
    }

    if (update_display[3]) {
      sprintf(&buffer[3][0], "Pump2: %s | Min: %c ", pump_2_status.c_str(),
              limit_min_status);
    }
    break;
  }
}

// Updates the frame to print to the lcd screen
void frame_task(void) {
  populate_display_frame(frame);

  for (int i = 0; i <= 3; i++) {
    if (update_display[i]) {
      update_display[i] = false;
      lcd.setCursor(0, i);
      lcd.print(buffer[i]);
      // Serial.print("Display Update: ");
      // Serial.println(i);
    }
  }
}
