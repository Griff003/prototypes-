// Define the digital pin connected to the MQ-4 sensor
#define MQ4_PIN D5

void setup() {
  // Start serial communication at a baud rate of 9600
  Serial.begin(9600);
  // Initialize the digital pin as an input
  pinMode(MQ4_PIN, INPUT);
  Serial.println("MQ-4 Gas Sensor Digital Output Test");
}

void loop() {
  // Read the digital value from the MQ-4 sensor
  int sensorValue = digitalRead(MQ4_PIN);

  // Display the readings
  if (sensorValue == HIGH) {
    Serial.println("Gas concentration is below the set threshold (No gas detected).");
  } else {
    Serial.println("Gas concentration is above the set threshold (Gas detected).");
  }

  // Wait for 1 second before taking the next reading
  delay(1000);
}
