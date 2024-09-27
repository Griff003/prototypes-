#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

// WiFi credentials
const char* ssid = "NodeMCU";       
const char* password = "123456789"; 

// Pin definitions
#define DHT11_PIN D2   // Digital pin connected to the DHT11 sensor
#define MQ4_PIN D6     // Digital pin connected to the MQ-4 sensor
#define MQ9_PIN A0     // Analog pin connected to the MQ-9 sensor
#define MQ7_PIN D5     // Digital pin connected to the MQ-7 sensor

// Define the type of DHT sensor
#define DHTTYPE DHT11

// Initialize the DHT sensor
DHT dht(DHT11_PIN, DHTTYPE);

ESP8266WebServer server(80);

// Variables to store sensor readings
float temperature;
float humidity;
int mq4Value;
float mq9Voltage;
int mq7Value;

void setup() {
  // Start serial communication at a baud rate of 9600
  Serial.begin(9600);

  // Initialize the DHT sensor
  dht.begin();

  // Initialize the digital pins as inputs for MQ-4 and MQ-7 sensors
  pinMode(MQ4_PIN, INPUT);
  pinMode(MQ7_PIN, INPUT);

  // Initialize WiFi in Access Point mode
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("Access Point IP:");
  Serial.println(myIP);
  
  // Set up web server routes
  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);
  
  server.begin();
  Serial.println("HTTP Server Started");
}

void loop() {
  server.handleClient();

  // Read DHT11 sensor
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  // Check if any DHT11 reads failed
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
  }

  // Read the digital value from the MQ-4 sensor
  mq4Value = digitalRead(MQ4_PIN);

  // Read the analog value from the MQ-9 sensor and convert to voltage
  int mq9Value = analogRead(MQ9_PIN);
  mq9Voltage = mq9Value * (3.3 / 1023.0);

  // Read the digital value from the MQ-7 sensor
  mq7Value = digitalRead(MQ7_PIN);

  delay(1000); // Optional delay
}

void handle_OnConnect() {
  server.send(200, "text/html", updateWebpage()); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}
String updateWebpage() {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>Sensor Readings</title>\n";
  ptr += "<style>";
  ptr += "html { font-family: 'Comic Sans MS', cursive, sans-serif; display: inline-block; margin: 0px auto; text-align: center; background-color: #f0f8ff;}\n";
  ptr += "body {margin-top: 50px; background: linear-gradient(to bottom, #ffccff 0%, #ccffff 100%); padding: 20px; border-radius: 15px; box-shadow: 0px 0px 20px rgba(0,0,0,0.2);}\n";
  ptr += "h1 {color: #ff66b2; margin: 20px auto; font-size: 36px; animation: color-change 3s infinite alternate;}\n";
  ptr += "h3 {color: #3366cc; margin-bottom: 30px;}\n";
  ptr += ".reading {font-size: 24px; color: #ff9933; margin-bottom: 15px; background-color: #ffffff; padding: 10px; border-radius: 10px; box-shadow: 0px 0px 10px rgba(0,0,0,0.1);}\n";
  ptr += ".reading:nth-child(even) {background-color: #ffe6ff;}\n";
  ptr += ".sensor-box {display: inline-block; width: 80%; padding: 10px; background-color: #ffffff; border-radius: 15px; box-shadow: 0px 0px 15px rgba(0, 0, 0, 0.2); margin-bottom: 15px;}\n";
  ptr += ".sensor-box:hover {transform: scale(1.05); transition: 0.3s;}\n";
  ptr += "@keyframes color-change { from { color: #ff66b2; } to { color: #ff9966; }}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<h1>Maciej Sudra Air Quality Monitoring System</h1>\n";
  ptr += "<h3>Real-Time Sensor Readings</h3>\n";

  ptr += "<div class='sensor-box'>";
  ptr += "<p class='reading'>🌡️ Temperature: " + String(temperature) + " &deg;C</p>\n";
  ptr += "<p class='reading'>💧 Humidity: " + String(humidity) + " %</p>\n";

  if (mq4Value == HIGH) {
    ptr += "<p class='reading'>🟢 MQ-4: No gas detected (Safe)</p>\n";
  } else {
    ptr += "<p class='reading'>🔴 MQ-4: Gas detected! (Be careful)</p>\n";
  }

  ptr += "<p class='reading'>⚡ MQ-9 Sensor Voltage: " + String(mq9Voltage) + " V</p>\n";

  if (mq7Value == HIGH) {
    ptr += "<p class='reading'>🟢 MQ-7: No gas detected (Safe)</p>\n";
  } else {
    ptr += "<p class='reading'>🔴 MQ-7: Gas detected! (Be careful)</p>\n";
  }
  ptr += "</div>";

  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}

