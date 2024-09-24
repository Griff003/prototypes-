// Define the pin for MQ-4 gas sensor
#define GAS_SENSOR_PIN A0  // Analog pin for the MQ-4 gas sensor

void setup() {
  Serial.begin(115200);  // Start the serial communication
  pinMode(GAS_SENSOR_PIN, INPUT);
  delay(1000);           // Give some time for setup
}

void loop() {
  int gasValue = analogRead(GAS_SENSOR_PIN);  // Read the value from the MQ-4 sensor
  float voltage = (gasValue / 1024.0) * 3.3;  // Convert the analog reading to voltage (for ESP8266's 3.3V range)
  
  Serial.print("Gas Sensor Value: ");
  Serial.print(gasValue);
  Serial.print(" | Voltage: ");
  Serial.print(voltage);
  Serial.println("V");

  delay(2000);  // Wait for 2 seconds before taking another reading
}
