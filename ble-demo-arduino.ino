#include <ArduinoBLE.h>

void setup() {
  Serial.begin(9600);
  Serial.println("Hello Computer");

  pinMode(LED_BUILTIN, OUTPUT);

  BLE.begin();

  Serial.println("Tank. Pew Pew");
  blink();  //blink when starting


  // start scanning for peripherals
  BLE.scanForUuid("E6B81F14-F9E5-40C9-A739-4DE4564264D1");
}

void blink() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  BLEDevice peripheral = BLE.available();
  if (peripheral) {
    // discovered a peripheral, print out address, local name, and advertised service
    Serial.print("Found ");
    Serial.print(peripheral.address());
    Serial.print(" '");
    Serial.print(peripheral.localName());
    Serial.print("' ");
    Serial.print(peripheral.advertisedServiceUuid());
    Serial.println();
    BLE.stopScan();
    delay(100);

    acceptControl(peripheral);
    BLE.scanForUuid("E6B81F14-F9E5-40C9-A739-4DE4564264D1");
  }

  delay(1000);
}

void acceptControl(BLEDevice peripheral) {
  blink();  //blink when connecting
  Serial.println("Connecting ...");

  if (peripheral.connect()) {
    Serial.println("Connected");
  } else {
    Serial.println("Failed to connect!");
    return;
  }

  // discover peripheral attributes
  Serial.println("Discovering attributes ...");
  if (peripheral.discoverAttributes()) {
    Serial.println("Attributes discovered");
  } else {
    Serial.println("Attribute discovery failed!");
    peripheral.disconnect();
    return;
  }

  BLECharacteristic leftAtt = peripheral.characteristic("82CA4F75-6FC8-48C0-8652-06F4595ADF20");
  BLECharacteristic rightAtt = peripheral.characteristic("82CA4F75-6FC8-48C0-8652-06F4595ADF21");

  leftAtt.subscribe();
  rightAtt.subscribe();

  if (!leftAtt || !rightAtt) {
    Serial.println("Peripheral does not have characteristics!");
    peripheral.disconnect();
    return;
  }

  updateLoop(peripheral, leftAtt, rightAtt);

  blink();  //blink when disconnecting
}

void updateLoop(BLEDevice peripheral, BLECharacteristic leftAtt, BLECharacteristic rightAtt){
  int left = 0;
  int right = 0;

  Serial.println("Entering connection loop");
  while (peripheral.connected()) {

    if (leftAtt.valueUpdated()) { // does not block
      leftAtt.readValue(left);
      Serial.print("value updated: left = ");
      Serial.println(left);
      blink();
    }
    if (rightAtt.valueUpdated()) {
      rightAtt.readValue(right);
      Serial.print("value updated: right = ");
      Serial.println(right);
      blink();
    }
  }
}
