/**
 * @file mock_si_lcd.h
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief
 * @date 2024-03-05
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#ifndef MOCK_SI_LCD_H
#define MOCK_SI_LCD_H

#include "si_lcd.h"

void si_lcd_write(si_lcd_line_t row, const char *new_line);

const char *si_lcd_mock_display_line_get(si_lcd_line_t line);

#endif // SI_LCD_H