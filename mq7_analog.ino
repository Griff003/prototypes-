// Define the analog pin where the MQ-7 sensor is connected
#define MQ7_PIN A0

void setup() {
  // Start serial communication at a baud rate of 9600
  Serial.begin(9600);
  // Wait for the serial port to connect (for debugging)
  while (!Serial) {
    ; // Wait for serial port to connect. Needed for native USB
  }
  Serial.println("MQ-7 Gas Sensor Test");
}

void loop() {
  // Read the analog value from the MQ-7 sensor
  int sensorValue = analogRead(MQ7_PIN);

  // Convert the analog reading (0-1023) to voltage (0-3.3V)
  float voltage = sensorValue * (3.3 / 1023.0);

  // Display the readings
  Serial.print("Sensor Value: ");
  Serial.print(sensorValue);
  Serial.print(" | Voltage: ");
  Serial.print(voltage);
  Serial.println(" V");

  // Wait for 1 second before taking the next reading
  delay(1000);
}
