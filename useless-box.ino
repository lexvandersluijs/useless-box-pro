// Platform libraries.
#include <Arduino.h>  // To add IntelliSense for platform constants.

// Third-party libraries.

// My classes.
#include "speed-servo.h"
#include "status-led.h"
#include "proximity-sensor.h"

#include "config.h"  // To store configuration and secrets.

SpeedServo lidServo;
SpeedServo switchServo;
StatusLed led;
ProximitySensor sensor;

int lastSwitchState = 0;
long playCount = 0;
bool isLidOpen = false;
bool monitorSensor = false;

bool setRandomMode = true;

int mode = 0;
const int NUM_MODES = 10;

void setup() {
  initSerial();
  delay(100);
  initServos();
  delay(100);
  initLed();
  delay(100);
  initSensor();
  delay(100);
  pinMode(PIN_SWITCH, INPUT);
  delay(100);

  Serial.printf("Application version: %s\n", APP_VERSION);
  Serial.println("Setup completed.");
  delay(100);

  if(setRandomMode) // also set the first mode to a random one
    setNextMode(random(0, NUM_MODES));
}

void initSerial() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Initializing serial connection DONE.");
}

void initServos() {
  lidServo.attach(PIN_LID_SERVO);
  lidServo.moveNowTo(LID_START_POSITION);

  switchServo.attach(PIN_SWITCH_SERVO);
  switchServo.moveNowTo(SWITCH_START_POSITION);
}

void initLed() {
  led.setPin(LED_BUILTIN);
  led.turnOff();
}

void initSensor() {
  sensor.attach(PIN_SENSOR_SDA, PIN_SENSOR_SCL, SENSOR_TRIGGER_THRESHOLD);
}

void loop() {
  int switchState = digitalRead(PIN_SWITCH);
  boolean isSwitchTurnedOn = (switchState != lastSwitchState) && (switchState == LOW);

  if (isSwitchTurnedOn) {
    led.turnOn();
    run();
    isLidOpen = false;
    led.turnOff();
  } else {
    // Check the proximity sensor.
    if (sensor.isInRange()) {
      if (!isLidOpen && monitorSensor) {
        openLidFast();
        isLidOpen = true;
      }
    } else {
      if (isLidOpen) {
        closeLidFast();
        isLidOpen = false;
      }
    }
  }

  lastSwitchState = switchState;

  // Wait 250 ms before next reading (required for the sensor).
  delay(250);
}

void setNextMode(int nextMode)
{
    mode = nextMode;
    switch(mode)
    {
        case 1: 
        monitorSensor = true;
        break;

        case 5: 
        monitorSensor = true;
        break;

        case 8: 
        monitorSensor = true;
        break;

        default:
        monitorSensor = false; 
        break;     
    }
}

void run() {
  switch (mode) {
    case 0:
    case 1:
      runSlow();
      break;
    case 2:
      runWaitThenFast();
      break;
    case 3:
      runFast();
      break;
    case 4:
      runFastThenClap();
      break;
    case 5:
      runOpenCloseThenFast();
      break;
    case 6:
      runPeekThenFast();
      break;
    case 7:
      runFastWithDelay();
      break;
    case 8:
      runClap();
      break;
    case 9:
      runHalf(); 
      break;
    default:
      break;
  }

  playCount++;

  if(setRandomMode)
    setNextMode(random(0, NUM_MODES));
  else
    setNextMode(playCount % 10);
}

void runSlow() {
  openLidSlow();
  flipSwitchSlow();
  closeLidSlow();
}

void runWaitThenFast() {
  delay(5000);
  flipSwitchFast();
}

void runFast() {
  flipSwitchFast();
}

void runFastThenClap() {
  flipSwitchFast();
  clapLid();
}

void runOpenCloseThenFast() {
  openLidSlow();
  delay(2000);
  closeLidSlow();
  delay(2000);
  flipSwitchFast();
}

void runPeekThenFast() {
  switchServo.moveSlowTo(SWITCH_HALF_POSITION);
  delay(3000);
  switchServo.moveFastTo(SWITCH_START_POSITION);
  delay(3000);
  flipSwitchFast();
}

void runFastWithDelay() {
  openLidSlow();
  delay(4000);
  flipSwitchFast();
  closeLidFast();
}

void runClap() {
  clapLid();
  clapLid();
  clapLid();
  clapLid();
  openLidFast();
  flipSwitchFast();
  closeLidFast();
}

void runHalf() {
  switchServo.moveSlowTo(SWITCH_HALF_POSITION);
  delay(3000);
  switchServo.moveFastTo(SWITCH_END_POSITION);
  switchServo.moveFastTo(SWITCH_START_POSITION);
}

void openLidSlow() {
  lidServo.moveSlowTo(LID_END_POSITION);
}

void openLidFast() {
  lidServo.moveFastTo(LID_END_POSITION);
}

void closeLidSlow() {
  lidServo.moveSlowTo(LID_START_POSITION);
}

void closeLidFast() {
  lidServo.moveFastTo(LID_START_POSITION);
}

void clapLid() {
  openLidFast();
  closeLidFast();
}

void flipSwitchSlow() {
  switchServo.moveSlowTo(SWITCH_END_POSITION);
  switchServo.moveSlowTo(SWITCH_START_POSITION);
}

void flipSwitchFast() {
  switchServo.moveFastTo(SWITCH_END_POSITION);
  switchServo.moveFastTo(SWITCH_START_POSITION);
}
