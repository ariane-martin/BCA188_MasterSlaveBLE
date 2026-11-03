#include <BLEDevice.h>

void setup() {
  Serial.begin(115200);
  delay(1000);

  BLEDevice::init("MAC_CHECKER");

  Serial.println("\nESP32 BLE MAC ADDRESS:");
  Serial.println(BLEDevice::getAddress().toString().c_str());
}

void loop() {
}