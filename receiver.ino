
#include <ESP8266WiFi.h>
#include <espnow.h>




#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Update these with values suitable for your network.
const char* ssid = "PIXEL CONNECT";
const char* password = "Pixel@frica2023.";
const char* mqtt_server = "broker.emqx.io";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (100)
#define LOG_CHANNEL "/logs/1"
#define SUDENT_REPORT "/students_report/1"
#define COMMAND_CHANNEL "/commandStu"

char msg[MSG_BUFFER_SIZE];

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

  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }



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

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

   client.publish(LOG_CHANNEL, "Connected");
  delay(1000);

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


// Structure to hold GPS data
typedef struct {
  float latitude;
  float longitude;
  float altitude;
  float speed;
  int satellites;
  int hdop;
  int updated;
  int id;
} struct_message;

struct_message gpsData;

// Callback when data is received
void OnDataRecv(uint8_t *mac_addr, uint8_t *incomingData, uint8_t len) {
  memcpy(&gpsData, incomingData, sizeof(gpsData));


 unsigned long time=timeClient.getEpochTime();
  

  String data=String(gpsData.id)+"|" +String (gpsData.latitude)+ "|"+String(gpsData.longitude)+"|"+String(gpsData.updated)+"|"+String(time);
  data.toCharArray(msg, data.length()+1);
  client.publish(SUDENT_REPORT, msg);
  
  // Print received GPS data to Serial Monitor
  Serial.print("Latitude: ");
  Serial.println(gpsData.latitude, 6);
  Serial.print("Longitude: ");
  Serial.println(gpsData.longitude, 6);
  Serial.print("Altitude: ");
  Serial.println(gpsData.altitude);
  Serial.print("Speed: ");
  Serial.println(gpsData.speed);
  Serial.print("Satellites: ");
  Serial.println(gpsData.satellites);
  Serial.print("HDOP: ");
  Serial.println(gpsData.hdop);
  Serial.print("Updated: ");
  Serial.println(gpsData.updated);
  Serial.print("Id: ");
  Serial.println(gpsData.id);
  Serial.println();
  
}

void setup() {
  // Start Serial Monitor
  Serial.begin(9600);
  delay(100);
  Serial.println("Setup started");
WiFi.mode(WIFI_AP_STA);
  setup_wifi();
  timeClient.begin();
  timeClient.update();

  
   
  client.setServer(mqtt_server, 1883);
 
  client.setCallback(callback);
  
  // // reconnect();
  // WiFi.mode(WIFI_AP_STA);



  // WiFi.begin(ssid, password);
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(1000);
  //   Serial.println("Setting as a Wi-Fi Station..");
  // }
  // Serial.print("Station IP Address: ");
  // Serial.println(WiFi.localIP());
  // Serial.print("Wi-Fi Channel: ");
  // Serial.println(WiFi.channel());
  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
    
  }
  else{
     Serial.println("ESP-NOW Initialization successful");
  }

 

  // Set up receive callback
  // esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
   esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
  // esp_now_register_recv_cb(OnDataRecv);
  delay(1000);
  Serial.println("Done with setup");
}




void loop() {
  // Main loop does nothing as all work is done in the callback function
   if (!client.connected()) {
    reconnect();
    snprintf (msg, MSG_BUFFER_SIZE, "%ld | Info |Reporting for duty", 23);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(LOG_CHANNEL, msg);
  }
  client.loop();


}
