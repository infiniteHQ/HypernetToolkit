#include "../../../../vortex.h"

namespace Vortex {
namespace hstd {
namespace Providers {

//=============================================================================
// Interface :
//=============================================================================
// - protocol = wifi
// - ssid = [SSID]
// - encrypt = [Encrypt Lvl (wpa2, etc...)]
// - passd = [password],
//=============================================================================

class Wifi : public VortexProvider {
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
    this->ProviderLabel = "TCP/IP";
    this->ProviderProtocol = "TCPIP"; // Standardized transmission protocol name

    // Créez une nouvelle instance de VxProviderInterface et assignez-la à
    // this->interface.
    this->interface = new VxProviderInterface();

    // Hypernet standards for TCP/IP :
    // Ip address buffer : "IP_ADDR", Port buffer : "PORT"
    // Possible custom interfaces specifications : "DOMAIN"
    this->interface->AddValueToInterface("_protocol","TCPIP"); // Remplace exemple with params provided in constructor
    this->interface->AddValueToInterface("IP_ADDR",ip); // Remplace exemple with params provided in constructor
    this->interface->AddValueToInterface("PORT", port);

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
MOD_PROVIDER(TCPP);
//-----------------------------------------------------------------------------


} // namespace Providers
} // namespace hstd
} // namespace Vortex