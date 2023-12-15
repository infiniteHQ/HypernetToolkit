
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

#include "../../../../htk.h"
#include "../../../../htk_internals.h"
#include <chrono>
#include <thread>

namespace HToolkit {
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
struct C_TimerEvents : public HToolkitChannel {

//=============================================================================
// Base & constructors
//=============================================================================
    virtual void constructor(HToolkitType* _type) override {
        this->type = _type;
        // this->emmitermode = _isEmmiter;
        // Configures type flow
        //this->type = &_type;
        // Configures limits & transmissions handlers
        // Add custom events (to call from outside)
        this->bounces = 200;
        this->OnCustomEvent("OnConstructed");
    }

    virtual void initializator(){};
//=============================================================================

    int i = 0;


//=============================================================================
// Input/Output
//=============================================================================
    virtual void receive(hString _buffer)   override {
    }; // Interpreting with concording HToolkitType

    virtual void send()      override {

        hString _buffer = this->type->output();
        HToolkitTransmission transmission;

        transmission.Configure(this->id, "1");
        transmission.AttachBuffer(_buffer);
               //_buffer.clear();

        
        for(auto element : this->subscribers){
            transmission.target = &element.interface;

            this->SendTransmission(transmission);
            this->OnCustomEvent("OnSent");
            this->i++;
        }
    };
//=============================================================================


//=============================================================================
// Logic functions
//=============================================================================
    void StartEmmitter(int _bounces){
        this->bounces = _bounces;
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
    void OnElementConnect(HToolkitElement& element) override {
        this->subscribers.push_back(element);
    }

    virtual void OnTest() override {
            std::cout << "class succefully instanciaded" << std::endl;

    };

    virtual void OnChannelReceived(HToolkitTransmission& transmission) override {
            std::cout << "Traitement of " << transmission.buffer.c_str() << std::endl;
    };
//=============================================================================
    

    // Custom events to call from logics fonctions
    // Interfaces


    // Dedicated properties
    hString emmiter;
    hVector<HToolkitElement> subscribers;
    std::thread receiveThread;
    
    HToolkitType* type;

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