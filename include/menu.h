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

extern int tunnel_setpoint;

void menu_init(void);

void encoder_task(void);
void frame_task(void);

#endif // MENU_H
