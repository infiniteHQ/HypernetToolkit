
//=============================================================================
// Matrix : SimpleDecentralized (::hstd::SimpleDecentralized in runtime)
//=============================================================================

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



// Logique pour Targeting distribué
// Logique pour la transmission distribuée
// Logique de la decouverte de matrice
// Logique de la resolution des pairs 

#include "../../../../vortex.h"
#include "../../../../vortex_internals.h"
#include <chrono>
#include <thread>

namespace Vortex {
namespace hstd {
namespace Channels {

//=============================================================================
// MainStructure : SimpleDecentralized
//=============================================================================


// Faire une queue des transmission ?

enum TimeEvent_Params { 
        type   = 1,
        bounces   = 1,
};

enum TimeEvent_Functions { 
};


// To override channel + override function to adapt the final app ?
struct C_TimerEvents : public VortexChannel {

//=============================================================================
// Base & constructors
//=============================================================================
    virtual void constructor(VortexType* _type) override {
        this->type = _type;
        std::cout << "Constructed " << _type << std::endl;
        // this->emmitermode = _isEmmiter;
        // Configures type flow
        //this->type = &_type;
        // Configures limits & transmissions handlers
        // Add custom events (to call from outside)
        this->bounces = 2;
        this->OnCustomEvent("OnConstructed");
    }

    virtual void initializator(){};
//=============================================================================

    int i = 0;


//=============================================================================
// Input/Output
//=============================================================================
    virtual void receive(VxString _buffer)   override {
    }; // Interpreting with concording VortexType

    virtual void send()      override {

        VxString _buffer = this->type->output();
        VortexTransmission transmission;

        std::cout << "BUFF :" <<  _buffer.c_str() << std::endl;

        transmission.Configure(this->id, "1");
        transmission.AttachBuffer(_buffer);
        _buffer.clear();

        
        for(auto element : this->subscribers){
            transmission.target = &element.interface;

            this->SendTransmission(transmission);
            this->OnCustomEvent("OnSent");

            std::cout << this->i << std::endl;
            this->i++;
        }
    };
//=============================================================================


//=============================================================================
// Logic functions
//=============================================================================
    void StartEmmitter(){
            std::cout << "ggg" << std::endl;
        std::thread Thread([&]() {
            while(true){
            std::this_thread::sleep_for (std::chrono::milliseconds(this->bounces));
            this->send();
            }
        });

        this->receiveThread.swap(Thread);
        
    };

    void StopEmmitter(){
        this->receiveThread.join();
    }

    void AddSubscribers(){}; // detecte quand un element rejoint ce channel et l'affecte a la liste
//=============================================================================


//=============================================================================
// Overrideable events
//=============================================================================
    void OnElementConnect(VortexElement& element) override {
        // Afficher les InterfaceVariables pour le nouvel élément
        std::cout << "InterfaceVariables for the new element:" << std::endl;
        for (auto e : element.interface.InterfaceVariables) {
            std::cout << e.key.c_str() << std::endl;
        }
        this->subscribers.push_back(element);
    }

    virtual void OnTest() override {
            std::cout << "class succefully instanciaded" << std::endl;

    };

    virtual void OnChannelReceived(VortexTransmission& transmission) override {
            std::cout << "Traitement of " << transmission.buffer.c_str() << std::endl;


    };
//=============================================================================
    

    // Custom events to call from logics fonctions
    // Interfaces


    // Dedicated properties
    VxString emmiter;
    VxVector<VortexElement> subscribers;
    std::thread receiveThread;
    
    VortexType* type;

    bool emmitermode;
    int bounces; // Milliseconds (1000ms = 1second)

};
//=============================================================================



//=============================================================================
MOD_CHANNEL(HSTD, C_TimerEvents);
//=============================================================================


}; // namespace Matrixes
}; // namespace hstd
}; // namespace HypernetToolkit
//________________________________________