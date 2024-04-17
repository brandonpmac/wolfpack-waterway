/**
 * @file reset.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief reset to default setting
 * @date 2024-04-17
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#include <Arduino.h>
#include <SimpleSerialShell.h>

#include "control.h"
#include "log.h"

int reset_hndlr(int argc, char **argv) {
  write_to_eeprom(20, EEPROM_LOCATION_P_CONST);
  write_to_eeprom(0, EEPROM_LOCATION_I_CONST);
  write_to_eeprom(15, EEPROM_LOCATION_D_CONST);
  write_to_eeprom(0, EEPROM_LOCATION_PUMP);
  write_to_eeprom(1270, EEPROM_LOCATION_FLOW_CORRECTION);
  LOG_INF("System: Reset to Default Settings");
}