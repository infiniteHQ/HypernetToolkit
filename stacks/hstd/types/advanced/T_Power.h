
//=============================================================================
// Transmission Type : Boolean (::boolean in runtime)
//=============================================================================

//=============================================================================
// What is a transmission type ? : This is a formalisation of a data 
//=============================================================================
// And about Boolean ? : This is a basic boolean type with all integrated logics
//=============================================================================



// Logique pour Targeting distribué
// Logique pour la transmission distribuée
// Logique de la decouverte de matrice
// Logique de la resolution des pairs 

#include "../../../../vortex.h"
#include "../../../../vortex_internals.h"
namespace Vortex {
namespace hstd {
namespace Types {

//=============================================================================
// Type of data : T_Boolean
//=============================================================================
struct T_Boolean : public VortexType{
    // Constructors
    void consctructor(){
        // Construction du type et implementaiton des fonctionnalités


        // Configures type flow
        // Configures limits & transmissions handlers
        // Add custom events (to call from outside)
    };
    
    virtual void input() override {
        if(this->buffer == "true")this->value = true;
        if(this->buffer == "false")this->value = false;
    };

    virtual VxString output()  override {
        if(this->value) return "true";
        if(!this->value) return "false";
        return "null";
    };
    
    void set(bool newValue){
        if(newValue)this->value = true;
        if(!newValue)this->value = false;
    }

    T_Boolean get(bool newValue){
        return *this;
    }

    // All boolean logic functions & components
    bool value = false; 
};
//=============================================================================



//=============================================================================
MOD_TYPE(HSTD, T_Boolean);
//=============================================================================


}; // namespace Matrixes
}; // namespace hstd
}; // namespace HypernetToolkit
//________________________________________