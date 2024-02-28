/**
 * @file menu.h
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief menu header file
 * @date 2024-01-30
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 */

#ifndef MENU_H
#define MENU_H

typedef enum {
  DISPLAY_FRAME_INIT,
  DISPALY_FRAME_HOME,
  DISPLAY_FRAME_IDLE,
  DISPLAY_FRAME_RUN,
  DISPLAY_FRAME_ERROR,
} display_frame_t;

void frame_set(display_frame_t);

void encoder_task(void);
void frame_task(void);

#endif // MENU_H
