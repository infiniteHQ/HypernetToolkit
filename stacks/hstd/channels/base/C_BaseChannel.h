
// Logique pour Targeting distribué
// Logique pour la transmission distribuée
// Logique de la decouverte de matrice
// Logique de la resolution des pairs 

#include "../../../../htk.h"
#include "../../../../htk_internals.h"
#include <chrono>
#include <thread>

#include "../../../../stacks/hstd/types/primitives/T_String.h"

namespace HToolkit {
namespace hstd {
namespace Channels {

//=============================================================================
// MainStructure : SimpleDecentralized
//=============================================================================

struct C_BaseChannel : public HToolkitChannel {
    virtual void constructor(hArgs* args) override {
        // Attempted components

        hVector<hString>*           AttemptedElements = args->get<hVector<hString>*>("attempted_elements", nullptr);
        hVector<HtkChannelPortal>*  AttemptedPortals = args->get<hVector<HtkChannelPortal>*>("attempted_portals", nullptr); 
        hString                     AttemptedType = args->get<hString>("attempted_type", "null");
        // Properties
        ///======================================================================================================================================================
        ///                 Property name                                       Value
        ///======================================================================================================================================================
        this->SetProp(HToolkitChannel_Props_Default_CreatedAt,          hTime::GetCurrentTimestamp());
        this->SetProp(HToolkitChannel_Props_Default_JoinedAt,           hTime::GetCurrentTimestamp());
        ///======================================================================================================================================================

        // Parameters
        ///======================================================================================================================================================
        ///                 Parameter Name                                  Tag               Default Value         Description
        ///======================================================================================================================================================
        this->SetParameter("cutting_limit",         args->get<hString>("cutting_limit",         "1024")); 
        this->SetParameter("segmentation_limit",    args->get<hString>("segmentation_limit",    "4")); 
        this->SetParameter("initiator",             args->get<hString>("initiator",             "null"));    
        this->SetParameter("max_elements",          args->get<hString>("max_elements",          "1024"));      
        ///======================================================================================================================================================

        this->OnCustomEvent("OnConstructed", args);
    };

    virtual void init(hArgs* args) override {
        // Properties
        ///======================================================================================================================================================
        ///                 Property name                                       Value
        ///======================================================================================================================================================
        this->SetProp(HToolkitChannel_Props_Default_CreatedAt,          hTime::GetCurrentTimestamp());
        this->SetProp(HToolkitChannel_Props_Default_JoinedAt,           hTime::GetCurrentTimestamp());
        ///======================================================================================================================================================

        // Parameters
        ///======================================================================================================================================================
        ///                 Parameter Name                                  Tag               Default Value         Description
        ///======================================================================================================================================================
        this->SetParameter("cutting_limit",         args->get<hString>("cutting_limit",         "unknow"));      
        this->SetParameter("segmentation_limit",    args->get<hString>("segmentation_limit",    "unknow"));    
        this->SetParameter("max_elements",          args->get<hString>("max_elements",          "unknow"));  
        ///======================================================================================================================================================
    };


    virtual void start(hArgs* args) override {

    };


    virtual void stop(hArgs* args) override {

    };


    virtual void receive(hArgs* args)   override {
        HToolkitTransmission* transmission  = args->get<HToolkitTransmission*>("transmission", nullptr);

        hString current_cut = transmission->GetParameter("current_cut");
        hString total_cuts = transmission->GetParameter("total_cuts");
        hString encoded = transmission->GetParameter("encoded");
        hString total_sequences = transmission->GetParameter("total_sequences");
        hString current_sequence = transmission->GetParameter("current_sequence");

        if(std::atoi(total_sequences.c_str()) > 0){ 
            //this->SendTransmission()
            this->OnCustomEvent("OnTransmitted", args);
            return;
        }

        if(encoded == "true"){

        }

        if(std::atoi(total_cuts.c_str()) == 0){ 

            // HToolkit::BufferToType(transmission.buffer, channel.type)
            this->OnCustomEvent("OnReceive", args);
            return;
        }
        else{
        }


        if(total_cuts == current_cut){
            // hString final buffer
            // for(auto buffer_fragment : transmission.buffers){

                // final buffer += buffer_fragment

            //}
            // HToolkit::BufferToType(hString buffer, channel.type)


            this->OnCustomEvent("OnReceive", args);
            return;
        }
    };

    virtual void send(hArgs* args) override {
        HtkContext &ctx = *CHToolkit;
        HToolkitChannel* channel = this;
        HToolkitMatrix *matrix =    ctx.IO.GetActiveMatrix(channel->matrixID);

        for(auto element : this->loaded_elements){
            if(element->id.c_str() != matrix->loaded_self->id.c_str()){
            args->add("channel", channel);
            args->add("provider", &element->interface);
            this->SendTransmission(args);
            this->OnCustomEvent("OnSent", args);
            }
        }
    };

};

CHANNEL_CUSTOM_EVENT(HSTD, OnReceive, C_BaseChannel, {

})

CHANNEL_CUSTOM_EVENT(HSTD, OnAskToJoin, C_BaseChannel, {
    HToolkitChannel* channel    = args->get<HToolkitChannel*>("channel", nullptr);
    HToolkitMatrix* matrix      = args->get<HToolkitMatrix*>("matrix", nullptr);
    hString eid                 = args->get<hString>("eid", "null");

    for(auto element : matrix->loaded_elements){
        if(element->id == eid){
            // Register into matrix 
            channel->loaded_elements.push_back(element);
            HToolkit::WelcomeElementOnChannel(*element, *channel);
            hArgs update_args;
            update_args.add("cid", channel->GetId());
            update_args.add("element", element);
            HToolkit::UpdateChannelElementsList(&update_args);
        }
    }
})

CHANNEL_CUSTOM_EVENT(HSTD, OnNewElement, C_BaseChannel, {
        HtkContext &ctx = *CHToolkit;
        hString eid =               args->get<hString>("eid", "null");
        HToolkitChannel *channel =  args->get<HToolkitChannel *>("channel", nullptr);
        HToolkitMatrix *matrix =    ctx.IO.GetActiveMatrix(channel->matrixID);

        for(auto element : matrix->get_registered_element(args)){
            if(element->id == eid){
                channel->loaded_elements.push_back(element);
            }
        }
})


//=============================================================================
ADD_CHANNEL_TO_FACTORY(HSTD, C_BaseChannel);
//=============================================================================


}; // namespace Matrixes
}; // namespace hstd
}; // namespace HypernetToolkit
//________________________________________