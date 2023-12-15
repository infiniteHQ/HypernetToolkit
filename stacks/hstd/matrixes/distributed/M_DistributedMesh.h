
// Here is all basic logics of a distributed matrix

// On peut faire heriter de cette fonction a toutes autres matrixe voulant un foncitonnement précis et plus clair.


//===================================================================================
//  Distributed Matrix
//===================================================================================

//===================================================================================
// Logic Functions                   Description
//===================================================================================
// AddPeerToMatrix()
// RemovePeerFromMatrix()
// CreateChannelBetweenPeet()
// StartNewAuthority()
//===================================================================================

//===================================================================================
// Overrides                          Description
//===================================================================================
// AddPeerToMatrix()
// RemovePeerFromMatrix()
// CreateChannelBetweenPeet()
// StartNewAuthority()
//===================================================================================





// Logique pour Targeting distribué
// Logique pour la transmission distribuée
// Logique de la decouverte de matrice
// Logique de la resolution des pairs 

#include "../../../../vortex.h"
#include "../../../../vortex_internals.h"

//_____________________________________________________________________________
// (Core) Absolute core matrixes
//_____________________________________________________________________________
namespace HypernetToolkit {
namespace hstd {
namespace Matrixes {

//-----------------------------------------------------------------------------
// Matrix : decentralized_nodes (::decentralized_nodes in runtime)
//-----------------------------------------------------------------------------
struct DistributedSimple : public VortexMatrix {

  // Construction script
  void constructor() {
    this->SetProp(VortexMatrix_Props_Limits_MaxNodes, "1");
    this->SetProp(VortexMatrix_Props_Limits_MaxChannels, "1");
    this->SetProp(VortexMatrix_Props_Limits_MaxElements, "2");
  }

  void AddChannelType(VortexChannel* channel){
    this->SetChannel(channel);
  }





  // OnChannelJoined
  // OnChannelJoinedBySomeone

  // OnTransmissionReceived // Traiter avec la matrice et transmettre a d'autres pairs.
  // OnTransmissionSent


};
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
MOD_MATRIX(hstd, DistributedSimple);
//-----------------------------------------------------------------------------


}; // namespace Matrixes
}; // namespace hstd
}; // namespace HypernetToolkit
//________________________________________