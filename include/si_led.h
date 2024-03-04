/**
 * @file si_led.h
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief system interface: led
 * @date 2024-02-28
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#ifndef SI_LED_H
#define SI_LED_H

typedef enum {
  OFF,
  RED,
  GREEN,
  BLUE,
  YELLOW,
  MAGENTA,
  CYAN,
  WHITE,
} si_led_color_t;

void si_led_color_1_set(si_led_color_t color);

void si_led_color_2_set(si_led_color_t color);

void si_led_task(void);

#endif // SI_LED_H
