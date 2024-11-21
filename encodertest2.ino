// left to do: calibrate with actual stepper motor for numbers
// setting up backwards motion(?) lemmie think if this is necessary

#include "AiEsp32RotaryEncoder.h"
#include "Arduino.h"

#define ROTARY_ENCODER_A_PIN D2
#define ROTARY_ENCODER_B_PIN D4
#define ROTARY_ENCODER_BUTTON_PIN 21
#define ROTARY_ENCODER_VCC_PIN -1
// How many rotations corresponds to one "turn"; may have to calibrate with actual stepper motor
#define ROTARY_ENCODER_STEPS 100 

#define MAX_VALUE 100
#define MIN_VALUE -100

int TIMES_FULLY_ROTATED = 0;

//instead of changing here, rather change numbers above
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);

void rotary_onButtonClick()
{
    static unsigned long lastTimePressed = 0; // Soft debouncing
    if (millis() - lastTimePressed < 500)
    {
            return;
    }
    lastTimePressed = millis();
    Serial.print("button pressed ");
    Serial.print(millis());
    Serial.println(" milliseconds after restart");
}

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

void setup()
{
    Serial.begin(250000);

    //we must initialize rotary encoder
    rotaryEncoder.begin();
    rotaryEncoder.setup(readEncoderISR);
    //set boundaries and if values should cycle or not
    //in this example we will set possible values between 0 and 1000;
    bool circleValues = true;
    rotaryEncoder.setBoundaries(MIN_VALUE, MAX_VALUE, circleValues); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)
    Serial.print("setted up");
    /*Rotary acceleration introduced 25.2.2021.
   * in case range to select is huge, for example - select a value between 0 and 1000 and we want 785
   * without accelerateion you need long time to get to that number
   * Using acceleration, faster you turn, faster will the value raise.
   * For fine tuning slow down.
   */
    rotaryEncoder.disableAcceleration(); //acceleration is now enabled by default - disable if you dont need it
    //rotaryEncoder.setAcceleration(250); //or set the value - larger number = more accelearation; 0 or 1 means disabled acceleration
}

void loop()
{
    //in loop call your custom function which will process rotary encoder values
    rotary_loop();
    delay(50); //or do whatever you need to do...
}