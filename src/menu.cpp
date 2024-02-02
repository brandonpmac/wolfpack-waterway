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

#include "encoder.h"
#include "lcd.h"

static int my_target = 0;

static char *buffer[] = {
    "Target :        mm/s",
    "Current:   7583 mm/s",
    "Pump1: OFF  245  L/s",
    "Pump2: OFF  437  L/s",
};

void menu_init(void) {

  // Startup Message
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("--------------------");
  lcd.setCursor(6,1);
  lcd.print("Wolfpack");
  lcd.setCursor(6
  ,2);
  lcd.print("Waterway");
  lcd.setCursor(0,3);
  lcd.print("--------------------");
}

void menu_task(void) {
  encoder_event_t event = encoder_event_get();
  bool update = true;

  switch (event) {
  case ENCODER_EVENT_NONE:
    update = false;
    break;

  case ENCODER_EVENT_LEFT:
    my_target -= 5;
    sprintf(&buffer[0][11], "%04d mm/s", my_target);
    break;

  case ENCODER_EVENT_RIGHT:
    my_target += 5;
    sprintf(&buffer[0][11], "%04d mm/s", my_target);
    break;

  case ENCODER_EVENT_BTN:
    break;

  default:
    Serial.println("unhandled encoder event");
    break;
  }

// This is what would become frame task
  if (update) {
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