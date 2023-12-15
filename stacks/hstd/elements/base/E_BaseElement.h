
#include "../../../../htk.h"
#include "../../../../htk_internals.h"


namespace HToolkit {
namespace hstd {
namespace Elements {

//=============================================================================
// Element : E_BaseElement
//=============================================================================

struct E_BaseElement : public HToolkitElement {
    virtual void bootstrapp(hArgs* args) override {
        HTK_LOG("Add to runtime");
    }

    virtual void constructor(hArgs* args) override {
        HTK_LOG("Construct element");
        this->id = hRandom::Generate6CharID();
    }

    virtual void init(hArgs* args) override{ 
    }

    hString adress;
};

//=============================================================================
ADD_ELEMENT_TO_FACTORY(HSTD, E_BaseElement);
//=============================================================================


}; // namespace HToolkit
}; // namespace hstd
}; // namespace Elements
//________________________________________