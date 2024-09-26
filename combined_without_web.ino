#include <DHT.h>

// Pin definitions
#define DHT11_PIN D2    // Digital pin connected to the DHT11 sensor
#define MQ4_PIN D6     // Digital pin connected to the MQ-4 sensor
#define MQ9_PIN A0     // Analog pin connected to the MQ-9 sensor
#define MQ7_PIN D5     // Digital pin connected to the MQ-7 sensor

// Define the type of DHT sensor
#define DHTTYPE DHT11

// Initialize the DHT sensor
DHT dht(DHT11_PIN, DHTTYPE);

void setup() {
  // Start serial communication at a baud rate of 9600
  Serial.begin(9600);

  // Initialize the DHT sensor
  dht.begin();

  // Initialize the digital pins as inputs for MQ-4 and MQ-7 sensors
  pinMode(MQ4_PIN, INPUT);
  pinMode(MQ7_PIN, INPUT);

  // Display startup messages
  Serial.println("DHT11 and MQ Series Gas Sensors Test Initialized");
}

void loop() {
  // Wait a few seconds between measurements for the DHT11 sensor
  delay(2000);

  // Read temperature as Celsius (the default)
  float temperature = dht.readTemperature();
  
  // Read humidity
  float humidity = dht.readHumidity();

  // Check if any reads failed and exit early (to try again).
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    Serial.print("Temperature = ");
    Serial.print(temperature);
    Serial.println(" *C");

    Serial.print("Humidity = ");
    Serial.print(humidity);
    Serial.println(" %");
  }

  // Read the digital value from the MQ-4 sensor
  int mq4Value = digitalRead(MQ4_PIN);

  // Display the MQ-4 sensor readings
  if (mq4Value == HIGH) {
    Serial.println("MQ-4: Gas concentration is below the set threshold (No gas detected).");
  } else {
    Serial.println("MQ-4: Gas concentration is above the set threshold (Gas detected).");
  }

  // Read the analog value from the MQ-9 sensor
  int mq9Value = analogRead(MQ9_PIN);
  
  // Convert the analog reading (0-1023) to voltage (0-3.3V)
  float mq9Voltage = mq9Value * (3.3 / 1023.0);

  // Display the MQ-9 sensor readings
  Serial.print("MQ-9 Sensor Value: ");
  Serial.print(mq9Value);
  Serial.print(" | Voltage: ");
  Serial.print(mq9Voltage);
  Serial.println(" V");

  // Read the digital value from the MQ-7 sensor
  int mq7Value = digitalRead(MQ7_PIN);

  // Display the MQ-7 sensor readings
  if (mq7Value == HIGH) {
    Serial.println("MQ-7: Gas concentration is below the set threshold (No gas detected).");
  } else {
    Serial.println("MQ-7: Gas concentration is above the set threshold (Gas detected).");
  }

  // Wait for 1 second before taking the next readings
  delay(1000);
}
