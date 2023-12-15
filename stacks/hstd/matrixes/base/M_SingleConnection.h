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


//=============================================================================
// MainStructure : SingleMatrix
//=============================================================================
struct M_SingleConnection : public HToolkitMatrix {


  // Constructor of a new matrix
   void constructor() override {
    // Set prop of myState on initializator
    // Set prop of myState on authority
    // Set security system
    // Set all context

    // Add roles

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

  void initializator() {
    // Si on ne les a pas eu envoyer une notification a l'initialisateur ou l'authorité pour avoir les confs
  }




  // All overrideable matrix functions
  virtual void OnElementWhoWantToJoin(HToolkitElement* element) override {
      this->loadedElements.push_back(element); // Registre
      loadedChannel->OnElementConnect(*element);  
      HToolkit::WelcomeElementOnMatrix(*element, *this);
      HToolkit::WelcomeElementOnChannel(*element, *this->loadedChannel);

  }

  virtual void OnChannelCreated(HToolkitChannel* channel) override{
      this->loadedChannel = channel;
      HTK_LOG("Created");
  }

  virtual void OnMatrixJoined()               override {this->initializator();}
  virtual void OnMatrixCreated()              override {this->constructor();}


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
MOD_MATRIX(HSTD, M_SingleConnection);
//=============================================================================


}; // namespace Matrixes
}; // namespace hstd
}; // namespace HypernetToolkit
//________________________________________