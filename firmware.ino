#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#define DEVICE_NAME     "LVS-ESP32" // All Lovense toys start with "LVS-"
#define DEVICE_SERVICE  "45440001-0023-4BD4-BBD5-A6920E4C5653" // Service UUID of a Gush (ED01001)
#define DEVICE_CHAR_RX  "45440002-0023-4BD4-BBD5-A6920E4C5653" // Write Characteristic UUID of a Gush (ED01001)
#define DEVICE_CHAR_TX  "45440003-0023-4BD4-BBD5-A6920E4C5653" // Notify Characteristic UUID of a Gush (ED01001)


// BLE Write Characteristic to mimic the RX of a serial connection
BLECharacteristic   blecRX (DEVICE_CHAR_RX, BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR);
// BLE Descriptor for blecRX
BLEDescriptor       bledRX (BLEUUID((uint16_t)0x2902));
// BLE Notify Characteristic to mimic the TX of a serial connection
BLECharacteristic   blecTX (DEVICE_CHAR_TX, BLECharacteristic::PROPERTY_NOTIFY);
// BLE Descriptor for blecTX
BLEDescriptor       bledTX (BLEUUID((uint16_t)0x2902));


// Public BLE stuff
BLEServer* server;
BLEService* service;
BLEAdvertising* advert;



// Method for handling Lovense messages
std::string lvsHandleMessage (std::string message, BLECharacteristic* blec);



// Start advertising ourselves
void startAdvertising () {
    server->getAdvertising()->start();
    Serial.println("[BLE] Waiting for client...");
}

// Server callbacks
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        Serial.printf("[BLE] Device connected. Now serving %i device(s).\n", pServer->getConnectedCount() + 1);
    };
    void onDisconnect(BLEServer* pServer) {
        Serial.printf("[BLE] Device disconnected. Now serving %i device(s).\n", pServer->getConnectedCount() - 1);
        startAdvertising();
    }
};

// Characterstic callbacks
class MyCharacteristicCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* c) {
        std::string message = c->getValue();

        // Handle the message
        std::string response = lvsHandleMessage(message, c);

        // Return the response (if available)
        if (response.length() > 0) {
            blecTX.setValue(response);
            blecTX.notify();
        }

        //Serial.printf("[BLE] LVS Traffic: \"%s\" -> \"%s\"\n", message.c_str(), response.c_str()); 
    }
};



void setup() {
    Serial.begin(115200);
    Serial.println("");

    // Create the BLE Device
    BLEDevice::init(DEVICE_NAME);
    Serial.printf("[BLE] Advertising as \"%s\"\n", DEVICE_NAME);   

    // Create the BLE Server
    server = BLEDevice::createServer();
    server->setCallbacks(new MyServerCallbacks());

    // Create the BLE Service
    service = server->createService(DEVICE_SERVICE);

    // Add the characteristics
    service->addCharacteristic(&blecRX);
    blecRX.addDescriptor(&bledRX);
    service->addCharacteristic(&blecTX);
    blecTX.addDescriptor(&bledTX);

    // Add necessary callbacks
    blecRX.setCallbacks(new MyCharacteristicCallbacks());

    // Start the service
    service->start();

    // Setup advertising
    BLEAdvertising* advert = BLEDevice::getAdvertising();
    advert->addServiceUUID(DEVICE_SERVICE);

    // Start advertising
    startAdvertising();
}

void loop() {}






std::string lvsHandleMessage (std::string message, BLECharacteristic* blec) {
    if (message == "Battery;") {
        Serial.println("[LVS] Got Battery message."); 
        return "100;";

    } else if (message == "DeviceType;") {
        // This is a completely fake device type, but the app doesn't seem to care, so I'll leave it like this for now
        Serial.println("[LVS] Got DeviceType message.");
        return "ED:000:DEADBEEFDEAD";

    } else if (message.substr(0, 7) == "Vibrate") {
        // This probably turns the heap into swiss cheese
        int level = atoi(message.substr(8, message.length() - 9).c_str());

        Serial.printf("[LVS] Got Vibrate message. (level=%i)\n", level);

        // Acknowledge vibration level change
        return "OK;";
    }

    return "";
}






