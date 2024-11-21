#include <ESP32Servo.h>
#include <AccelStepper.h>

// Define pin connections
const int vrxPin = 2;         // Joystick X-axis
const int vryPin = 3;         // Joystick Y-axis
const int servoxPin = 4;      // Servo pin
const int stepPin = 7;        // Stepper motor step pin
const int dirPin = 6;         // Stepper motor direction pin

Servo servo;

// Initialize the stepper with DRIVER type (for step and direction control)
AccelStepper stepper(AccelStepper::DRIVER, stepPin, dirPin);

void setup() {
  // Initialize Serial Monitor for debugging
  Serial.begin(115200);

  servo.attach(servoxPin);
  pinMode(vrxPin, INPUT);
  pinMode(vryPin, INPUT);
  // Set maximum speed and acceleration for the stepper motor
  stepper.setMaxSpeed(750);
  stepper.setAcceleration(3000);
}

void loop() {
  // Read joystick values
  int xValue = analogRead(vrxPin);
  int yValue = analogRead(vryPin);

  // Map the joystick X-axis value to a servo angle (0-180 degrees)
  int servoxAngle = map(xValue, 0, 4095, 0, 180);

  // Write the angle to the servo
  servo.write(servoxAngle);

  int minSpeed = 300;

  // Determine speed based on joystick Y-axis input
  if (yValue > 2600) {
    // Joystick pushed forward: set direction to clockwise (CW)
    int speed = map(yValue, 2601, 4095, minSpeed, stepper.maxSpeed());
    stepper.setSpeed(speed);
  }
  else if (yValue < 2100) {
    // Joystick pulled backward: set direction to counterclockwise (CCW)
    int speed = map(yValue, 2099, 0, -minSpeed, -stepper.maxSpeed());
    stepper.setSpeed(speed);
  }
  else {
    // Joystick in neutral Y position: stop the motor
    stepper.setSpeed(0);
  }

  // Run the stepper continuously at the set speed
  stepper.runSpeed();
}