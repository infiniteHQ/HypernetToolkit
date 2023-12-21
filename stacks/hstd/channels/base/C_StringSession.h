#include "../../../../htk.h"
#include "../../../../htk_internals.h"

//=============================================================================
// MainStructure : SimpleDecentralized
//=============================================================================

struct C_StringSession : public HToolkitChannel {
    virtual void constructor(hArgs* args)   override {};
    virtual void init(hArgs* args)          override {};
    virtual void start(hArgs* args)         override {};
    virtual void stop(hArgs* args)          override {};


    virtual void receive(hArgs* args)       override {};

    virtual void send(hArgs* args) override {};

};


//=============================================================================
ADD_CHANNEL_TO_FACTORY(HSTD, C_StringSession);
//=============================================================================

//________________________________________