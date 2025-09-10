#include <WiFi.h>
#include <WiFiMulti.h>
#include "arduino_secrets.h"
#include "OTAMgr.h"
#include "EventLogger.h"
WiFiMulti wifiMulti;

const String otaServerURL = "https://ota.sempleserve.co.uk/api/device/check";

EventLogger logger;
void setup() {

  Serial.begin(115200);

  Serial.println("OLD FIRMWARE");
  wifiMulti.addAP(SECRET_SSID1, SECRET_PASS1);
  wifiMulti.addAP(SECRET_SSID2, SECRET_PASS2);

  // String hostName = "Sketch:" + WiFI.MacAddress();
  // WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  // WiFi.setHostName(hostName.c_str());

  if (wifiMulti.run(3000) == WL_CONNECTED) {
    Serial.println("Connected to WiFi in setup()");
  } else {
    // Failed to connect in 3000 ms
  }

  g_OTAMgr.init(otaServerURL, "0.0.4", logger);
}

void loop() {
  
  delay(3000);

  while(wifiMulti.run(3000) != WL_CONNECTED) {
    Serial.println("Trying to connect to WiFi in loop()");
  }
  Serial.println("Connected - press 'u' to update");

  char incomingByte = Serial.read();
  if (incomingByte == 'u'){
    if (g_OTAMgr.isOK()) {
      if (! g_OTAMgr.checked()) {
        Serial.println("Checking for update...");
        g_OTAMgr.checkForUpdate();
      } else {
        Serial.println("already checked");
      }
    } else {
      Serial.println("OTA is not ok");
    }
  }
}