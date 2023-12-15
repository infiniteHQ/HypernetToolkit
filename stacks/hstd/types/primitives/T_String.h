
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
struct T_String : public HToolkitType{
    public: 
    
    // Constructors
    void consctructor(){
        // Construction du type et implementaiton des fonctionnalités


        // Configures type flow
        // Configures limits & transmissions handlers
        // Add custom events (to call from outside)
    };
    
    virtual void input(hString _input) override {   // Trouver la position de la virgule
        this->value = _input;
    };

    virtual hString output()  override {
        return this->value;
    };
    
    void set(hString _value){
        this->value = _value;
    }


    T_String get(bool newValue){
        return *this;
    }


    private:

    hString value;
};
//=============================================================================



//=============================================================================
ADD_TYPE_TO_FACTORY(HSTD, T_String);
//=============================================================================


}; // namespace Matrixes
}; // namespace hstd
}; // namespace HypernetToolkit
//________________________________________