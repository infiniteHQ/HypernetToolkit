#include "../../../../vortex.h"
#include <ESP8266WiFi.h>

namespace Vortex {
namespace hstd {
namespace Providers {

//=============================================================================
// Interface :
//=============================================================================
// - protocol = wifi
// - ssid = [SSID]
// - passd = [password],
// - ip = [password],
// - port = [password],
// - encrypt = [Encrypt Lvl (wpa2, etc...)]
//=============================================================================

static const char* ssid = "your_ssid";
static const char* pwssd = "your_password";
static int port = 1234;  // Remplacez par le port réel que vous souhaitez utiliser


class P_ESP8266_Wifi : public VortexProvider {
public:

void TrySend(VxProviderInterface& interface, const char* buffer) override {
}

void TryReceive() {
}

//-----------------------------------------------------------------------------
// (Overrideable logic) Start()
//-----------------------------------------------------------------------------
// The start is called by Vortex runtime to start the provider service.
// /!\ Remember : Threads are not supported by default by the runtime !
//-----------------------------------------------------------------------------
  void Start() override {
    if(this->initMode){ // -> Initiator mode
      WiFi.softAP(ssid, pwssd);
    }
    else{

    }
  }
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// (Overrideable logic) Stop()
//-----------------------------------------------------------------------------
// The stop is called by Vortex runtime to stop the provider service.
// /!\ Remember : Threads are not supported by default by the runtime !
//-----------------------------------------------------------------------------
  void Stop() override {
  }
//-----------------------------------------------------------------------------


  // Events :
  void onSent() override { std::cout << "Something sent !" << std::endl; }
  void onReceived() override { std::cout << "Something received !" << std::endl; }


  static void Configure(char *_ssid, char *_passd) { ssid = _ssid; pwssd = _passd; }
  
//-----------------------------------------------------------------------------
// Static configurations
//-----------------------------------------------------------------------------

private:
  int port;
  bool initMode;
  int bytesRead = 0;
  int bytesWritten = 0;



};


//-----------------------------------------------------------------------------
// Regiter including
//-----------------------------------------------------------------------------
// This line include the (TCP) provider into Vortex runtime registry 
// You can juste include this file to include provider
//-----------------------------------------------------------------------------
MOD_PROVIDER(P_ESP8266_Wifi);
//-----------------------------------------------------------------------------


} // namespace Providers
} // namespace hstd
} // namespace Vortex