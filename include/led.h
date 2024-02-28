/**
 * @file led.h
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief led header file
 * @date 2024-02-28
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

typedef enum {
  OFF,
  RED,
  GREEN,
  BLUE,
  YELLOW,
  MAGENTA,
  CYAN,
  WHITE,
} led_color_t;

void led_color_1_set(led_color_t new_color_1);
void led_color_2_set(led_color_t new_color_2);
void led_task(void);
