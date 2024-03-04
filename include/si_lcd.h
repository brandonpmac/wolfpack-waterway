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

#include <LiquidCrystal_I2C.h>
#include <stdint.h>

extern LiquidCrystal_I2C lcd;

typedef enum {
  LCD_LINE_1,
  LCD_LINE_2,
  LCD_LINE_3,
  LCD_LINE_4,

  LCD_LINE_CNT,
} si_lcd_line_t;

void si_lcd_update_set(si_lcd_line_t lcd_line);

// Plan to remove when i change logic to set values
bool si_lcd_update_get(si_lcd_line_t lcd_line);

void si_lcd_line_set(si_lcd_line_t lcd_line, char* new_line);

void si_lcd_write(void);

#endif // SI_LCD_H