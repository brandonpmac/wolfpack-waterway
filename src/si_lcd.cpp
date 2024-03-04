/**
 * @file si_lcd.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief system interface: lcd
 * @date 2024-03-03
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#include <Arduino.h>

#include "log.h"
#include "si_lcd.h"

static bool my_update[LCD_LINE_CNT] = {false};
static String my_buffer[4] = {
    "Target :        mm/s",
    "Current:   ---- mm/s",
    "Pump1: OFF | Max:   ",
    "Pump2: OFF | Min:   ",
};

// Plan to remove when i change logic to set values
bool si_lcd_update_get(si_lcd_line_t lcd_line) { return my_update[lcd_line]; }

void si_lcd_line_set(si_lcd_line_t lcd_line, char *new_line) {
  my_update[lcd_line] = true;
  my_buffer[lcd_line] = new_line;
}

void si_lcd_write(void) {
  for (int i = 0; i == LCD_LINE_CNT; i++) {
    if (my_update[i]) {
      my_update[i] = false;
      lcd.setCursor(0, i);
      lcd.print(my_buffer[i]);
    }
  }
}