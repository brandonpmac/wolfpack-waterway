/**
 * @file mock_si_lcd.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief
 * @date 2024-03-05
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#include "mock_si_lcd.h"
#include <stdio.h>
#include <string.h>

#define DISPLAY_SIZE (21)

static char my_display_frame[4][21] = {
    "abcdefghaijklmnopqrs",
    "____________________",
    "____________________",
    "____________________",
};

void si_lcd_write(si_lcd_line_t row, const char *new_line) {
  strcpy(&my_display_frame[row][0], new_line);
}

const char *si_lcd_mock_display_line_get(si_lcd_line_t line) {
  return my_display_frame[line];
}