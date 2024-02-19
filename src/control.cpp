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
static pump_enum_t pump_status = BOTH_OFF;
static bool first_last = random(2); // Bool to remember which pump was last off

static void switch_check(void);
static void single_pump(void);
static void pump_status_check(void);
static void pump_control(void);

void control_task(void) {

  // checking status and performing logic
  switch_check();
  pump_status_check();

  // controlling the water tunnel
  pump_control();
}


static void switch_check(void) {
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
}

static void single_pump(void) {
  // if the first pump was last off, set second to last off
  if (first_last) {
    pump_status = FIRST_ACTIVE;
    // if the second pump was the last off, set the first to last off
  } else {
    pump_status = SECOND_ACTIVE;
  }
  first_last = !first_last;
}

static void pump_status_check(void) {
  if (run_control) {
    switch (pump_status) {
    case BOTH_OFF:
      if (tunnel_setpoint > pump_trigger_on) {
        pump_status = BOTH_ACTIVE;
      } else {
        single_pump();
      }
      break;

    case BOTH_ACTIVE:
      if (tunnel_setpoint < pump_trigger_off) {
        single_pump();
      }
      break;

    case FIRST_ACTIVE:
      if (tunnel_setpoint > pump_trigger_on) {
        pump_status = BOTH_ACTIVE;
      }
      break;

    case SECOND_ACTIVE:
      if (tunnel_setpoint > pump_trigger_on) {
        pump_status = BOTH_ACTIVE;
      }
      break;

    default:
      Serial.println("unhandeled pump status");
      pump_status = BOTH_OFF;
    }
  } else {
    pump_status = BOTH_OFF;
  }
}

static void pump_control(void) {
  switch (pump_status) {
  case BOTH_OFF:
    digitalWrite(PUMP_RELAY_1, LOW);
    digitalWrite(PUMP_RELAY_2, LOW);
    break;

  case BOTH_ACTIVE:
    digitalWrite(PUMP_RELAY_1, HIGH);
    digitalWrite(PUMP_RELAY_2, HIGH);
    break;

  case FIRST_ACTIVE:
    digitalWrite(PUMP_RELAY_1, HIGH);
    digitalWrite(PUMP_RELAY_2, LOW);
    break;

  case SECOND_ACTIVE:
    digitalWrite(PUMP_RELAY_1, HIGH);
    digitalWrite(PUMP_RELAY_2, LOW);
    break;

  default:
    Serial.println("unhandeled pump event");
    break;
  }
}

