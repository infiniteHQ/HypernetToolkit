#include "htk_internals.h"

//_____________________________________________________________________________
// (Utilities) Basic utils
//_____________________________________________________________________________

// Convert interface information to string
static hString interface_to_string(HtkProviderInterface &interface)
{
  HtkContext &ctx = *CHToolkit;
  hString receiver = "[";
  for (auto *prov : ctx.IO.GetAllActiveReceivers())
  {
    if (interface.GetInterfaceVariable("protocol") ==
        prov->GetInterfaceVariable("protocol"))
    {
      bool first_pass = false;
      for (auto param : prov->InterfaceVariables)
      {
        if (first_pass)
        {
          receiver += ";";
        }
        hString value = prov->GetInterfaceVariable(param.key);
        receiver += param.key;
        receiver += ":";
        receiver += value;
        first_pass = true;
      }
    }
  }
  receiver += "]";
  return receiver;
}

//_____________________________________________________________________________

//_____________________________________________________________________________
// (Core) Absolute core channels
//_____________________________________________________________________________
namespace Channels
{

  //-----------------------------------------------------------------------------
  // Channel : hello_world (::hello_world in runtime)
  //-----------------------------------------------------------------------------
  struct hello_world : public HToolkitChannel
  {
    void OnElementConnect(HToolkitElement &e) override
    {
      HtkContext &ctx = *CHToolkit;
    };

    void OnChannelReceived(HToolkitTransmission &transmission) override
    {
    }
  };
  // CORE_CHANNEL(hello_world);
  //-----------------------------------------------------------------------------

}; // namespace Channels
//_____________________________________________________________________________

//_____________________________________________________________________________
// (Core) Absolute core matrixes
//_____________________________________________________________________________
namespace CoreMatrixes
{

  //-----------------------------------------------------------------------------
  // Matrix : decentralized_nodes (::decentralized_nodes in runtime)
  //-----------------------------------------------------------------------------
  struct decentralized_nodes : public HToolkitMatrix
  {
    decentralized_nodes()
    {
      this->SetTag("::decentralized_nodes");
      // this->GetEvents()->onMatrixCreated();

      this->SetProp(HToolkitMatrix_Props_Limits_MaxNodes, "128");
    };

    void OnChannelDiscovered(hString id) override
    {
      HtkContext &ctx = *CHToolkit;
      HToolkitChannel channel;
      channel.matrixID = this->id;
      channel.id = id;
      ctx.IO.AddCacheChannel(&channel);
    }
    hVector<std::pair<hString, HToolkitElement>> KnowNodes;
  };
  // CORE_MATRIX(decentralized_nodes);
  //-----------------------------------------------------------------------------

}; // namespace CoreMatrixes
//_____________________________________________________________________________

//_____________________________________________________________________________
// (Core) Absolute core matrixes
//_____________________________________________________________________________
namespace NotificationsHandlers
{

  //=============================================================================
  // Notification handler : pong
  // (::pong in runtime & communications [from Hypernet Communication Protocol])
  //=============================================================================
  struct pong : public HToolkitNotificationHandler
  {
    void OnNotificationReceived(HToolkitNotification &notification) override{

    };
  };
  //=============================================================================

  //=============================================================================
  // Notification handler : ping
  // (::ping in runtime & communications [from Hypernet Communication Protocol])
  //=============================================================================
  // Types :
  // <>
  // <OverMatrix>
  //=============================================================================
  // - OnReceived :
  //=============================================================================
  struct ping : public HToolkitNotificationHandler
  {
    void OnNotificationReceived(HToolkitNotification &notification) override{};
  };
  //=============================================================================

  //-----------------------------------------------------------------------------
  // Notification handler : error
  // (::error in runtime & communications [from Hypernet Communication Protocol])
  //-----------------------------------------------------------------------------
  struct error : public HToolkitNotificationHandler
  {
    void OnNotificationReceived(HToolkitNotification &notification) override
    {
      switch (str2int(notification.type.c_str()))
      {
      case str2int("whileJoiningMatrix"):
      {
        break;
      }

      case str2int("WhileJoiningChannel"):
      {
        break;
      }
      }
    };
  };
  //-----------------------------------------------------------------------------

  //-----------------------------------------------------------------------------
  // Notification handler : error
  // (::error in runtime & communications [from Hypernet Communication Protocol])
  //-----------------------------------------------------------------------------
  struct update : public HToolkitNotificationHandler
  {
    void OnNotificationReceived(HToolkitNotification &notification) override
    {
      HtkContext &ctx = *CHToolkit;
      hString mid = notification.GetParameter("mid");
      HToolkitMatrix *matrix = ctx.IO.GetActiveMatrix(mid);

      switch (str2int(notification.type.c_str()))
      {

      case str2int("MatrixElementsList"):
      {
        hString els = notification.GetParameter("els");
        hVector<hString> elements = HToolkit::InputStringVector(els);
        hArgs matrix_args;
        matrix_args.add("elements", elements);
        matrix->OnCustomEvent("UpdateElements", &matrix_args);
        break;
      }

      case str2int("Element"):
      {
        hString ety = notification.GetParameter("ety");
        hString eid = notification.GetParameter("eid");
        hString ero = notification.GetParameter("ero");
        hString rec = notification.GetParameter("rec");

        HToolkitElement *updatedEllement = new HToolkitElement;
        updatedEllement->SetTag(ety);
        updatedEllement->id = eid;
        updatedEllement->role = ero;
        updatedEllement->interface = HToolkit::InitProviderInterface(rec);

        hArgs matrix_args;
        matrix_args.add("element", updatedEllement);
        matrix_args.add("matrix", matrix);
        matrix->OnCustomEvent("OnNewElement", &matrix_args);
        break;
      }

      case str2int("ElementsList"):
      {
        hString els = notification.GetParameter("els");
        hVector<hString> elements = HToolkit::InputStringVector(els);

        hArgs matrix_args;
        matrix_args.add("matrix", matrix);
        matrix->OnCustomEvent("OnNewElementList", &matrix_args);

        for (auto element : elements)
        {

          size_t pos1 = element.find(";");
          if (pos1 != hString::npos)
          {
            hString eid = element.substr(0, pos1);

            size_t pos2 = element.find(";", pos1 + 1);
            if (pos2 != hString::npos)
            {
              hString type = element.substr(pos1 + 1, pos2 - pos1 - 1);
              HToolkitElement *nelement = ElementFactory::getInstance().createInstance(type.c_str());
              nelement->id = eid;
              nelement->tag = type;

              size_t pos3 = element.find(";", pos2 + 1);
              if (pos3 != hString::npos)
              {
                nelement->role = element.substr(pos2 + 1, pos3 - pos2 - 1);
                nelement->interface = HToolkit::InitProviderInterface(element.substr(pos3 + 1));
                matrix_args.add("element", nelement);
                matrix->OnCustomEvent("OnNewElement", &matrix_args);
              }
            }
          }
        }
        break;
      }

      case str2int("ChannelElementsList"):
      {
        hString cid = notification.GetParameter("cid");
        HToolkitChannel* channel = ctx.IO.GetActiveChannel(cid);

        hString els = notification.GetParameter("els");
        hVector<hString> elements = HToolkit::InputStringVector(els);

        hArgs channel_args;
        channel_args.add("channel", channel);
        channel->OnCustomEvent("OnNewElementList", &channel_args);

        for (auto element : elements)
        {
          channel_args.add("eid", element);
          channel_args.add("channel", channel);
          channel->OnCustomEvent("OnNewElement", &channel_args);
        }
        break;
      }

      case str2int("ChannelsList"):
      {

        hString chs = notification.GetParameter("chs");
        hString mid = notification.GetParameter("mid");
        hVector<hString> channels = HToolkit::InputStringVector(chs);
        hArgs matrix_args;
        matrix_args.add("channels", channels);
        HToolkitMatrix *matrix = ctx.IO.GetActiveMatrix(mid);
        for (auto chan : channels)
        {
          size_t pos = chan.find(";");

          if (pos != hString::npos)
          {
            // Extraire les deux sous-chaînes
            hString id = chan.substr(0, pos);
            hString tag = chan.substr(pos + 1);
            HToolkitChannel *chan = ChannelFactory::getInstance().createInstance(tag);
            chan->SetId(id);
            chan->channelType = tag;
            bool isExisting = false;
            for (auto register_chan : matrix->get_registered_channels(&matrix_args))
            {
              if (id == register_chan->GetId())
              {
                isExisting = true;
              }
            }

            if (!isExisting)
            {
              matrix->add_registered_channel(chan);
            }
          }
        }
        matrix->OnCustomEvent("UpdateChannels", &matrix_args);

        break;
      }
      case str2int("MatrixNodesList"):
      {
        break;
      }

      case str2int("MatrixProps"):
      {
        break;
      }
      }
    };
  };
  //-----------------------------------------------------------------------------

  //-----------------------------------------------------------------------------
  // Notification handler : welcome
  // (::welcome in runtime & communications [from Hypernet Communication
  // Protocol])
  //-----------------------------------------------------------------------------
  struct welcome : public HToolkitNotificationHandler
  {
    void OnNotificationReceived(HToolkitNotification &notification) override
    {

      switch (str2int(notification.type.c_str()))
      {

      case (str2int("OnMatrix")):
      {

        HtkContext &ctx = *CHToolkit;
        hArgs args;
        args.add("notification", &notification);
        HToolkit::InitMatrix(notification.GetParameter("ety"), &args);
        break;
      }
      case (str2int("OnChannel")):
      {
        HtkContext &ctx = *CHToolkit;
        hString mid = notification.GetParameter("mid");
        hString ty = notification.GetParameter("ty");
        hString cid = notification.GetParameter("cid");
        hString cfg = notification.GetParameter("cfg");
        
        HToolkitMatrix *matrix = ctx.IO.GetActiveMatrix(mid);
        HToolkitChannel *channel = matrix->InitChannel(cid, ty);
        break;
      }
      }
    }
  };
  //-----------------------------------------------------------------------------

  //-----------------------------------------------------------------------------
  // Notification handler : ask
  // (::ask in runtime & communications [from Hypernet Communication Protocol])
  //-----------------------------------------------------------------------------
  struct ask : public HToolkitNotificationHandler
  {
    /// @brief
    /// @param notification
    void OnNotificationReceived(HToolkitNotification &notification) override
    {
      HtkContext &ctx = *CHToolkit;

      switch (str2int(notification.type.c_str()))
      {
      case str2int("CreateChannel"):
        break;

      case str2int("JoinChannel"):
      {
        hString eid = notification.GetParameter("eid"); 
        hString cid = notification.GetParameter("cid"); 
        HToolkitChannel *channel = ctx.IO.GetActiveChannel(cid);
        HToolkitMatrix *matrix = ctx.IO.GetActiveMatrix(channel->matrixID);
        hArgs args;
        args.add("channel", channel);
        args.add("matrix", matrix);
        args.add("eid", eid);
        channel->OnCustomEvent("OnAskToJoin", &args);
        break;
      }
      case str2int("JoinMatrix"):
      {
        hString mid = notification.GetParameter("mid");
        HToolkitMatrix *matrix = ctx.IO.GetActiveMatrix(mid);
        hArgs args;
        args.add("notification", &notification);
        matrix->OnCustomEvent("OnAskToJoin", &args);

        break;
      }
      case str2int("MatrixUpdate"):
      {
        hString mid = notification.GetParameter("mid");
        HToolkitMatrix *matrix = ctx.IO.GetActiveMatrix(mid);
        hArgs args;
        args.add("notification", &notification);
        matrix->OnCustomEvent("OnAskToUpdate", &args);
        break;
      }
      }
    };
  };
  //-----------------------------------------------------------------------------

  //-----------------------------------------------------------------------------
  // Notification handler : result
  // (::result in runtime & communications [from Hypernet Communication Protocol])
  //-----------------------------------------------------------------------------
  struct result : public HToolkitNotificationHandler
  {
    void OnNotificationReceived(HToolkitNotification &notification) override
    {
      HtkContext &ctx = *CHToolkit;
    };
  };
  //-----------------------------------------------------------------------------

  //-----------------------------------------------------------------------------
  // Notification handler : hello
  // (::hello in runtime & communications [from Hypernet Communication Protocol])
  //-----------------------------------------------------------------------------
  struct hello : public HToolkitNotificationHandler
  {
    void OnNotificationSent(HToolkitNotification *notification) override{};
    void OnNotificationReceived(HToolkitNotification &notification) override
    {
      HtkContext &ctx = *CHToolkit;
    };
  };
  //-----------------------------------------------------------------------------

  //-----------------------------------------------------------------------------
  // Notification handler : get
  // (::get in runtime & communications [from Hypernet Communication Protocol])
  //-----------------------------------------------------------------------------
  struct get : public HToolkitNotificationHandler
  {

    // Output
    void MatrixChannelIDs();
    void MatrixChannelTypes();
    void MatrixUsers();
    void ChannelUsers();

    // Events & Inputs
    void OnNotificationReceived(HToolkitNotification &notification) override
    {
      HtkContext &ctx = *CHToolkit;

      switch (str2int(notification.type.c_str()))
      {
      default:
      case str2int("matrixSecurity"):
      {
        hString mid = notification.GetParameter("mid");
        HToolkitMatrix *matrix = ctx.IO.GetActiveMatrix(mid);
        hArgs args;
        matrix->OnCustomEvent("::get<matrixSecurity>", &args);
      }
      case str2int("matrixElements"):
      {
        hString mid = notification.GetParameter("mid");
        HToolkitMatrix *matrix = ctx.IO.GetActiveMatrix(mid);
        hArgs args;
        matrix->OnCustomEvent("::get<matrixElements>", &args);
      }
      }
    };
  };
  //-----------------------------------------------------------------------------

  CORE_NOTIFICATION_HANDLER(ping);
  CORE_NOTIFICATION_HANDLER(pong);
  CORE_NOTIFICATION_HANDLER(hello);
  CORE_NOTIFICATION_HANDLER(error);
  CORE_NOTIFICATION_HANDLER(ask);
  CORE_NOTIFICATION_HANDLER(welcome);
  CORE_NOTIFICATION_HANDLER(update);
  CORE_NOTIFICATION_HANDLER(result);
  CORE_NOTIFICATION_HANDLER(get);

  //_____________________________________________________________________________

} // namespace NotificationsHandlers
