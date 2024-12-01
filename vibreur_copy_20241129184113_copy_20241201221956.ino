int vibratorPin = 9;  // Pin where the vibrator is connected (change if needed)

void setup() {
  pinMode(vibratorPin, OUTPUT);  // Set the vibrator pin as an output
  Serial.begin(9600);  // Start serial communication at 9600 baud rate
}

void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read();  // Read the incoming byte

    if (command == '1') {  // If the command is '1', activate the vibrator
      digitalWrite(vibratorPin, HIGH);  // Turn on the vibrator
      delay(2000);  // Keep it on for 500ms (adjust as needed)
      digitalWrite(vibratorPin, LOW);  // Turn off the vibrator
    }
  }
}
