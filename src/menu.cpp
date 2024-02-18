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

int tunnel_setpoint = 0;

bool update_display = true; // for the memes
bool previous_pump_1 = false;
bool previous_pump_2 = false;
bool previous_limit_max = false;
bool previous_limit_min = false;

static char buffer[4][21] = {
    "Target :        mm/s",
    "Current:   ---- mm/s",
    "Pump1: OFF | Max:   ",
    "Pump2: OFF | Min:   ",
};

void menu_init(void) {
  // Startup Message
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("--------------------");
  lcd.setCursor(6, 1);
  lcd.print("Wolfpack");
  lcd.setCursor(6, 2);
  lcd.print("Waterway");
  lcd.setCursor(0, 3);
  lcd.print("--------------------");
}

void encoder_task(void) {
  encoder_event_t event = encoder_event_get();

  switch (event) {
  case ENCODER_EVENT_NONE:
    break;

  case ENCODER_EVENT_LEFT:
    if (tunnel_setpoint == 0) {
      break;
    }
    tunnel_setpoint -= 5;
    sprintf(&buffer[0][11], "%04d mm/s", tunnel_setpoint);
    update_display = true;
    break;

  case ENCODER_EVENT_RIGHT:
    tunnel_setpoint += 5;
    sprintf(&buffer[0][11], "%04d mm/s", tunnel_setpoint);
    update_display = true;
    break;

  case ENCODER_EVENT_BTN:
    break;

  default:
    Serial.println("unhandled encoder event");
    break;
  }

  if (pump_1 != previous_pump_1) {
    previous_pump_1 = pump_1;
    if (pump_1) {
      strcpy(&buffer[2][0], "Pump1: ON  | ");
      update_display = true;
    } else {
      strcpy(&buffer[2][0], "Pump1: OFF | ");
      update_display = true;
    }
  }

  if (pump_2 != previous_pump_2) {
    previous_pump_2 = pump_2;
    if (pump_2) {
      strcpy(&buffer[3][0], "Pump2: ON  | ");
      update_display = true;
    } else {
      strcpy(&buffer[3][0], "Pump2: OFF | ");
      update_display = true;
    }
  }

  if (limit_max != previous_limit_max) {
    previous_limit_max = limit_max;
    if (limit_max) {
      strcpy(&buffer[2][13], "Max: X ");
      update_display = true;
    } else {
      strcpy(&buffer[2][13], "Max:   ");
      Serial.println("It happened!!!!");
      update_display = true;
    }
  }

  if (limit_min != previous_limit_min) {
    previous_limit_min = limit_min;
    if (limit_min) {
      strcpy(&buffer[3][13], "Min: X ");
      update_display = true;
    } else {
      strcpy(&buffer[3][13], "Min:   ");
      update_display = true;
    }
  }
}

void frame_task(void) {
  if (update_display) {
    update_display = false;
    lcd.setCursor(0, 0);
    lcd.print(buffer[0]);
    lcd.setCursor(0, 1);
    lcd.print(buffer[1]);
    lcd.setCursor(0, 2);
    lcd.print(buffer[2]);
    lcd.setCursor(0, 3);
    lcd.print(buffer[3]);
  }
}
