/**
 * @file sm_run.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief run state file
 * @date 2024-04-02
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#include <Arduino.h>

#include "control.h"
#include "log.h"
#include "menu.h"
#include "scheduler.h"
#include "si_led.h"
#include "si_stepper.h"
#include "si_switch.h"
#include "sm.h"
#include "sm_run.h"
#include "sm_types.h"

#define TUNNEL_SPEED_START (600)

static sm_run_state_t my_run_state = RUN_NORMAL;
static uint16_t my_run_speed = 585;
static uint32_t my_run_start_time = 0;
static uint32_t my_run_duration = 0;
static uint32_t my_last_point = 0;
static uint8_t my_next_data = 0;
static uint16_t my_test_speeds[60] = {
    550, 545, 540, 535, 530, 525, 520, 515, 510, 505, 500, 495, 490, 485, 480,
    475, 470, 465, 460, 455, 450, 445, 440, 435, 430, 425, 420, 415, 410, 405,
    280, 275, 270, 265, 260, 255, 250, 245, 240, 235, 230, 225, 220, 215, 210,
    280, 275, 270, 265, 260, 255, 250, 245, 240, 235, 230, 225, 220, 215, 210,
};

static bool my_data_active = false;
static uint32_t my_data_delay_time = 0;

/// @brief run entry
/// @param last_event
void sm_run_entry(sm_event_t last_event) {
  frame_set(DISPLAY_FRAME_RUN);
  LOG_INF("Run Entry")

  // enable tasks
  scheduler.enableTask(2, true, true); // encoder task
  scheduler.enableTask(3, true, true); // switch task
  scheduler.enableTask(4, true, true); // flow Sensor taks
  scheduler.enableTask(5, true, true); // flow sensor task
  scheduler.enableTask(6, true, true); // control task

  // set speed to 500
  tunnel_setpoint_set(TUNNEL_SPEED_START);

  // Set the LED
  si_led_set(LED_RUN);
}

/// @brief run exit
/// @param
void sm_run_exit(void) {
  LOG_INF("Run Exit")
  scheduler.enableTask(2, false, false); // encoder task
  scheduler.enableTask(3, false, false); // switch task
  scheduler.enableTask(4, false, false); // switch task
  scheduler.enableTask(5, false, false); // flow sensor task
  scheduler.enableTask(6, false, false); // control task

  si_stepper_speed_set(0);
}

/// @brief run periodic
/// @param
void sm_run_periodic(void) {
  if (!sw_run_get()) {
    sm_event_send(SM_EVENT_SHUTDOWN);
    return;
  }

  uint32_t test_time = millis() - my_run_start_time;

  switch (my_run_state) {
  case RUN_NORMAL:
    break;

  case RUN_DATA:
    if ((millis() - my_data_delay_time) > 30000) {
      sm_run_test(my_test_speeds[my_next_data], 90000);
      my_next_data = my_next_data + 1;
    }
    break;

  case RUN_TEST_START:
    scheduler.enableTask(2, false, false); // encoder task
    my_last_point = 0;
    LOG_INF("Response Test Start");
    LOG_INF("Setpoint: %.4ldmm/s", my_run_speed);
    LOG_INF("Duration: %.4ld seconds", my_run_duration / 1000);
    LOG_INF("P_CONST: %.8d", pid_values_get(0));
    LOG_INF("I_CONST: %.8d", pid_values_get(1));
    LOG_INF("D_CONST: %.8d", pid_values_get(2));
    data_record_set(true, my_run_start_time);
    my_run_state = RUN_TEST_DELAY;
    break;

  case RUN_TEST_DELAY:
    if (test_time < 2000) {
    } else {
      tunnel_setpoint_set(my_run_speed);
      my_run_state = RUN_TEST;
    }
    break;

  case RUN_TEST_END:
    if ((my_next_data < 59) & my_data_active) {
      my_run_state = RUN_DATA;
      my_data_delay_time = millis();
    } else {
      scheduler.enableTask(2, true, true); // encoder task
      my_run_state = RUN_NORMAL;
    }
    LOG_INF("Response Test End");
    data_record_set(false, millis());
    tunnel_setpoint_set(585);
    break;

  case RUN_TEST:
    if (test_time < my_run_duration) {
    } else {
      my_run_state = RUN_TEST_END;
    }
    break;

  default:
  case RUN_ERROR:
    LOG_ERR("Unhandeled Run State")
    break;
  }
}

void sm_run_test(uint16_t speed, uint32_t time) {
  my_run_state = RUN_TEST_START;
  my_run_speed = speed;
  my_run_start_time = millis();
  my_run_duration = time;
}

void sm_run_data(void) {
  my_run_state = RUN_TEST_DELAY;
  my_data_delay_time = millis();
  my_data_active = true;
  tunnel_setpoint_set(585);
}