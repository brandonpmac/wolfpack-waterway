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
#include <LiquidCrystal_I2C.h>

#include "init.h"

/// Configure the scheduler
const uint8_t MAX_TASKS = 10;
const uint8_t MS_PER_TICK = 5;
LSched::Scheduler& scheduler = LSched::Scheduler::configure(MAX_TASKS, MS_PER_TICK);

/// Stupid timer emulation because Arduino doesn't have a built in timer
void ArduinoTimerTicker() {
	static uint32_t schedulerTimer;
	uint32_t time = millis();

    if(time - schedulerTimer >= MS_PER_TICK) {
    	schedulerTimer = time;
    	scheduler.timerTick();
    }
}

// set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27,20,4);  

void led_task(void) {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void setup() {
  initialize();

  lcd.init();                      // initialize the lcd 
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("--------------------");
  lcd.setCursor(6,1);
  lcd.print("Brandon");
  lcd.setCursor(1,2);
  lcd.print("Arduino IIC Screen");
  lcd.setCursor(0,3);
  lcd.print("--------------------");

  scheduler.addTask(led_task, 0, 1000);
}

void loop() {
  scheduler.run();

  // stupid timer emulation
  ArduinoTimerTicker();
}
