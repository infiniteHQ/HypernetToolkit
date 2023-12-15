//=============================================================================
// Caracteristics
//=============================================================================
// Type :             Decentralized
// Targeting :        By Provider Interface
// Element linking :  By Register
// Has Authority :    Yes, the initiator and inherited elements
//=============================================================================

#include "../../../../htk.h"
#include "../../../../htk_internals.h"

namespace HToolkit
{
  namespace hstd
  {
    namespace Matrixes
    {

      //=============================================================================
      // MainStructure : SingleMatrix
      //=============================================================================
      struct M_BaseMatrix : public HToolkitMatrix
      {
        // Base
        virtual void constructor(hArgs *args) override
        {
          HtkContext &ctx = *CHToolkit;
          // Definir la liste des composants attendus
          this->attempted_elements = *args->get<hVector<hString> *>("attempted_elements", nullptr);
          this->attempted_channels = *args->get<hVector<hString> *>("attempted_channels", nullptr);
          // Attempted protocols

          // Definir les paramètres
          this->SetProp(HToolkitMatrix_Props_Limits_MaxChannels, args->get<hString>(HToolkitMatrix_Props_Limits_MaxChannels, "8"));    // Empty or Default
          this->SetProp(HToolkitMatrix_Props_Limits_MaxElements, args->get<hString>(HToolkitMatrix_Props_Limits_MaxElements, "null")); // Le définir de manière statique pour spécialiser la matrix.
          this->SetProp(HToolkitMatrix_Props_Limits_MaxNodes, args->get<hString>(HToolkitMatrix_Props_Limits_MaxNodes, "null"));       // Le définir de manière statique pour spécialiser la matrix.

          // Adding base IO features
          // Initiator element receiver

          // Faire un registre des elements connus dans le context (ou plutot l'instance de la matrice, dans le context)
          // Faire un registre de ses propres instances d'elements.
          // Associer chaque element a un provider interface.

          // Define portals
          hVector<hString> *PortalMethods = args->get<hVector<hString> *>("portals_tag", nullptr);

          HToolkitMatrix *ma = this;
          // Construct portal(s) // On donne en param la matrice et il prendra tout ce dont il a besoin pour faire le portal (id, recvs, etc...)
          if (PortalMethods != nullptr)
          {
            for (auto neededPortals : *PortalMethods)
            {
              HtkMatrixPortal *portal = MatrixPortalFactory::getInstance().createInstance(neededPortals);
              hArgs args;
              args.add("matrix", ma);
              portal->constructor(&args);
              this->current_portals.insert(neededPortals, portal);
            }
          }

          this->OnCustomEvent("OnConstructed", args);
          args->add("already_configured", true);

          HToolkitElement *element = args->get<HToolkitElement *>("element", nullptr); // Matrix dedicated element instance.
          args->add("matrix", ma);                                                     // Matrix dedicated element instance.
          element->id = hRandom::Generate6CharID();
          element->role = "initializator";
          this->loaded_self = element;

          //this->init(args); // l'element suit ici et se fera initialisé comme les autres. (ici nous construisons ses elements d'intiateur (clés, etc))
        }

        virtual void destructor(hArgs *args) override
        {
          // Destruct all context dependencies

          //  user
          // delete element entry into context

          this->OnCustomEvent("OnDestructed", args);
        }

        virtual void init(hArgs *args) override
        {
          // Get configs
          hString cfg = args->get<hString>("configs", "null");
          this->InputConfigString(cfg);

          HToolkitElement *element = args->get<HToolkitElement *>("element", nullptr);
          this->loaded_self = element;
          this->OnCustomEvent("OnInitialized", args);
        }

        virtual void constructor_channel(hArgs *args) override
        {
          if (args->get<hString>("mode", "null") == "static")
          {
            hString channelType = args->get<hString>("type", "null");
            hString channelTag = args->get<hString>("tag", "null");
            hString distantElement = args->get<hString>("element", "null");

            this->add_registered_channel(this->CreateChannel(channelTag, args));
            this->OnCustomEvent("OnChannelContructed", args);

            for(auto element : this->loaded_elements){
              HToolkitMatrix* matrix = this;
              hArgs channelUpdate;
              channelUpdate.add("element", element);
              channelUpdate.add("matrix", matrix);
              HToolkit::UpdateMatrixChannelsList(&channelUpdate);
            }
          }
        }

        virtual void init_channel(hArgs *args) override
        {
          // Recuperer l'identifiant du channel concerné
          this->OnCustomEvent("OnChannelInitialized", args);
        }

        virtual void destruct_channel(hArgs *args) override
        {
          // Recuperer l'identifiant du channel concerné
          this->OnCustomEvent("OnChannelDestructed", args);
        }

        virtual hVector<HToolkitElement *> get_registered_element(hArgs *args) override
        {
          hVector<HToolkitElement *> result = this->registered_elements;
          result.push_front(loaded_self);
          return result;
        };

        virtual hVector<HToolkitChannel *> get_registered_channels(hArgs *args) override
        {
          return this->registered_channels;
        };

        virtual hVector<HToolkitNode *> get_registered_nodes(hArgs *args) override
        {
          return this->registered_nodes;
        };

        virtual void clear_registered_element(hArgs *args) override
        {
            this->registered_nodes.clear();
        };


        virtual void constructor_element(hArgs *args) override
        {
          hString element = args->get<hString>("element_type", "null");
          this->OnCustomEvent("OnElementContructed", args);
        }; // Creation générale d'un element

        // Load another element
        virtual void init_element(hArgs *args) override{

        };

        // Unload element
        virtual void destruct_element(hArgs *args) override{

        }; // destructiuon locale d'un element

        virtual void add_registered_element(HToolkitElement *element) override
        {
          this->registered_elements.push_back(element);
        };

        virtual void add_registered_channel(HToolkitChannel *channel) override
        {
          this->registered_channels.push_back(channel);
        };

        virtual void add_registered_node(HToolkitNode *node) override
        {
          this->registered_nodes.push_back(node);
        };

        // Override functions

        // Proper logic

        // Variables
        hVector<HToolkitNode *> registered_nodes;
        hVector<HToolkitElement *> registered_elements;
        hVector<HToolkitChannel *> registered_channels;
      };
      //=============================================================================

      // Register event for matrix, in a construction, on récupère les events et on les attache
      MATRIX_CUSTOM_EVENT(HSTD, OnNewElement, M_BaseMatrix, {
        HtkContext &ctx = *CHToolkit;
        HToolkitMatrix *matrix = args->get<HToolkitMatrix *>("matrix", nullptr);
        HToolkitElement *element = args->get<HToolkitElement *>("element", nullptr);
        if (matrix != nullptr && element != nullptr)
        {
          bool existing = false;
          for(auto current : matrix->loaded_elements){
            if(current->id.c_str() == element->id.c_str()){
              existing = true;
            }
          if(element->id.c_str() == matrix->loaded_self->id.c_str()){
              existing = true;
          }
          }

          if(!existing){
            matrix->loaded_elements.push_back(element);
            matrix->add_registered_element(element);
            element->runtime_state = "know";
          }
        }
      })

      // Register event for matrix, in a construction, on récupère les events et on les attache
      MATRIX_CUSTOM_EVENT(HSTD, OnNewElementList, M_BaseMatrix, {
        HToolkitMatrix *matrix = args->get<HToolkitMatrix *>("matrix", nullptr);
        matrix->clear_registered_element(args);
      })

      // Register event for matrix, in a construction, on récupère les events et on les attache
      MATRIX_CUSTOM_EVENT(HSTD, OnAskToUpdate, M_BaseMatrix, {
        HtkContext &ctx = *CHToolkit;
        HToolkitNotification *notification = args->get<HToolkitNotification *>("notification", nullptr);
        hString mid = notification->GetParameter("mid");
        hString eid = notification->GetParameter("eid");
        HToolkitMatrix *m = ctx.IO.GetActiveMatrix(mid);
      })

      // Register event for matrix, in a construction, on récupère les events et on les attache
      MATRIX_CUSTOM_EVENT(HSTD, OnNewNode, M_BaseMatrix, {
        HtkContext &ctx = *CHToolkit;
        HToolkitMatrix *matrix = args->get<HToolkitMatrix *>("matrix", nullptr);
        HToolkitElement *b_element = args->get<HToolkitElement *>("b_element", nullptr);

        for (auto runtimeelements : ctx.IO.activeRuntimeElements)
        {
          if (runtimeelements->attached_matrix->GetId() == matrix->GetId())
          {
            HToolkitNode *newnode = new HToolkitNode;
            newnode->init(runtimeelements, b_element, hRandom::Generate6CharID());
            matrix->loaded_nodes.push_back(newnode);
          }
        }
      })

      // Register event for matrix, in a construction, on récupère les events et on les attache
      MATRIX_CUSTOM_EVENT(HSTD, OnAskToJoin, M_BaseMatrix, {
        HtkContext &ctx = *CHToolkit;
        HToolkitNotification *notification = args->get<HToolkitNotification *>("notification", nullptr);
        hString mid = notification->GetParameter("mid");
        hString ety = notification->GetParameter("ety");
        HtkProviderInterface inter = HToolkit::InitProviderInterface(notification->GetParameter("rec"));

        hVector<hString> vecr = HToolkit::InputStringVector(ety);

        HToolkitMatrix *m = ctx.IO.GetActiveMatrix(mid);

        for (auto all : vecr)
        {
          for (auto attempted : m->attempted_elements)
          {
            if (all == attempted)
            {
              HToolkitElement *element = ElementFactory::getInstance().createInstance(ety.c_str());
              element->tag = ety;
              element->attached_matrix = m;
              element->interface = inter;
              element->id = hRandom::Generate6CharID();
              element->role = "member";

              HToolkit::WelcomeElementOnMatrix(*element, *m, "0");
              hArgs OnNewElement_args;
              OnNewElement_args.add("element", element);
              OnNewElement_args.add("matrix", m);
              m->OnCustomEvent("OnNewElement", &OnNewElement_args);

              hArgs OnNewNode_args;
              OnNewNode_args.add("b_element", element);
              OnNewNode_args.add("matrix", m);
              m->OnCustomEvent("OnNewNode", &OnNewNode_args); // core dumped

              hArgs update_args;
              update_args.add("element", element);
              update_args.add("matrix", m);
              // Sync pour le nouveau
              HToolkit::UpdateElementsList(&update_args); // Uniquement le nouvel element

              // Sync simple pour tout les autres
              HToolkit::UpdateElement(&update_args); // Tout les elements

              // Sync le nouveau pour tout les channels
              HToolkit::UpdateMatrixChannelsList(&update_args);
            
          }
              return;
            }
          
        }
        HToolkit::ReportErrorWhileJoinMatrix(inter, "Type not allowed");
      })

      // Register event for matrix, in a construction, on récupère les events et on les attache
      MATRIX_CUSTOM_EVENT(HSTD, OnJoined, M_BaseMatrix, {
        hString eid = args->get<hString>("eid", "null"); // Pour s'auto attribuer
        hString mid = args->get<hString>("mid", "null");
      })

      //=============================================================================
      ADD_MATRIX_TO_FACTORY(HSTD, M_BaseMatrix);
      //=============================================================================

    }; // namespace HToolkit
  };   // namespace hstd
};     // namespace Matrixes
       //________________________________________