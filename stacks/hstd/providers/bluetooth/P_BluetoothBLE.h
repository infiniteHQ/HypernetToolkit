#include "../../../../vortex.h"

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"


namespace Vortex {
namespace hstd {
namespace Providers {

static char *ip;
static int port;

class ESPBluetooth : public VortexProvider {
public:

void TrySend(VxProviderInterface* interface, const char* buffer) override {
   
}

void TryReceive() {
    
}

//-----------------------------------------------------------------------------
// (Overrideable logic) Start()
//-----------------------------------------------------------------------------
// The start is called by Vortex runtime to start the provider service.
// /!\ Remember : Thread are not supported by default by the runtime !
//-----------------------------------------------------------------------------
  void Start() override {
    this->ProviderLabel = "Bluetooth";
    this->ProviderProtocol = "Bluetooth"; 

    this->interface = new VxProviderInterface();
    this->interface->AddValueToInterface("_protocol","Bluetooth"); // Remplace exemple with params provided in constructor
    this->interface->AddValueToInterface("SERVICE_UUID", SERVICE_UUID); // Remplace exemple with params provided in constructor
    this->interface->AddValueToInterface("CHARACTERISTIC_UUID", CHARACTERISTIC_UUID); // Remplace exemple with params provided in constructor

    Serial.begin(115200);
  Serial.println("Starting BLE work!");

  BLEDevice::init("Long name works now");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setValue("Hello World says Neil");
  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");

    Vortex::AddReceiver(this->interface);
      std::thread Thread([&]() {
        this->TryReceive();
    });
    receiveThread.swap(Thread);

    this->Connect();
  }
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// (Overrideable logic) Stop()
//-----------------------------------------------------------------------------
// The stop is called by Vortex runtime to stop the provider service.
// /!\ Remember : Thread are not supported by default by the runtime !
//-----------------------------------------------------------------------------
  void Stop() override {
    // Attendez que les threads se terminent
    this->receiveThread.join();
  }
//-----------------------------------------------------------------------------

  // IO : 
  void Send(const char* data, VxProviderInterface* interface) override {
    if(interface == nullptr){
        std::cout << "The provider interface is not valid !!" << std::endl;
        return;
    }
    this->TrySend(interface, data);
    this->DataSent += sizeof(data);
    this->onSent();
  }

  void Receive(const char* data, VxProviderInterface* interface) override {
    this->onReceived();
    this->DataReceived += sizeof(data);
    Vortex::HandleReceiveData(data, interface);
  }

  void Connect() override {
    std::cout << "Connect null provider..." << std::endl;
  }

  void Disconnect() override {
    std::cout << "HSTD::Providers::TCP/IP" << std::endl;
  }

  // Events :
  void onSent() override { std::cout << "Something sent !" << std::endl; }
  void onReceived() override { std::cout << "Something received !" << std::endl; }
  
//-----------------------------------------------------------------------------
// Static configurations
//-----------------------------------------------------------------------------
  static void SetIP(char *_ip){ip = _ip;};  
  static void SetPort(int _port){port = _port;};
//-----------------------------------------------------------------------------
  
  std::thread receiveThread;
  int bytesRead = 0;
  int bytesWritten = 0;

};


//-----------------------------------------------------------------------------
// Regiter including
//-----------------------------------------------------------------------------
// This line include the (TCP) provider into Vortex runtime registry 
// You can juste include this file to include provider
//-----------------------------------------------------------------------------
MOD_PROVIDER(ESPBluetooth);
//-----------------------------------------------------------------------------


} // namespace Providers
} // namespace hstd
} // namespace Vortex