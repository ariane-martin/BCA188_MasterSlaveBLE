#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#define SLAVE_NAME "BLE_SLAVE_2"
#define LED_PIN 23

#define SERVICE_UUID "12345678-1234-1234-1234-123456789000"
#define CHARACTERISTIC_UUID "12345678-1234-1234-1234-123456789001"

class CommandCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *characteristic) override {
    std::string value = characteristic->getValue();
    String command = String(value.c_str());

    Serial.print("Received command: ");
    Serial.println(command);

    if (command == "ON") {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("Slave 2 LED: ON");
    }
    else if (command == "OFF") {
      digitalWrite(LED_PIN, LOW);
      Serial.println("Slave 2 LED: OFF");
    }
    else {
      Serial.println("Unknown command");
    }
  }
};

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  BLEDevice::init(SLAVE_NAME);

  BLEServer *server = BLEDevice::createServer();
  BLEService *service = server->createService(SERVICE_UUID);

  BLECharacteristic *characteristic =
    service->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |
      BLECharacteristic::PROPERTY_WRITE
    );

  characteristic->setCallbacks(new CommandCallback());
  characteristic->setValue("Ready");

  service->start();

  BLEAdvertising *advertising = BLEDevice::getAdvertising();
  advertising->addServiceUUID(SERVICE_UUID);
  advertising->setScanResponse(true);

  BLEDevice::startAdvertising();

  Serial.println("==========================");
  Serial.println("BLE SLAVE 2 STARTED");
  Serial.println("Device: BLE_SLAVE_2");
  Serial.println("Waiting for Master...");
  Serial.println("==========================");
}

void loop() {
  delay(100);
}