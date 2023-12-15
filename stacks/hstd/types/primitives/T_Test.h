
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


#include "./T_Boolean.h"
#include "./T_String.h"

namespace HToolkit {
namespace hstd {
namespace Types {

//=============================================================================
// Type of data : T_Boolean
//=============================================================================
struct T_Test : public HToolkitType{
    public: 
    
    // Constructors
    void consctructor(){
        // Construction du type et implementaiton des fonctionnalités


        // Configures type flow
        // Configures limits & transmissions handlers
        // Add custom events (to call from outside)
    };
    
    virtual void input(hString _input) override {   // Trouver la position de la virgule
    };

    virtual hString output()  override {
        hString _value;
        
        _value += "[bool:";
        _value += this->boolvalue.output();
        _value += "][string:";
        _value += this->stringvalue.output();
        _value += "]";


        return _value;
    };
    
    void set(bool test, hString string){
        this->boolvalue.set(test);
        this->stringvalue.set(string);
    }


    T_Test get(bool newValue){
        return *this;
    }


    private:
    T_Boolean boolvalue;
    T_String  stringvalue;
};
//=============================================================================



//=============================================================================
ADD_TYPE_TO_FACTORY(HSTD, T_Test);
//=============================================================================


}; // namespace Matrixes
}; // namespace hstd
}; // namespace HypernetToolkit
//________________________________________