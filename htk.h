// The Hypernet Project, version 1.2 ALPHA
// [header files]

// See licence in LICSENCE.md
// (c) 2022-2023, Diego E. Moreno
// Concepts, theory & code made with passion by Diego E. Moreno for the world and all works to try to make it better.


//_____________________________________________________________________________
// Introduction
//_____________________________________________________________________________
//_____________________________________________________________________________




//_____________________________________________________________________________
// Project map
//_____________________________________________________________________________
//_____________________________________________________________________________




//_____________________________________________________________________________
// Ressources
//_____________________________________________________________________________
//_____________________________________________________________________________




//_____________________________________________________________________________
// Versions & Build identification
//_____________________________________________________________________________
// (Integer encoded as XYYZZ for use in #if preprocessor conditionals, e.g. '#if VORTEX_VERSION_NUM >= 12345')
#define HTOOLKIT_VERSION       "1.2 ALPHA"
#define HTOOLKIT_VERSION_NUM   10200

// Hypernet Transmission protocol compatibility (with retro compatibility !)
#define HCP_VER             "1.0"
// Hypernet Security protocol compatibility (with retro compatibility !)
#define HSP_VER             "1.0"
//_____________________________________________________________________________




//_____________________________________________________________________________
// Index of this file
//_____________________________________________________________________________
// [SECTION] Header mess
// [SECTION] Forward declarations of structures
// [SECTION] HToolkit end-use API functions
// [SECTION] Flags & Enumerations
// [SECTION] Structures
//_____________________________________________________________________________


//_____________________________________________________________________________
// Configuration
//_____________________________________________________________________________
#pragma once
// Configuration file with compile-time options
// (edit htkconfigs.h or '#define VORTEX_USER_CONFIG "myfilename.h" from your build system')
#ifdef VORTEX_USER_CONFIG
#include VORTEX_USER_CONFIG
#endif
#include "htk_configs.h"

#ifndef VORTEX_DISABLE
//_____________________________________________________________________________



//_____________________________________________________________________________
// [SECTION] Header mess
//_____________________________________________________________________________
// Includes
#include <stddef.h>                 // NULL
#include <stdio.h>                  // NULL
#include <stdexcept>                // NULL
#include <cstring>                  // strchr, strncpy

#ifdef USE_REGISTERING
#include <chrono>
#endif // USE_REGISTERING

#ifdef USE_HSP
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#endif


#ifndef HTOOLKIT_API
#define HTOOLKIT_API
#endif
#ifndef VORTEX_EXT_API
#define VORTEX_EXT_API              HTOOLKIT_API
#endif


// Helper Macros
#ifndef HTK_ASSERT
#include <assert.h>
#define HTK_ASSERT(_EXPR)            assert(_EXPR)                               // You can override the default assert handler by editing vortex_configs.h
#endif
#define HTK_ARRAYSIZE(_ARR)          ((int)(sizeof(_ARR) / sizeof(*(_ARR))))     // Size of a static C-style array. Don't use on pointers!
#define HTK_UNUSED(_VAR)             ((void)(_VAR))                              // Used to silence "unused variable warnings". Often useful as asserts may be stripped out from final builds.
#define HTK_OFFSETOF(_TYPE,_MEMBER)  offsetof(_TYPE, _MEMBER)                    // Offset of _MEMBER within _TYPE. Standardized as offsetof() in C++11
#define HTK_CHECKVERSION()           HToolkit::DebugCheckVersionAndDataLayout(HTOOLKIT_VERSION) // Version


// Disable some of MSVC most aggressive Debug runtime checks in function header/footer (used in some simple/low-level functions)
#if defined(_MSC_VER) && !defined(__clang__)  && !defined(__INTEL_COMPILER) && !defined(IMGUI_DEBUG_PARANOID)
#define IM_MSVC_RUNTIME_CHECKS_OFF      __pragma(runtime_checks("",off))     __pragma(check_stack(off)) __pragma(strict_gs_check(push,off))
#define IM_MSVC_RUNTIME_CHECKS_RESTORE  __pragma(runtime_checks("",restore)) __pragma(check_stack())    __pragma(strict_gs_check(pop))
#else
#define IM_MSVC_RUNTIME_CHECKS_OFF
#define IM_MSVC_RUNTIME_CHECKS_RESTORE
#endif

// Warnings
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable: 26495)    // [Static Analyzer] Variable 'XXX' is uninitialized. Always initialize a member variable (type.6).
#endif
#if defined(__clang__)
#pragma clang diagnostic push
#if __has_warning("-Wunknown-warning-option")
#pragma clang diagnostic ignored "-Wunknown-warning-option"         // warning: unknown warning group 'xxx'
#endif
#pragma clang diagnostic ignored "-Wunknown-pragmas"                // warning: unknown warning group 'xxx'
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wfloat-equal"                    // warning: comparing floating point with == or != is unsafe
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#pragma clang diagnostic ignored "-Wreserved-identifier"            // warning: identifier '_Xxx' is reserved because it starts with '_' followed by a capital letter
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"          // warning: unknown option after '#pragma GCC diagnostic' kind
#pragma GCC diagnostic ignored "-Wclass-memaccess"  // [__GNUC__ >= 8] warning: 'memset/memcpy' clearing/writing an object of type 'xxxx' with no trivial copy-assignment; use assignment or value-initialization instead
#endif

#ifndef USE_LOG
#define HTK_LOG(_LOG)
#endif

#ifdef USE_LOG
#include <iostream>
#define HTK_LOG(_LOG) std::cout << _LOG << std::endl;
#endif

//_____________________________________________________________________________


//_____________________________________________________________________________
// [SECTION] Forward declarations of structures
//_____________________________________________________________________________
template<typename T>
struct hVector;

template <typename K, typename V>
struct hMap;

struct hString;
struct hArgs;

// Main structures
struct HToolkitRuntime;

// Components skeletons
struct HToolkitProvider;
struct HToolkitChannel;
struct HToolkitElement;
struct HToolkitTransmission;
struct HToolkitNotification;
struct HToolkitMatrix;
struct HToolkitNode;
struct HToolkitType;

// Components stuffs
struct HToolkitNotificationHandler;
struct HToolkitCustomEvent;

// Matrix stuffs
struct HtkMatrixPortal;

// Mecanics skeletons
struct HToolkitRuntimeHook;

// Provider structures
struct HtkProviderInterface;

// Contents skeletons
struct HToolkitService;

// Internals (from vortex_internals.h)
struct HtkContext;
//_____________________________________________________________________________


typedef int HToolkitChannel_Props;
typedef int HToolkitMatrix_Props;


// Callback and functions types
typedef void*   (*HToolkitMemAllocFunc)(size_t sz, void* user_data);               // Function signature for HToolkit::SetAllocatorFunctions()
typedef void    (*HToolkitMemFreeFunc)(void* ptr, void* user_data);                // Function signature for HToolkit::SetAllocatorFunctions()



//_____________________________________________________________________________
// [SECTION] HToolkit end-use API functions
//_____________________________________________________________________________
// Note for all HToolkit internals modifications ! : Please considere 
// (Note that HToolkit:: being a namespace, you can add extra HToolkit:: functions 
// in your own separate file. Please don't modify HToolkit source files!)
//_____________________________________________________________________________
namespace HToolkit
{
    // Runtime creation & manipulations
    HTOOLKIT_API HtkContext*       CreateContext();
    HTOOLKIT_API void             DestroyContext(HtkContext* context = NULL);
    HTOOLKIT_API HtkContext*       GetCurrentContext();
    HTOOLKIT_API void             SetCurrentContext(HtkContext* context);
    HTOOLKIT_API void             AddReceiver(HtkProviderInterface* interface);


    // Main & Basics
    HTOOLKIT_API void             Initialize();
    HTOOLKIT_API void             GetRegisteredContents(); 

    // Channels
    HTOOLKIT_API void             NewChannel(HToolkitChannel* c);

    // Matrix
    HTOOLKIT_API void             NewMatrix(HToolkitChannel* c);
    HTOOLKIT_API void             AttachHookToChannelType(HToolkitCustomEvent* event, hString eventName, hString channelType);

    HTOOLKIT_API void HandleMatrixPortal(hArgs* args);
    HTOOLKIT_API void HandleChannelPortal(hArgs* args);

    HTOOLKIT_API hString GetCurrentMatrixID(hString contextElementType, HToolkitMatrix* matrix);
    
    // Providers functions
    HTOOLKIT_API void             StartProviders();
    HTOOLKIT_API HToolkitProvider*  FindProvidersByProtocol(hString protocol);


    // Minimal Standard contents

    // Matrixes functions
    template<typename MatrixType>   
    HTOOLKIT_API MatrixType*        CreateMatrix(hString matrixtype);
    HTOOLKIT_API HToolkitChannel* CreateChannelToInit(hString channeltype, hArgs* args);
    HTOOLKIT_API HToolkitMatrix*    CreateMatrixToInit(hString matrixtype, hArgs* args);
    HTOOLKIT_API HToolkitMatrix*    CreateMatrix(hString matrixtype, hString elementtype, hArgs* args);
    HTOOLKIT_API HToolkitMatrix*    InitMatrix(hString elementtype, hArgs* args); 

    HTOOLKIT_API HToolkitMatrix&    CreateStandardMatrix();
    HTOOLKIT_API void             DestroyMatrix();
    HTOOLKIT_API void             RegisterMatrix(HtkMatrixPortal portal);

    HTOOLKIT_API HToolkitElement*  CreateElementType(hString elementtype, hArgs* args);


    HTOOLKIT_API hVector<hString> InputStringVector(hString& string);

    // Matrices portal functions
    HTOOLKIT_API hString         GetPortal(hString mid);

    // General input data handling
    HTOOLKIT_API void             HandleReceiveData(const char* Data, HtkProviderInterface& interface);

    // Transmissions functions
    HTOOLKIT_API void             HandleTransmission(HToolkitTransmission& notification);

    // Notifications functions
    HTOOLKIT_API void             AwaitResponse(HToolkitNotification* notification);
    HTOOLKIT_API void             HandleNotification(HToolkitNotification& notification); 
    HTOOLKIT_API void             HandleV1Notification(HToolkitNotification notification); 
    HTOOLKIT_API void             SendNotification(HToolkitNotification& notification);

    // Contents functions
    HTOOLKIT_API void             RegisterMatrix(HToolkitMatrix* matrix);
    HTOOLKIT_API void             RegisterNotificationHandler(HToolkitNotificationHandler* handler);
    #define ADD_NOTIFICATION_HANDLER(handler) RegisterNotificationHandler(handler)
    

    template <typename ReturnType, typename... Parameters>
    HTOOLKIT_API void             RegisterTransmissionHandler(const char* name, ReturnType(*function)(Parameters...));

    template <typename ReturnType, typename... Parameters>
    HTOOLKIT_API void             RegisterChannelType(const char* name, ReturnType(*function)(Parameters...));

    template <typename ReturnType, typename... Parameters>
    HTOOLKIT_API void             RegisterElementType(const char* name, ReturnType(*function)(Parameters...));

    template <typename ReturnType, typename... Parameters>
    HTOOLKIT_API void             RegisterContent(const char* name, ReturnType(*function)(Parameters...));
    
    HTOOLKIT_API hMap<hString, HToolkitProvider*> GetProviders();
    HTOOLKIT_API void             RegisterProvider(HToolkitProvider* providers_ptr);
    #define                     ADD_PROVIDER(provider) HToolkit::RegisterProvider(provider)

    bool DebugCheckVersionAndDataLayout(const char* version);


    // Memory Allocators
    // - Those functions are not reliant on the current context.
    // - DLL users: heaps and globals are not shared across DLL boundaries! You will need to call SetCurrentContext() + SetAllocatorFunctions()
    //   for each static/DLL boundary you are calling from. Read "Context and Memory Allocators" section of HToolkit.cpp for more details.
    HTOOLKIT_API void          SetAllocatorFunctions(HToolkitMemAllocFunc alloc_func, HToolkitMemFreeFunc free_func, void* user_data = NULL);
    HTOOLKIT_API void          GetAllocatorFunctions(HToolkitMemAllocFunc* p_alloc_func, HToolkitMemFreeFunc* p_free_func, void** p_user_data);
    HTOOLKIT_API void*         MemAlloc(size_t size);
    HTOOLKIT_API void          MemFree(void* ptr);
    
} // namespace HToolkit
//_____________________________________________________________________________




//_____________________________________________________________________________
// [SECTION] Flags & Enumerations
//_____________________________________________________________________________


//=============================================================================
// HToolkitTransmission enums
//=============================================================================
enum HToolkitTransmission_Props_ {
    HToolkitTransmission_   = 1,
};
//=============================================================================

enum HToolkitMatrix_Errors_ {
    FFFFF                  = 0,   
};

//=============================================================================
// HToolkitChannel enums
//=============================================================================
enum HToolkitChannel_Roles_ {
    HToolkitChannel_Roles_Emmiter                     = 1, 
    HToolkitChannel_Roles_Receiver                    = 1,
    HToolkitChannel_Roles_Transmitter                 = 1,
    HToolkitChannel_Roles_Subscriber                  = 1, 
    HToolkitChannel_Roles_Broker                      = 1,
};

enum HToolkitChannel_Events_ { // Last states
    HToolkitChannel_Event_Main_Connected              = 1,
    HToolkitChannel_Event_Main_Disconnected           = 1,
    HToolkitChannel_Event_Element_ElementConnected    = 1,
    HToolkitChannel_Event_Element_ElementDisconnected = 1,
    HToolkitChannel_Event_Coms_SomethingReceived      = 1,
    HToolkitChannel_Event_Coms_SomethingSent          = 1,
    HToolkitChannel_Event_Coms_SomethingTransmitted   = 1,
};

enum HToolkitChannel_Props_ {
    HToolkitChannel_Props_Default_Tag                 = 1,
    HToolkitChannel_Props_Default_UseAuthority        = 2,
    HToolkitChannel_Props_Default_UseEncryption       = 3,
    HToolkitChannel_Props_Default_UseSignature        = 4,
    HToolkitChannel_Props_Default_CreatedAt           = 5,
    HToolkitChannel_Props_Default_JoinedAt            = 6,
    HToolkitChannel_Props_Default_LastUpdated         = 7,
    HToolkitChannel_Props_Limits_MaxElements          = 8,
    HToolkitChannel_Props_Perms_AllowBounces          = 9,
    HToolkitChannel_Props_Packets_DataType            = 10, 
    HToolkitChannel_Props_Packets_MaxBufferLenght     = 11,
    HToolkitChannel_Props_Packets_Mode                = 12,
    HToolkitChannel_Props_Packets_Encoding            = 13,
    HToolkitChannel_Props_Encyption_KeyExchange       = 300,
    HToolkitChannel_Props_Encyption_Signature         = 301,
    HToolkitChannel_Props_Encyption_Encryption        = 302,
};

enum HToolkitChannel_State_{
    HToolkitChannel_State_Sleeping                    = 0,
    HToolkitChannel_State_Active                      = 1,
    HToolkitChannel_State_Deleted                     = 2,
};

enum HToolkitChannelEncryption_{
    HToolkitChannelEncryption_SHA256                  = 0,
    HToolkitChannelEncryption_AES                     = 1,        
    HToolkitChannelEncryption_AUTRE                   = 2,        
};
//=============================================================================

//=============================================================================
// HToolkitMatrix enums 
//=============================================================================
enum HToolkitMatrix_Events_ {
    HToolkitMatrix_Event_Matrix_Created               = 1,
    HToolkitMatrix_Event_Matrix_StateChanged          = 1,
    HToolkitMatrix_Event_Matrix_Deleted               = 1,
    HToolkitMatrix_Event_Security_NewPK               = 1,
    HToolkitMatrix_Event_Elements_ElementJoined       = 1,
    HToolkitMatrix_Event_Elements_ElementLeaved       = 1,
};

enum HToolkitMatrix_Props_ {
    HToolkitMatrix_Props_Limits_MaxElements           = 1,
    HToolkitMatrix_Props_Limits_MaxChannels           = 2,
    HToolkitMatrix_Props_Limits_MaxNodes              = 3,
    HToolkitMatrix_Props_MyLimits_MaxElements         = 5,
    HToolkitMatrix_Props_MyLimits_MaxChannels         = 6,
    HToolkitMatrix_Props_MyLimits_MaxNodes            = 7,
    HToolkitMatrix_Props_Encryption_AuthorityPK       = 4,
};

enum HToolkitMatrixJoiningError_{
    HToolkitMatrixState_Refused                       = 0,
    HToolkitMatrixState_HandshakeDenied               = 1,        
};

enum HToolkitMatrixState_{
    HToolkitMatrixState_Waiting                       = 0,
    HToolkitMatrixState_Active                        = 1,        
    HToolkitMatrixState_Sleeping                      = 2,        
};
//=============================================================================



//=============================================================================
// (Properties) HToolkitTransmission_Props_ 
//=============================================================================
enum HToolkitType_Props_ {
        HToolkitType_Props_Encoding   = 1,
};
//=============================================================================

//=============================================================================
// (Base enum) HToolkitChannelEncryption_ 
//=============================================================================
//=============================================================================


//_____________________________________________________________________________



struct HtkNewWrapper {};
inline void* operator new(size_t, HtkNewWrapper, void* ptr) { return ptr; }
inline void  operator delete(void*, HtkNewWrapper, void*)   {} // This is only required so we can use the symmetrical new()
#define HTK_ALLOC(_SIZE)                     HToolkit::MemAlloc(_SIZE)
#define HTK_FREE(_PTR)                       HToolkit::MemFree(_PTR)
#define HTK_PLACEMENT_NEW(_PTR)              new(HtkNewWrapper(), _PTR)
#define HTK_NEW(_TYPE)                       new(HtkNewWrapper(), HToolkit::MemAlloc(sizeof(_TYPE))) _TYPE
template<typename T> void HTK_DELETE(T* p)   { if (p) { p->~T(); HToolkit::MemFree(p); } }

//=============================================================================
// hVector<>
// Lightweight hVector<>-like class to avoid dragging dependencies (also, some implementations of STL with debug enabled are absurdly slow, we bypass it so our code runs fast in debug).
//=============================================================================
// - You generally do NOT need to care or use this ever. But we need to make it available in imgui.h because some of our public structures are relying on it.
// - We use std-like naming convention here, which is a little unusual for this codebase.
// - Important: clear() frees memory, resize(0) keep the allocated buffer. We use resize(0) a lot to intentionally recycle allocated buffers across frames and amortize our costs.
// - Important: our implementation does NOT call C++ constructors/destructors, we treat everything as raw data! This is intentional but be extra mindful of that,
//   Do NOT use this class as a std::vector replacement in your own code! Many of the structures used by dear imgui can be safely initialized by a zero-memset.
//=============================================================================

template<typename T>
struct hVector
{
    int                 Size;
    int                 Capacity;
    T*                  Data;

    // Provide standard typedefs but we don't use them ourselves.
    typedef T                   value_type;
    typedef value_type*         iterator;
    typedef const value_type*   const_iterator;

    // Constructors, destructor
    inline hVector()                                       { Size = Capacity = 0; Data = NULL; }
    inline hVector(const hVector<T>& src)                 { Size = Capacity = 0; Data = NULL; operator=(src); }
    inline hVector<T>& operator=(const hVector<T>& src)   { clear(); resize(src.Size); if (src.Data) memcpy(Data, src.Data, (size_t)Size * sizeof(T)); return *this; }
    inline ~hVector()                                      { if (Data) HTK_FREE(Data); } // Important: does not destruct anything

    inline void         clear()                             { if (Data) { Size = Capacity = 0; HTK_FREE(Data); Data = NULL; } }  // Important: does not destruct anything
    inline void         clear_delete()                      { for (int n = 0; n < Size; n++) IM_DELETE(Data[n]); clear(); }     // Important: never called automatically! always explicit.
    inline void         clear_destruct()                    { for (int n = 0; n < Size; n++){Data[n].~T();}clear();}

    inline bool         empty() const                       { return Size == 0; }
    inline int          size() const                        { return Size; }
    inline int          size_in_bytes() const               { return Size * (int)sizeof(T); }
    inline int          max_size() const                    { return 0x7FFFFFFF / (int)sizeof(T); }
    inline int          capacity() const                    { return Capacity; }
    inline T&           operator[](int i)                   { HTK_ASSERT(i >= 0 && i < Size); return Data[i]; }
    inline const T&     operator[](int i) const             { HTK_ASSERT(i >= 0 && i < Size); return Data[i]; }

    inline T*           begin()                             { return Data; }
    inline const T*     begin() const                       { return Data; }
    inline T*           end()                               { return Data + Size; }
    inline const T*     end() const                         { return Data + Size; }
    inline T&           front()                             { HTK_ASSERT(Size > 0); return Data[0]; }
    inline const T&     front() const                       { HTK_ASSERT(Size > 0); return Data[0]; }
    inline T&           back()                              { HTK_ASSERT(Size > 0); return Data[Size - 1]; }
    inline const T&     back() const                        { HTK_ASSERT(Size > 0); return Data[Size - 1]; }
    inline void         swap(hVector<T>& rhs)               { int rhs_size = rhs.Size; rhs.Size = Size; Size = rhs_size; int rhs_cap = rhs.Capacity; rhs.Capacity = Capacity; Capacity = rhs_cap; T* rhs_data = rhs.Data; rhs.Data = Data; Data = rhs_data; }

    inline int          _grow_capacity(int sz) const        { int new_capacity = Capacity ? (Capacity + Capacity / 2) : 8; return new_capacity > sz ? new_capacity : sz; }
    inline void         resize(int new_size)                { if (new_size > Capacity) reserve(_grow_capacity(new_size)); Size = new_size; }
    
    inline void         resize(int new_size, const T& v)    { if (new_size > Capacity)reserve(_grow_capacity(new_size));if (new_size > Size){for (int n = Size; n < new_size; n++){new (&Data[n]) T(v); }}Size = new_size;} 
    inline void         shrink(int new_size)                { HTK_ASSERT(new_size <= Size); Size = new_size; } // Resize a vector to a smaller size, guaranteed not to cause a reallocation
    inline void         reserve(int new_capacity)           { if (new_capacity <= Capacity) return; T* new_data = (T*)HTK_ALLOC((size_t)new_capacity * sizeof(T)); if (Data) { memcpy(new_data, Data, (size_t)Size * sizeof(T)); HTK_FREE(Data); } Data = new_data; Capacity = new_capacity; }
    inline void         reserve_discard(int new_capacity)   { if (new_capacity <= Capacity) return; if (Data) HTK_FREE(Data); Data = (T*)HTK_ALLOC((size_t)new_capacity * sizeof(T)); Capacity = new_capacity; }

    // NB: It is illegal to call push_back/push_front/insert with a reference pointing inside the hVector data itself! e.g. v.push_back(v[10]) is forbidden.
    inline void         push_back(const T& v)               {if (Size == Capacity)reserve(_grow_capacity(Size + 1));new (&Data[Size]) T(v);Size++;}
    inline void         pop_back()                          { HTK_ASSERT(Size > 0); Size--; }
    inline void         push_front(const T& v)              { if (Size == 0) push_back(v); else insert(Data, v); }
    
    inline T*           erase(const T* it)                  { HTK_ASSERT(it >= Data && it < Data + Size);const ptrdiff_t off = it - Data;Data[off].~T();memmove(Data + off, Data + off + 1, ((size_t)Size - (size_t)off - 1) * sizeof(T));Size--;return Data + off;} 
    inline T*           erase(const T* it, const T* it_last){ HTK_ASSERT(it >= Data && it < Data + Size && it_last >= it && it_last <= Data + Size); const ptrdiff_t count = it_last - it; const ptrdiff_t off = it - Data; memmove(Data + off, Data + off + count, ((size_t)Size - (size_t)off - (size_t)count) * sizeof(T)); Size -= (int)count; return Data + off; }
    inline T*           erase_unsorted(const T* it)         { HTK_ASSERT(it >= Data && it < Data + Size);  const ptrdiff_t off = it - Data; if (it < Data + Size - 1) memcpy(Data + off, Data + Size - 1, sizeof(T)); Size--; return Data + off; }
    inline T*           insert(const T* it, const T& v)     { HTK_ASSERT(it >= Data && it <= Data + Size); const ptrdiff_t off = it - Data; if (Size == Capacity) reserve(_grow_capacity(Size + 1)); if (off < (int)Size) memmove(Data + off + 1, Data + off, ((size_t)Size - (size_t)off) * sizeof(T)); memcpy(&Data[off], &v, sizeof(v)); Size++; return Data + off; }
    inline bool         contains(const T& v) const          { const T* data = Data;  const T* data_end = Data + Size; while (data < data_end) if (*data++ == v) return true; return false; }
    inline T*           find(const T& v)                    { T* data = Data;  const T* data_end = Data + Size; while (data < data_end) if (*data == v) break; else ++data; return data; }
    inline const T*     find(const T& v) const              { const T* data = Data;  const T* data_end = Data + Size; while (data < data_end) if (*data == v) break; else ++data; return data; }
    inline int          find_index(const T& v) const        { const T* data_end = Data + Size; const T* it = find(v); if (it == data_end) return -1; const ptrdiff_t off = it - Data; return (int)off; }
    inline bool         find_erase(const T& v)              { const T* it = find(v); if (it < Data + Size) { erase(it); return true; } return false; }
    inline bool         find_erase_unsorted(const T& v)     { const T* it = find(v); if (it < Data + Size) { erase_unsorted(it); return true; } return false; }
    inline int          index_from_ptr(const T* it) const   { HTK_ASSERT(it >= Data && it < Data + Size); const ptrdiff_t off = it - Data; return (int)off; }
};
//=============================================================================

//=============================================================================
// hMap<>
//=============================================================================
template <typename K, typename V>
class hMap 
{
private:
    struct KeyValuePair {K key;V value;};
    hVector<KeyValuePair> data;

public:
    inline void         insert(const K& key, const V& value){ for (int i = 0; i < data.size(); ++i) {if (data[i].key == key) {data[i].value = value;return;}}KeyValuePair pair;pair.key = key;pair.value = value;data.push_back(pair);}
    inline bool         find(const K& key, V& value) const  { for (int i = 0; i < data.size(); ++i) {if (data[i].key == key) {value = data[i].value;return true;}}return false;}
    inline bool         remove(const K& key)                { for (int i = 0; i < data.size(); ++i) {if (data[i].key == key) {data.erase(data.begin() + i);return true;}}return false;}
    inline int          size() const                        { return data.size(); }
    inline const V&     find(const K& key) const            {for (int i = 0; i < data.size(); ++i) {if (data[i].key == key) {return data[i].value;}}throw std::out_of_range("Key not found in hMap");}
    inline const V&     at(const K& key) const              {for (int i = 0; i < data.size(); ++i) {if (data[i].key == key) {return data[i].value;}}throw std::out_of_range("Key not found in hMap");}

    bool                operator!=(const K& key) const      {for (int i = 0; i < data.size(); ++i) {if (data[i].key == key) {return false;}}return true;}
    V&                  operator[](const K& key)            {for (int i = 0; i < data.size(); ++i) {if (data[i].key == key) {return data[i].value;}}KeyValuePair pair;pair.key = key;data.push_back(pair);return data.back().value;}

    typename            hVector<KeyValuePair>::iterator        begin() { return data.begin();}
    typename            hVector<KeyValuePair>::iterator        end()   { return data.end();}
    typename            hVector<KeyValuePair>::const_iterator  begin() const {return data.begin();}
    typename            hVector<KeyValuePair>::const_iterator  end() const {return data.end();}
};
//=============================================================================

constexpr unsigned int str2int(const char *str, int h = 0) {
  return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ str[h];
}

static long stringtoint(const char *s)
{
    long i;
    i = 0;
    while(*s >= '0' && *s <= '9')
    {
        i = i * 10 + (*s - '0');
        s++;
    }
    return i;
}


//=============================================================================
// hString<>
//=============================================================================
struct hString
{
    hVector<char> Buf;
    HTOOLKIT_API static char EmptyString[1];

    hString() : Data(nullptr), Size(0), Capacity(0) { }
    hString(const char* str) {this->append(str);}
    hString(int intValue) {char buffer[20];snprintf(buffer, sizeof(buffer), "%d", intValue);this->append(buffer);}

    size_t              find(const char* substr, size_t start = 0) const        {if (Buf.Data == nullptr || substr == nullptr || start >= Buf.Size) {return npos;}const char* result = std::strstr(Buf.Data + start, substr);if (result == nullptr) {return npos;}return result - Buf.Data;}
    size_t              rfind(const char* substr, size_t start = npos) const    {if (Buf.Data == nullptr || substr == nullptr) {return npos;}size_t searchStart = (start == npos) ? Size - 1 : std::min(start, Size - 1);for (size_t i = searchStart; i < Size; --i) {const char* result = std::strstr(Buf.Data + i, substr);if (result != nullptr) {return i;}}return npos;}
    hString             substr(size_t pos, size_t len = npos) const             {if (Buf.Data == nullptr || pos >= Buf.Size) {return hString();}if (len == npos || pos + len > Buf.Size) {len = Buf.Size - pos;}hString result;result.reserve(static_cast<int>(len));result.append(Buf.Data + pos, Buf.Data + pos + len);return result;}
    size_t              find_first_of(const char* charsToFind, size_t start = 0) const {if (Data == nullptr || charsToFind == nullptr || start >= Size) {return npos;}for (size_t i = start; i < Size; ++i) {char c = Data[i];if (strchr(charsToFind, c) != nullptr) {return i;}}return npos; }
    int                 length() const                                          {return static_cast<int>(Size);}
    void                push_back(char c)                                       {Buf.push_back(c);}
    const char*         begin() const { return Buf.Data ? &Buf.front() : EmptyString; }
    const char*         end() const { return Buf.Data ? &Buf.back() : EmptyString; }
    int                 size() const { return Buf.Size ? Buf.Size - 1 : 0; }
    bool                empty() const { return Buf.Size <= 1; }
    void                reserve(int capacity) {if (capacity <= Capacity) {return;}char* newData = new char[capacity];if (Data) {memcpy(newData, Data, Size);delete[] Data;}Data = newData;Capacity = capacity;}
    void                clear() {if (Data) {delete[] Data;Data = nullptr;Size = 0;Capacity = 0;}}
    const char*         c_str() const { return Buf.Data ? Buf.Data : EmptyString; }

    HTOOLKIT_API void   append(const char* str, const char* str_end = NULL);

    inline char         operator[](int i) const { HTK_ASSERT(Buf.Data != NULL); return Buf.Data[i]; }
    bool                operator==(const char* str) const {if (Buf.Data == nullptr && str == nullptr) {return true; }if (Buf.Data == nullptr || str == nullptr) {return false; }return strcmp(Buf.Data, str) == 0;}
    bool                operator==(const hString& other) const {return operator==(other.c_str());}
    hString&            operator=(const char* str) {this->clear();this->append(str);return *this;}
    hString&            operator+=(int value) {char strValue[12]; snprintf(strValue, sizeof(strValue), "%d", value);this->append(strValue);return *this;}
    hString&            operator+=(const char* str) {this->append(str);return *this;}
    hString&            operator+=(const hString& other) {this->append(other.c_str());return *this;}
    bool                operator!=(const char* rhs) const {return strcmp(Data, rhs) != 0;}
    friend hString      operator+(const char* lhs, const hString& rhs) {hString result(lhs);result.append(rhs.c_str());return result;}
    friend hString      operator+(const hString& lhs, const char* rhs) {hString result(lhs);result.append(rhs);return result;}
    friend hString      operator+(const hString& lhs, const hString& rhs) {hString result(lhs);result.append(rhs.c_str());return result;}

    static const size_t npos = static_cast<size_t>(-1);
    char* Data;
    size_t Size;
    size_t Capacity;

};
//=============================================================================

//=============================================================================
//  h64
//=============================================================================
struct h64
{
    hString encode;
    hString decode;
};

//=============================================================================

#include <chrono>
struct hTime
{
    static hString GetCurrentTimestamp();
};


//=============================================================================
//  h64
//=============================================================================
struct hEncrypt
{
    
};

//=============================================================================

//=============================================================================
//  hArgs
//=============================================================================
class hArgs {
public:
    template<typename T> void add(const hString& tag, T value) {arguments[tag] = new ArgumentHolder<T>(value); registered_arguments.push_back(tag);}
    template<typename T> T get(const hString& tag, const T& defaultT) const {if(registered_arguments.contains(tag)){auto it = arguments.find(tag); return dynamic_cast<ArgumentHolder<T>*>(it)->getValue();}return defaultT;}

    ~hArgs() {for (const auto& pair : arguments) {delete pair.value;}}

private:
    class ArgumentBase {public: virtual ~ArgumentBase() = default;};

    template<typename T> 
    class ArgumentHolder : public ArgumentBase {
    public:
        ArgumentHolder(T value) : storedValue(value) {}
        T getValue() const { return storedValue;}
    private:
        T storedValue;
    };

    hVector<hString> registered_arguments;
    hMap<hString, ArgumentBase*> arguments;
};
//=============================================================================

//_____________________________________________________________________________
// [SECTION] Structures
//_____________________________________________________________________________



//=============================================================================
// (Provider Input/Output) HtkProviderContext
//=============================================================================
// Context of a provider
//=============================================================================
struct HtkProviderInterface
{
    const char* ProviderIdentication;
    // Key = Value.
    // Contain all informations of a provider Iterface
    //std::unordered_map< hString, hString> InterfaceVariables;
    hMap<hString, hString> InterfaceVariables;

    // Hypernet Tags (r for receptor, e for emmiter & t for transmitter)
    hString    GetInterfaceVariable(hString key);
    void        AddValueToInterface(hString key, hString value);
    void        SetValueToInterface(hString key, hString value);
};
//=============================================================================


//-----------------------------------------------------------------------------
// (Base structure) HToolkitProvider 
//-----------------------------------------------------------------------------
// A provider is a component can provider third transport protocol support.
//-----------------------------------------------------------------------------
struct HToolkitProvider{
    const char*         ProviderLabel;
    hString             ProviderProtocol;
    void*               ProviderIdentication;


    void                SetActive();
    void                UnsetActive();


    virtual void        TrySend(HtkProviderInterface& interface, const char* buffer) {};

    HtkProviderInterface*interface;

    // Events
    virtual void        onSent() {};
    virtual void        onReceived() {};
    virtual void        onConnected() {};
    virtual void        onDisconnected() {};
    virtual void        onError() {};

    // I/O
    virtual void        Start() {}; // Start service
    virtual void        Stop() {}; // Stop service
    virtual void        Send(const char* data, HtkProviderInterface& interface) {}; // Send over a Emmiter
    virtual void        Receive(const char* data, HtkProviderInterface& interface) {}; // Receive with a Receiver
    virtual void        Connect() {}; // Connect with another provider of the same protocol
    virtual void        Disconnect() {}; // Disconnect from another provider of the same protocol
    virtual void        GetProviderIdentication() {}; // Get all provider link informations

    virtual void        OnTargetNotResponse() {};

    int                 DataReceived;
    int                 DataSent;
    int                 NumberOfConnexions;
    
};
//-----------------------------------------------------------------------------


struct HToolkitCustomEvent{
    virtual void on(hArgs* args) {HTK_LOG("Not implemented");};
    
    hString eventName;
    hString channelType;
    hString _for;
    hString _type;
};


//-----------------------------------------------------------------------------
// (Base structure) HToolkitChannel
//-----------------------------------------------------------------------------
// A channel is a communication way with dedicated properties between elements.
// This structure have the absolute base of HToolkitChannel interaction in the
// context.
//-----------------------------------------------------------------------------
struct HToolkitChannel{
    
    hString                 id; 
    hString                 state;
    hString                 matrixID;
    hString                 channelType; 

    hMap<hString, hString>  configs;
    hMap<HToolkitMatrix_Props_, hString>    props;

    hVector<HToolkitElement*> loaded_elements;


    hVector<hString>        available_events;
    hMap<hString, HToolkitCustomEvent*>     custom_events;

    // Informative Metrics
    int                     dataReceived;
    int                     bufferReceived;

    int                     metricsDataSent;
    int                     metricsElementsConnected;
    int                     metricsChannelCreatedAt;

    // Core functions
    virtual void            receive(hArgs* args) {};
    virtual void            send(hArgs* args) {HTK_LOG("Please implement send() function.")};
    virtual void            constructor(hArgs* args){};
    virtual void            init(hArgs* args) {};
    virtual void            start(hArgs* args) {};
    virtual void            stop(hArgs* args) {};

    void                    AddCustomEvent(hString name, HToolkitCustomEvent* event);
    void                    OnCustomEvent(hString name, hArgs* args);

    // All properties & params utils
    HTOOLKIT_API void       SendTransmission(hArgs* args);
    HTOOLKIT_API void       SendTransmission(HToolkitTransmission& transmission);
    hString                 GetId(){return this->id;};
    void                    SetId(hString _id){this->id = _id;};
    void                    SetState(hString _state){this->state = _state;};
    void                    AddParameter(hString key, hString value){this->configs.insert(key, value);}
    void                    SetParameter(hString key, hString value){this->configs.insert(key, value);}
    hString                 GetParameter(hString key){hString value;this->configs.find(key, value);return value;}    
    hString                 GetProp(HToolkitMatrix_Props_ prop){hString value; this->props.find(prop, value); return value;};
    hString                 GetProp(HToolkitMatrix_Props prop){hString value;this->props.find(HToolkitMatrix_Props_(prop), value); return value;};
    void                    SetProp(HToolkitMatrix_Props_ prop, hString value){this->props.insert(prop, value);};
    void                    SetProp(HToolkitMatrix_Props prop, hString value){this->props.insert(HToolkitMatrix_Props_(prop), value);};

    // Other Utils
    hString                 OutputElementString();
    void                    InputElementString(hString& string);
    hString                 OutputConfigString();
    void                    InputConfigString(hString& str);

    // All I/O, Self & Context events
    virtual void            OnChannelCreated() {};
    virtual void            OnChannelInitialized() {};
    virtual void            OnTest() {};

    virtual void            OnChannelReceived(HToolkitTransmission& transmission) {};
    virtual void            OnChannelSent(HToolkitTransmission* transmission) {};
    virtual void            OnChannelConnected() {};
    virtual void            OnChannelDisconnected() {};

    virtual bool            OnJoiningRequestReceived() {return false;};
    virtual void            OnElementConnect(HToolkitElement& e) {};
    virtual void            OnAuthorityAssigned() {};
};
//-----------------------------------------------------------------------------



struct SendingNotificationQueue
{
    hString                 ID;
    int                     timeout;
    int                     delayt;
    bool                    finish = false;
    hVector<HToolkitNotification> qeued_notification;
};


struct NotificationInQueue
{
    hMap<int, HToolkitNotification*> queue; 
    static void AddToQueue(); 
    static HToolkitNotification* PopNext();
};

struct TransmissionsInQueue
{

};

struct HToolkitType{
    virtual void            input(hString _input) {};
    virtual hString         output() {return "NaN";};

    hMap<HToolkitType_Props_, hString> props;

    virtual void setBuffer(hString buff){this->buffer.clear(); this->buffer = buff;}

    hString                 GetProp(HToolkitType_Props_ prop){hString value; this->props.find(prop, value); return value;};
    void                    SetProp(HToolkitType_Props_ prop, hString value){this->props.insert(prop, value);};

    hString                 buffer;
};
/*

enum HToolkitTransmission_Props_ {
        HToolkitTransmission_Props_BufferLenght   = 1,
};
*/

//-----------------------------------------------------------------------------
// (Base structure) HToolkitTransmission
//-----------------------------------------------------------------------------
// A transmission is a programmated communication on a channel
//-----------------------------------------------------------------------------
// Hypernet Standard Notification : t:version:{channel(params)[buffer]}
//-----------------------------------------------------------------------------
struct HToolkitTransmission{
    // Properties
    hString                 typetag; // pas besoin vu que le channel sait ce qu'il doit faire
    hString                 version;
    hString                 channel; // id
    HToolkitType            type;
    HtkProviderInterface*   origin;
    hString                 originID;
    hString                 timestamp;
    HtkProviderInterface*   target;
    hMap<hString, hString>  params;
    hString                 buffer;

    void                    AddParameter(hString key, hString value){this->params.insert(key, value);}
    void                    SetParameter(hString key, hString value){this->params.insert(key, value);}
    hString                 GetParameter(hString key){hString value;this->params.find(key, value);return value;}

    void                    Configure(hString _channel, hString _version);
    void                    AttachBuffer(hString& _buffer);
    void                    init(int lenght, int seq, int tseq);

    hString                 OutputTransmission();
    static HToolkitTransmission InitFromData(const char* data);
};
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// (Base structure) HToolkitNotification
//-----------------------------------------------------------------------------
// A notification is a minimal and simple communication without need to open
// a channel
//-----------------------------------------------------------------------------
struct HToolkitNotificationHandler{
    hString                 notificationLabel;
    hString                 type;

    virtual void            OnNotificationReceived(HToolkitNotification& notification) {};
    virtual void            OnNotificationSent(HToolkitNotification* notification) {};
};
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// (Base structure) HToolkitNotification
//-----------------------------------------------------------------------------
// A notification is a minimal and simple communication without need to open
// a channel
//-----------------------------------------------------------------------------
// Hypernet Standard Notification : n:version:{notification<Type>(Params)[Body]}
//-----------------------------------------------------------------------------
struct HToolkitNotification {
    hString                 type;
    hString                 version;
    hString                 tag;
    hString                 body;
    bool                    trace;
    hString                 traceID;
    hMap<hString, hString>  params;
    hString                 timestamp;
    HtkProviderInterface*   origin;
    HtkProviderInterface*   target;
    int                     lenght;

    void                    Configure(hString _tag, hString _type, hString _version, HtkProviderInterface& _target){this->tag = _tag;this->version = _version;this->target = &_target;this->type = _type;}
    void                    Configure(hString _tag, hString _type, hString _version, HtkProviderInterface& _target, bool _trace){this->tag = _tag;this->version = _version;this->target = &_target;this->type = _type; this->trace = _trace;}
    static HToolkitNotification  InitFromData(const char* Buffer);
    hString                 OutputNotification();
    void                    SetType(const char* type){this->type = type;}
    void                    SetTag(const char* tag){this->tag = tag;}
    void                    SetBody(const char* body){this->body = body;}
    void                    AddParameter(hString key, hString value){this->params.insert(key, value);}
    void                    SetParameter(hString key, hString value){this->params.insert(key, value);}
    hString                 GetParameter(hString key){hString value;this->params.find(key, value);return value;}
    void                    SeeNotif();
};
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// (Base structure) HToolkitMatrix
//-----------------------------------------------------------------------------
// A matrix is a point of matrix can interact with each others
//-----------------------------------------------------------------------------
struct HToolkitMatrix
{
    // Runtime content filters
    hVector<hString>        attempted_channels;
    hVector<hString>        attempted_elements;

    // Content loaded in the runtime. (Warning : this is not a Hypernet content register !)
    HToolkitElement*        loaded_self; 
    hVector<HToolkitElement*> loaded_elements; 
    hVector<HToolkitNode*>  loaded_nodes;

    // Note : loaded channels are registered in the IO directly.
    hVector<hString>        available_events;
    hMap<hString, HtkMatrixPortal*>     current_portals;
    hMap<hString, HToolkitCustomEvent*> custom_events;

    public:
    void                    AddElementTitle(){};
    hString                 GetId(){return this->id;};
    hString                 GetTag(){return this->tag;};
    hString                 GetState(){return this->state;};
    hVector<HtkProviderInterface>& GetReceivers(){return this->boostrapp_receivers;};
    hVector<hString>        GetChannels();
    void                    SetId(hString _id){this->id = _id;};
    void                    SetTag(hString _tag){this->tag = _tag;};
    void                    SetState(hString _state){this->state = _state;}

    void                    AddCustomEvent(hString name, HToolkitCustomEvent* event);
    static void             AddCustomEvent(hString name, HToolkitCustomEvent* event, HToolkitMatrix* matrix);
    void                    OnCustomEvent(hString name, hArgs* args);

    hString                 GetProp(HToolkitMatrix_Props_ prop){hString value; this->props.find(prop, value); return value;};
    hString                 GetProp(HToolkitMatrix_Props prop){hString value;this->props.find(HToolkitMatrix_Props_(prop), value); return value;};
    void                    SetProp(HToolkitMatrix_Props_ prop, hString value){this->props.insert(prop, value);};
    void                    SetProp(HToolkitMatrix_Props prop, hString value){this->props.insert(HToolkitMatrix_Props_(prop), value);};

    // Channels functions
    HTOOLKIT_API HToolkitChannel* CreateChannel(hString channeltype, hArgs* args);
    HTOOLKIT_API HToolkitElement* CreateElement(hString elementtype, hArgs* args);
    HTOOLKIT_API HToolkitChannel* InitChannel(hString channelID, hString channeltype);

    void                    SetChannel(HToolkitChannel*);
    void                    AddNewRole(int priority, hString tag){};

    virtual void            constructor(hArgs* args) {};
    virtual void            destructor(hArgs* args) {};
    virtual void            init(hArgs* args) {HTK_LOG("Please implement init(args) function to initialize a new matrix.")};

    virtual void            constructor_channel(hArgs* args) {};
    virtual void            destructor_channel(hArgs* args) {};
    virtual void            init_channel(hArgs* args) {};
    virtual void            destruct_channel(hArgs* args) {};

    virtual void            constructor_element(hArgs* args) {};
    virtual void            destructor_element(hArgs* args) {};
    virtual void            init_element(hArgs* args) {};
    virtual void            destruct_element(hArgs* args) {};

    // Important : "registered" is not "loaded". A register can give to runtime a list of elements without load it.
    virtual hVector<HToolkitElement*>   get_registered_element(hArgs* args) {};
    virtual hVector<HToolkitChannel*>   get_registered_channels(hArgs* args) {};
    virtual hVector<HToolkitNode*>      get_registered_nodes(hArgs* args) {};
    virtual void            add_registered_element(HToolkitElement* element) {};
    virtual void            add_registered_channel(HToolkitChannel* channel) {};
    virtual void            add_registered_node(HToolkitNode* ndoe) {};
    virtual void            clear_registered_element(hArgs* args) {};




//==================================================================================================================
// Matrix Overrideable Events
//==================================================================================================================
//               Function                      |      Target Notification       |    Compatible with Hypernet V.   |
//==================================================================================================================
    // From Outside elements requests
    /*
    virtual void OnElementListAsked();
    virtual void OnElementPositionAsked();
    virtual void OnElementAssetAsked();
    virtual void OnElementInterfaceAsked();

    // From Outside nodes requests
    virtual void OnNodeListAsked(); // Manager POV // Output : [id,id,...]
    virtual void OnNodePairAsked(); // Manager POV // Output : [id=ida+idb,id=ida+idb,...]

    virtual void OnNodeListAskedToMe(); // SELF POV // Output : [id,id,...]
    virtual void OnNodePairAskedToMe(); // SELF POV // Output : [id=ida+idb,id=ida+idb,...]


    virtual void OnNodeIdentificationMethodAsked();

    // From Outside channels requests
    virtual void OnChannelListAsked();
    virtual void OnChannelNodesAsked();*/
    virtual void    HandleNewJoinAsk(HToolkitNotification* notification) {}; // ::ask

    // Events To Override
    virtual void    OnMatrixCreated() {HTK_LOG("No implementation of OnMatrixCreated()") /* Envoyer une erreur : not implemented ! */};
    virtual void    OnNewNode() {};
    virtual void    OnMatrixJoined() {};
    virtual void    OnMatrixLeaved() {};
    virtual void    OnMatrixDiscovered() {};
    virtual void    OnElementDiscovered() {};      
    virtual void    OnElementCanJoinMatrix(HToolkitElement* element, HToolkitNotification* notification) {};
    virtual void    OnElementCanJoinChannel(HToolkitElement* element) {};

    virtual void    OnChannelDiscovered() {};
    virtual void    OnChannelDiscovered(hString id) {};
    virtual void    OnChannelUpdated() {};
    virtual void    OnChannelJoined() {};
    virtual void    OnChannelLeaved() {};
    virtual void    OnChannelCreated(HToolkitChannel* channel) {};
    virtual void    OnChannelInitialized(HToolkitChannel* channel) {};
    virtual void    OnChannelSent() {};
    virtual void    OnChannelReceived() {};
    virtual void    OnChannelJoinByOther() {};


    hString         OutputChannelString();
    void            InputChannelString(hString& string);
    hString         OutputConfigString();
    void            InputConfigString(hString& str);

    hString         id;

    hVector<HtkProviderInterface>   boostrapp_receivers;

    private:
    hString         eid; 
    hString         tag; 
    hString         state; 
    
    hMap<hString, hString> configs;
    hMap<HToolkitMatrix_Props_, hString> props;
    HtkMatrixPortal* portal;
};
//-----------------------------------------------------------------------------

struct HToolkitNode{
    HToolkitElement*        a;
    HToolkitElement*        b;
    hString                 nodeID;
    virtual void            init(HToolkitElement* _a, HToolkitElement* _b, hString _nodeID);
};

//-----------------------------------------------------------------------------
// (Base structure) HToolkitElement 
//-----------------------------------------------------------------------------
// A element is a point of a matrix can transmit communications & have services.
//-----------------------------------------------------------------------------
struct HToolkitElement{
    void SetTag(hString newtag);

    // Properties
    hString                id;
    hString                role;
    hString                runtime_state;
    hString                tag = "null";
    hString                address;
    HtkProviderInterface   interface;

    virtual void            constructor(hArgs* args) {};
    virtual void            init(hArgs* args) {};
    virtual void            bootstrapp(hArgs* args) {};

    // Relations
    hVector<HToolkitChannel *> channels;
    HToolkitMatrix *        attached_matrix;

    virtual void            OnUpdate(HToolkitNotification notification) {};
};
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// (Base structure) HToolkitMatrix
//-----------------------------------------------------------------------------
// A matrix is a point of matrix can interact with each others
//-----------------------------------------------------------------------------
struct HtkMatrixPortal{
    virtual void            constructor(hArgs* args) {};
    virtual void            handle(hArgs* args) {};
    hString                 value;
};
//-----------------------------------------------------------------------------


struct HtkChannelPortal{
    virtual void            constructor(hArgs* args) {};
    virtual void            handle(hArgs* args) {};
    hString                 value;
};

//-----------------------------------------------------------------------------
// (Runtime structure) NotificationHandlerRegistry
//-----------------------------------------------------------------------------
// The notifications handler registry register all basics structures and 
// types we want by default on the runtime. It register all type before creating
// any context, and it swap all register content into the context.
// With that, we can easely create custom types, custom components & hypernet
// custom behaviors. 
//-----------------------------------------------------------------------------
class NotificationHandlerRegistry {
public:
    static NotificationHandlerRegistry& getInstance() {static NotificationHandlerRegistry instance;return instance;}
    void                                registerType(const hString& typeName, HToolkitNotificationHandler* instance) {typeMap[typeName] = instance;}
    HToolkitNotificationHandler*        createInstance(const hString& typeName) const {if (typeMap.find(typeName)) {return typeMap.at(typeName);}return nullptr;}
    hVector<hString>                    getRegisteredTypes() const {hVector<hString> types;for (const auto& pair : typeMap) {types.push_back(pair.key);}return types;}
    hMap<hString, HToolkitNotificationHandler*> typeMap;
};
//-----------------------------------------------------------------------------

class MatrixFactory {
public:
    using CreatorFunction = HToolkitMatrix* (*)();

    static MatrixFactory& getInstance() {
        static MatrixFactory instance;
        return instance;
    }

    void registerType(const hString& typeName, CreatorFunction creator) {
        registered.push_back(typeName.c_str());
        creatorMap[typeName] = creator;
    }


    HToolkitMatrix* createInstance(const hString& typeName) const {
        if (creatorMap.find(typeName)) {return (creatorMap.at(typeName))();}return nullptr;
    }

    hVector<hString> registered;
    hMap<hString, CreatorFunction> creatorMap;

};

class ChannelFactory {
public:
    using CreatorFunction = HToolkitChannel* (*)();

    static ChannelFactory& getInstance() {
        static ChannelFactory instance;
        return instance;
    }

    void registerType(const hString& typeName, CreatorFunction creator) {
        registered.push_back(typeName.c_str());
        creatorMap[typeName] = creator;
    }

    HToolkitChannel* createInstance(const hString& typeName) const {
        if (creatorMap.find(typeName)) {return (creatorMap.at(typeName))();}return nullptr;
    }

    hVector<hString> registered;
    hMap<hString, CreatorFunction> creatorMap;

};

class ElementFactory {
public:
    using CreatorFunction = HToolkitElement* (*)();

    static ElementFactory& getInstance() {
        static ElementFactory instance;
        return instance;
    }

    void registerType(const hString& typeName, CreatorFunction creator) {
        creatorMap[typeName] = creator;
    }

    HToolkitElement* createInstance(const hString& typeName) const {
        if (creatorMap.find(typeName)) {return (creatorMap.at(typeName))();}return nullptr;
    }

    hMap<hString, CreatorFunction> creatorMap;

};


class ProviderFactory {
public:
    using CreatorFunction = HToolkitProvider* (*)();

    static ProviderFactory& getInstance() {
        static ProviderFactory instance;
        return instance;
    }

    void registerType(const hString& typeName, CreatorFunction creator) {
        creatorMap[typeName] = creator;
    }

    HToolkitProvider* createInstance(const hString& typeName) const {
        if (creatorMap.find(typeName)) {return (creatorMap.at(typeName))();}return nullptr;
    }

    hMap<hString, CreatorFunction> creatorMap;

};


class MatrixPortalFactory {
public:
    using CreatorFunction = HtkMatrixPortal* (*)();

    static MatrixPortalFactory& getInstance() {
        static MatrixPortalFactory instance;
        return instance;
    }

    void registerType(const hString& typeName, CreatorFunction creator) {
        creatorMap[typeName] = creator;
    }

    HtkMatrixPortal* createInstance(const hString& typeName) const {
        if (creatorMap.find(typeName)) {return (creatorMap.at(typeName))();}return nullptr;
    }

    hMap<hString, CreatorFunction> creatorMap;

};


class ChannelPortalFactory {
public:
    using CreatorFunction = HtkChannelPortal* (*)();

    static ChannelPortalFactory& getInstance() {
        static ChannelPortalFactory instance;
        return instance;
    }

    void registerType(const hString& typeName, CreatorFunction creator) {
        creatorMap[typeName] = creator;
    }

    HtkChannelPortal* createInstance(const hString& typeName) const {
        if (creatorMap.find(typeName)) {return (creatorMap.at(typeName))();}return nullptr;
    }

    hMap<hString, CreatorFunction> creatorMap;

};


class TypeFactory {
public:
    using CreatorFunction = HToolkitType* (*)();

    static TypeFactory& getInstance() {
        static TypeFactory instance;
        return instance;
    }

    void registerType(const hString& typeName, CreatorFunction creator) {
        creatorMap[typeName] = creator;
    }

    HToolkitType* createInstance(const hString& typeName) const {
        if (creatorMap.find(typeName)) {return (creatorMap.at(typeName))();}return nullptr;
    }

    hMap<hString, CreatorFunction> creatorMap;

};


class MatrixEventFactory {
public:
    using CreatorFunction = HToolkitCustomEvent* (*)();

    static MatrixEventFactory& getInstance() {
        static MatrixEventFactory instance;
        return instance;
    }

    void registerType(const hString& typeName, CreatorFunction creator) {
        creatorMap[typeName] = creator;
    }

    HToolkitCustomEvent* createInstance(const hString& typeName) const {
        if (creatorMap.find(typeName)) {return (creatorMap.at(typeName))();}return nullptr;
    }

    hMap<hString, CreatorFunction> creatorMap;

};

class MatrixEventRegistry {
public:
    static MatrixEventRegistry&             getInstance() {static MatrixEventRegistry instance;return instance;}
    void                                registerType(const hString& typeName, HToolkitCustomEvent* instance) {typeMap[typeName] = instance;}
    HToolkitCustomEvent*                      createInstance(const hString& typeName) const {if (typeMap.find(typeName)) {return typeMap.at(typeName);}return nullptr;}
    hVector<hString>                  getRegisteredTypes() const {hVector<hString> types;for (const auto& pair : typeMap) {types.push_back(pair.key);}return types;}
    hMap<hString, HToolkitCustomEvent*>     typeMap;
};

class ChannelEventRegistry {
public:
    static ChannelEventRegistry&             getInstance() {static ChannelEventRegistry instance;return instance;}
    void                                registerType(const hString& typeName, HToolkitCustomEvent* instance) {typeMap[typeName] = instance;}
    HToolkitCustomEvent*                      createInstance(const hString& typeName) const {if (typeMap.find(typeName)) {return typeMap.at(typeName);}return nullptr;}
    hVector<hString>                  getRegisteredTypes() const {hVector<hString> types;for (const auto& pair : typeMap) {types.push_back(pair.key);}return types;}
    hMap<hString, HToolkitCustomEvent*>     typeMap;
};



#define ADD_MATRIXEVENT_TO_FACTORY(typeName, className) \
    namespace { \
        inline HToolkitCustomEvent* create_##className() { return new className(); } \
        struct className##Registrar { \
            className##Registrar() { \
                MatrixEventFactory::getInstance().registerType("::"#typeName"::"#className, create_##className); \
            } \
        }; \
        inline static className##Registrar className##RegistrarInstance; \
    }

#define ADD_TYPE_TO_FACTORY(typeName, className) \
    namespace { \
        inline HToolkitType* create_##className() { return new className(); } \
        struct className##Registrar { \
            className##Registrar() { \
                TypeFactory::getInstance().registerType("::"#typeName"::"#className, create_##className); \
            } \
        }; \
        inline static className##Registrar className##RegistrarInstance; \
    }

#define ADD_CHANNELPORTAL_TO_FACTORY(typeName, className) \
    namespace { \
        inline HtkChannelPortal* create_##className() { return new className(); } \
        struct className##Registrar { \
            className##Registrar() { \
                ChannelPortalFactory::getInstance().registerType("::"#typeName"::"#className, create_##className); \
            } \
        }; \
        inline static className##Registrar className##RegistrarInstance; \
    }

#define ADD_MATRIXPORTAL_TO_FACTORY(typeName, className) \
    namespace { \
        inline HtkMatrixPortal* create_##className() { return new className(); } \
        struct className##Registrar { \
            className##Registrar() { \
                MatrixPortalFactory::getInstance().registerType("::"#typeName"::"#className, create_##className); \
            } \
        }; \
        inline static className##Registrar className##RegistrarInstance; \
    }

#define ADD_CHANNEL_TO_FACTORY(typeName, className) \
    namespace { \
        inline HToolkitChannel* create_##className() { return new className(); } \
        struct className##Registrar { \
            className##Registrar() { \
                ChannelFactory::getInstance().registerType("::"#typeName"::"#className, create_##className); \
            } \
        }; \
        inline static className##Registrar className##RegistrarInstance; \
    }

#define ADD_PROVIDER_TO_FACTORY(typeName, className) \
    namespace { \
        inline HToolkitProvider* create_##className() { return new className(); } \
        struct className##Registrar { \
            className##Registrar() { \
                ProviderFactory::getInstance().registerType("::"#typeName"::"#className, create_##className); \
            } \
        }; \
        inline static className##Registrar className##RegistrarInstance; \
    }


#define ADD_MATRIX_TO_FACTORY(typeName, className) \
    namespace { \
        inline HToolkitMatrix* create_##className() { return new className(); } \
        struct className##Registrar { \
            className##Registrar() { \
                MatrixFactory::getInstance().registerType("::"#typeName"::"#className, create_##className); \
            } \
        }; \
        inline static className##Registrar className##RegistrarInstance; \
    }

#define ADD_ELEMENT_TO_FACTORY(typeName, className) \
    namespace { \
        inline HToolkitElement* create_##className() { return new className(); } \
        struct className##Registrar { \
            className##Registrar() { \
                ElementFactory::getInstance().registerType("::"#typeName"::"#className, create_##className); \
            } \
        }; \
        inline static className##Registrar className##RegistrarInstance; \
    }

#define CORE_NOTIFICATION_HANDLER(className) \
    class className; \
    namespace { \
        inline className* create_##className() { return new className(); } \
        struct className##Registrar { \
            className##Registrar() { \
                NotificationHandlerRegistry::getInstance().registerType("::"#className, create_##className()); \
            } \
        }; \
        inline static className##Registrar className##RegistrarInstance; \
    }

#define MOD_NOTIFICATION_HANDLER(modName, className) \
    class className; \
    namespace { \
        inline className* create_##className() { return new className(); } \
        struct className##Registrar { \
            className##Registrar() { \
                NotificationHandlerRegistry::getInstance().registerType("::"#modName"::"#className, create_##className()); \
            } \
        }; \
        inline className##Registrar className##RegistrarInstance; \
    }


#define MATRIX_CUSTOM_EVENT(modName, className, typeName, code) \
    struct className : public HToolkitCustomEvent { \
        void on(hArgs* args) override { \
            code \
        } \
        className(hString type){this->_type = type;} \
    }; \ 
    namespace { \
        inline className* create_##className() { return new className("::"#modName"::"#typeName); } \
        struct className##Registrar { \
            className##Registrar() { \
                MatrixEventRegistry::getInstance().registerType(#className, create_##className()); \
            } \
        }; \
        inline static className##Registrar className##RegistrarInstance; \
    }

#define CHANNEL_CUSTOM_EVENT(modName, className, typeName, code) \
    struct className : public HToolkitCustomEvent { \
        void on(hArgs* args) override { \
            code \
        } \
        className(hString type){this->_type = type;} \
    }; \ 
    namespace { \
        inline className* create_##className() { return new className("::"#modName"::"#typeName); } \
        struct className##Registrar { \
            className##Registrar() { \
                ChannelEventRegistry::getInstance().registerType(#className, create_##className()); \
            } \
        }; \
        inline static className##Registrar className##RegistrarInstance; \
    }


//_____________________________________________________________________________




#endif // #ifndef VORTEX_DISABLE