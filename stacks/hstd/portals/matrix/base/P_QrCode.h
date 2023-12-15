// SimpleDecentralized Matrix (::hstd::SimpleDecentralized in runtime)

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


//=============================================================================
// Caracteristics
//=============================================================================
// Type :             Decentralized 
// Targeting :        By Provider Interface
// Element linking :  By Register
// Has Authority :    Yes, the initiator and inherited elements
//=============================================================================



// Logique pour Targeting distribué
// Logique pour la transmission distribuée
// Logique de la decouverte de matrice
// Logique de la resolution des pairs 

#include "../../../../htk.h"
#include "../../../../htk_internals.h"

namespace HToolkit {
namespace hstd {
namespace Portals {

//=============================================================================
// MainStructure : SingleMatrix
//=============================================================================
struct P_PortalCode : public HtkMatrixPortal {
  // Base 
  virtual void constructor(hArgs* args) override {
    HToolkitMatrix* matrix = args->get<HToolkitMatrix*>("matrix", nullptr);
      hString code = "[id(";
      code += matrix->GetId();

      code += "),";

      // if(matrixType->tag != "null")
      //{
      code += "ty(";
      code += matrix->GetTag();

      code += "),";
      code += "pb(";

  #ifdef USE_HSP
      // Obtenez la clé sous forme de séquence d'octets
      unsigned char *key_bytes = NULL;
      int key_length = i2d_PrivateKey(ctx.keyPair.second, &key_bytes);

      // Convertissez les octets en hString
      std::string key_string(reinterpret_cast<char *>(key_bytes), key_length);
      std::string key_hex;
      for (int i = 0; i < key_length; ++i) {
        char hex_byte[3];
        snprintf(hex_byte, sizeof(hex_byte), "%02X", key_bytes[i]);
        key_hex += hex_byte;
      }

      hString vx_key_string(key_string.c_str());

      code += key_hex.c_str();
#endif // USE_HSP

      code += "),";
      code += "r(";

      bool first_r_pass = false;
      int seq = 0;

      for (auto receiver : matrix->GetReceivers()) {
        seq++;
        if (first_r_pass) {
          code += ",";
        }
        code += "r";
        code += static_cast<char>(seq);
        code += "[";

        bool firstdf_pass = false;

        for (auto params : receiver.InterfaceVariables) {
          if (firstdf_pass) {
            code += ",";
          }
          code += params.key; // key
          code += ":";
          hString value = receiver.GetInterfaceVariable(params.key);
          code += value;

          firstdf_pass = true;
        }

        code += "]";
      }

      code += ")]";
      value = code;
  }

};
//=============================================================================

//=============================================================================
MOD_MATRIXPORTAL(HSTD, P_PortalCode)
//=============================================================================

}; // namespace HToolkit
}; // namespace hstd
}; // namespace Matrixes
//________________________________________
