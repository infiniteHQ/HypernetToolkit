// The HToolkit Project, version 1.1 ALPHA
// [internal structures & internal hypernet standard]

// See licence in LICSENCE.md
// (c) 2022-2023, Diego E. Moreno
// Concepts, theory & code made with passion by Diego E. Moreno for the world
// and all works to try to make it better.

//_____________________________________________________________________________
// Informations about this file
//_____________________________________________________________________________
// This file have all primitives types of vortex runtime & api.
// There is the low level part of the project.
// Please read introductions & informations on vortex.h before do anything.
//_____________________________________________________________________________

#pragma once
#ifndef VORTEX_DISABLE

//_____________________________________________________________________________
// [SECTION] Header mess
//_____________________________________________________________________________
// Includes
#include <random>

#ifndef VORTEX_VERSION
#include "htk.h"
#endif
//_____________________________________________________________________________


// Enable SSE intrinsics if available
#if (defined __SSE__ || defined __x86_64__ || defined _M_X64 || (defined(_M_IX86_FP) && (_M_IX86_FP >= 1))) && !defined(IMGUI_DISABLE_SSE)
#define IMGUI_ENABLE_SSE
#include <immintrin.h>
#endif

// Visual Studio warnings
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable: 4251)     // class 'xxx' needs to have dll-interface to be used by clients of struct 'xxx' // when IMGUI_API is set to__declspec(dllexport)
#pragma warning (disable: 26812)    // The enum type 'xxx' is unscoped. Prefer 'enum class' over 'enum' (Enum.3). [MSVC Static Analyzer)
#pragma warning (disable: 26495)    // [Static Analyzer] Variable 'XXX' is uninitialized. Always initialize a member variable (type.6).
#if defined(_MSC_VER) && _MSC_VER >= 1922 // MSVC 2019 16.2 or later
#pragma warning (disable: 5054)     // operator '|': deprecated between enumerations of different types
#endif
#endif

// Clang/GCC warnings with -Weverything
#if defined(__clang__)
#pragma clang diagnostic push
#if __has_warning("-Wunknown-warning-option")
#pragma clang diagnostic ignored "-Wunknown-warning-option"         // warning: unknown warning group 'xxx'
#endif
#pragma clang diagnostic ignored "-Wunknown-pragmas"                // warning: unknown warning group 'xxx'
#pragma clang diagnostic ignored "-Wfloat-equal"                    // warning: comparing floating point with == or != is unsafe // storing and comparing against same constants ok, for ImFloor()
#pragma clang diagnostic ignored "-Wunused-function"                // for stb_textedit.h
#pragma clang diagnostic ignored "-Wmissing-prototypes"             // for stb_textedit.h
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#pragma clang diagnostic ignored "-Wdouble-promotion"
#pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"  // warning: implicit conversion from 'xxx' to 'float' may lose precision
#pragma clang diagnostic ignored "-Wmissing-noreturn"               // warning: function 'xxx' could be declared with attribute 'noreturn'
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"              // warning: unknown option after '#pragma GCC diagnostic' kind
#pragma GCC diagnostic ignored "-Wclass-memaccess"      // [__GNUC__ >= 8] warning: 'memset/memcpy' clearing/writing an object of type 'xxxx' with no trivial copy-assignment; use assignment or value-initialization instead
#endif


//_____________________________________________________________________________
// [SECTION] Forward declarations
//_____________________________________________________________________________
// Absolute basis
struct HtkContext;
struct HtkIO;

// Position structures;
struct hVectorAdress;
struct HToolkitNotification;

//_____________________________________________________________________________

//_____________________________________________________________________________
// [SECTION] Basic types
//_____________________________________________________________________________
// Scalar data types
typedef const char *HToolkitChannelID;
typedef const char *HToolkitMatrixID;
typedef const char *HToolkitElementID;
//_____________________________________________________________________________

//_____________________________________________________________________________
// [SECTION] Enumerations
//_____________________________________________________________________________
//_____________________________________________________________________________

//_____________________________________________________________________________
// [SECTION] Flags
//_____________________________________________________________________________
//_____________________________________________________________________________

//_____________________________________________________________________________
// [SECTION] Memory, Containers
//_____________________________________________________________________________
//_____________________________________________________________________________

//_____________________________________________________________________________
// [SECTION] Main runtime pointer
// See implementation of this variable in imgui.cpp for comments and details.
//_____________________________________________________________________________
#ifndef CHToolkit
extern HTOOLKIT_API HtkContext *CHToolkit; // Current implicit context pointer
#endif
//_____________________________________________________________________________

//_____________________________________________________________________________
// [SECTION]: Macros
//_____________________________________________________________________________
// Static Asserts
#define HTK_STATIC_ASSERT(_COND) static_assert(_COND, "")
//_____________________________________________________________________________

//_____________________________________________________________________________
// [SECTION]: Generic helpers & utils
//_____________________________________________________________________________

#define HTK_STRING_TO_INT(_str, _h) str2int(_str, _h);
#define HTK_BASE64ENCODE(_toencodebuffer) base64_encode(_toencodebuffer);
#define HTK_BASE64DECODE(_todecodebuffer) base64_decode(_todecodebuffer);

//-----------------------------------------------------------------------------
// (Base structure) HToolkitMatrix
//-----------------------------------------------------------------------------
// A matrix is a point of matrix can interact with each others
//-----------------------------------------------------------------------------

struct hRandom {
  static hString GenerateUUIDv4();
  static hString Generate6CharID();
};
//-----------------------------------------------------------------------------

//_____________________________________________________________________________


//_____________________________________________________________________________
// [SECTION]: Internal structures
//_____________________________________________________________________________

//-----------------------------------------------------------------------------
// (Security) HToolkitSecurity
//-----------------------------------------------------------------------------
// All security tools for HSP & security utils
//-----------------------------------------------------------------------------
struct HToolkitSecurity {
public:
#ifdef USE_HSP
  static hString Encrypt(hString communication){return "null";};
  static hString Decrypt(hString communication){return "null";};

  static hString CreateSignature(hString communication){return "null";};
  static bool CompareSignature(){return "null";};

  static std::pair<EVP_PKEY *, EVP_PKEY *> GenerateKeys() {
    auto bne =
        BN_new(); // refer to https://www.openssl.org/docs/man1.0.2/man3/bn.html
    auto ret = BN_set_word(bne, RSA_F4);

    int bits = 2048;
    RSA *r = RSA_new();
    RSA_generate_key_ex(r, bits, bne, NULL); // here we generate the RSA keys

    // we use a memory BIO to store the keys
    BIO *bp_public = BIO_new(BIO_s_mem());
    PEM_write_bio_RSA_PUBKEY(bp_public, r);

    BIO *bp_private = BIO_new(BIO_s_mem());
    PEM_write_bio_RSAPrivateKey(bp_private, r, NULL, NULL, 0, NULL, NULL);

    auto pri_len = BIO_pending(bp_private); // once the data is written to a
                                            // memory/file BIO, we get the size
    auto pub_len = BIO_pending(bp_public);
    char *pri_key = (char *)malloc(pri_len + 1);
    char *pub_key = (char *)malloc(pub_len + 1);

    BIO_read(bp_private, pri_key, pri_len); // now we read the BIO into a buffer
    BIO_read(bp_public, pub_key, pub_len);

    pri_key[pri_len] = '\0';
    pub_key[pub_len] = '\0';

    // printf("\n%s\n:\n%s\n", pri_key, pub_key);fflush(stdout);  //now we print
    // the keys to stdout (DO NOT PRINT private key in production code, this has
    // to be a secret)

    BIO *pbkeybio = NULL;
    pbkeybio = BIO_new_mem_buf(
        (void *)pub_key,
        pub_len); // we create a buffer BIO
                  //(this is different from the memory BIO created earlier)
    BIO *prkeybio = NULL;
    prkeybio = BIO_new_mem_buf((void *)pri_key, pri_len);

    RSA *pb_rsa = NULL;
    RSA *p_rsa = NULL;

    pb_rsa = PEM_read_bio_RSAPublicKey(pbkeybio, &pb_rsa, NULL,
                                       NULL); // now we read the
                                              // BIO to get the RSA key
    p_rsa = PEM_read_bio_RSAPrivateKey(prkeybio, &p_rsa, NULL, NULL);

    EVP_PKEY *evp_pbkey =
        EVP_PKEY_new(); // we want EVP keys , openssl libraries
                        // work best with this type,
                        // https://wiki.openssl.org/index.php/EVP
    EVP_PKEY_assign_RSA(evp_pbkey, pb_rsa);

    EVP_PKEY *evp_prkey = EVP_PKEY_new();
    EVP_PKEY_assign_RSA(evp_prkey, p_rsa);

    // clean up
    free(pri_key);
    free(pub_key);
    BIO_free_all(bp_public);
    BIO_free_all(bp_private);
    BIO_free(pbkeybio);
    BIO_free(prkeybio);
    BN_free(bne);
    RSA_free(r);

    return {evp_pbkey, evp_prkey};
  }

private:
  static std::vector<unsigned char>
  envelope_open(EVP_PKEY *priv_key, unsigned char *ciphertext,
                int ciphertext_len, unsigned char *encrypted_key,
                int encrypted_key_len, unsigned char *iv) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;

    /* Create and initialise the context */
    ctx = EVP_CIPHER_CTX_new();

    /* Initialise the decryption operation. The asymmetric private key is
     * provided and priv_key, whilst the encrypted session key is held in
     * encrypted_key */
    EVP_OpenInit(ctx, EVP_aes_256_cbc(), encrypted_key, encrypted_key_len, iv,
                 priv_key);

    std::vector<unsigned char> plaintext(ciphertext_len);
    /* Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_OpenUpdate can be called multiple times if necessary
     */
    EVP_OpenUpdate(ctx, &plaintext[0], &len, ciphertext, ciphertext_len);
    plaintext_len = len;

    /* Finalise the decryption. Further plaintext bytes may be written at
     * this stage.
     */
    EVP_OpenFinal(ctx, &plaintext[0] + len, &len);
    plaintext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);
    plaintext.resize(plaintext_len);
    return plaintext;
  }

  static std::vector<unsigned char>
  envelope_seal(EVP_PKEY **pub_key, unsigned char *plaintext, int plaintext_len,
                unsigned char **encrypted_key, int *encrypted_key_len,
                unsigned char *iv) {
    EVP_CIPHER_CTX *ctx;
    int ciphertext_len;
    int len;

    /* Create and initialise the context */
    ctx = EVP_CIPHER_CTX_new();

    /* Initialise the envelope seal operation. This operation generates
     * a key for the provided cipher, and then encrypts that key a number
     * of times (one for each public key provided in the pub_key array). In
     * this example the array size is just one. This operation also
     * generates an IV and places it in iv. */
    EVP_SealInit(ctx, EVP_aes_256_cbc(), encrypted_key, encrypted_key_len, iv,
                 pub_key, 1);

    int blocksize = EVP_CIPHER_CTX_block_size(ctx);
    /* Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_SealUpdate can be called multiple times if necessary
     */
    std::vector<unsigned char> cyphered(
        plaintext_len + blocksize -
        1); // https://www.openssl.org/docs/man1.1.1/man3/EVP_EncryptInit.html
    // The amount of data written depends on the block alignment of the
    // encrypted data. For most ciphers and modes, the amount of data written
    // can be anything from zero bytes to (inl + cipher_block_size - 1) bytes.

    len = cyphered.size();
    EVP_SealUpdate(ctx, &cyphered[0], &len, plaintext, plaintext_len);
    ciphertext_len = len;

    /* Finalise the encryption. Further ciphertext bytes may be written at
     * this stage.
     */
    EVP_SealFinal(ctx, &cyphered[0] + len, &len);
    ciphertext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);
    cyphered.resize(ciphertext_len);
    return cyphered;
  }
  #endif // USE_HSP
};
//-----------------------------------------------------------------------------





//-----------------------------------------------------------------------------
// (Context registering) ReceivedNotifications & SentNotifications
//-----------------------------------------------------------------------------
// Here are registered add received and sent notifications for debug & see
// all behaviors and details.
//-----------------------------------------------------------------------------
#ifdef USE_REGISTERING
struct ReceivedNotifications {
  hVector<HToolkitNotification*> notifications;
  void Clear();
  void Add(HToolkitNotification& notification) {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    struct tm local_tm = *localtime(&now_c);
    int year = local_tm.tm_year + 1900;
    int month = local_tm.tm_mon + 1;   
    int day = local_tm.tm_mday;        
    int hour = local_tm.tm_hour;    
    int minute = local_tm.tm_min;     
    int second = local_tm.tm_sec;     

    hString dtimestamp;
    dtimestamp += year;
    dtimestamp += "-";
    dtimestamp += month;
    dtimestamp += "-";
    dtimestamp += day;
    dtimestamp += "-";
    dtimestamp += hour;
    dtimestamp += ":";
    dtimestamp += minute;
    dtimestamp += ":";
    dtimestamp += second;

    notification.timestamp = dtimestamp;
    this->notifications.push_back(&notification);
  };
};


struct SentNotifications {
  hVector<HToolkitNotification *> notifications;
  void Clear();
  void Add(HToolkitNotification& notification) {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    struct tm local_tm = *localtime(&now_c);

    int year = local_tm.tm_year + 1900; 
    int month = local_tm.tm_mon + 1;     
    int day = local_tm.tm_mday;      
    int hour = local_tm.tm_hour;      
    int minute = local_tm.tm_min;     
    int second = local_tm.tm_sec;      

    hString dtimestamp;
    dtimestamp += year;
    dtimestamp += "-";
    dtimestamp += month;
    dtimestamp += "-";
    dtimestamp += day;
    dtimestamp += "-";
    dtimestamp += hour;
    dtimestamp += ":";
    dtimestamp += minute;
    dtimestamp += ":";
    dtimestamp += second;

    notification.timestamp = dtimestamp;
    this->notifications.push_back(&notification);
  };
};

struct ReceivedTransmissions {
  hVector<HToolkitTransmission *> transmissions;
  void Clear();
  void Add(HToolkitTransmission& transmission) {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    struct tm local_tm = *localtime(&now_c);

    int year = local_tm.tm_year + 1900;
    int month = local_tm.tm_mon + 1;   
    int day = local_tm.tm_mday;     
    int hour = local_tm.tm_hour;        
    int minute = local_tm.tm_min;     
    int second = local_tm.tm_sec;   

    hString dtimestamp;
    dtimestamp += year;
    dtimestamp += "-";
    dtimestamp += month;
    dtimestamp += "-";
    dtimestamp += day;
    dtimestamp += "-";
    dtimestamp += hour;
    dtimestamp += ":";
    dtimestamp += minute;
    dtimestamp += ":";
    dtimestamp += second;

    transmission.timestamp = dtimestamp;
    this->transmissions.push_back(&transmission);
  };
};


struct SentTransmissions {
  hVector<HToolkitTransmission*> transmissions;
  void Clear();
  void Add(HToolkitTransmission& transmission) {

    auto now = std::chrono::system_clock::now();

    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    struct tm local_tm = *localtime(&now_c);

    int year = local_tm.tm_year + 1900; 
    int month = local_tm.tm_mon + 1;    
    int day = local_tm.tm_mday;         
    int hour = local_tm.tm_hour;      
    int minute = local_tm.tm_min;      
    int second = local_tm.tm_sec;       

    hString dtimestamp;
    dtimestamp += year;
    dtimestamp += "-";
    dtimestamp += month;
    dtimestamp += "-";
    dtimestamp += day;
    dtimestamp += "-";
    dtimestamp += hour;
    dtimestamp += ":";
    dtimestamp += minute;
    dtimestamp += ":";
    dtimestamp += second;

    transmission.timestamp = dtimestamp;
    this->transmissions.push_back(&transmission);
  };
};
#endif // USE_REGISTERING
//-----------------------------------------------------------------------------





//-----------------------------------------------------------------------------
// (Context) HToolkitIO
//-----------------------------------------------------------------------------
// The IO contain all active functions and utils link to input and output 
// interactions. This is a part of the context.
//-----------------------------------------------------------------------------
struct HtkIO {
  // Metrics
  int                               MetricsActiveAllocations; // Number of active allocations
  int                               MetricsCachedElements;
  int                               MetricsActiveProviders;
  int                               MetricsActiveChannels;
  int                               MetricsCachedChannels;
  int                               MetricsActiveMatrixes;
  int                               MetricsCachedMatrixes;
  int                               MetricsActiveTransmissions;
  int                               MetricsCachedTransmissions;
  int                               MetricsActiveNotification;
  int                               MetricsCachedNotification;
  int                               MetricsTracedNotification;
  int                               MetricsActiveTunnels;

  //-----------------------------------------------------------------------------
  // Cache Matrix functions
  //-----------------------------------------------------------------------------
  void                              AddCacheMatrix(HToolkitMatrix *matrix);
  HToolkitMatrix*                   GetCacheMatrix(hString& id);
  void                              DropCacheMatrix(hString &id);
  void                              SwapMatrixFromCacheToActive(hString &id);
  hVector<HToolkitMatrix *>         GetAllCachedMatrixes();
  //-----------------------------------------------------------------------------

  //-----------------------------------------------------------------------------
  // Active Matrix functions
  //-----------------------------------------------------------------------------
  void                              AddActiveMatrix(HToolkitMatrix* matrix);
  HToolkitMatrix*                   GetActiveMatrix(hString &id);
  void                              DropActiveMatrix(hString &id);
  void                              SwapMatrixFromActiveToCache(hString &id);
  hVector<HToolkitMatrix *>         GetAllActiveMatrixes();
  //-----------------------------------------------------------------------------

  //-----------------------------------------------------------------------------
  // Cache Channel functions
  //-----------------------------------------------------------------------------
  void                              AddCacheChannel(HToolkitChannel* channel);
  HToolkitChannel*                  GetCacheChannel(hString &id);
  void                              DropCacheChannel(hString &id);
  void                              SwapChannelFromCacheToActive(hString &id);
  hVector<HToolkitChannel *>        GetAllCachedChannels();
  //-----------------------------------------------------------------------------

  //-----------------------------------------------------------------------------
  // Active Channel functions
  //-----------------------------------------------------------------------------
  void                              AddActiveChannel(HToolkitChannel* channel);
  HToolkitChannel*                  GetActiveChannel(hString &id);
  void                              DropActiveChannel(hString &id);
  void                              SwapChannelFromActiveToCache(hString &id);
  hVector<HToolkitChannel *>        GetAllActiveChannels();
  //-----------------------------------------------------------------------------

  //-----------------------------------------------------------------------------
  // Traced notification functions
  //-----------------------------------------------------------------------------
  void                              AddTraceNotification(HToolkitNotification& notification);
  HToolkitNotification*             GetTraceNotification(hString &traceID);
  void                              DropTraceNotification(hString &traceID);
  hVector<HToolkitNotification *>   GetAllTracedNotifications();
  //-----------------------------------------------------------------------------

  //-----------------------------------------------------------------------------
  // I/O Runtime elements
  //-----------------------------------------------------------------------------
  void                              AttacheElementToIO(HToolkitElement* element);
  void                              AddActiveReceiver(HtkProviderInterface *interface);
  hVector<HtkProviderInterface *>   GetAllActiveReceivers();
  //-----------------------------------------------------------------------------

  // Hooks
  hVector<HToolkitCustomEvent*>     registeredHookForChannelsTypes;
  hMap<hString, HToolkitChannel*>   registeredDefaultChannelInitiators;
  
  hVector<HToolkitElement *>        activeRuntimeElements;
  hVector<hString>                  currentElementTypes;
  hVector<HToolkitProvider *>       activeProviders;

private:
  // Active instances
  hVector<HToolkitMatrix *>         activeMatrixes;
  hVector<HToolkitChannel *>        activeChannels; 

  // Cached instances
  hVector<HToolkitMatrix *>         cachedMatrixes; 
  hVector<HToolkitChannel *>        cachedChannels;

  // Cached items
  hVector<HToolkitNotification *>   tracedNotification; 

  // Actors
  hVector<HToolkitMatrix *>         knowMatrixes; 

  // Tunnels actifs
  hVector<HtkProviderInterface *>   activeReceivers;
  hVector<HtkProviderInterface *>   activeSenders;

  HtkContext                        *ctx;
};
//-----------------------------------------------------------------------------

struct HToolkitDebugAllocInfo {
  int                               TotalAllocCount; // Number of call to MemAlloc().
  int                               TotalFreeCount;
  HToolkitDebugAllocInfo()          { memset(this, 0, sizeof(*this)); }
};

//-----------------------------------------------------------------------------
// (Context) HToolkitContext => Main HToolkit context.
//-----------------------------------------------------------------------------
// This context contain all user data about HToolkit functionnal interfaces &
// all instances of custom contents.
//-----------------------------------------------------------------------------
struct HtkContext {
  bool                              initialized;
  bool                              need_encryption;
  hString                           adress;
  HToolkitDebugAllocInfo            debugAllocInfo;

  // Input/Output component
  HtkIO                             IO;

  // Register components
  hVector<HToolkitChannel *>        registeredImportedChannels;
  NotificationHandlerRegistry       registeredNotificationHandlers;
  MatrixPortalFactory*              registeredMatrixPortals;
  ChannelPortalFactory*             registeredChannelPortals;
  MatrixFactory*                    registeredMatrixes;
  ProviderFactory*                  registeredProviders;
  ElementFactory*                   registeredElements;
  TypeFactory*                      registeredTypes;
  ChannelFactory*                   registeredChannels;
  MatrixEventRegistry               registeredMatrixCustomEvents;
  ChannelEventRegistry              registeredChannelCustomEvents;

#ifdef USE_REGISTERING
  ReceivedNotifications             registeredNotificationReceived;
  SentNotifications                 registeredNotificationSent;
  ReceivedTransmissions             registeredTransmissionsReceived;
  SentTransmissions                 registeredTransmissionsSent;
#endif // USE_REGISTERING
};
//-----------------------------------------------------------------------------

//_____________________________________________________________________________



//__________________________________________________________________________________________________________________
// [SECTION]: Internal API of HToolkit
//__________________________________________________________________________________________________________________
// Internals functions of HypernetTK. Contain all basic stuff and low level
// features. All of theses functions are called by upper level api & user-api.
// Basicly, you don't need to care about this. For all user interactions of
// Hypernet & HTK, go on the main userapi on vortex.h
//__________________________________________________________________________________________________________________
namespace HToolkit {

// Utils & Base
HTOOLKIT_API void DebugAllocHook(HToolkitDebugAllocInfo *info, void *ptr,size_t size); // size >= 0 : alloc, size = -1 : free


//==================================================================================================================
// Internals self manipulations features (Context, internal logic, behaviors)
//==================================================================================================================
//==================================================================================================================


//==================================================================================================================
// Internals I/O manipulations features (Actions with I/O hypernet coms & self logics)
//==================================================================================================================
// Channel functions
HTOOLKIT_API bool                   AddChannelToContext(HToolkitChannel* channel); 
HTOOLKIT_API bool                   ChangeChannelState(HToolkitChannel* channel);
HTOOLKIT_API bool                   DeleteChannelFromContext(HToolkitChannel* channel);

// Elements functions
HTOOLKIT_API void                   InvokeRelativeElementFromInterface();
HTOOLKIT_API HToolkitElement*       InvokeElementFromInterface(HtkProviderInterface& interface, hString elementId);
HTOOLKIT_API HtkProviderInterface   InitProviderInterface(hString interfacebuffer);

// Matrixes functions
HTOOLKIT_API void                   SwapMatrix();
HTOOLKIT_API void                   UpdateMatrixProperties();

// Portal functions 
HTOOLKIT_API void                   GeneratePortalCode();
HTOOLKIT_API void                   GeneratePortalSession();


//==================================================================================================================

//==================================================================================================================
// Internals Hypernet features
//==================================================================================================================
//               Function                      |      Target Notification       |    Compatible with Hypernet V.   |
//==================================================================================================================
// Ping, Pong, Trace
HTOOLKIT_API void PingElement(HToolkitElement& e);   // ::ping<>                             // Hypernet V1
HTOOLKIT_API void PongElement(HToolkitElement& e);   // ::pong<>                             // Hypernet V1

// Boostrapping
HTOOLKIT_API void QuickJoinToMatrix();             // ::ask<JoinMatrix>                    // Hypernet V1
HTOOLKIT_API void AskToJoinChannel(                // ::ask<JoinChannel>                   // Hypernet V1
  HToolkitChannel* channel,
  HToolkitElement* elementToAsk, 
  HToolkitElement* askElement
  );              

  
HTOOLKIT_API void AskForMatrixUpdate(                 // ::ask<JoinMatrix>                    // Hypernet V1
  HToolkitMatrix* matrix);              

HTOOLKIT_API void AskToJoinMatrix(                 // ::ask<JoinMatrix>                    // Hypernet V1
  HToolkitMatrix* matrix);              

// Welcome
HTOOLKIT_API void WelcomeElementOnMatrix(          // ::welcome<OnMatrix>                  // Hypernet V1
  HToolkitElement& element,
  HToolkitMatrix& matrix,
  hString traceID);
HTOOLKIT_API void WelcomeElementOnChannel(         // ::welcome<OnChannel>                 // Hypernet V1
  HToolkitElement& element,
  HToolkitChannel& channel);

// Errors
HTOOLKIT_API void ReportErrorWhileJoinMatrix(      // ::error<JoinMatrix>                  // Hypernet V1
  HtkProviderInterface interface, 
  hString details);

HTOOLKIT_API void ReportNotImplementedError(       // ::error<JoinMatrix>                  // Hypernet V1
  HToolkitNotification& initialNotification);

HTOOLKIT_API void ReportErrorWhileJoinChannel();   // ::error<JoinChannel>                 // Hypernet V1
HTOOLKIT_API void ReportErrorWhileGetChannelList();// ::error<GetChannelList>              // Hypernet V1
HTOOLKIT_API void ReportErrorWhileGetElementList();// ::error<GetElementList>              // Hypernet V1
HTOOLKIT_API void ReportErrorWhileGetMatrixList(); // ::error<GetMatrixList>               // Hypernet V1

// Result
HTOOLKIT_API void ResultMatrixList();              // ::result<MatrixList>                 // Hypernet V1
HTOOLKIT_API void ResultElementList();             // ::result<ElementList>                // Hypernet V1
HTOOLKIT_API void ResultChannelList();             // ::result<ChannelList>                // Hypernet V1

// Update one element (add, remove, refresh)
HTOOLKIT_API void UpdateElement(hArgs* args);      // ::update<Element>                     // Hypernet V1

// Update All elements in one time
HTOOLKIT_API void UpdateElementsList(hArgs* args);      // ::update<Element>                     // Hypernet V1

// Update one channel (add, remove, refresh)
HTOOLKIT_API void UpdateChannel(hArgs* args);      // ::update<Channel>                     // Hypernet V1

// Update all channels in one time
HTOOLKIT_API void UpdateMatrixChannelsList(hArgs* args);  // ::update<ChannelsList>                // Hypernet V1

// Update all elements of a channel in one time
HTOOLKIT_API void UpdateChannelElementsList(hArgs* args);

HTOOLKIT_API void UpdateOwningMatrix();            // ::update<Matrix>                     // Hypernet V1

// Get
HTOOLKIT_API void GetMatrixList();                 // ::get<MatrixList>                    // Hypernet V1
HTOOLKIT_API void GetElementList(                  // ::get<MatrixElementList>             // Hypernet V1
  HToolkitMatrix& matrix);                
HTOOLKIT_API void GetElementList(                  // ::get<ChannelElementList>            // Hypernet V1
  HToolkitChannel& channel);                
HTOOLKIT_API void GetChannelList(                  // ::get<MatrixChannelList>             // Hypernet V1
  HToolkitMatrix& matrix);        

// Calibration
HTOOLKIT_API void AskToCalibrateMatrix();          // ::ask<CalibrateMatrix>               // Hypernet V1
HTOOLKIT_API void AskToCalibrateChannel();         // ::ask<CalibrateChannel>              // Hypernet V1

// Handshaking
HTOOLKIT_API void StartHandshakeToJoinMatrix();    // ::handshake<JoinMatrix>
HTOOLKIT_API void StartHandshakeToJoinChannel();   // ::handshake<JoinChannel>
//==================================================================================================================

}
//_____________________________________________________________________________

#endif // VORTEX_DISABLE