// simple tcp/ip provider for UNIX/Linux systems

// Copyright (c) 2023 Diego Enzo Moreno

//=============================================================================
// What is a provider ? : A provider give you the possibilitie to communicatte
// with each other elements, actors, services and many more.
//=============================================================================
// And about tcpip provider ? : This provider can give a simple interface for
// TCP/IP transport protocol. 
//=============================================================================


//=============================================================================
// Caracteristics
//=============================================================================
//=============================================================================

//=============================================================================
// Interface :
//=============================================================================
// - protocol = tcpip
// - ip = [IP adress]
// - port = [Port number]
//=============================================================================


#include "../../../../htk.h"

#include <arpa/inet.h>
#include <cstddef>
#include <cstdlib>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sstream>
#include <thread>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <unistd.h>




namespace HToolkit {
namespace hstd {
namespace Providers {

static char *ip;
static int port;

class P_TcpIp : public HToolkitProvider {
public:

void TrySend(HtkProviderInterface& interface, const char* buffer) override {
    std::string DestinationIp = interface.GetInterfaceVariable("ip").c_str();
    int DestinationPort = std::atoi((char *)interface.GetInterfaceVariable("port").c_str());
    char msg[2048];
    struct hostent *host = gethostbyname((char *)DestinationIp.c_str());
    sockaddr_in sendSockAddr;
    bzero((char *)&sendSockAddr, sizeof(sendSockAddr));
    sendSockAddr.sin_family = AF_INET;
    sendSockAddr.sin_addr.s_addr =inet_addr(inet_ntoa(*(struct in_addr *)*host->h_addr_list));
    sendSockAddr.sin_port = htons(DestinationPort);
    int clientSd = socket(AF_INET, SOCK_STREAM, 0);

    int status = connect(clientSd, (sockaddr *)&sendSockAddr, sizeof(sendSockAddr));
    if (status < 0) {
        // Error connecting to socket!;
    } else {
        // Connected to the server!
        int bytesRead, bytesWritten = 0;
        struct timeval start1, end1;
        gettimeofday(&start1, NULL);
            std::string data = buffer;
            memset(&msg, 0, sizeof(msg)); 
            strcpy(msg, data.c_str());
            bytesWritten += send(clientSd, (char *)&msg, strlen(msg), 0);
        gettimeofday(&end1, NULL);
        close(clientSd);
        // Connection closed
    }
}

void TryReceive() {
    char msg[2048]; 
    int bytesRead = 0; 

    sockaddr_in servAddr;
    bzero((char *)&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);

    int serverSd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSd < 0) {
      // Error establishing the server socket
      exit(0);
    }
    int bindStatus =
        bind(serverSd, (struct sockaddr *)&servAddr, sizeof(servAddr));
    if (bindStatus < 0) {
      // Error binding socket to local address
      exit(0);
    }

    while(1){
    listen(serverSd, 5);
    sockaddr_in newSockAddr;
    socklen_t newSockAddrSize = sizeof(newSockAddr);
    int newSd = accept(serverSd, (sockaddr *)&newSockAddr, &newSockAddrSize);
    if (newSd < 0) {
      // Error accepting request from client!;
      exit(1);
    }
    struct timeval start1, end1;
    gettimeofday(&start1, NULL);
    
    while(1)
    {
      memset(&msg, 0, sizeof(msg));
      bytesRead += recv(newSd, &msg, sizeof(msg), 0);
      
      if (bytesRead > 0) {
        HtkProviderInterface interface;
        struct sockaddr_in sa;
        char str[INET_ADDRSTRLEN];
        interface.AddValueToInterface("protocol", "tcpip");
        interface.AddValueToInterface("ip", str);
        interface.AddValueToInterface("port", (char *)std::to_string(newSockAddr.sin_port).c_str());

        this->Receive(msg, interface);
        memset(&msg, 0, sizeof(msg));//clear the buffer
        memset(&bytesRead, 0, sizeof(bytesRead));//clear the buffer
      } 
      else if (bytesRead == 0) {
        break;
      }  
    }
  }    
}

//-----------------------------------------------------------------------------
// (Overrideable logic) Start()
//-----------------------------------------------------------------------------
// The start is called by HToolkit runtime to start the provider service.
// /!\ Remember : Thread are not supported by default by the runtime !
//-----------------------------------------------------------------------------
  void Start() override {
    this->ProviderLabel = "tcpip";
    this->ProviderProtocol = "tcpip"; // Standardized transmission protocol name

    // Créez une nouvelle instance de HtkProviderInterface et assignez-la à
    // this->interface.
    this->interface = new HtkProviderInterface();

    // Hypernet standards for TCP/IP :
    // Ip address buffer : "IP_ADDR", Port buffer : "PORT"
    // Possible custom interfaces specifications : "DOMAIN"
    this->interface->AddValueToInterface("protocol","tcpip"); // Remplace exemple with params provided in constructor
    this->interface->AddValueToInterface("ip",ip); // Remplace exemple with params provided in constructor
    this->interface->AddValueToInterface("port", port);
    HToolkit::AddReceiver(this->interface);
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
// The stop is called by HToolkit runtime to stop the provider service.
// /!\ Remember : Thread are not supported by default by the runtime !
//-----------------------------------------------------------------------------
  void Stop() override {
    // Attendez que les threads se terminent
    this->receiveThread.join();
  }
//-----------------------------------------------------------------------------

  // IO : 
  virtual void Send(const char* data, HtkProviderInterface& interface) override {
    HTK_LOG("Send something")
    this->TrySend(interface, data);
    this->DataSent += sizeof(data);
    this->onSent();
  }

  virtual void Receive(const char* data, HtkProviderInterface& interface) override {
    HTK_LOG("Receive : " << data); 
    this->onReceived();
    this->DataReceived += sizeof(data);
    HToolkit::HandleReceiveData(data, interface);
  }

  void Connect() override {
  }

  void Disconnect() override {
  }

  // Events :
  void onSent() override {  }
  void onReceived() override {  }
  
//-----------------------------------------------------------------------------
// Static configurations
//-----------------------------------------------------------------------------
  static void configure(char *_ip, int _port){ip = _ip;port = _port;};
  static void setIP(char *_ip){ip = _ip;};  
  static void setPort(int _port){port = _port;};
//-----------------------------------------------------------------------------
  
  std::thread receiveThread;
  int bytesRead = 0;
  int bytesWritten = 0;

};


//-----------------------------------------------------------------------------
// Regiter including
//-----------------------------------------------------------------------------
// This line include the (TCP) provider into HToolkit runtime registry 
// You can juste include this file to include provider
//-----------------------------------------------------------------------------
ADD_PROVIDER_TO_FACTORY(HSTD, P_TcpIp);
//-----------------------------------------------------------------------------


} // namespace Providers
} // namespace hstd
} // namespace HToolkit