const int stepPin = D3;  // Pin connected to STEP input on driver

void setup() {
  pinMode(stepPin, OUTPUT); // Set step pin as output
}

void loop() {
  // Send a pulse to step the motor
  digitalWrite(stepPin, HIGH);  // Step HIGH
  delay(1);       // Wait for 500 microseconds
  digitalWrite(stepPin, LOW);   // Step LOW
  delay(1);       // Wait for 500 microseconds

  // Repeat for continuous rotation
}