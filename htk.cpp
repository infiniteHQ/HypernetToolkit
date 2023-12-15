// The HToolkit Project, version 1.1 ALPHA
// [Main sources & docs]

// Index of this file:

//  [Includes]
//  [Context Utility: RegisterProvider] => Register a Provider into the context

//-------------------------------------------------------------------------
// [INCLUDES]
//-------------------------------------------------------------------------

#include "htk.h"
#ifndef VORTEX_DISABLE
#include "htk_internals.h"
#include "htk_core.h"

//-----------------------------------------------------------------------------
// [SECTION] Forward declarations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// [SECTION] CONTEXT AND MEMORY ALLOCATORS
//-----------------------------------------------------------------------------
// Current runtime pointer.
#ifndef CHToolkit
HtkContext *CHToolkit = NULL;
#endif

// Memory Allocator functions. Use SetAllocatorFunctions() to change them.
// - You probably don't want to modify that mid-program, and if you use
// global/static e.g. ImVector<> instances you may need to keep them accessible
// during program destruction.
// - DLL users: read comments above.
#ifndef DISABLE_DEFAULT_ALLOCATORS
static void *MallocWrapper(size_t size, void *user_data) {
  HTK_UNUSED(user_data);
  return malloc(size);
}
static void FreeWrapper(void *ptr, void *user_data) {
  HTK_UNUSED(user_data);
  free(ptr);
}
#else
static void *MallocWrapper(size_t size, void *user_data) {
  HTK_UNUSED(user_data);
  HTK_UNUSED(size);
  HTK_ASSERT(0);
  return NULL;
}
static void FreeWrapper(void *ptr, void *user_data) {
  HTK_UNUSED(user_data);
  HTK_UNUSED(ptr);
  HTK_ASSERT(0);
}
#endif
static HToolkitMemAllocFunc CHtkAllocatorAllocFunc = MallocWrapper;
static HToolkitMemFreeFunc CHtkAllocatorFreeFunc = FreeWrapper;
static void *CHtkAllocatorUserData = NULL;

//-----------------------------------------------------------------------------
// [Context Function: CreateContext] => Create a context for a HToolkit use
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

HTOOLKIT_API HtkContext *HToolkit::CreateContext() {
  /*
  HtkContext *context = new HtkContext();
  // HTK_NEW
  // malloc(sizeof(context));
  SetCurrentContext(context);
  return context;
  */

  HtkContext *prev_ctx = GetCurrentContext();
  HtkContext *ctx = HTK_NEW(HtkContext);
  SetCurrentContext(ctx);
  Initialize();
  if (prev_ctx != NULL)
    SetCurrentContext(
        prev_ctx); // Restore previous context if any, else keep new one.

        ctx->IO.MetricsActiveMatrixes = 0;
        ctx->IO.MetricsCachedMatrixes = 0;

  return ctx;
}

//-----------------------------------------------------------------------------
// [Context Function: SetCurrentContext] => Set the main context
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void HToolkit::SetCurrentContext(HtkContext *ctx) {
#ifdef USE_CURRENT_CONTEXT_FUNC
  USE_CURRENT_CONTEXT_FUNC(ctx); // For custom thread-based hackery you may want
                                 // to have control over this.
#else
  CHToolkit = ctx;
#endif
}

//-----------------------------------------------------------------------------

void HToolkit::SetAllocatorFunctions(HToolkitMemAllocFunc alloc_func,
                                   HToolkitMemFreeFunc free_func,
                                   void *user_data) {
  CHtkAllocatorAllocFunc = alloc_func;
  CHtkAllocatorFreeFunc = free_func;
  CHtkAllocatorUserData = user_data;
}

// This is provided to facilitate copying allocators from one static/DLL
// boundary to another (e.g. retrieve default allocator of your executable
// address space)
void HToolkit::GetAllocatorFunctions(HToolkitMemAllocFunc *p_alloc_func,
                                   HToolkitMemFreeFunc *p_free_func,
                                   void **p_user_data) {
  *p_alloc_func = CHtkAllocatorAllocFunc;
  *p_free_func = CHtkAllocatorFreeFunc;
  *p_user_data = CHtkAllocatorUserData;
}

//-----------------------------------------------------------------------------
// [Context Function: GetCurrentContext] => Get the current main context
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
HTOOLKIT_API void HToolkit::DestroyContext(HtkContext *ctx) {
  HtkContext *prev_ctx = GetCurrentContext();
  if (ctx == NULL) //-V1051
    ctx = prev_ctx;
  SetCurrentContext(ctx);
  SetCurrentContext((prev_ctx != ctx) ? prev_ctx : NULL);
  HTK_DELETE(ctx);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// [Context Function: GetCurrentContext] => Get the current main context
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
HTOOLKIT_API HtkContext *HToolkit::GetCurrentContext() { return CHToolkit; }
//-----------------------------------------------------------------------------


HTOOLKIT_API void HToolkit::RegisterNotificationHandler(HToolkitNotificationHandler* handler) {
  HtkContext &ctx = *CHToolkit;
  ctx.registeredNotificationHandlers.typeMap.insert(handler->notificationLabel,
                                            handler);
}

HTOOLKIT_API void HToolkit::Initialize() {
  HtkContext &ctx = *CHToolkit;

  // Swap all registered current component to context
  ctx.registeredNotificationHandlers =    NotificationHandlerRegistry::getInstance();

  ctx.registeredMatrixCustomEvents =      MatrixEventRegistry::getInstance();
  ctx.registeredChannelCustomEvents =      ChannelEventRegistry::getInstance();


  ctx.registeredMatrixPortals = &MatrixPortalFactory::getInstance();
  ctx.registeredChannelPortals = &ChannelPortalFactory::getInstance();
  ctx.registeredMatrixes = &MatrixFactory::getInstance();
  ctx.registeredProviders = &ProviderFactory::getInstance();
  ctx.registeredElements = &ElementFactory::getInstance();
  ctx.registeredTypes = &TypeFactory::getInstance();
  ctx.registeredChannels = &ChannelFactory::getInstance();




  // If HSL in RSA mode
  #ifdef USE_HSP
  ctx.keyPair = HToolkitSecurity::GenerateKeys();
  #endif // USE_HSP

  // All current element props
  ctx.adress = hRandom::GenerateUUIDv4();

  // Set initialized flags
  ctx.initialized = true;
}

// IM_ALLOC() == ImGui::MemAlloc()
void *HToolkit::MemAlloc(size_t size) {
  void *ptr = (*CHtkAllocatorAllocFunc)(size, CHtkAllocatorUserData);
#ifndef IMGUI_DISABLE_DEBUG_TOOLS
  if (HtkContext *ctx = CHToolkit)
    DebugAllocHook(&ctx->debugAllocInfo, ptr, size);
#endif
  return ptr;
}

// HTK_FREE() == HToolkit::MemFree()
void HToolkit::MemFree(void *ptr) {
  if (ptr)
    if (HtkContext *ctx = CHToolkit)
      ctx->IO.MetricsActiveAllocations--;
  return (*CHtkAllocatorFreeFunc)(ptr, CHtkAllocatorUserData);
}

// We record the number of allocation in recent frames, as a way to
// audit/sanitize our guiding principles of "no allocations on idle/repeating
// frames"
void HToolkit::DebugAllocHook(HToolkitDebugAllocInfo *info, void *ptr,
                            size_t size) {
  // HToolkitDebugAllocEntry* entry = &info->LastEntriesBuf[info->LastEntriesIdx];
  HTK_UNUSED(ptr);

  /*
  if (entry->FrameCount != frame_count)
  {
      info->LastEntriesIdx = (info->LastEntriesIdx + 1) %
  IM_ARRAYSIZE(info->LastEntriesBuf); entry =
  &info->LastEntriesBuf[info->LastEntriesIdx]; entry->FrameCount = frame_count;
      entry->AllocCount = entry->FreeCount = 0;
  }
  if (size != (size_t)-1)
  {
      entry->AllocCount++;
      info->TotalAllocCount++;
      //printf("[%05d] MemAlloc(%d) -> 0x%p\n", frame_count, size, ptr);
  }
  else
  {
      entry->FreeCount++;
      info->TotalFreeCount++;
      //printf("[%05d] MemFree(0x%p)\n", frame_count, ptr);
  }*/
}


HTOOLKIT_API void HToolkit::StartProviders() {
  HtkContext &ctx = *CHToolkit;
  for (auto prov : ctx.registeredProviders->creatorMap){
    HToolkitProvider* provider = ProviderFactory::getInstance().createInstance(prov.key);
    ctx.IO.MetricsActiveProviders++;
    ctx.IO.activeProviders.push_back(provider);
    provider->Start();
  }
}

HTOOLKIT_API HToolkitProvider *HToolkit::FindProvidersByProtocol(hString protocol) {
  HtkContext &ctx = *CHToolkit;
  for (auto provider : ctx.IO.activeProviders) {

    if (protocol == provider->ProviderProtocol) {
      return provider;
    }
  }
  return nullptr;
}


HTOOLKIT_API void HToolkit::AttachHookToChannelType(HToolkitCustomEvent* _event, hString _eventName, hString _channelType){
  HtkContext &ctx = *CHToolkit;
  _event->eventName = _eventName;
  _event->channelType = _channelType;
  ctx.IO.registeredHookForChannelsTypes.push_back(_event);
}

HTOOLKIT_API void HToolkit::HandleMatrixPortal(hArgs* args){
  HtkContext &ctx = *CHToolkit;
  hString portal_type = args->get<hString>("portal_type", "null");
  HtkMatrixPortal* matrixportal = MatrixPortalFactory::getInstance().createInstance(portal_type);
  matrixportal->handle(args);
  delete(matrixportal);
}

HTOOLKIT_API void HToolkit::HandleChannelPortal(hArgs* args){
  HtkContext &ctx = *CHToolkit;
  hString portal_type = args->get<hString>("portal_type", "null");
  HtkChannelPortal* channelprotal = ChannelPortalFactory::getInstance().createInstance(portal_type);
  channelprotal->handle(args);
  delete(channelprotal);
}

HTOOLKIT_API void HToolkit::HandleReceiveData(const char *Data,
                                          HtkProviderInterface &interface) {
  HtkContext &ctx = *CHToolkit;
  
  const char *separator = ":{";

  char input[strlen(Data) + 1];
  strcpy(input, Data);

  char *flag = strtok(input, separator);
  if (strcmp(input, "n") == 0) {
    HToolkitNotification newnotification = HToolkitNotification::InitFromData(Data);
    newnotification.origin = &interface;
    HToolkit::HandleNotification(newnotification);
  }

  if (strcmp(input, "t") == 0) {
    HToolkitTransmission transmission = HToolkitTransmission::InitFromData(Data);
    transmission.origin = &interface;
    HToolkit::HandleTransmission(transmission);
  }

  if (strcmp(input, "c") == 0) {
    // Your custom behavior here !
  }
}

HTOOLKIT_API void HToolkit::HandleTransmission(HToolkitTransmission& transmission) {
  HtkContext &ctx = *CHToolkit;
  HToolkitChannel* channel = ctx.IO.GetActiveChannel(transmission.channel);
    //channel->bufferReceived += stringtoint(transmission.GetParameter("buffer_lenght").c_str());
    //channel->dataReceived += stringtoint(transmission.GetParameter("data_lenght").c_str());
    //channel->OnChannelReceived(transmission); // obsolete

    hArgs args;
    args.add("transmission", &transmission);
    channel->receive(&args);

#ifdef USE_REGISTERING
  ctx.registeredTransmissionsReceived.Add(transmission);
#endif // USE_REGISTERING
}

HTOOLKIT_API void HToolkit::HandleNotification(HToolkitNotification& notification) {
  HtkContext &ctx = *CHToolkit;
  HToolkitNotificationHandler *it = ctx.registeredNotificationHandlers.typeMap.find(notification.tag.c_str());

  if (it) {
    it->OnNotificationReceived(notification);

  } else {
    HTK_LOG("No Notification handler !")
  }

#ifdef USE_REGISTERING
  ctx.registeredNotificationReceived.Add(notification);
#endif // USE_REGISTERING
}

HTOOLKIT_API void HToolkit::SendNotification(HToolkitNotification& notification) {
  HtkContext &ctx = *CHToolkit;

  hString protocol = notification.target->GetInterfaceVariable("protocol");

  if (protocol != "nullptr") {
    HToolkitProvider *providerToUse = HToolkit::FindProvidersByProtocol(protocol);
    if (providerToUse != nullptr) {

      if(notification.trace){
        notification.traceID = hRandom::Generate6CharID();
        notification.AddParameter("tid", notification.traceID);
        ctx.IO.AddTraceNotification(notification);
      }

  #ifdef USE_REGISTERING
  ctx.registeredNotificationSent.Add(notification);
  #endif // USE_REGISTERING
      providerToUse->Send(
          (char *)notification.OutputNotification().c_str(),
          *notification.target);
    } else {
      // No provider
    }
  } else {
  }
}

// Structures functions

void HtkProviderInterface::AddValueToInterface(hString key, hString value) {
  this->InterfaceVariables.insert(key, value);
}

void HtkProviderInterface::SetValueToInterface(hString key, hString value) {
  this->InterfaceVariables.insert(key, value);
}

hString HtkProviderInterface::GetInterfaceVariable(hString key) {
  hString value = this->InterfaceVariables.at(key);
  return value;
}

HToolkitNotification HToolkitNotification::InitFromData(const char *Buffer) {
  HToolkitNotification notification;
  const char *selectionBuffer = Buffer;

const char *start = strchr(selectionBuffer, ':');
const char *end = nullptr;

start = strchr(selectionBuffer, ':');
if (start != nullptr) {
  const char *end = strchr(start + 1, ':');
  if (end != nullptr) {
    size_t length = end - start - 1;
    char entry[length + 1];
    strncpy(entry, start + 1, length);
    entry[length] = '\0';

    notification.version = entry;
  } else {
    notification.version = "null";
  }
} else {
  notification.version = "null";
}


  start = strchr(selectionBuffer, '{');
  end = strchr(selectionBuffer, '<');

  if (start != nullptr && end != nullptr && start < end) {
    size_t length = end - start - 1;
    char entry[length + 1];
    strncpy(entry, start + 1, length);
    entry[length] = '\0';

    const char *result = entry;
    notification.tag = entry;
  } else {
    notification.tag = "null";
  }

  start = strchr(selectionBuffer, '<');
  end = strchr(selectionBuffer, '>');

  if (start != nullptr && end != nullptr && start < end) {
    size_t length = end - start - 1;
    char entry[length + 1];
    strncpy(entry, start + 1, length);
    entry[length] = '\0';

    const char *result = entry;
    notification.type = result;
  } else {
    notification.type = "null";
  }

    start = strchr(selectionBuffer, '(');
  end = strchr(selectionBuffer, ')');

  if (start != nullptr && end != nullptr && start < end) {
    size_t length = end - start - 1;
    char entry[length + 1];
    strncpy(entry, start + 1, length);
    entry[length] = '\0';

    hString inputStr(entry);
    size_t pos = 0;

    while (pos < inputStr.size()) {
      size_t colonPos = inputStr.find(":", pos);

      if (colonPos != hString::npos) {
        hString key = inputStr.substr(pos, colonPos - pos);
        pos = colonPos + 1;

        if (key.find("\"") != hString::npos) {
          size_t openingQuote = key.find("\"");
          size_t closingQuote = key.find("\"", openingQuote + 1);
          if (closingQuote != hString::npos) {
            key = key.substr(openingQuote + 1, closingQuote - openingQuote - 1);
          }
        }

        if (pos < inputStr.size() && inputStr[pos] == '"') {
          pos++;
          size_t closingQuote = inputStr.find("\"", pos);
          while (closingQuote != hString::npos && inputStr[closingQuote - 1] == '\\') {
            closingQuote = inputStr.find("\"", closingQuote + 1);
          }

          if (closingQuote != hString::npos) {
            hString value = inputStr.substr(pos, closingQuote - pos);
            pos = closingQuote + 1;
            notification.AddParameter(key, value);
          } else {
          }
        } else {
          size_t commaPos = inputStr.find(",", pos);
          if (commaPos != hString::npos) {
            hString value = inputStr.substr(pos, commaPos - pos);
            pos = commaPos + 1;
            notification.AddParameter(key, value);
          } else {
            hString value = inputStr.substr(pos);
            notification.AddParameter(key, value);
            break;
          }
        }
      } else {
        // Handle error: no colon found for key
        break;
      }
    }
  } else {
    // Handle error: no parentheses found
  }

  start = strchr(selectionBuffer, '[');
  end = strchr(selectionBuffer, ']');

  if (start != nullptr && end != nullptr && start < end) {
    size_t length = end - start - 1;
    char entry[length + 1];
    strncpy(entry, start + 1, length);
    entry[length] = '\0';
    notification.body = entry;
  } else {
    notification.body = "null";
  }

  return notification;
}


hString HToolkitNotification::OutputNotification() {
  hString notification = "n:v" + this->version + ":{" + this->tag + "<" + this->type + ">(";

  bool FirstIterationPassed = false;
  for (auto param : this->params) {
    if (FirstIterationPassed)
      notification += ",";
    hString value;
    this->params.find(param.key, value);
    notification += "\"" + param.key + "\"" + ":" + "\"" + value + "\"";
    FirstIterationPassed = true;
  }
  notification += ")[" + this->body + "]}";

  return notification;
}

HTOOLKIT_API HToolkitMatrix* HToolkit::InitMatrix(hString elementtype, hArgs* args) { 
  HtkContext &ctx = *CHToolkit;
  
  HToolkitNotification* notification = args->get<HToolkitNotification*>("notification", nullptr);

  hString mid = notification->GetParameter("mid");

  HToolkitMatrix* matrix = ctx.IO.GetCacheMatrix(mid);
    if (matrix->GetState() == "waiting") { 
      matrix->SetState("active");
      ctx.IO.SwapMatrixFromCacheToActive(mid);
      //matrix->OnCustomEvent("OnAskToJoin", args);

      hArgs matrix_args;
      //matrix_args.add<hString>("channels", notification->GetParameter("chs"));
      matrix_args.add<hString>("configs", notification->GetParameter("cfg"));


      HToolkitElement* element = ElementFactory::getInstance().createInstance(elementtype.c_str());
      element->attached_matrix = matrix;
      element->id = notification->GetParameter("eid");
      element->role = "member";
      element->SetTag(elementtype);
      ctx.IO.activeRuntimeElements.push_back(element);
      matrix_args.add("element", element); 
      matrix_args.add("matrix", matrix);

      matrix->init(&matrix_args);
    }
  return nullptr;
}


HTOOLKIT_API HToolkitMatrix* HToolkit::CreateMatrixToInit(hString matrixtype, hArgs* args) { 
  HtkContext &ctx = *CHToolkit;
      HToolkitMatrix* matrix = MatrixFactory::getInstance().createInstance(matrixtype.c_str());
      matrix->SetTag(matrixtype.c_str());         
      matrix->SetState("waiting");     

      // Adding custom events from registry
      for(auto events : ctx.registeredMatrixCustomEvents.typeMap){        
        if(matrixtype == events.value->_type){         
          matrix->AddCustomEvent(events.key, events.value);
        }          
      }
      return matrix;
}


HTOOLKIT_API HToolkitMatrix* HToolkit::CreateMatrix(hString matrixtype, hString elementtype, hArgs* args) {
  HtkContext &ctx = *CHToolkit;

      HToolkitMatrix* matrix = MatrixFactory::getInstance().createInstance(matrixtype.c_str());

      matrix->SetTag(matrixtype.c_str());
      matrix->SetId(hRandom::GenerateUUIDv4());
      matrix->SetState("active");
      for (auto receiver : ctx.IO.GetAllActiveReceivers()) {
        matrix->boostrapp_receivers.push_back(*receiver);
      }
      
      HToolkitElement* element = ElementFactory::getInstance().createInstance(elementtype.c_str());
      element->tag = elementtype;
      element->attached_matrix = matrix;
      element->interface = *ctx.IO.GetAllActiveReceivers()[0];
      ctx.IO.activeRuntimeElements.push_back(element);

      ctx.IO.AddActiveMatrix(matrix);
      args->add("element", element);

      // Adding custom events from registry
      for(auto events : ctx.registeredMatrixCustomEvents.typeMap){
        if(matrixtype == events.value->_type){
          matrix->AddCustomEvent(events.key, events.value);
        }
      }

      // Construct matrix
      matrix->constructor(args);
      matrix->OnMatrixCreated();
      return matrix;
}

/*
template<typename MatrixType> // ???
HTOOLKIT_API MatrixType* HToolkit::CreateMatrix(hString matrixtype){
        HToolkitMatrix* _m = HToolkit::CreateMatrix(matrixtype);
        MatrixType* SuperMatrix = dynamic_cast<MatrixType*>(_m);
        return SuperMatrix;
}
*/

HTOOLKIT_API HToolkitElement* HToolkit::CreateElementType(hString elementtype, hArgs* args){ 
  HtkContext &ctx = *CHToolkit;
    hString str = elementtype;

    HToolkitElement* element = ElementFactory::getInstance().createInstance(elementtype.c_str());
    element->SetTag(elementtype.c_str());
    element->constructor(args);

    return element;
}



HTOOLKIT_API HToolkitChannel* HToolkitMatrix::CreateChannel(hString channeltype, hArgs* args) {
  HtkContext &ctx = *CHToolkit;
      HToolkitChannel* channel = ChannelFactory::getInstance().createInstance(channeltype);

      channel->channelType = channeltype;
      channel->SetId(hRandom::GenerateUUIDv4());
      channel->matrixID = this->GetId();
      channel->SetState("active");
      //channel.matrixID = this->GetId();
  
      // Adding custom events from registry
      for(auto events : ctx.registeredChannelCustomEvents.typeMap){
        if(channeltype == events.value->_type){
          channel->AddCustomEvent(events.key, events.value);
        }
      }

      ctx.IO.AddActiveChannel(channel);
      channel->OnChannelCreated();  
      channel->constructor(args); 
      this->OnChannelCreated(channel);

      return channel;
}

/*
HTOOLKIT_API HToolkitElement* HToolkitMatrix::CreateElement(hString elementtype, hArgs* args) {
  HtkContext &ctx = *CHToolkit;

  for (auto channelType : ctx.registeredElements.typeMap) {
    if (channelType.key == elementtype) {

      HToolkitElement element = *ElementRegistry::getInstance().createInstance(channelType.key);
      // l'ajouter au contenus de la matrice
      this->loaded_elements.push_back(&element);
      element.constructor(args);
      
      return &element;
    }
  }

  return nullptr;
}
*/

HTOOLKIT_API HToolkitChannel* HToolkitMatrix::InitChannel(hString channelID, hString channeltype){

  HtkContext &ctx = *CHToolkit;

      HToolkitChannel* channel;

      for(auto cached_chans : ctx.IO.GetAllCachedChannels()){
        if(cached_chans->GetId() == channelID){
          cached_chans->channelType = channeltype;
          cached_chans->SetId(channelID); 
          cached_chans->matrixID = this->GetId();
          cached_chans->SetState("active");
          //channel.matrixID = this->GetId();
  
          ctx.IO.SwapChannelFromCacheToActive(channelID);
          channel = cached_chans;

         
      for(auto events : ctx.registeredChannelCustomEvents.typeMap){
        if(channeltype == events.value->_type){
          channel->AddCustomEvent(events.key, events.value);
        }
      }

          this->OnChannelInitialized(channel); 
          channel->OnChannelInitialized();
          return channel;
        }

      }
          channel = ChannelFactory::getInstance().createInstance(channeltype);
          channel->channelType = channeltype;
          channel->SetId(channelID);
          channel->matrixID = this->GetId();
          channel->SetState("active");
          //channel.matrixID = this->GetId();
  
          ctx.IO.AddActiveChannel(channel);
          channel->OnChannelInitialized();

         
      for(auto events : ctx.registeredChannelCustomEvents.typeMap){
        if(channeltype == events.value->_type){
          channel->AddCustomEvent(events.key, events.value);
        }
      }
          this->OnChannelInitialized(channel);
      return channel;
    }

HTOOLKIT_API void HToolkit::AddReceiver(HtkProviderInterface *interface) {
  HtkContext &ctx = *CHToolkit;
  ctx.IO.AddActiveReceiver(interface);
}

HTOOLKIT_API hString HToolkit::GetPortal(hString mid) {
  HtkContext &ctx = *CHToolkit;

  for (auto matrixType : ctx.IO.GetAllActiveMatrixes()) {
    if (matrixType->GetId() == mid) {
      hString code = "[id(";
      code += matrixType->GetId();

      code += "),";

      // if(matrixType->tag != "null")
      //{
      code += "ty(";
      code += matrixType->GetTag();

      code += "),";
      code += "pb(";

  #ifdef USE_HSP
      unsigned char *key_bytes = NULL;
      int key_length = i2d_PrivateKey(ctx.keyPair.second, &key_bytes);

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

      for (auto receiver : matrixType->GetReceivers()) {
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
          code += params.key; 
          code += ":";
          hString value = receiver.GetInterfaceVariable(params.key);
          code += value;

          firstdf_pass = true;
        }

        code += "]";
      }

      code += ")]";
      // code = HTK_BASE64ENCODE(code) return code;
  return code;
    }
  }

  return "nullptr";
}


HTOOLKIT_API void HToolkitChannel::SendTransmission(HToolkitTransmission& transmission){
  HtkContext &ctx = *CHToolkit;
            hString term = "protocol";
  hString protocol = transmission.target->GetInterfaceVariable(term);

  if (protocol != "null") {
    HToolkitProvider *providerToUse = HToolkit::FindProvidersByProtocol(protocol);
    if (providerToUse != nullptr) {

  #ifdef USE_REGISTERING
  ctx.registeredTransmissionsSent.Add(transmission);
  #endif // USE_REGISTERING
  
      providerToUse->Send(
          (char *)transmission.OutputTransmission().c_str(),
          *transmission.target);
    } else {
      // No provider
    }
  } else {
  }

}


HTOOLKIT_API void HToolkitChannel::SendTransmission(hArgs* args){
  HtkContext &ctx                 = *CHToolkit;
  hString version                 = args->get<hString>("version", "?");
  hString buffer                  = args->get<hString>("buffer", "null");
  HtkProviderInterface* provider  = args->get<HtkProviderInterface*>("provider", nullptr);
  HToolkitChannel* channel        = args->get<HToolkitChannel*>("channel", nullptr);
  HToolkitMatrix* matrix          = ctx.IO.GetActiveMatrix(channel->matrixID);
  int cutting_limit               = stringtoint(args->get<hString>("cutting_limit", "1024").c_str());
  
  if(buffer.length() < cutting_limit){ 
        HToolkitTransmission transmission; 
        transmission.version = version;
        transmission.AttachBuffer(buffer);
        transmission.target = provider;  
        transmission.channel = channel->GetId();  
        transmission.originID = matrix->loaded_self->id;  
        this->SendTransmission(transmission);
  }
  else {
    int allSequences = buffer.size() / cutting_limit;
    return;

    for (int i = 0; i < allSequences; ++i) {
        HToolkitTransmission seq_transmission;
        seq_transmission.version = version;
        int begin = i * cutting_limit;
        int fin = begin + cutting_limit;
        hString pieceOfBuffer = buffer.substr(begin, cutting_limit);
        HToolkitTransmission transmission;
        seq_transmission.AttachBuffer(pieceOfBuffer);
        seq_transmission.AddParameter("current_cut", i);
        seq_transmission.AddParameter("total_cuts", allSequences);
        seq_transmission.target = provider; 
        transmission.channel = channel->GetId();  
        transmission.originID = matrix->loaded_self->id;  

        this->SendTransmission(seq_transmission);
    }
  }
}


HToolkitMatrix* HtkIO::GetActiveMatrix(hString &id) {
  for (auto matrix : this->activeMatrixes) {
    if (id == matrix->GetId())
      return matrix;
  }
  return nullptr;
};


void HtkIO::AddActiveMatrix(HToolkitMatrix* matrixToAdd) {
  for(auto matrix : this->GetAllActiveMatrixes()){
    if(matrix->GetId() == matrixToAdd->GetId()){
      return;
    }
  }
  this->activeMatrixes.push_back(matrixToAdd);
  this->MetricsActiveMatrixes++;
}

void HtkIO::DropActiveMatrix(hString &id) {
  int i = 0;
  for (auto matrix : this->activeMatrixes) {
    if (id == matrix->GetId()) {
      this->activeMatrixes.erase(this->activeMatrixes.begin() + i);
      this->MetricsActiveMatrixes--;
    }
    i++;
  }
}

void HtkIO::AddCacheMatrix(HToolkitMatrix* matrixToAdd) {
  for(auto matrix : this->GetAllCachedMatrixes()){
    if(matrix->GetId() == matrixToAdd->GetId()){
      HTK_LOG("Matrix already cached !")
      return;
    }
  }
  this->cachedMatrixes.push_back(matrixToAdd);
  this->MetricsCachedMatrixes++;
}

HToolkitMatrix* HtkIO::GetCacheMatrix(hString& id) {
  for (auto matrix : this->cachedMatrixes) {
    if (id == matrix->GetId()){
      return matrix;
    }
  }
  return nullptr;
};

void HtkIO::DropCacheMatrix(hString &id) {
  int i = 0;
  for (auto matrix : this->cachedMatrixes) {
    if (id == matrix->GetId()) {
      this->cachedMatrixes.erase(this->cachedMatrixes.begin() + i);
      this->MetricsCachedMatrixes--;
    }
    i++;
  }
}

void HtkIO::SwapMatrixFromCacheToActive(hString &id) {
  for (auto matrix : this->cachedMatrixes) {
    if (id == matrix->GetId()) {
      this->AddActiveMatrix(matrix);
      this->DropCacheMatrix(id);
    }
  }
}

void HtkIO::SwapMatrixFromActiveToCache(hString &id) {
  for (auto matrix : this->activeMatrixes) {
    if (id == matrix->GetId()) {
      this->AddCacheMatrix(matrix);
      this->DropActiveMatrix(id);
    }
  }
}

void HtkIO::AddCacheChannel(HToolkitChannel* channel) {
  this->cachedChannels.push_back(channel);
  this->MetricsCachedChannels++;
}

HToolkitChannel* HtkIO::GetCacheChannel(hString &id) {
  for (auto channel : this->cachedChannels) {
    if (id == channel->GetId())
      return channel;
  }
  return nullptr;
}

void HtkIO::DropCacheChannel(hString &id) {

  int i = 0;
  for (auto channel : this->cachedChannels) {
    if (id == channel->id) {
      this->cachedChannels.erase(this->cachedChannels.begin() + i);
      this->MetricsCachedChannels--;
    }
    i++;
  }
}

void HtkIO::SwapChannelFromCacheToActive(hString &id) {
  for (auto channel : this->cachedChannels) {
    if (id == channel->id) {
      this->AddActiveChannel(channel);
      this->DropCacheChannel(id);
    }
  }
}

void HtkIO::AddActiveChannel(HToolkitChannel* channel){
  this->activeChannels.push_back(channel);
  this->MetricsActiveChannels++;
}

void HtkIO::AddTraceNotification(HToolkitNotification& notification) {
  this->tracedNotification.push_back(&notification);
  this->MetricsTracedNotification++;
}


HToolkitNotification* HtkIO::GetTraceNotification(hString &traceID){
  for (auto notification : this->tracedNotification) {
    if (traceID == notification->traceID)
      return notification;
  }
  return nullptr;
}


  void HtkIO::DropTraceNotification(hString &traceID){
  int i = 0;
  for (auto notification : this->tracedNotification) {
    if (traceID == notification->traceID) {
      this->tracedNotification.erase(this->tracedNotification.begin() + i);
      this->MetricsTracedNotification--;
    }
    i++;
  }
  }
  


HToolkitChannel* HtkIO::GetActiveChannel(hString &id){
  for (auto channel : this->activeChannels) {
    if (id == channel->GetId())
      return channel;
  }
  return nullptr;
}

  hVector<HToolkitMatrix *> HtkIO::GetAllCachedMatrixes() {
    return this->cachedMatrixes;
  };

  hVector<HToolkitMatrix *> HtkIO::GetAllActiveMatrixes() {
    return this->activeMatrixes;
  };

  hVector<HToolkitChannel *> HtkIO::GetAllCachedChannels() {
    return this->cachedChannels;
  };

  hVector<HToolkitChannel *> HtkIO::GetAllActiveChannels() {
    return this->activeChannels;
  };

void HtkIO::DropActiveChannel(hString &id){
  int i = 0;
  for (auto channel : this->activeChannels) {
    if (id == channel->id) {
      this->activeChannels.erase(this->activeChannels.begin() + i);
      this->MetricsActiveChannels--;
    }
    i++;
  }

}

void HtkIO::SwapChannelFromActiveToCache(hString &id){
  for (auto channel : this->activeChannels) {
    if (id == channel->id) {
      this->AddCacheChannel(channel);
      this->DropActiveChannel(id);
    }
  }

}


hVector<HToolkitNotification *> HtkIO::GetAllTracedNotifications() {
    return this->tracedNotification;
};

void HtkIO::AttacheElementToIO(HToolkitElement* element){
    this->activeRuntimeElements.push_back(element);
  }

void HtkIO::AddActiveReceiver(HtkProviderInterface *interface) {
    this->activeReceivers.push_back(interface);
};

hVector<HtkProviderInterface *> HtkIO::GetAllActiveReceivers() {
    return this->activeReceivers;
};




//-----------------------------------------------------------------------------
// [SECTION] ImGuiTextBuffer, ImGuiTextIndex
//-----------------------------------------------------------------------------

// On some platform vsnprintf() takes va_list by reference and modifies it.
// va_copy is the 'correct' way to copy a va_list but Visual Studio prior to
// 2013 doesn't have it.
#ifndef va_copy
#if defined(__GNUC__) || defined(__clang__)
#define va_copy(dest, src) __builtin_va_copy(dest, src)
#else
#define va_copy(dest, src) (dest = src)
#endif
#endif

char hString::EmptyString[1] = {0};

void hString::append(const char *str, const char *str_end) {
  int len = str_end ? (int)(str_end - str) : (int)strlen(str);

  // Add zero-terminator the first time
  const int write_off = (Buf.Size != 0) ? Buf.Size : 1;
  const int needed_sz = write_off + len;
  if (write_off + len >= Buf.Capacity) {
    int new_capacity = Buf.Capacity * 2;
    Buf.reserve(needed_sz > new_capacity ? needed_sz : new_capacity);
  }

  Buf.resize(needed_sz);
  memcpy(&Buf[write_off - 1], str, (size_t)len);
  Buf[write_off - 1 + len] = 0;
}

  hString HToolkitMatrix::OutputChannelString(){
        HtkContext &ctx = *CHToolkit;
        hString o;
        bool first = true;
        hVector<HToolkitChannel*> chans;
        
        for(auto c : ctx.IO.GetAllActiveChannels()){if(c->matrixID == this->GetId()){chans.push_back(c);};};
        for(auto c : ctx.IO.GetAllCachedChannels()){if(c->matrixID == this->GetId()){chans.push_back(c);};};

        for(auto config : chans){
            o += config->id;
            if(!first){o += ",";};
            first = false;
        }

        if(o.empty()){o = "none";};

        return o;
};

    hString HToolkitChannel::OutputElementString(){
        hString o;
        bool first = true;
        for(auto config : this->props){
            if(!first){o += ",";};
            o += config.key;
            o += "=";
            o += config.value;
            first = false;
        }

        if(o.empty()){o = "none";};

        return o;
};

    void HToolkitChannel::InputElementString(hString& str)
{
    size_t pos = 0;
    hString string = str.c_str();


   while (true) {
    size_t equalPos = string.find("=", pos);
    size_t semicolonPos = string.find(",", pos);

    if (equalPos != std::string::npos) {
        if (semicolonPos != std::string::npos) {
            hString key = string.substr(pos, equalPos - pos);
            hString value = string.substr(equalPos + 1, semicolonPos - equalPos - 1);

            try {
                HToolkitMatrix_Props prop = stringtoint(key.c_str());
                this->SetProp(prop, value);
            } catch (const std::exception& e) {
            }
            pos = semicolonPos + 1;
        } else {
            hString key = string.substr(pos, equalPos - pos);
            hString value = string.substr(equalPos + 1);
            try {
                HToolkitMatrix_Props prop = stringtoint(key.c_str());
                this->SetProp(prop, value);
            } catch (const std::exception& e) {
            }
            break; 
        }
    } else {
        break;
    }
}

}


hString HToolkitChannel::OutputConfigString(){
        hString o;
        bool first = true;
        for(auto config : this->props){
            if(!first){o += ",";};
            o += config.key;
            o += "=";
            o += config.value;
            first = false;
        }

        if(o.empty()){o = "none";};

        return o;
};

    void HToolkitChannel::InputConfigString(hString& str)
{
    size_t pos = 0;
    hString string = str.c_str();


   while (true) {
    size_t equalPos = string.find("=", pos);
    size_t semicolonPos = string.find(",", pos);

    if (equalPos != std::string::npos) {
        if (semicolonPos != std::string::npos) {
            hString key = string.substr(pos, equalPos - pos);
            hString value = string.substr(equalPos + 1, semicolonPos - equalPos - 1);

            try {
                HToolkitMatrix_Props prop = stringtoint(key.c_str());
                this->SetProp(prop, value);
            } catch (const std::exception& e) {
            }
            pos = semicolonPos + 1;
        } else {
            hString key = string.substr(pos, equalPos - pos);
            hString value = string.substr(equalPos + 1);
            try {
                HToolkitMatrix_Props prop = stringtoint(key.c_str());
                this->SetProp(prop, value);
            } catch (const std::exception& e) {
            }
            break; 
        }
    } else {
        break;
    }
}

}




HTOOLKIT_API HToolkitElement* HToolkit::InvokeElementFromInterface(HtkProviderInterface& _interface, hString elementId){
      HToolkitElement* newelement = new HToolkitElement;
      newelement->interface = _interface;
      newelement->id = elementId;
      return newelement;
}


HTOOLKIT_API HtkProviderInterface HToolkit::InitProviderInterface(hString interfacebuffer){
         HtkProviderInterface in;
      size_t startPos = 0;
      while (true) {
        size_t openBracketPos = interfacebuffer.find("[", startPos);
        if (openBracketPos == hString::npos) {
          break;
        }

        size_t closeBracketPos = interfacebuffer.find("]", openBracketPos);
        if (closeBracketPos == hString::npos) {
          break;
        }

        hString insideBrackets = interfacebuffer.substr(
            openBracketPos + 1, closeBracketPos - openBracketPos - 1);
        size_t startPosInside = 0;

        while (startPosInside < insideBrackets.size()) {
          size_t colonPos = insideBrackets.find(":", startPosInside);
          if (colonPos == hString::npos) {
            break;
          }

          hString key =
              insideBrackets.substr(startPosInside, colonPos - startPosInside);
          startPosInside = colonPos + 1;

          size_t commaPos = insideBrackets.find(";", startPosInside);
          if (commaPos == hString::npos) {
            commaPos = insideBrackets.size();
          }

          hString value =
              insideBrackets.substr(startPosInside, commaPos - startPosInside);
          startPosInside = commaPos + 1;

          in.AddValueToInterface(key, value);
        }

        startPos = closeBracketPos + 1;
      }
      return in;
}


HTOOLKIT_API hVector<hString> HToolkit::InputStringVector(hString& string)
{
  hVector<hString> elements;
    size_t pos = 0;
    size_t commaPos = string.find(",");

    while (commaPos != hString::npos) {
        hString element = string.substr(pos, commaPos - pos);
        elements.push_back(element);

        pos = commaPos + 1;
        commaPos = string.find(",", pos);
    }
    hString lastElement = string.substr(pos);
    if (!lastElement.empty()) {
        elements.push_back(lastElement);
    }
    return elements;
}

 void HToolkitChannel::AddCustomEvent(hString name, HToolkitCustomEvent* event){
        this->available_events.push_back(name);
        event->eventName = name;
        this->custom_events.insert(name, event);
    }

void HToolkitChannel::OnCustomEvent(hString name, hArgs* args) {
    for (auto events : this->available_events) {
        if (events == name) {
            HToolkitCustomEvent* event = this->custom_events.find(name);
            if (event) {
                event->on(args);
            } else {
                HTK_LOG("Event Not Found")
            }
            return;
        }
    }
    HTK_LOG("No event implemented for : " << name.c_str());
}


void HToolkitTransmission::Configure(hString _channel, hString _version){this->channel = _channel; this->version = _version;}
    void HToolkitTransmission::AttachBuffer(hString& _buffer){
        this->buffer.clear();
        this->buffer = _buffer;
        }

    void HToolkitTransmission::init(int lenght, int seq, int tseq){
        this->buffer = this->type.output();
    }

    hString HToolkitTransmission::OutputTransmission(){
        hString data;

        data += "t:v";
        data += this->version;
        data += ":{";
        data += "::" + this->channel;

        if(this->originID.length() == 0){
        data += "::null";
        }
        else{
        data += "::" + this->originID;
        }

        data += "(";
          bool FirstIterationPassed = false;
  for (auto param : this->params) {
    if (FirstIterationPassed)
      data += ",";
    hString value;
    this->params.find(param.key, value);
    data += "\"" + param.key + "\"" + ":" + "\"" + value + "\"";
    FirstIterationPassed = true;
  }


        data += ")";


        data += "[";
        data += this->buffer;
        data += "]";
        data += "}";

        return data;
    };
    

HToolkitTransmission HToolkitTransmission::InitFromData(const char* data){
         HToolkitTransmission transmission;
  const char *selectionBuffer = data;

  hString input = data;
  hString parameters;

    size_t firstDoubleColon = input.find("::");
    if (firstDoubleColon != hString::npos) {

        size_t vPosition = input.find("v");
        transmission.version = input.substr(vPosition + 1, firstDoubleColon - vPosition - 3);
        size_t secondDoubleColon = input.find("::", firstDoubleColon + 2);
        if (secondDoubleColon != hString::npos) {

            transmission.channel = input.substr(firstDoubleColon + 2, secondDoubleColon - firstDoubleColon - 2);
            size_t openParenthesis = input.find("(", secondDoubleColon);
            if (openParenthesis != hString::npos) {

                transmission.originID = input.substr(secondDoubleColon + 2, openParenthesis - secondDoubleColon - 2);
                size_t closeParenthesis = input.find(")", openParenthesis);
                if (closeParenthesis != hString::npos) {

                    parameters = input.substr(openParenthesis + 1, closeParenthesis - openParenthesis - 1);
                    size_t openBracket = input.find("[", closeParenthesis);
                    if (openBracket != hString::npos) {
                            transmission.buffer = input.substr(openBracket + 1, input.length() - 3);
                    }
                }
            }
        }
    }


    transmission.AddParameter("data_lenght", input.length());
    transmission.AddParameter("buffer_lenght", transmission.buffer.length());

    return transmission;

  }


    void HToolkitMatrix::AddCustomEvent(hString name, HToolkitCustomEvent* event){
        this->available_events.push_back(name);
        event->eventName = name;
        this->custom_events.insert(name, event);
    }


    void HToolkitMatrix::AddCustomEvent(hString name, HToolkitCustomEvent* event, HToolkitMatrix* matrix){
        matrix->available_events.push_back(name);
        event->eventName = name;
        matrix->custom_events.insert(name, event);
    }
    

void HToolkitMatrix::OnCustomEvent(hString name, hArgs* args) {
    for (auto events : this->available_events) {
        if (events == name) {
            HToolkitCustomEvent* event = this->custom_events.find(name);
                event->on(args);
            return;
        }
    }
    HTK_LOG("No event implemented for : " << name.c_str());
}



    void HToolkitMatrix::InputChannelString(hString& string)
    {
        hString element = "";
    for (char c : string) {
        if (c != ',') {
            element += c;
        } else {
            this->OnChannelDiscovered(string);


            element = "";
        }
    }
    
    if (!element.empty()) {
            this->OnChannelDiscovered(string);
    }

    }

    hString HToolkitMatrix::OutputConfigString(){
        hString o;
        bool first = true;
        for(auto config : this->props){
            if(!first){o += ",";};
            o += config.key;
            o += "=";
            o += config.value;
            first = false;
        }

        if(o.empty()){o = "none";};

        return o;
};

    void HToolkitMatrix::InputConfigString(hString& str)
{
    size_t pos = 0;
    hString string = str.c_str();

   while (true) {
    size_t equalPos = string.find("=", pos);
    size_t semicolonPos = string.find(",", pos);

    if (equalPos != hString::npos) {
        if (semicolonPos != hString::npos) {
            hString key = string.substr(pos, equalPos - pos);
            hString value = string.substr(equalPos + 1, semicolonPos - equalPos - 1);

            try {
                HToolkitMatrix_Props prop = stringtoint(key.c_str());
                this->SetProp(prop, value);
            } 
            catch (int i) {
            }
            pos = semicolonPos + 1;
        } else {
            hString key = string.substr(pos, equalPos - pos);
            hString value = string.substr(equalPos + 1);
            try {
                HToolkitMatrix_Props prop = stringtoint(key.c_str());
                this->SetProp(prop, value);
            }
            catch (int i) {
            }
            break; 
        }
    } else {
        break;
    }
}

}

   void HToolkitNode::init(HToolkitElement* _a, HToolkitElement* _b, hString _nodeID){
        this->a = _a;
        this->b = _b;
        this->nodeID = _nodeID;
    }


    void HToolkitElement::SetTag(hString newtag){this->tag = newtag;}

// Utilities : 
hString hTime::GetCurrentTimestamp(){
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
    return dtimestamp;
}

hString hRandom::GenerateUUIDv4() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);

    const char *hex_chars = "0123456789abcdef";
    hString ss;

    for (int i = 0; i < 36; ++i) {
      if (i == 8 || i == 13 || i == 18 || i == 23) {
        ss += "-";
      } else if (i == 14) {
        ss += "4";
      } else if (i == 19) {
        ss += hex_chars[dis(gen) & 0x3 | 0x8]; 
      } else {
        ss += hex_chars[dis(gen)]; 
      }
    }
    return ss;
  }


  
hString hRandom::Generate6CharID() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(100000, 999999); 

    hString randomCode;

    int randomNumber = dis(gen);
    for (int i = 0; i < 6; ++i) {
        char digit = '0' + (randomNumber % 10);
        randomCode = digit + randomCode;  
        randomNumber /= 10;
    }

    return randomCode;
}


//==================================================================================================================
// Internals Hypernet features
//=================================================================================================================

// Ping, Pong, Trace
HTOOLKIT_API void HToolkit::PingElement(HToolkitElement& e){
    HToolkitNotification notification;
    notification.Configure("::ping", "", "1", e.interface);
    HToolkit::SendNotification(notification);
}

HTOOLKIT_API void PongElement(HToolkitElement& e){
    HToolkitNotification notification;
    notification.Configure("::pong", "", "1", e.interface);
    HToolkit::SendNotification(notification);
}

// Boostrapping
HTOOLKIT_API void QuickJoinToMatrix();

HTOOLKIT_API void HToolkit::AskToJoinChannel(HToolkitChannel* channel, HToolkitElement* elementToAsk, HToolkitElement* askElement){
    HtkContext &ctx = *CHToolkit;
    HToolkitNotification notification;
    notification.Configure("::ask", "JoinChannel", "1", elementToAsk->interface);
    notification.AddParameter("cid", channel->id);
    notification.AddParameter("eid", askElement->id);

    channel->state = "waiting";
    ctx.IO.AddCacheChannel(channel);
    HToolkit::SendNotification(notification);
}

HTOOLKIT_API void HToolkit::AskToJoinMatrix(HToolkitMatrix* matrix){
    HtkContext &ctx = *CHToolkit;
    HToolkitNotification notification;

    HtkProviderInterface interface = matrix->GetReceivers()[0]; // need a queue

    notification.Configure("::ask", "JoinMatrix", "1", interface, true);
    notification.AddParameter("rec",(char *)interface_to_string(interface).c_str());
    notification.AddParameter("mid", matrix->GetId());

    hString runtimeElements;
    bool firstpass = false;
    for(auto ety : ctx.IO.activeRuntimeElements){
      if(firstpass){runtimeElements += ",";}
      runtimeElements += ety->tag;
      firstpass = true;
    }


    notification.AddParameter("ety", runtimeElements);

    matrix->SetState("waiting");
    ctx.IO.AddCacheMatrix(matrix);
    HToolkit::SendNotification(notification);
}

HTOOLKIT_API void HToolkit::AskForMatrixUpdate(HToolkitMatrix* matrix){
    HtkContext &ctx = *CHToolkit;
    HToolkitNotification notification;

    HtkProviderInterface interface = matrix->GetReceivers()[0]; // need a queue

    notification.Configure("::ask", "MatrixUpdate", "1", interface, true);
    notification.AddParameter("mid", matrix->GetId());

    for(auto matrix : ctx.IO.GetAllActiveMatrixes()){
      for(auto element : ctx.IO.activeRuntimeElements){
        if(element->attached_matrix->GetId() == matrix->GetId()){
          notification.AddParameter("eid", element->id);
          HToolkit::SendNotification(notification);
        }
      }
    }
}

HTOOLKIT_API void HToolkit::WelcomeElementOnChannel(HToolkitElement& element, HToolkitChannel& channel){
    HToolkitNotification notification;
    notification.Configure("::welcome", "OnChannel", "1", element.interface);
    notification.AddParameter("ty", channel.channelType);
    notification.AddParameter("mid", channel.matrixID);
    notification.AddParameter("cfg", channel.OutputConfigString());
    notification.AddParameter("cid", channel.GetId());
    //notification.AddParameter("ele", channel.OutputElementString()); 
    HToolkit::SendNotification(notification);
}


HTOOLKIT_API void HToolkit::WelcomeElementOnMatrix(HToolkitElement& element, HToolkitMatrix& matrix, hString traceID){
    HToolkitNotification notification;
    notification.Configure("::welcome", "OnMatrix", "1", element.interface, false);

   // if(traceID != "0"){notification.AddParameter("tid", traceID);}

    notification.AddParameter("cfg", matrix.OutputConfigString());
    //notification.AddParameter("chs", matrix.OutputChannelString()); 
    notification.AddParameter("mid", matrix.GetId());
    notification.AddParameter("ety", element.tag);
    notification.AddParameter("eid", element.id);
    HToolkit::SendNotification(notification);
}

HTOOLKIT_API void HToolkit::UpdateElement(hArgs* args){
    HToolkitNotification notification;
    HToolkitMatrix* matrix = args->get<HToolkitMatrix*>("matrix", nullptr);
    HToolkitElement* elementToUpdate = args->get<HToolkitElement*>("element", nullptr); 
    notification.AddParameter("mid", matrix->GetId());


    hString list;

    bool first = true;

      for(auto element : matrix->loaded_elements){
        if(element->id.c_str() != elementToUpdate->id.c_str()){
          notification.Configure("::update", "Element", "1", element->interface, false);
          notification.SetParameter("ety", elementToUpdate->tag);
          notification.SetParameter("eid", elementToUpdate->id);
          notification.SetParameter("ero", elementToUpdate->role);
          notification.SetParameter("rec", interface_to_string(elementToUpdate->interface));
          HToolkit::SendNotification(notification);
        }
    }
}
      
HTOOLKIT_API void HToolkit::UpdateChannelElementsList(hArgs* args){
    HtkContext &ctx = *CHToolkit;
    HToolkitNotification notification;
    hString cid = args->get<hString>("cid", "null");
    HToolkitElement* element = args->get<HToolkitElement*>("element", nullptr);
    HToolkitChannel* channel = ctx.IO.GetActiveChannel(cid);
    HToolkitMatrix* matrix = ctx.IO.GetActiveMatrix(channel->matrixID);


    notification.Configure("::update", "ChannelElementsList", "1", element->interface, false);
    hString list;

    bool first = true;

      for(auto element : channel->loaded_elements){
        if(!first){
          list += ",";
        }
        if(first){
          list += matrix->loaded_self->id;
        }
        if(!first){
          list += element->id;
        }
        first = false;
      }

    notification.AddParameter("els",list); 
    notification.AddParameter("cid",cid); 
    HToolkit::SendNotification(notification);
}


HTOOLKIT_API void HToolkit::UpdateElementsList(hArgs* args){
    HToolkitNotification notification;
    HToolkitMatrix* matrix = args->get<HToolkitMatrix*>("matrix", nullptr);
    HToolkitElement* element = args->get<HToolkitElement*>("element", nullptr);
    notification.AddParameter("mid", matrix->GetId());


    notification.Configure("::update", "ElementsList", "1", element->interface, false);
    hString list;

    bool first = true;

      for(auto element : matrix->get_registered_element(args)){
        if(!first){
          list += ",";
        }
        list += element->id;
        list += ";";
        list += element->tag;
        list += ";";
        list += element->role;
        list += ";";
        list += (char *)interface_to_string(element->interface).c_str();

        first = false;
      }

    notification.AddParameter("els",list); 
    HToolkit::SendNotification(notification);
}



HTOOLKIT_API void HToolkit::UpdateMatrixChannelsList(hArgs* args){
    HtkContext &ctx = *CHToolkit;
    HToolkitMatrix* matrix = args->get<HToolkitMatrix*>("matrix", nullptr);
    HToolkitElement* element = args->get<HToolkitElement*>("element", nullptr);
    HToolkitNotification notification;


      notification.Configure("::update", "ChannelsList", "1", element->interface, false);

      hString list;
      bool first = true;

      for(auto channel : ctx.IO.GetAllActiveChannels()){
        if(!first){
          list += ",";
        }
        if(matrix->GetId() == channel->matrixID){
          list += channel->GetId();
          list += ";";
          list += channel->channelType;
        }
        first = false;
      }

      notification.AddParameter("chs",list);
      notification.AddParameter("mid", matrix->GetId()); 
      HToolkit::SendNotification(notification);


}

HTOOLKIT_API void HToolkit::ReportErrorWhileJoinMatrix(HtkProviderInterface interface, hString details){
    HToolkitNotification notification;

    notification.Configure("::error", "whileJoinMatrix", "1", interface);
    notification.SetBody(details.c_str());

    HToolkit::SendNotification(notification);
}

HTOOLKIT_API void HToolkit::ReportErrorWhileJoinChannel(/*Error Flag*/){
    HToolkitNotification notification;
    //notification->Configure("::error", "JoinChannel", "1", element->interface);
    notification.AddParameter("det", "Details here");
    HToolkit::SendNotification(notification);
}   

HTOOLKIT_API void HToolkit::ReportErrorWhileGetChannelList(/*Error Flag*/){
    HToolkitNotification notification;
    //notification->Configure("::error", "GetChannelList", "1", element->interface);
    notification.AddParameter("det", "Details here");
    HToolkit::SendNotification(notification);
}   

HTOOLKIT_API void HToolkit::ReportErrorWhileGetElementList(/*Error Flag*/){
    HToolkitNotification notification;
    //notification->Configure("::error", "GetElementList", "1", element->interface);
    notification.AddParameter("det", "Details here");
    HToolkit::SendNotification(notification);
}   

HTOOLKIT_API void HToolkit::ReportErrorWhileGetMatrixList(/*Error Flag*/){
    HToolkitNotification notification;
    //notification->Configure("::error", "GetMatrixList", "1", element->interface);
    notification.AddParameter("det", "Details here");
    HToolkit::SendNotification(notification);
}   

HTOOLKIT_API void HToolkit::ResultMatrixList(){
    HToolkitNotification notification;
    //notification->Configure("::result", "MatrixList", "1", element->interface);
    notification.AddParameter("mal", "");
    HToolkit::SendNotification(notification);
}

HTOOLKIT_API void HToolkit::ResultElementList(){
    HToolkitNotification notification;
    //notification->Configure("::result", "ElementList", "1", element->interface);
    notification.AddParameter("ell", "");
    HToolkit::SendNotification(notification);
}

HTOOLKIT_API void HToolkit::ResultChannelList(){
    HToolkitNotification notification;
    //notification->Configure("::result", "ChannelList", "1", element->interface);
    notification.AddParameter("chl", "");
    HToolkit::SendNotification(notification);
}


HTOOLKIT_API void HToolkit::GetMatrixList(){ 

}

HTOOLKIT_API void HToolkit::GetElementList(HToolkitChannel& channel){
    HToolkitNotification notification;
    HtkContext &ctx = *CHToolkit;
    HToolkitMatrix* matrix = ctx.IO.GetActiveMatrix(channel.matrixID);

    //notification->Configure("::get", "ChannelList", "1", );

    HToolkit::SendNotification(notification);
}


HTOOLKIT_API void HToolkit::GetElementList(HToolkitMatrix& matrix){
    HToolkitNotification notification;
    //notification->Configure("::result", "ChannelList", "1", element->interface);



    HToolkit::SendNotification(notification);
  
}
             
HTOOLKIT_API void HToolkit::GetChannelList(HToolkitMatrix& matrix){
    HToolkitNotification notification;
    //notification->Configure("::result", "ChannelList", "1", element->interface);
    HToolkit::SendNotification(notification);
}


HTOOLKIT_API void HToolkit::ReportNotImplementedError(HToolkitNotification& initialNotification){
    HToolkitNotification notification;
    notification.Configure("::error", "noImplemented", "1", *initialNotification.origin);
    notification.AddParameter("tra", "???");
    HToolkit::SendNotification(notification);
}



#endif // VORTEX_DISABLE