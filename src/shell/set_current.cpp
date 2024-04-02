/**
 * @file set_current.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief 
 * @date 2024-04-02
 * 
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 * 
 */

#include <Arduino.h>
#include <SimpleSerialShell.h>

#include "control.h"
#include "menu.h"

int set_speed_hndlr(int argc, char **argv) {
  if (atoi(argv[1])==1) {
    tunnel_speed_set( true ,atoi(argv[2]));
  } else {
    tunnel_speed_set( false ,atoi(argv[2]));
  }
  display_notification_send(DISPLAY_NOTIFICATION_TUNNEL_SPEED);
  Serial.println(tunnel_speed_get());
  return EXIT_SUCCESS;
}