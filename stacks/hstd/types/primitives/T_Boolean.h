
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

#include "../../../../htk.h"
#include "../../../../htk_internals.h"
namespace HToolkit {
namespace hstd {
namespace Types {

//=============================================================================
// Type of data : T_Boolean
//=============================================================================
struct T_Boolean : public HToolkitType{
    public: 

    // Constructors
    void consctructor(){
        // Construction du type et implementaiton des fonctionnalités


        // Configures type flow
        // Configures limits & transmissions handlers
        // Add custom events (to call from outside)
    };
    
    virtual void input(hString _input) override {
        if(_input == "true")this->value = true;
        if(_input == "false")this->value = false;
    };

    virtual hString output()  override {
        if(this->value) return "true";
        if(!this->value) return "false";
        return "null";
    };
    
    void set(bool _value){
        if(_value)this->value = true;
        if(!_value)this->value = false;
    }

    T_Boolean get(bool _value){
        return *this;
    }


    private:

    // All boolean logic functions & components
    bool value = false; 
};
//=============================================================================



//=============================================================================
ADD_TYPE_TO_FACTORY(HSTD, T_Boolean);
//=============================================================================


}; // namespace Matrixes
}; // namespace hstd
}; // namespace HypernetToolkit
//________________________________________