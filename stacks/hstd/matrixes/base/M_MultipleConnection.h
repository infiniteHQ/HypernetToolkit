// SimpleDecentralized Matrix (::hstd::SimpleDecentralized in runtime)

//=============================================================================
// What is a matrix ? : A matrix is a environment, a context for all elements,
// actors, channels & components of a Hypernet based network. It provide all
// informations, rules, security system needed 
//=============================================================================
// And about Simple Decentralized matrix ? : This matrix is made around 
// decentralized concept of Hypernet, it provide many basic features to
// manage the matrice, have autorities, resilience and the best security as
// possible.
//=============================================================================


//=============================================================================
// Caracteristics
//=============================================================================
// Type :             Decentralized 
// Targeting :        By Provider Interface
// Element linking :  By Register
// Has Authority :    Yes, the initiator and inherited elements
//=============================================================================



// Logique pour Targeting distribué
// Logique pour la transmission distribuée
// Logique de la decouverte de matrice
// Logique de la resolution des pairs 

#include "../../../../htk.h"
#include "../../../../htk_internals.h"

namespace HToolkit {
namespace hstd {
namespace Matrixes {


struct OnGetMatrixSecurity : public HToolkitCustomEvent{ // Ajouter des hooks pour récupe des elements
void on(hArgs* args) override {
  HTK_LOG("Received ::get<matrixSecurity> !")
}
};

// Tout les handlers de types


/*
Analyser les demandes d'entrées avec un element tagué et le transmettre. (si on est sur une matrix d'IOT sub/broker/emmiter avoir un handle pour chaques types)
*/

struct OnAskToJoinMatrix : public HToolkitCustomEvent{ // Ajouter des hooks pour récupe des elements
void on(hArgs* args) override {
      HtkContext &ctx = *CHToolkit;
      HToolkitNotification* notification = args->get<HToolkitNotification*>("notification", nullptr);

      hString mid = notification->GetParameter("mid");

      hString ety = notification->GetParameter("ety");
      hString eid = notification->GetParameter("eid");
      HtkProviderInterface inter = HToolkit::InitProviderInterface(notification->GetParameter("rec"));

      hVector<hString> vecr = HToolkit::InputStringVector(ety);
      hString test = "58587283846858586995771011151159710310185115101114"; // 

      if(vecr.contains(test.c_str())){
        
        HToolkitElement element;
        element.interface = inter;
      
        HToolkitMatrix* m = ctx.IO.GetActiveMatrix(mid);

         // J'en susi a la !!! Maintenant, reprendre ceci par la matrice en question et taiter la demande d'arrivée
        // dans les matrices que nous créérons plus tard, puis gérer l'implémentation des elements pour ensuite gérer le targeting, etc...

        // Verifier le respect des limites, handshake, etc...

        // Initialisation du nouvelle element (dans le cadre d'une matrice de message, incsription du pseudo, etc...)
        // Attribution d'un ID propre a la matrice
        // Si aucuns problèmes renvoyer un welcome avec les informations de l'element.
        m->OnElementCanJoinMatrix(&element, notification);

        //element.Update();

      }
      else{
        HToolkit::ReportErrorWhileJoinMatrix(inter,"Type not allowed");
      }

      // Accepting element as ::HSTD::E_MessageUser
      

      }
};

//=============================================================================
// MainStructure : SingleMatrix
//=============================================================================
struct M_MultipleConnection : public HToolkitMatrix {



  // Constructor of a new matrix
   void constructor(hArgs* args) override {
    OnGetMatrixSecurity* getMatrixSecurity = new OnGetMatrixSecurity;
    OnAskToJoinMatrix* askToJoinMatrix = new OnAskToJoinMatrix;

    this->AddCustomEvent("::get<matrixSecurity>", getMatrixSecurity);
    this->AddCustomEvent("::ask<joinMatrix>", askToJoinMatrix);

    // Set prop of myState on initializator
    // Set prop of myState on authority
    // Set security system
    // Set all context

    // Add roleffs

    // Set notable important parameters
    this->SetProp(HToolkitMatrix_Props_Limits_MaxNodes,     "1");
    this->SetProp(HToolkitMatrix_Props_Limits_MaxChannels,  "8");
    this->SetProp(HToolkitMatrix_Props_Limits_MaxElements,  "2");


    std::cout << "=================== MATRIX PORTAL ======================" << std::endl;
    std::cout << (char *)HToolkit::GetPortal(this->id).c_str() << std::endl;
    std::cout << "========================================================" << std::endl;

    //VxString Code = HTK_BASE64ENCODE((char *)HToolkit::GetPortal(this->id).c_str());
    //std::cout << (char *)Code.c_str() << std::endl;    
  }

  virtual void init(hArgs* args) override {
    // Envoyer les notifs nessesaire afin d'intialiser les .
    // Si on ne les a pas eu envoyer une notification a l'initialisateur ou l'authorité pour avoir les confs
  }




  // All overrideable matrix functions
    virtual void OnElementCanJoinMatrix(HToolkitElement* element, HToolkitNotification* notification) override {
      this->loadedElements.push_back(element); // Registre

      HToolkit::WelcomeElementOnMatrix(*element, *this, "0");

      //HToolkit::WelcomeElementOnChannel(*element, *this->loadedChannel);
      HToolkit::WelcomeElementOnChannel(*element, *this->loadedChannel);
      loadedChannel->OnElementConnect(*element);  

  }

  virtual void OnChannelCreated(HToolkitChannel* channel) override{
      this->loadedChannel = channel;
      HTK_LOG("Created");
  }


  // Definir des roles (pour ensuite pouvoir envoyer des messages au roles subscriber par exemple) egalement pour etablir la hierarchie



    /*
    
  
  virtual void OnSomeoneAsked ToJoinMatrix()   override {};
  virtual void OnSomeoneAskedToJoinChannel()  override {};
  virtual void OnSomeoneJoinChannel()         override {};
  virtual void OnSomeoneJoinMatrix()          override {};
  virtual void OnTransmissionSent()           override {};
  virtual void OnTransmissionReceived()       override {};*/

  HToolkitChannel* loadedChannel;
  hVector<HToolkitElement*> loadedElements;


  // Metrics 
  int metric_NumberOfTransmissionsReceived;
  int metric_NumberOfTransmissionsSended;
  int metric_BytesReceived;
  int metric_BytesSended;
  int metric_NumberOfLoadedNodes;
  int metric_NumberOfLoadedElements;
  int metric_NumberOfLoadedChannels;

};
//=============================================================================



//=============================================================================
MOD_MATRIX(HSTD, M_MultipleConnection);
//=============================================================================


}; // namespace Matrixes
}; // namespace hstd
}; // namespace HypernetToolkit
//________________________________________