
#include "../../../../htk.h"
#include "../../../../htk_internals.h"


namespace HToolkit {
namespace hstd {
namespace Elements {

//=============================================================================
// Element : E_Base
//=============================================================================

struct E_Base : public HToolkitElement {
    virtual void bootstrapp(hArgs* args) override {
        HTK_LOG("Add to runtime");
    }

    // Matrixes can construct elements with this function
    virtual void constructor(hArgs* args) override {
        HTK_LOG("Construct element");
        this->id = hRandom::Generate6CharID();
    }

    virtual void init(hArgs* args) override{ 
    }

    hString adress;
};

//=============================================================================
ADD_ELEMENT_TO_FACTORY(HSTD, E_Base);
//=============================================================================


}; // namespace HToolkit
}; // namespace hstd
}; // namespace Elements
//________________________________________