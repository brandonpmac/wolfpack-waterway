/**
 * @file si_lcd.h
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief system interface: lcd header file
 * @date 2024-03-03
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#ifndef SI_LCD_H
#define SI_LCD_H

#ifndef TEST_BUILD
#include <LiquidCrystal_I2C.h>
extern LiquidCrystal_I2C lcd;
#endif // TEST_BUILD

#include <stdint.h>

typedef enum {
  LCD_LINE_1,
  LCD_LINE_2,
  LCD_LINE_3,
  LCD_LINE_4,

  LCD_LINE_CNT,
} si_lcd_line_t;

void si_lcd_write(si_lcd_line_t row, const char *new_line);

#endif // SI_LCD_H