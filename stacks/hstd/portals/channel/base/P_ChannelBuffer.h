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

#include "../../../../../htk.h"
#include "../../../../../htk_internals.h"

namespace HToolkit
{
  namespace hstd
  {
    namespace Portals
    {

      //=============================================================================
      // MainStructure : SingleMatrix
      //=============================================================================
      struct P_ChannelBuffer : public HtkChannelPortal
      {
        // Base

        virtual void handle(hArgs *args) override
        {
          HToolkitChannel *channel = new HToolkitChannel;

          hString buffer = args->get<hString>("buffer", "null");

          // Extract ID
          size_t poPos = buffer.find("po(");
          if (poPos != hString::npos)
          {
            poPos += 3;
            size_t commaPos = buffer.find(",", poPos);
            size_t endPos = buffer.find(")", poPos);
            if (commaPos != hString::npos && endPos != hString::npos)
            {
              // matrix->SetId(buffer.substr(poPos, commaPos - poPos - 1));
            }
          }

          // Extract ID
          size_t idPos = buffer.find("id(");
          if (idPos != hString::npos)
          {
            idPos += 3;
            size_t commaPos = buffer.find(",", idPos);
            size_t endPos = buffer.find(")", idPos);
            if (commaPos != hString::npos && endPos != hString::npos)
            {
              channel->SetId(buffer.substr(idPos, commaPos - idPos - 1));
            }
          }

          // Extract Tag
          size_t tyPos = buffer.find("ty(");
          if (tyPos != hString::npos)
          {
            tyPos += 3;
            size_t commaPos = buffer.find(",", tyPos);
            size_t endPos = buffer.find(")", tyPos);
            if (commaPos != hString::npos && endPos != hString::npos)
            {
              channel->channelType = buffer.substr(tyPos, commaPos - tyPos - 1);
            }
          }

          size_t startPos = 4;
          while (true)
          {
            size_t openBracketPos = buffer.find("[", startPos);
            if (openBracketPos == hString::npos)
            {
              break;
            }

            size_t closeBracketPos = buffer.find("]", openBracketPos);
            if (closeBracketPos == hString::npos)
            {
              break;
            }

            hString subString =
                buffer.substr(openBracketPos + 1, closeBracketPos - openBracketPos - 1);

            size_t rPos = buffer.rfind("r", openBracketPos);
            if (rPos != hString::npos)
            {
              hString rValue =
                  buffer.substr(rPos - 1, openBracketPos - rPos + 1).c_str();
              size_t subStartPos = 0;
              while (true)
              {
                size_t commaPos = subString.find(",", subStartPos);
                if (commaPos == hString::npos)
                {
                  hString value = subString.substr(subStartPos).c_str();
                  size_t colonPos = value.find(":");
                  if (colonPos != hString::npos)
                  {
                    hString key = value.substr(0, colonPos);
                    hString val = value.substr(colonPos + 1);

                    int rValueInt = std::atoi(rValue.c_str()) - 1;

                    if (rValueInt >= 0 &&
                        rValueInt < matrix->GetReceivers().size())
                    {
                      matrix->boostrapp_receivers[rValueInt].AddValueToInterface(key,
                                                                                 val);
                    }
                    else
                    {
                      HtkProviderInterface newinteface;
                      newinteface.AddValueToInterface(key, val);
                      matrix->boostrapp_receivers.push_back(newinteface);
                    }
                  }
                  break;
                }
                else
                {
                  hString value =
                      subString.substr(subStartPos, commaPos - subStartPos).c_str();
                  size_t colonPos = value.find(":");
                  if (colonPos != hString::npos)
                  {
                    hString key = value.substr(0, colonPos);
                    hString val = value.substr(colonPos + 1);

                    int rValueInt = std::atoi(rValue.c_str()) - 1;
                    if (rValueInt >= 0 &&
                        rValueInt < matrix->GetReceivers().size())
                    {

                      matrix->boostrapp_receivers[rValueInt].AddValueToInterface(key,
                                                                                 val);
                    }
                    else
                    {
                      HtkProviderInterface newinteface;
                      newinteface.AddValueToInterface(key, val);
                      matrix->boostrapp_receivers.push_back(newinteface);
                    }
                  }
                  subStartPos = commaPos + 1;
                }
              }
            }

            startPos = closeBracketPos + 1;
          }
          HToolkit::AskToJoinChannel(channel);
        };

        virtual void constructor(hArgs *args) override
        {
          HToolkitChannel *channel = args->get<HToolkitChannel *>("channel", nullptr);
          hString code = "[po(";
          code += "::HSTD::P_ChannelBuffer";
          code += "";

          code += "),";

          code += "id(";
          code += channel->GetId();

          code += "),";

          code += "ty(";
          code += channel->channelType;

          code += "),";
          code += "pb(";

#ifdef USE_HSP
          unsigned char *key_bytes = NULL;
          int key_length = i2d_PrivateKey(ctx.keyPair.second, &key_bytes);

          std::string key_string(reinterpret_cast<char *>(key_bytes), key_length);
          std::string key_hex;
          for (int i = 0; i < key_length; ++i)
          {
            char hex_byte[3];
            snprintf(hex_byte, sizeof(hex_byte), "%02X", key_bytes[i]);
            key_hex += hex_byte;
          }

          hString vx_key_string(key_string.c_str());

          code += key_hex.c_str();
#endif

          code += "),";
          code += "r(";
          // Ceux qui gèrent la demande.
          code += ")]";
          value = code;
        }
      };
      //=============================================================================

      //=============================================================================
      ADD_CHANNELPORTAL_TO_FACTORY(HSTD, P_ChannelBuffer)
      //=============================================================================

    }; // namespace HToolkit
  };   // namespace hstd
};     // namespace Matrixes
//________________________________________
