
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



/*

Donc ça c'est un channel de base, on peut fair un peu tout ce qu'on veut avec, mais il n'est pas "spécialisé"
Il traite les utilisateurs dans un registre.


*/





//=============================================================================
// MainStructure : SimpleDecentralized
//=============================================================================

struct C_BaseChannel : public HToolkitChannel {
    // General
    virtual void constructor(hArgs* args) override {
        // Definir la liste des composants attendus
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
        this->SetParameter("cutting_limit",         args->get<hString>("cutting_limit",         "1024"));       // (si 1024, un buffer de 4096 octets donnera 4 transmissions de 1024)
        this->SetParameter("segmentation_limit",    args->get<hString>("segmentation_limit",    "4"));          // Max transmission bounces
        this->SetParameter("initiator",             args->get<hString>("initiator",             "null"));    
        this->SetParameter("max_elements",          args->get<hString>("max_elements",          "1024"));      
        ///======================================================================================================================================================

        // Encoding method


        // Define portals

        // Add custom events
    
        // Construct portal(s)
        // Preparing the contruction of channels & elements
        this->OnCustomEvent("OnConstructed", args);
    };

    // Self
    virtual void init(hArgs* args) override {
        // Properties
        ///======================================================================================================================================================
        ///                 Property name                                       Value
        ///======================================================================================================================================================
        this->SetProp(HToolkitChannel_Props_Default_CreatedAt,          hTime::GetCurrentTimestamp());
        this->SetProp(HToolkitChannel_Props_Default_JoinedAt,           hTime::GetCurrentTimestamp());
        ///======================================================================================================================================================

        // Parameters
        // Note : La valeur par défaut peut être définie ici pour un channel pragmatique, sinon, mettre tout les champs attendus sous unknow
        ///======================================================================================================================================================
        ///                 Parameter Name                                  Tag               Default Value         Description
        ///======================================================================================================================================================
        this->SetParameter("cutting_limit",         args->get<hString>("cutting_limit",         "unknow"));       // (si 1024, un buffer de 4096 octets donnera 4 transmissions de 1024)
        this->SetParameter("segmentation_limit",    args->get<hString>("segmentation_limit",    "unknow"));          // Max transmission bounces
        this->SetParameter("max_elements",          args->get<hString>("max_elements",          "unknow"));       // Max transmission bounces
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

        // Si séquencé, transmettre :
        if(std::atoi(total_sequences.c_str()) > 0){ // 0 cut = 1 buffer, 1 cut = 2 buffers, 2 cut = 3 buffers, etc...
            // transmttre
            //this->SendTransmission()
            this->OnCustomEvent("OnTransmitted", args);
            return;
        }

        // Si encodé, décoder :*
        if(encoded == "true"){

        }

        // Si découpé, reformer :
        if(std::atoi(total_cuts.c_str()) == 0){ // 0 cut = 1 buffer, 1 cut = 2 buffers, 2 cut = 3 buffers, etc...
            // Créer un transmission directe

            // HToolkit::BufferToType(transmission.buffer, channel.type)
            this->OnCustomEvent("OnReceive", args);
            return;
        }
        else{
            // créer une transmission tracée (avec l'api)
            // Et implementer la première séquance du buffer (avec l'api)
        }

        // Si le découpage et le sequancage terminé, reconstituer le type

        if(total_cuts == current_cut){
            // hString final buffer
            // for(auto buffer_fragment : transmission.buffers){

                // final buffer += buffer_fragment

            //}
            // HToolkit::BufferToType(hString buffer, channel.type)


            this->OnCustomEvent("OnReceive", args);
            return;
        }

        // Traiter le type.
        
        // (Reconstitution de la communication a partir des configurations du channel)

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

// Register event for matrix, in a construction, on récupère les events et on les attache
CHANNEL_CUSTOM_EVENT(HSTD, OnReceive, C_BaseChannel, {

})
// Register event for matrix, in a construction, on récupère les events et on les attache
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

    // Récupérer l'element au près de la matrice.
    // Valider (bon type, bonnes configs...)
    // Envoyer la decision
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