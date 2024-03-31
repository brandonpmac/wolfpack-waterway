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
  DISPLAY_FRAME_HOME,
  DISPLAY_FRAME_IDLE,
  DISPLAY_FRAME_RUN,
  DISPLAY_FRAME_ERROR,

  DISPLAY_FRAME_CNT,
} display_frame_t;

typedef enum {
  DISPLAY_NOTIFICATION_TUNNEL_SETPOINT,
  DISPLAY_NOTIFICATION_TUNNEL_SPEED,
  DISPLAY_NOTIFICATION_SW_LIMIT_MIN,
  DISPLAY_NOTIFICATION_SW_LIMIT_MAX,
  DISPLAY_NOTIFICATION_RELAY_PUMP_1,
  DISPLAY_NOTIFICATION_RELAY_PUMP_2,

  DISPLAY_NOTIFICATION_CNT,
  DISPLAY_NOTIFICATION_ERR = DISPLAY_NOTIFICATION_CNT,
} display_notification_t;

void frame_set(display_frame_t new_frame);

void display_notification_send(display_notification_t notification);

void encoder_task(void);
void frame_task(void);

#endif // MENU_H
