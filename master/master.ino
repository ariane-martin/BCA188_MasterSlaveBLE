#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEClient.h>
#include <string.h>

#define BUTTON_PIN 18

#define SERVICE_UUID "12345678-1234-1234-1234-123456789000"
#define CHARACTERISTIC_UUID "12345678-1234-1234-1234-123456789001"

// Replace these with the actual MAC addresses
// obtained from your two Slave ESP32 boards.

#define SLAVE1_MAC "REPLACE_WITH_SLAVE_1_MAC"
#define SLAVE2_MAC "REPLACE_WITH_SLAVE_2_MAC"

bool ledState = false;

void scanDevices() {
  Serial.println("\nScanning for BLE devices...");

  BLEScan *scan = BLEDevice::getScan();
  scan->setActiveScan(true);

  BLEScanResults results = scan->start(5, false);

  Serial.print("Devices found: ");
  Serial.println(results.getCount());

  for (int i = 0; i < results.getCount(); i++) {
    BLEAdvertisedDevice device = results.getDevice(i);

    Serial.print("Device: ");
    Serial.print(device.getName().c_str());

    Serial.print(" | Address: ");
    Serial.println(device.getAddress().toString().c_str());
  }

  scan->clearResults();
}

bool sendCommand(
  const char *macAddress,
  const char *command,
  const char *slaveName
) {
  Serial.print("\nConnecting to ");
  Serial.println(slaveName);

  BLEClient *client = BLEDevice::createClient();

  if (!client->connect(BLEAddress(macAddress))) {
    Serial.println("Connection failed!");
    delete client;
    return false;
  }

  Serial.println("Connected!");

  BLERemoteService *service =
    client->getService(BLEUUID(SERVICE_UUID));

  if (service == nullptr) {
    Serial.println("Service not found!");
    client->disconnect();
    delete client;
    return false;
  }

  BLERemoteCharacteristic *characteristic =
    service->getCharacteristic(BLEUUID(CHARACTERISTIC_UUID));

  if (characteristic == nullptr) {
    Serial.println("Characteristic not found!");
    client->disconnect();
    delete client;
    return false;
  }

  characteristic->writeValue(
    (uint8_t *)command,
    strlen(command),
    true
  );

  Serial.print(slaveName);
  Serial.print(" received command: ");
  Serial.println(command);

  client->disconnect();
  delete client;

  Serial.print(slaveName);
  Serial.println(" disconnected.");

  return true;
}

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  BLEDevice::init("BLE_MASTER");

  Serial.println("==========================");
  Serial.println("BLE MASTER STARTED");
  Serial.println("==========================");

  scanDevices();

  Serial.println("\nMaster is ready.");
  Serial.println("Press the button to toggle both LEDs.");
}

void loop() {
  static bool lastButtonState = HIGH;
  static unsigned long lastDebounceTime = 0;

  bool reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if (millis() - lastDebounceTime > 50) {
    static bool buttonHandled = false;

    if (reading == LOW && !buttonHandled) {
      buttonHandled = true;

      ledState = !ledState;

      const char *command = ledState ? "ON" : "OFF";

      Serial.println("\n==========================");
      Serial.print("MASTER COMMAND: ");
      Serial.println(command);
      Serial.println("==========================");

      sendCommand(SLAVE1_MAC, command, "SLAVE 1");
      sendCommand(SLAVE2_MAC, command, "SLAVE 2");
    }

    if (reading == HIGH) {
      buttonHandled = false;
    }
  }

  lastButtonState = reading;
  delay(10);
}