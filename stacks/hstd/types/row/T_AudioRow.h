
//=============================================================================
// Transmission Type : Boolean (::boolean in runtime)
//=============================================================================

//=============================================================================
// What is a transmission type ? : A "Type" is a model of data transmission
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
// Type of data : T_Vector
//=============================================================================
// Warning : This type is a mathematical vector with axis, not a array of data.
// If you need array of data go to : T_Array
//=============================================================================
struct T_Vector : public VortexType{
    // Constructors
    void consctructor(){
        // Construction du type et implementaiton des fonctionnalités


        // Configures type flow
        // Configures limits & transmissions handlers
        // Add custom events (to call from outside)
    };
    
    virtual void input() override {
    };

    virtual VxString output()  override {
        //return std::to_string(this->value);
    };
    
    void set(int newValue){
    }

    T_Vector get(bool newValue){
    }

    int major; // THIS,x
    int minus; // x,THIS
};
//=============================================================================



//=============================================================================
MOD_TYPE(HSTD, T_Vector);
//=============================================================================


}; // namespace Matrixes
}; // namespace hstd
}; // namespace HypernetToolkit
//________________________________________