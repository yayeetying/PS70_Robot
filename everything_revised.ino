#include <ESP32Servo.h>
#include <AccelStepper.h>
#include "AiEsp32RotaryEncoder.h"

#define ROTARY_ENCODER_A_PIN D18
#define ROTARY_ENCODER_B_PIN D19
// #define ROTARY_ENCODER_BUTTON_PIN 21
// The PIN below is NOT in use!
#define ROTARY_ENCODER_VCC_PIN -1 
// How many rotations corresponds to one "turn"; may have to calibrate with actual stepper motor
#define ROTARY_ENCODER_STEPS 100 

#define MAX_VALUE 100
#define MIN_VALUE -100

int TIMES_FULLY_ROTATED = 0;

// Define pin connections
const int vrxPin = D2;         // Joystick X-axis
const int vryPin = D15;         // Joystick Y-axis
const int servoxPin = D12;      // Servo pin
const int stepPin = D5;        // Stepper motor step pin
const int dirPin = D23;         // Stepper motor direction pin

Servo servo;

// Initialize the stepper with DRIVER type (for step and direction control)
AccelStepper stepper(AccelStepper::DRIVER, stepPin, dirPin);

AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);

void rotary_loop()
{
  if (rotaryEncoder.encoderChanged())
  {
    int value = rotaryEncoder.readEncoder();

    Serial.print("Value: ");
    Serial.print(value);
    Serial.print(" | Times fully rotated: ");
    Serial.println(TIMES_FULLY_ROTATED);

    if (value >= MAX_VALUE)
    {
      TIMES_FULLY_ROTATED++;
    }
  }
}

void IRAM_ATTR readEncoderISR()
{
    rotaryEncoder.readEncoder_ISR();
}

void setup() {
  // Initialize Serial Monitor for debugging
  Serial.begin(115200);

  servo.attach(servoxPin);
  pinMode(vrxPin, INPUT);
  pinMode(vryPin, INPUT);
  rotaryEncoder.begin();
  rotaryEncoder.setup(readEncoderISR);
  bool circleValues = true;
  rotaryEncoder.setBoundaries(MIN_VALUE, MAX_VALUE, circleValues);
  // Set maximum speed and acceleration for the stepper motor
  stepper.setMaxSpeed(750);
  stepper.setAcceleration(3000);
  rotaryEncoder.disableAcceleration();
}

void loop() {
  rotary_loop();
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
