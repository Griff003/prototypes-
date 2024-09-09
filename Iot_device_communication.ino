
#if defined(ESP32)
  #include <WiFi.h>       // For ESP32
#elif defined(ESP8266)
  #include <ESP8266WiFi.h> // For ESP8266
#else
  #error "Unsupported board"
#endif
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define deviceId "66ab3dc4521c84e469c83c24"

// Update these with values suitable for your network.
const char* ssid = "PIXEL CONNECT";
const char* password = "Pixel@frica2023.";
const char* mqtt_server = "broker.emqx.io";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
#define LOG_CHANNEL "66ab3dc4521c84e469c83c24/logs/mqtt"
#define COMMAND_CHANNEL "66ab3dc4521c84e469c83c24/actuator/main-light"
#define SENSOR_CHANNEL "66ab3dc4521c84e469c83c24/sensor/temperature"

#include "DHT.h"


#define RELAY_PIN 15  


#define DHTPIN 17   
#define DHTTYPE DHT11   

// Initialize the DHT sensor
DHT dht(DHTPIN, DHTTYPE);

char msg[MSG_BUFFER_SIZE];
int value = 0;
int lightState;


// Define NTP Client to get time
int utcOffsetInSeconds =0;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);


void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  
  if ((char)payload[0] == '1') {
    //digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
     digitalWrite(RELAY_PIN, LOW);
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
    lightState=1;
    Serial.println("Turning on");
  } else {
   // digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
     digitalWrite(RELAY_PIN, HIGH);
     lightState=0;
     Serial.println("Turning off");
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(LOG_CHANNEL, "Connected");
      // ... and resubscribe
      client.subscribe(COMMAND_CHANNEL);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

float readTemperature(){
  
  // Read the temperature as Celsius (default)
  float tempC = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(tempC)) {
    Serial.println("Failed to read from DHT sensor!");
    return -1;
  }

  // Print the temperature in Celsius
  Serial.print("Temperature: ");
  Serial.print(tempC);
  Serial.println(" °C");
  return tempC;
  delay(5000);
}

void setup() {
  //pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(9600);
  setup_wifi();
  timeClient.begin();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  Serial.println("DHT11 Temperature Sensor");

  // Initialize the DHT sensor
  dht.begin();

  // Initialize the relay pin as an output
  pinMode(RELAY_PIN, OUTPUT);
  
  // Turn off the relay at startup
  digitalWrite(RELAY_PIN, LOW);
}



void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();


  timeClient.update();

  
  Serial.print(", ");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());
  unsigned long time=timeClient.getEpochTime();

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    float temperature=readTemperature();
    snprintf (msg, MSG_BUFFER_SIZE, "%ld | Info |Last temperature reading %f", time, temperature);
    Serial.print("Publish message: ");
    Serial.println(msg);
    delay(5000);
    client.publish(LOG_CHANNEL, msg);
    snprintf(msg, MSG_BUFFER_SIZE, "%f", temperature);
    client.publish(SENSOR_CHANNEL, msg);
  }
}
