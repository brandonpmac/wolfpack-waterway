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

void led_task(void) { digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); }

void setup() {
  initialize();

  scheduler.addTask(led_task, 0, 1000);
  scheduler.addTask(encoder_task, 1, 15);
  scheduler.addTask(control_task, 2, 20);
  scheduler.addTask(frame_task, 3, 100);
  scheduler.addTask(shell_task, 9, 100);
}

void loop() {
  scheduler.run();
  // stupid timer emulation
  ArduinoTimerTicker();
}
