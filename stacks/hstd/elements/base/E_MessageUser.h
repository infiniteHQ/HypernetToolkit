
//=============================================================================
// Matrix : SimpleDecentralized (::hstd::SimpleDecentralized in runtime)
//=============================================================================

//=============================================================================
// What is a matrix ? : A matrix is a environment, a context for all elements,
// actors, channels & components of a Hypernet based network. It provide all
// informations, rules, security system needed 
//=============================================================================
// And about Simple Decentralized matrix ? : This matrix is made around 
// decentralized concept of Hypernet, it provide many basic features to
// manage the matrice, have autorities, resilience and the best security as
// possible.
//=============================================================================



// Logique pour Targeting distribué
// Logique pour la transmission distribuée
// Logique de la decouverte de matrice
// Logique de la resolution des pairs 

#include "../../../htk.h"
#include "../../../htk_internals.h"


namespace HToolkit {
namespace hstd {
namespace Elements {

//=============================================================================
// MainStructure : SimpleDecentralized
//=============================================================================


/*


Faire une fonction de contexte : AttachElementTypeToRuntime()
Cela permet d'ajouter un type possible d'elements, ce qui permet de faire hériter au runtime
des fonctions d'elements.

Pour du polytype d'elements, a voir comment mettre cela en place.

A noter: c'est l'element qui définit l'adresse, pas le runtime !
Une matrice peut assigner notre adresse d'element, sinon, nous pouvons
s'auto adresser.


*/

// To override channel + override function to adapt the final app ?
struct E_MessageUser : public HToolkitElement {
    void constructor(hString _username){
        this->id = hRandom::Generate6CharID();
        this->username = _username;
        this->joinedAt = hTime::GetCurrentTimestamp();
    }


    void initialize(hString _id, hString _username, hString _timestamp){
        this->id = _id;
        this->username = _username;
        this->joinedAt = _timestamp;
    }


    void Update(){        
        HTK_LOG("Registering a new element of type : ?")
        HToolkitNotification updatenotification;        
        HTK_LOG("Registering a new element of type : ?")

        updatenotification.Configure("::update", "element", "1", this->interface, false);
        HTK_LOG("Registering a new element of type : ?")
        updatenotification.AddParameter("c_username", this->username);        
        HTK_LOG("Registering a new element of type : ?")
        updatenotification.AddParameter("c_id",  this->id);        
        HTK_LOG("Registering a new element of type : ?")
        updatenotification.AddParameter("c_timestamp",  this->joinedAt);
        HTK_LOG("Registering a new d of type : ?")
        HToolkit::SendNotification(updatenotification);
    }


    virtual void OnUpdate(HToolkitNotification notification) override {
        this->initialize(notification.GetParameter("c_id"), notification.GetParameter("c_username"), notification.GetParameter("c_timestamp"));
    }


    hString id;
    hString username;
    hString joinedAt; // Pourquoi pas un type ?
};

//=============================================================================
MOD_ELEMENT(HSTD, E_MessageUser);
//=============================================================================


}; // namespace Matrixes
}; // namespace hstd
}; // namespace HypernetToolkit
//________________________________________