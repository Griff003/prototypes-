#include <DHT.h>

#define DHT11_PIN 2

// Define the type of DHT sensor
#define DHTTYPE DHT11

DHT dht(DHT11_PIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin(); // Initialize the DHT sensor
}

void loop() {
  // Wait a few seconds between measurements
  delay(2000);

  // Read temperature as Celsius (the default)
  float temperature = dht.readTemperature();
  
  // Read humidity
  float humidity = dht.readHumidity();

  // Check if any reads failed and exit early (to try again).
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Temperature = ");
  Serial.print(temperature);
  Serial.println(" *C");
  
  Serial.print("Humidity = ");
  Serial.print(humidity);
  Serial.println(" %");

  delay(1000); // Optional delay to make the output more readable
}
