
#include <Arduino.h>
#include <ESP32Servo.h>

Servo liftMotor;
Servo grabMotor;

// ========== PINS ==========
const int LIFT_PIN = 18;
const int GRAB_PIN = 19;

// ========== SERVO SIGNALS ==========
// Typical continuous-rotation servo values.
// Neutral may need calibration.
const int STOP_US = 1500;
const int FORWARD_US = 1600;
const int REVERSE_US = 1400;

// ========== SAFETY ==========
// Maximum run time per command.
// This is NOT a mechanical limit sensor.
const unsigned long MAX_RUN_MS = 500;

// Motor state
bool liftRunning = false;
bool grabRunning = false;

unsigned long liftStopTime = 0;
unsigned long grabStopTime = 0;

// ========== FUNCTIONS ==========

void stopLift() {
  liftMotor.writeMicroseconds(STOP_US);
  liftRunning = false;
  Serial.println("LIFT stopped");
}

void stopGrab() {
  grabMotor.writeMicroseconds(STOP_US);
  grabRunning = false;
  Serial.println("GRAB stopped");
}

void stopAll() {
  stopLift();
  stopGrab();
}

void runLift(int pulse, unsigned long duration) {
  if (duration > MAX_RUN_MS) {
    duration = MAX_RUN_MS;
  }

  liftMotor.writeMicroseconds(pulse);
  liftStopTime = millis() + duration;
  liftRunning = true;

  Serial.println("LIFT running");
}

void runGrab(int pulse, unsigned long duration) {
  if (duration > MAX_RUN_MS) {
    duration = MAX_RUN_MS;
  }

  grabMotor.writeMicroseconds(pulse);
  grabStopTime = millis() + duration;
  grabRunning = true;

  Serial.println("GRAB running");
}

void printHelp() {
  Serial.println();
  Serial.println("===== SMARTIVITY ROBOT =====");
  Serial.println("LF : Lift / pull");
  Serial.println("LR : Release lift");
  Serial.println("GC : Close gripper");
  Serial.println("GO : Open gripper");
  Serial.println("S  : Stop both motors");
  Serial.println("HELP : Show commands");
  Serial.println();
  Serial.println("Each movement lasts max 500ms");
  Serial.println("===========================");
}

void handleCommand(String cmd) {
  cmd.trim();
  cmd.toUpperCase();

  if (cmd == "LF") {
    runLift(FORWARD_US, 200);
  }
  else if (cmd == "LR") {
    runLift(REVERSE_US, 200);
  }
  else if (cmd == "GC") {
    runGrab(FORWARD_US, 200);
  }
  else if (cmd == "GO") {
    runGrab(REVERSE_US, 200);
  }
  else if (cmd == "S") {
    stopAll();
  }
  else if (cmd == "HELP") {
    printHelp();
  }
  else {
    Serial.println("Unknown command");
  }
}

// ========== SETUP ==========

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(30);

  liftMotor.setPeriodHertz(50);
  grabMotor.setPeriodHertz(50);

  liftMotor.attach(LIFT_PIN, 1000, 2000);
  grabMotor.attach(GRAB_PIN, 1000, 2000);

  // Neutral signals
  liftMotor.writeMicroseconds(STOP_US);
  grabMotor.writeMicroseconds(STOP_US);

  Serial.println("Smartivity Dual Motor Ready");
  printHelp();
}

// ========== LOOP ==========

void loop() {
  // Read user commands
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    handleCommand(cmd);
  }

  // Timed motor stops
  unsigned long now = millis();

  if (liftRunning &&
      (long)(now - liftStopTime) >= 0) {
    stopLift();
  }

  if (grabRunning &&
      (long)(now - grabStopTime) >= 0) {
    stopGrab();
  }
}
