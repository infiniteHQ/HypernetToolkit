
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
struct T_Integer : public HToolkitType{
    public: 
    
    // Constructors
    void consctructor(){
        // Construction du type et implementaiton des fonctionnalités


        // Configures type flow
        // Configures limits & transmissions handlers
        // Add custom events (to call from outside)
    };
    
    virtual void input(hString _input) override {   // Trouver la position de la virgule
        this->value = std::stoi(_input.c_str());
    };

    virtual hString output()  override {
        hString _value;
        _value += this->value;
        return _value;
    };
    
    void set(int _value){
        this->value = _value;
    }


    T_Integer get(bool newValue){
        return *this;
    }


    private:

    int value;
};
//=============================================================================



//=============================================================================
ADD_TYPE_TO_FACTORY(HSTD, T_Integer);
//=============================================================================


}; // namespace Matrixes
}; // namespace hstd
}; // namespace HypernetToolkit
//________________________________________