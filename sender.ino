//put on the students, has GPS for location
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>


// GPS Setup
TinyGPSPlus gps;
SoftwareSerial gpsSerial(D5, D7); // RX, TX (D5, D7)
constexpr char WIFI_SSID[] = "PIXEL CONNECT";
// Receiver MAC Address
uint8_t receiverAddress[] = {0x08, 0xF9, 0xE0, 0x75, 0xE2, 0xC5}; // Replace 'XX' with the correct value


int32_t getWiFiChannel(const char *ssid) {
  if (int32_t n = WiFi.scanNetworks()) {
      for (uint8_t i=0; i<n; i++) {
          if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
              return WiFi.channel(i);
          }
      }
  }
  return 0;
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
  int id=100;
  
} struct_message;

struct_message gpsData;

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  Serial.println(sendStatus == 0 ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  // Start Serial Monitor
  Serial.begin(9600);
  gpsSerial.begin(9600);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  int32_t channel = getWiFiChannel(WIFI_SSID);


WiFi.printDiag(Serial); // Uncomment to verify channel number before
  wifi_promiscuous_enable(1);
  wifi_set_channel(channel);
  wifi_promiscuous_enable(0);
  WiFi.printDiag(Serial); // Uncomment to verify channel change after
  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }



  // Set up send callback
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  esp_now_add_peer(receiverAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}


void readSendGpsData(){

  

 while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
    if (gps.location.isUpdated()) {
      // Fill the structure with GPS data
      gpsData.latitude = gps.location.lat();
      gpsData.longitude = gps.location.lng();
      gpsData.altitude = gps.altitude.meters();
      gpsData.speed = gps.speed.kmph();
      gpsData.satellites = gps.satellites.value();
      gpsData.hdop = gps.hdop.value();

      // Print GPS data to Serial Monitor
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
      Serial.println();

      // Send the GPS data via ESP-NOW
     
    }

else {
gpsData.latitude = 0;
      gpsData.longitude = 0;
      gpsData.altitude = 0;
      gpsData.speed = 0;
      gpsData.satellites =0;
      gpsData.hdop =0;
      gpsData.updated=0;
  
}
 esp_now_send(receiverAddress, (uint8_t *)&gpsData, sizeof(gpsData));

  }

   esp_now_send(receiverAddress, (uint8_t *)&gpsData, sizeof(gpsData));
}
unsigned long initial=0;
int period =1000;
void loop() {

// Implement better delay
unsigned long now =millis();
if (now-initial>=period){
readSendGpsData();
initial=millis();
}
 
}
