/**
 * @file main.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief main task
 * @date 2024-01-30
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 */

#include <Arduino.h>
#include <lsched.hpp>

#include "control.h"
#include "init.h"
#include "menu.h"
#include "shell/shell.h"
#include "si_led.h"
#include "si_stepper.h"
#include "sm.h"

/// Configure the scheduler
#define MAX_TASKS (10)
#define MS_PER_TICK (5)

LSched::Scheduler &scheduler =
    LSched::Scheduler::configure(MAX_TASKS, MS_PER_TICK);

/// Stupid timer emulation because Arduino doesn't have a built in timer
void ArduinoTimerTicker() {
  static uint32_t schedulerTimer;
  uint32_t time = millis();

  if (time - schedulerTimer >= MS_PER_TICK) {
    schedulerTimer = time;
    scheduler.timerTick();
  }
}

void setup() {
  initialize();
  scheduler.addTask(sm_task, 0, 100, true, true);
  scheduler.addTask(si_led_task, 1, 500, true, true);
  scheduler.addTask(encoder_task, 2, 15, false, false);
  scheduler.addTask(switch_task, 3, 20, false, false);
  scheduler.addTask(flow_sensor_task, 4, 500, false, false);
  scheduler.addTask(control_task, 5, 100, false, false);
  scheduler.addTask(frame_task, 6, 100, false, false);
  scheduler.addTask(shell_task, 9, 100, true, true);
}

void loop() {
  scheduler.run();
  // stupid timer emulation
  ArduinoTimerTicker();
}
