
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

static void convertFloatToInt(float nombre, int &partieEntiere, int &partieDecimale) {
    partieEntiere = static_cast<int>(nombre);
    partieDecimale = static_cast<int>((nombre - partieEntiere) * 100);
}

//=============================================================================
// Type of data : T_Boolean
//=============================================================================
struct T_Float : public HToolkitType{
    public: 
    
    // Constructors
    void consctructor(){
        // Construction du type et implementaiton des fonctionnalités


        // Configures type flow
        // Configures limits & transmissions handlers
        // Add custom events (to call from outside)
    };
    
    virtual void input(VxString _input) override {   // Trouver la position de la virgule
    size_t pos = _input.find(",");

    // Extraire le major et le minor en convertissant les sous-chaînes en entiers
    if (pos != VxString::npos && pos > 0 && pos < _input.length() - 1) {
        VxString majorStr = _input.substr(0, pos);
        VxString minorStr = _input.substr(pos + 1);

        this->major = std::stoi(majorStr.c_str());
        this->minus = std::stoi(minorStr.c_str());

        // Afficher les résultats
        std::cout << "Major: " << this->major << std::endl;
        std::cout << "Minor: " << this->minus << std::endl;
    } else {
        std::cerr << "Format de chaîne incorrect." << std::endl;
    }

    };

    virtual VxString output()  override {
        VxString value;
        value += this->major;
        value += ",";
        value += this->minus;
        return value;
    };
    
    void set(float value){
        int _major, _minus;
        convertFloatToInt(value, _major, _minus);
        this->major = _major;
        this->minus = _minus;
    }

    void set(int _major, int _minus){
        this->major = _major;
        this->minus = _minus;
    }

    T_Float get(bool newValue){
        return *this;
    }


    private:

    // All boolean logic functions & components
    int major;
    int minus;
};
//=============================================================================



//=============================================================================
ADD_TYPE_TO_FACTORY(HSTD, T_Float);
//=============================================================================


}; // namespace Matrixes
}; // namespace hstd
}; // namespace HypernetToolkit
//________________________________________