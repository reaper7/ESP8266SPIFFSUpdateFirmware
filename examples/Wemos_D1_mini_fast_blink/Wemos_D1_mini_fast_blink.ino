#include <ESP8266SPIFFSUpdateFirmware.h>

#define BTNPIN      D3
#define LEDPIN      BUILTIN_LED

const String currentSketchName = "Wemos_D1_mini_fast_blink";
const String nextSketchName = "Wemos_D1_mini_slow_blink";
bool btnstate = false;

SPIFFSUpdateFirmwareClass firmware;

void updStart() {
  digitalWrite(LEDPIN, LOW);
  Serial.println("Update start");
}

void updEnd() {
  digitalWrite(LEDPIN, HIGH);
  Serial.println("Update end");
}

void updProgress(unsigned int progress, unsigned int total) {
  digitalWrite(LEDPIN, !digitalRead(LEDPIN));
  uint8_t proc = (progress / (total / 100));
  Serial.printf("Progress: %u%%\r", proc); 
}

void btnpressed() {
  detachInterrupt(BTNPIN);
  btnstate = true;  
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("Start sketch: ");
  Serial.println(currentSketchName);

  pinMode(BTNPIN, INPUT);
  pinMode(LEDPIN, OUTPUT);

  firmware.onStart(updStart);
  firmware.onEnd(updEnd);
  firmware.onProgress(updProgress);
  
  attachInterrupt(BTNPIN, btnpressed, FALLING);
}

void loop() {
  uint8_t nextfirmwareindex = 255;

  if (!btnstate) {
    digitalWrite(LEDPIN, LOW);
    delay(100);
    digitalWrite(LEDPIN, HIGH);
    delay(100);
  } else {
    Serial.print("SPIFFS init...");
    if (firmware.begin()) {
      Serial.println("OK");
      Serial.print("Firmware file(s) count: ");
      uint8_t filesnumber = firmware.getCount();
      Serial.println(filesnumber);
      if (filesnumber > 0) {
        Serial.println("Firmware file(s) list:");
        for (uint8_t i = 0; i < filesnumber; i++) {
          Serial.print(" - ");
          Serial.print(i);
          Serial.print(" ;Size: ");
          Serial.print(firmware.getSize(i));
          Serial.print(" ;Name: ");
          Serial.println(firmware.getName(i));
          if (firmware.getName(i) == nextSketchName) {
            nextfirmwareindex = i;
            Serial.print("Found next firmware [");
            Serial.print(nextSketchName);
            Serial.print("] at index: ");
            Serial.println(i);
          }
        }
        if (nextfirmwareindex != 255 && nextfirmwareindex < filesnumber) {
          bool res = firmware.startUpdate(nextfirmwareindex, true);  
        }
      }     
    } else {
      Serial.println("FAIL");  
    }
    btnstate = false;  
  }
}
