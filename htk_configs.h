

#pragma once

//#define USE_HSP
#define USE_DEBUG
   #define USE_LOG




//_____________________________________________________________________________
// [General Config] Hypernet Security Protocol 
//_____________________________________________________________________________
// This macro unlock all encryption layers of API.
// /!\ WARNING : Never remove USE_HSP without any preocupations about security in third party providers /!\.
// (i) Please see https://doc.vortex.com/security/remove_api_sec
//_____________________________________________________________________________
//#define USE_HSP // HSP = Hypernet Security Protocol

// Note :   The API layer security (USE_HSP) is the Hypernet Security Protocol implementation, but you can add another security protocol (like TLS) hover it.
//          You can also diable the Hypernet Security protocol and add your own C/I/A method (like TLS) in a embedded system case for example.
//          WARNING : you can have dramatic C/I/A issues if you disable the Hypernet Security Protocol and if you don't apply another secure protocol.
//          Note Hypernet Security protocol concern only the transmissions between peer (for C/I/A secuity), Hypernet also provide another security
//          called "security by design". You can discover it in https://doc.vortex.com/security/design
//_____________________________________________________________________________




#define USE_QUEUED_NOTIFICATIONS
#define USE_QUEUED_TRANSMISSIONS

