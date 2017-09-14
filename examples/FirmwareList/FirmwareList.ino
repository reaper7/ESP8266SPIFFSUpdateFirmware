#include <ESP8266SPIFFSUpdateFirmware.h>

SPIFFSUpdateFirmware firmware;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Start sketch");

  Serial.print("SPIFFS init...");
  if (firmware.begin()) {
    Serial.println("OK");
    Serial.print("Firmware file(s) count: ");
    uint8_t filesnumber = firmware.getCount();
    Serial.println(filesnumber);
    if (filesnumber > 0) {
      Serial.println("Firmware file(s) list:");
      for (uint8_t i = 0; i < filesnumber; i++) {
        Serial.print(" -> ");
        Serial.print(i);
        Serial.print("; Size: ");
        Serial.print(firmware.getSize(i));
        Serial.print("; Name: ");
        Serial.println(firmware.getName(i));
      }
    }     
  } else
    Serial.println("FAIL");  
}

void loop() {

}
