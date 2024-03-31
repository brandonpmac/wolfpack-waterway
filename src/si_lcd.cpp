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

// Plan to remove when i change logic to set values
void si_lcd_write(si_lcd_line_t row, const char *new_line) {
  lcd.setCursor(0, row);
  lcd.print(new_line);
}