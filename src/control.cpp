/**
 * @file control.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief main control loop
 * @date 2024-02-08
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 */

#include "control.h"
#include "init.h"
#include "menu.h"
#include "pins.h"

const int steps_per_revolution = 200;
int current_step = 0;

bool limit_max = false;
bool limit_min = false;

bool run_control = false;
bool pump_1 = false;
bool pump_2 = false;

static int pump_trigger_off = 90;
static int pump_trigger_on = 120;
static int single_pump = -1;

void control_task(void) {

  // checking the max limit switch
  if (digitalRead(SW_LIMIT_MAX) == LOW) {
    limit_max = true;
  } else {
    limit_max = false;
  }
  // checking the min limit switch
  if (digitalRead(SW_LIMIT_MIN) == LOW) {
    limit_min = true;
  } else {
    limit_min = false;
  }

  // check if the control switch is flipped
  if (digitalRead(SW_RUN) == LOW) {
    run_control = true;
  } else {
    run_control = false;
  }

  // single pump logic
  if ((single_pump == -1) && ((tunnel_setpoint < pump_trigger_off))) {
    single_pump = random(0, 2);
  } else if ((single_pump != -1) && ((tunnel_setpoint > pump_trigger_on))) {
    single_pump = -1;
  }

  // controling pump 1
  if (run_control & (single_pump != 0)) {
    digitalWrite(RELAY_1, HIGH);
    pump_1 = true;
  } else {
    digitalWrite(RELAY_1, LOW);
    pump_1 = false;
  }

  // controling pump 2
  if (run_control & (single_pump != 1)) {
    digitalWrite(RELAY_2, HIGH);
    pump_2 = true;
  } else {
    digitalWrite(RELAY_2, LOW);
    pump_2 = false;
  }
}
