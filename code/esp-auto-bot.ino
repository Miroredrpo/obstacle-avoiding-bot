#include <Arduino.h>
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

// Motor Pins (L298N)
#define ENA 5      // PWM for motor A
#define IN1 18
#define IN2 19

#define ENB 17     // PWM for motor B
#define IN3 16
#define IN4 4

// Ultrasonic Sensor Pins
#define TRIG_PIN 12
#define ECHO_PIN 14

// Movement Mode
bool autonomousMode = true;  // true = obstacle avoidance, false = manual (Bluetooth)

// Setup PWM channels for ESP32
const int freq = 5000;
const int resolution = 8; // 0-255
const int motorAChannel = 0;
const int motorBChannel = 1;

// Robot movement functions
void forward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  ledcWrite(motorAChannel, 200);
  ledcWrite(motorBChannel, 200);
}

void backward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  ledcWrite(motorAChannel, 200);
  ledcWrite(motorBChannel, 200);
}

void left() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  ledcWrite(motorAChannel, 200);
  ledcWrite(motorBChannel, 200);
}

void right() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  ledcWrite(motorAChannel, 200);
  ledcWrite(motorBChannel, 200);
}

void stopRobot() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  ledcWrite(motorAChannel, 0);
  ledcWrite(motorBChannel, 0);
}

// Ultrasonic distance measurement
long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH);
  long distance = duration * 0.034 / 2; // cm
  return distance;
}

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Robot"); // Bluetooth device name

  // Motor pins
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Ultrasonic pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Setup PWM channels
  ledcSetup(motorAChannel, freq, resolution);
  ledcSetup(motorBChannel, freq, resolution);
  ledcAttachPin(ENA, motorAChannel);
  ledcAttachPin(ENB, motorBChannel);

  stopRobot();
}

void loop() {
  // Check if Bluetooth has a command
  if (SerialBT.available()) {
    char cmd = SerialBT.read();
    autonomousMode = false; // Switch to manual control when a command is received

    switch(cmd) {
      case 'F': forward(); break;
      case 'B': backward(); break;
      case 'L': left(); break;
      case 'R': right(); break;
      case 'S': stopRobot(); break;
      case 'A': autonomousMode = true; break; // Switch back to autonomous mode
    }
  }

  // Autonomous obstacle avoidance
  if (autonomousMode) {
    long distance = getDistance();
    Serial.print("Distance: ");
    Serial.println(distance);

    if (distance > 20) { // Safe distance
      forward();
    } else {
      stopRobot();
      delay(200);
      backward();
      delay(300);
      if (random(0, 2) == 0) {
        left();
      } else {
        right();
      }
      delay(300);
    }
  }
}




