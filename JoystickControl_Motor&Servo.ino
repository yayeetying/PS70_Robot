#include <AccelStepper.h>
#include <ESP32Servo.h>

// Joystick Servo Controller Class
class JoystickServoController {
private:
    int servoPin;
    int vrxPin;
    int vryPin;
    int deadzone;
    int position;
    int xValue_offset;
    Servo servo;

public:
    JoystickServoController(int servoPin, int vrxPin, int vryPin, int deadzone)
        : servoPin(servoPin), vrxPin(vrxPin), vryPin(vryPin), deadzone(deadzone), position(122) {}

    void begin() {
        servo.attach(servoPin);
        servo.write(position);

        pinMode(vrxPin, INPUT);
        pinMode(vryPin, INPUT);

        xValue_offset = analogRead(vrxPin);
    }

    void update() {
        int xValue = analogRead(vrxPin) - xValue_offset;

        if (xValue > -deadzone && xValue < deadzone) {
            position = 90;
        } else {
            position = map(xValue, -4095, 4095, 0, 180);
            position = constrain(position, 0, 180);
        }

        servo.write(position);
    }
};

// Joystick Motor Controller Class
class JoystickMotorController {
private:
    int vryPin;
    int stepPin;
    int dirPin;
    int yValue_offset;
    AccelStepper stepper;
    int minSpeed;
    int maxSpeed;
    int deadzone;

public:
    JoystickMotorController(int vry, int step, int dir, int minSpd, int maxSpd, int dz)
        : vryPin(vry), stepPin(step), dirPin(dir),
          stepper(AccelStepper::DRIVER, stepPin, dirPin),
          minSpeed(minSpd), maxSpeed(maxSpd), deadzone(dz) {}

    void begin() {
        yValue_offset = analogRead(vryPin);

        stepper.setMaxSpeed(maxSpeed);
        //stepper.setAcceleration(3000);
        stepper.setAcceleration(200);
        stepper.setSpeed(0);
    }

    void update() {
        int yValue = analogRead(vryPin) - yValue_offset;

        if (yValue > deadzone) {
            int speed = map(yValue, deadzone, 1023, minSpeed, maxSpeed);
            stepper.setSpeed(speed);
        } else if (yValue < -deadzone) {
            int speed = map(yValue, -1023, -deadzone, -maxSpeed, -minSpeed);
            stepper.setSpeed(speed);
        } else {
            stepper.setSpeed(0);
            stepper.stop();
        }

        stepper.runSpeed();
    }
};

// Pin Definitions
const int servoPin = 27;
const int vrxPin = 25;
const int vryPin = 32;
const int stepPin = 16;
const int dirPin = 15;

// Motor Settings
const int minSpeed = 300;
const int maxSpeed = 800;
const int deadzone = 20;

JoystickMotorController motorController(vryPin, stepPin, dirPin, minSpeed, maxSpeed, deadzone);
JoystickServoController servoController(servoPin, vrxPin, vryPin, deadzone);

void setup() {
    Serial.begin(9600);
    motorController.begin();
    servoController.begin();
}

void loop() {
    // Update both controllers without delays
    motorController.update();
    servoController.update();
}
