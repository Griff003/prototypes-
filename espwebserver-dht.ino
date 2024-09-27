#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

/* Put your SSID & Password */
const char* ssid = "NodeMCU";       
const char* password = "123456789"; 

#define DHT11_PIN D5  // DHT11 data pin is connected to GPIO2 (D4 on NodeMCU)
#define DHTTYPE DHT11

DHT dht(DHT11_PIN, DHTTYPE);

ESP8266WebServer server(80);

float temperature;
float humidity;

void setup() {
  Serial.begin(9600);
  dht.begin(); // Initialize the DHT sensor
  
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("Access Point IP:");
  Serial.println(myIP);
  
  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);
  
  server.begin();
  Serial.println("HTTP Server Started");
}

void loop() {
  server.handleClient();
  // Read temperature as Celsius
  temperature = dht.readTemperature();
  
  // Read humidity
  humidity = dht.readHumidity();

  // Check if any reads failed and exit early (to try again)
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
}

void handle_OnConnect() {
  server.send(200, "text/html", updateWebpage(temperature, humidity)); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String updateWebpage(float temperature, float humidity){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>DHT11 Sensor Readings</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr += ".reading {font-size: 24px;color: #3498db;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<h1>ESP8266 Web Server</h1>\n";
  ptr += "<h3>DHT11 Sensor Readings</h3>\n";
  ptr += "<p class=\"reading\">Temperature: " + String(temperature) + " &deg;C</p>\n";
  ptr += "<p class=\"reading\">Humidity: " + String(humidity) + " %</p>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}
