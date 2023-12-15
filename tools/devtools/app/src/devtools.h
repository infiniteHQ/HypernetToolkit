// UI (with imgui)
#include "../../backend/Source/Devtools/Application.h"
#include "../../backend/Source/Devtools/EntryPoint.h"
#include "../../backend/Platform/GUI/Devtools/Image.h"
#include "../../backend/Platform/GUI/Devtools/UI/UI.h"
#include "../../vendor/imgui/imgui.h"

// HToolkit Runtime
#include "../../../../htk.h"
#include "../../../../htk_internals.h"
#include "../../../../htk_core.h"

using namespace HToolkit;

struct Content
{
    std::string name;
    std::string from;
    std::string type;
    std::string state;

    Content(std::string a, std::string b, std::string c, std::string d)
    {
        name = a;
        from = b;
        type = c;
        state = d;
    }
};

struct Pannel
{
    Pannel(HToolkitMatrix *m)
    {
        if (ImGui::Begin("Matrixes"))
        {
            ImGui::Text("MatrixColumns");
            ImGui::End();
        }
    };
};

// Make the UI compact because there are so many fields
static void PushStyleCompact()
{
    ImGuiStyle &style = ImGui::GetStyle();
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, (float)(int)(style.FramePadding.y * 0.60f)));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(style.ItemSpacing.x, (float)(int)(style.ItemSpacing.y * 0.60f)));
}

static void PopStyleCompact()
{
    ImGui::PopStyleVar(2);
}

class ExampleLayer : public Walnut::Layer
{
public:
    ExampleLayer(HtkContext *ctx) { this->m_ctx = ctx; };

    virtual void OnUIRender() override
    {

        // ImGui::Begin("Hello");
        // ImGui::Button("Button");
        // ImGui::End();

        //ImGui::ShowDemoWindow();

        // Elements
        UI_ShowContextMatrixes(); // change to active label
        UI_ShowActiveReceivers();
        UI_ShowActiveChannels();

        // Networking
        UI_ShowNetworkReceivedCommunications();
        UI_ShowNetworkSentCommunications();
        UI_ShowNetworkReceivedTransmissions();
        UI_ShowNetworkSentTransmissions();

        UI_ShowCachedElements();

        UI_ShowRegisteredNotificationsHandlers();
        UI_ShowRegisteredMatrixes();
        UI_ShowRegisteredProviders();
        UI_ShowRegisteredChannels();
        UI_ShowRegisteredElements();

        UI_ShowMatrixPannel();
        UI_ShowChannelPannel();
        UI_ShowContextRuntimeElements();

        // Extras
        UI_JoinMatrix();
    }

    void ShowMatrixPannel(HToolkitMatrix *m)
    {
        this->m_currentMatrixForPannel = m;
        m_UI_ShowMatrixPannel = true;
    }
    void ShowChannelPannel(HToolkitChannel *c)
    {
        this->m_currentChannelForPannel = c;
        m_UI_ShowChannelPannel = true;
    }

    // Fonction pour obtenir la couleur en fonction de la valeur de l'entier
    ImU32 GetColorForValue(int value)
    {
        // Calculer les composantes de couleur en fonction de la valeur
        int red = 255 - (value * 255 / 1500);
        int green = value * 255 / 1500;
        int blue = 255;

        // Assurez-vous que les composantes sont comprises entre 0 et 255
        red = (red < 0) ? 0 : (red > 255) ? 255
                                          : red;
        green = (green < 0) ? 0 : (green > 255) ? 255
                                                : green;
        blue = (blue < 0) ? 0 : (blue > 255) ? 255
                                             : blue;

        // Créez la couleur ImGui avec les composantes calculées
        return IM_COL32(red, green, blue, 255);
    }

    void UI_ShowChannelPannel()
    {
        if (!m_UI_ShowChannelPannel)
            return;

        if (ImGui::Begin("Channel Pannel"))
        {
            float oldsize = ImGui::GetFont()->Scale;
            ImGui::GetFont()->Scale *= 1.5;
            ImGui::PushFont(ImGui::GetFont());

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Matrix :");
            ImGui::SameLine();
            ImGui::Text((char *)this->m_currentChannelForPannel->channelType.c_str());
            ImGui::GetFont()->Scale = oldsize;
            ImGui::PopFont();

            ImGui::SameLine();
            if (m_currentChannelForPannel->state == "active")
            {
                ImGui::TextColored(ImVec4(0.1f, 1.0f, 0.1f, 1.0f), "Active");
            }

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "ID :");
            ImGui::SameLine();
            ImGui::Text((char *)this->m_currentChannelForPannel->id.c_str());
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "State :");
            ImGui::SameLine();
            ImGui::Text((char *)this->m_currentChannelForPannel->state.c_str());

            ImGui::Separator();
            ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
            if (ImGui::BeginTabBar("MatrixPannelTabBar", tab_bar_flags))
            {
                if (ImGui::BeginTabItem("Actions"))
                {
                    ImGui::Text("Send type with a buffer :");

                    static char portal[4096] = "";
                    ImGui::InputText("Buffer", portal, 4096);
                    if(ImGui::Button("Handle")){
                        hArgs args;
                        hString buff = portal;
                        args.add("buffer", buff);
                        m_currentChannelForPannel->send(&args);
                    }
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Properties"))
                {
                    ImGui::Text("This is the Avocado tab!\nblah blah blah blah blah");
                    ImGui::EndTabItem();
                }
                    
                std::string element_title = "Element (";
                element_title += std::to_string(m_currentChannelForPannel->loaded_elements.size());
                element_title += ")";
                if (ImGui::BeginTabItem((char*)element_title.c_str()))
                {
                    static ImGuiTableFlags flags =
                        ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable |
                        ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                    int selected_row = -1; // Variable pour suivre la ligne sélectionnée
                    ImGui::Text("Registered events");
                    if (ImGui::BeginTable("qsdf", 1, flags))
                    {
                        ImGui::TableSetupColumn("Identifier", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableHeadersRow();
                        for (int row = 0; row < m_currentChannelForPannel->loaded_elements.size(); row++)
                        {
                            ImGui::TableNextRow();

                            for (int column = 0; column < 1; column++)
                            {
                                ImGui::TableSetColumnIndex(column);

                                if (column == 0)
                                {
                                    ImGui::Text((char *)m_currentChannelForPannel->loaded_elements[row]->id.c_str());
                                }

                                // Rendre la ligne sélectionnable
                                if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
                                {
                                    selected_row = row; // Enregistrer la ligne sélectionnée
                                }
                            }
                        }
                        ImGui::EndTable();
                    }

                    ImGui::EndTabItem();
                }

                
                std::string events_title = "Events (";
                events_title += std::to_string(m_currentChannelForPannel->custom_events.size());
                events_title += ")";
                if (ImGui::BeginTabItem((char*)events_title.c_str()))
                {
                    static ImGuiTableFlags flags =
                        ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable |
                        ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                    int selected_row = -1; // Variable pour suivre la ligne sélectionnée
                    ImGui::Text("Registered events");
                    if (ImGui::BeginTable("qsdf", 1, flags))
                    {
                        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableHeadersRow();
                        for (int row = 0; row < m_currentChannelForPannel->available_events.size(); row++)
                        {
                            ImGui::TableNextRow();

                            for (int column = 0; column < 1; column++)
                            {
                                ImGui::TableSetColumnIndex(column);

                                if (column == 0)
                                {
                                    ImGui::Text((char *)m_currentChannelForPannel->available_events[row].c_str());
                                }

                                // Rendre la ligne sélectionnable
                                if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
                                {
                                    selected_row = row; // Enregistrer la ligne sélectionnée
                                }
                            }
                        }
                        ImGui::EndTable();
                    }

                    ImGui::EndTabItem();
                }

                
                if (ImGui::BeginTabItem("Hooks"))
                {
                    static hVector<HToolkitCustomEvent> chans;
                    chans.clear();
                    for (auto chan : m_ctx->IO.registeredHookForChannelsTypes)
                    {
                        if (chan->channelType == this->m_currentChannelForPannel->channelType)
                        {
                            chans.push_back(*chan);
                        }
                    };

                    static ImGuiTableFlags flags =
                        ImGuiTableFlags_ScrollY |
                        ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter |
                        ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable |
                        ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                    int selected_row = -1; // Variable pour suivre la ligne sélectionnée

                    if (ImGui::BeginTable("HToolkitCustomEventchnanelm", 2, flags))
                    {
                        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableHeadersRow();

                        for (int row = 0; row < chans.size(); row++)
                        {
                            ImGui::TableNextRow();

                            for (int column = 0; column < 1; column++)
                            {
                                int numberparam = 0;
                                ImGui::TableSetColumnIndex(column);

                                if (column == 0)
                                {
                                    ImGui::Text((char *)chans[row].eventName.c_str());
                                }
                                if (column == 1)
                                {
                                    ImGui::Text((char *)chans[row].eventName.c_str());
                                }

                                // Rendre la ligne sélectionnable
                                if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
                                {
                                    selected_row = row; // Enregistrer la ligne sélectionnée
                                }
                            }
                        }
                        ImGui::EndTable();
                    }
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Elements"))
                {
                    ImGui::Text("This is the Broccoli tab!\nblah blah blah blah blah");
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Channels"))
                {
                    static hVector<HToolkitChannel *> chans = m_ctx->IO.GetAllActiveChannels();
                    for (auto chan : m_ctx->IO.GetAllCachedChannels())
                    {
                        chans.push_back(chan);
                    };

                    static ImGuiTableFlags flags =
                        ImGuiTableFlags_ScrollY |
                        ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter |
                        ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable |
                        ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                    int selected_row = -1; // Variable pour suivre la ligne sélectionnée

                    if (ImGui::BeginTable("tableofsentcommunications", 7, flags))
                    {
                        ImGui::TableSetupColumn("Overview", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Data received", ImGuiTableColumnFlags_WidthStretch);
                        ImGui::TableSetupColumn("Data sent", ImGuiTableColumnFlags_WidthStretch);
                        ImGui::TableSetupColumn("State", ImGuiTableColumnFlags_WidthStretch);
                        ImGui::TableHeadersRow();

                        for (int row = 0; row < chans.size(); row++)
                        {
                            ImGui::TableNextRow();

                            for (int column = 0; column < 7; column++)
                            {
                                int numberparam = 0;
                                ImGui::TableSetColumnIndex(column);

                                if (column == 0)
                                {
                                    ImGui::Button("Pannel");
                                }
                                if (column == 1)
                                {
                                    ImGui::TextColored(ImVec4(0.8f, 0.5f, 1.0f, 1.0f), "(C)");
                                    ImGui::SameLine();
                                    ImGui::Text((char *)chans[row]->channelType.c_str());
                                }
                                if (column == 2)
                                {
                                    ImGui::Text((char *)chans[row]->id.c_str());
                                }
                                if (column == 3)
                                {
                                    ImGui::Text("%i bytes", chans[row]->dataReceived);
                                }
                                if (column == 4)
                                {
                                    ImGui::Text("%i bytes", chans[row]->metricsDataSent);
                                }
                                if (column == 5)
                                {
                                    ImGui::Text((char *)chans[row]->state.c_str());
                                }

                                // Rendre la ligne sélectionnable
                                if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
                                {
                                    selected_row = row; // Enregistrer la ligne sélectionnée
                                }
                            }
                        }
                        ImGui::EndTable();
                    }
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Nodes"))
                {
                    ImGui::Text("This is the Cucumber tab!\nblah blah blah blah blah");
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Elements"))
                {
                    ImGui::Text("This is the Cucumber tab!\nblah blah blah blah blah");
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
        }
        ImGui::End();
    }

    static hVector<HToolkitChannel *> GetActiveChans(HtkContext *ctx, HToolkitMatrix *m)
    {
        hVector<HToolkitChannel *> chans;

        for (auto chan : ctx->IO.GetAllActiveChannels())
        {
            if (m->id == chan->matrixID)
            {
                chans.push_back(chan);
            }
        }
        return chans;
    }

    void UI_ShowMatrixPannel()
    {
        if (!m_UI_ShowMatrixPannel)
            return;

        if (ImGui::Begin("Matrix Pannel"))
        {
            float oldsize = ImGui::GetFont()->Scale;
            ImGui::GetFont()->Scale *= 1.5;
            ImGui::PushFont(ImGui::GetFont());

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Matrix :");
            ImGui::SameLine();
            ImGui::Text((char *)this->m_currentMatrixForPannel->GetTag().c_str());
            ImGui::GetFont()->Scale = oldsize;
            ImGui::PopFont();

            ImGui::SameLine();
            if (m_currentMatrixForPannel->GetState() == "active")
            {
                ImGui::TextColored(ImVec4(0.1f, 1.0f, 0.1f, 1.0f), "Active");
            }

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "ID :");
            ImGui::SameLine();
            ImGui::Text((char *)this->m_currentMatrixForPannel->GetId().c_str());
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "State :");
            ImGui::SameLine();
            ImGui::Text((char *)this->m_currentMatrixForPannel->GetState().c_str());

            ImGui::Separator();
            ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
            if (ImGui::BeginTabBar("MatrixPannelTabBar", tab_bar_flags))
            {
                if (ImGui::BeginTabItem("Portals"))
                {
                    for(auto portal : m_currentMatrixForPannel->current_portals){
                        ImGui::InputText((char*)portal.key.c_str(), (char*)portal.value->value.c_str(), 4096);
                    }

                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Contents"))
                {
                    static ImGuiTableFlags flags =
                        ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable |
                        ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                    int selected_row = -1; // Variable pour suivre la ligne sélectionnée
                    ImGui::Text("Attempted channels types");
                    if (ImGui::BeginTable("dgdfgdfgdfg", 1, flags))
                    {
                        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableHeadersRow();

                        for (int row = 0; row < m_currentMatrixForPannel->attempted_channels.size(); row++)
                        {
                            ImGui::TableNextRow();

                            for (int column = 0; column < 1; column++)
                            {
                                ImGui::TableSetColumnIndex(column);

                                if (column == 0)
                                {
                                    ImGui::Text((char *)m_currentMatrixForPannel->attempted_channels[row].c_str());
                                }

                                // Rendre la ligne sélectionnable
                                if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
                                {
                                    selected_row = row; // Enregistrer la ligne sélectionnée
                                }
                            }
                        }
                        ImGui::EndTable();
                    }

                    ImGui::Text("Attempted element types");
                    if (ImGui::BeginTable("ddddd", 1, flags))
                    {
                        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableHeadersRow();

                        for (int row = 0; row < m_currentMatrixForPannel->attempted_elements.size(); row++)
                        {
                            ImGui::TableNextRow();

                            for (int column = 0; column < 1; column++)
                            {
                                ImGui::TableSetColumnIndex(column);

                                if (column == 0)
                                {
                                    ImGui::Text((char *)m_currentMatrixForPannel->attempted_elements[row].c_str());
                                }

                                // Rendre la ligne sélectionnable
                                if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
                                {
                                    selected_row = row; // Enregistrer la ligne sélectionnée
                                }
                            }
                        }
                        ImGui::EndTable();
                    }

                    ImGui::EndTabItem();
                }
                std::string elements_title = "Elements (";
                elements_title += std::to_string(m_currentMatrixForPannel->get_registered_element(nullptr).size());
                elements_title += ")";
                if (ImGui::BeginTabItem((char*)elements_title.c_str()))
                {
                    int selected_row = -1; // Variable pour suivre la ligne sélectionnée

                    static ImGuiTableFlags flagsd =
                        ImGuiTableFlags_ScrollY |
                        ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter |
                        ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable |
                        ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;


                    if (ImGui::BeginTable("sdfg", 5, flagsd))
                    {
                        ImGui::TableSetupColumn("Identifier", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Role", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Parent Matrix", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Type of Element", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Runtime state", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableHeadersRow();

                        for (auto element : m_currentMatrixForPannel->get_registered_element(nullptr))
                        { 
                            
                            ImGui::TableNextRow();

                            for (int column = 0; column < 5; column++)
                            {
                                ImGui::TableSetColumnIndex(column);

                                if (column == 0)
                                {
                                    ImGui::Text((char *)element->id.c_str());
                                }
                                if (column == 1)
                                {
                                    ImGui::Text((char *)element->role.c_str());
                                }
                                if (column == 2)
                                {
                                    //ImGui::Text((char *)element->attached_matrix->GetId().c_str());
                                }
                                if (column == 3)
                                {
                                    ImGui::Text((char *)element->tag.c_str());
                                }
                                if (column == 4)
                                {
                                    ImGui::Text((char *)element->runtime_state.c_str());
                                }
                            }
                        }
                        ImGui::EndTable();
                    }
                    ImGui::EndTabItem();
                }
                
                if (ImGui::BeginTabItem("Hooks"))
                {

                    static ImGuiTableFlags flags =
                        ImGuiTableFlags_ScrollY |
                        ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter |
                        ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable |
                        ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                    int selected_row = -1; // Variable pour suivre la ligne sélectionnée

                    if (ImGui::BeginTable("HToolkitCustdfghdfgomEventchnanelm", 2, flags))
                    {
                        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("sss", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableHeadersRow();

                        for (int row = 0; row < m_currentMatrixForPannel->custom_events.size(); row++)
                        {
                            ImGui::TableNextRow();

                            for (int column = 0; column < 1; column++)
                            {
                                int numberparam = 0;
                                ImGui::TableSetColumnIndex(column);

                                if (column == 0)
                                {
                                    ImGui::Text((char *)m_currentMatrixForPannel->custom_events[row]->eventName.c_str());
                                }
                                if (column == 1)
                                {
                                    ImGui::Text((char *)m_currentMatrixForPannel->custom_events[row]->eventName.c_str());
                                }

                                // Rendre la ligne sélectionnable
                                if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
                                {
                                    selected_row = row; // Enregistrer la ligne sélectionnée
                                }
                            }
                        }
                        ImGui::EndTable();
                    }
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Properties"))
                {
                    static ImGuiTableFlags flags =
                        ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable |
                        ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                    int selected_row = -1; // Variable pour suivre la ligne sélectionnée
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Limits");
                    if (ImGui::BeginTable("MatrixPannelPropsLimits", 5, flags))
                    {
                        ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Property Tag", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Property ID", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableHeadersRow();

                        ImGui::TableNextRow();

                        for (int column = 0; column < 5; column++)
                        {
                            int numberparam = 0;
                            ImGui::TableSetColumnIndex(column);

                            if (column == 0)
                            {
                                ImGui::Text("Limit of Element");
                            }
                            if (column == 1)
                            {
                                hString pp = m_currentMatrixForPannel->GetProp(HToolkitMatrix_Props_Limits_MaxElements);
                                if (pp.empty())
                                {
                                    ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "none");
                                }
                                else
                                {
                                    ImGui::Text((char *)m_currentMatrixForPannel->GetProp(HToolkitMatrix_Props_Limits_MaxElements).c_str());
                                }
                            }
                            if (column == 2)
                            {
                                ImGui::Text("HToolkitMatrix_Props_Limits_MaxElements");
                            }
                            if (column == 3)
                            {
                                ImGui::Text("1");
                            }
                            if (column == 4)
                            {
                                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "int");
                            }

                            ImGui::TableSetColumnIndex(column);

                            if (column == 0)
                            {
                                ImGui::Text("Limit of Channels");
                            }
                            if (column == 1)
                            {
                                hString pp = m_currentMatrixForPannel->GetProp(HToolkitMatrix_Props_Limits_MaxChannels);
                                if (pp.empty())
                                {
                                    ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "none");
                                }
                                else
                                {
                                    ImGui::Text((char *)m_currentMatrixForPannel->GetProp(HToolkitMatrix_Props_Limits_MaxChannels).c_str());
                                }
                            }
                            if (column == 2)
                            {
                                ImGui::Text("HToolkitMatrix_Props_Limits_MaxChannels");
                            }
                            if (column == 3)
                            {
                                ImGui::Text("2");
                            }
                            if (column == 4)
                            {
                                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "int");
                            }

                            ImGui::TableSetColumnIndex(column);

                            if (column == 0)
                            {
                                ImGui::Text("Limit of Nodes");
                            }
                            if (column == 1)
                            {
                                hString pp = m_currentMatrixForPannel->GetProp(HToolkitMatrix_Props_Limits_MaxNodes);

                                if (pp.empty())
                                {
                                    ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "none");
                                }
                                else
                                {
                                    ImGui::Text((char *)m_currentMatrixForPannel->GetProp(HToolkitMatrix_Props_Limits_MaxNodes).c_str());
                                }
                            }
                            if (column == 2)
                            {
                                ImGui::Text("HToolkitMatrix_Props_Limits_MaxNodes");
                            }
                            if (column == 3)
                            {
                                ImGui::Text("3");
                            }
                            if (column == 4)
                            {
                                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "int");
                            }
                        }
                        ImGui::EndTable();
                    }

                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Security");

                    if (ImGui::BeginTable("MatrixPannelPropsLimits", 5, flags))
                    {
                        ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Property Tag", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Property ID", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableHeadersRow();

                        ImGui::TableNextRow();

                        for (int column = 0; column < 4; column++)
                        {
                            int numberparam = 0;
                            ImGui::TableSetColumnIndex(column);

                            if (column == 0)
                            {
                                ImGui::Text("Authority Public Key");
                            }
                            if (column == 1)
                            {
                                hString pp = m_currentMatrixForPannel->GetProp(HToolkitMatrix_Props_Encryption_AuthorityPK);
                                if (pp.empty())
                                {
                                    ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "none");
                                }
                                else
                                {
                                    ImGui::Text((char *)m_currentMatrixForPannel->GetProp(HToolkitMatrix_Props_Encryption_AuthorityPK).c_str());
                                }
                            }
                            if (column == 2)
                            {
                                ImGui::Text("HToolkitMatrix_Props_Limits_MaxElements");
                            }
                            if (column == 3)
                            {
                                ImGui::Text("4");
                            }
                            if (column == 4)
                            {
                                ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "string");
                            }
                        }
                        ImGui::EndTable();
                    }

                    ImGui::EndTabItem();
                }
                
                
                std::string events_title = "Events (";
                events_title += std::to_string(m_currentMatrixForPannel->custom_events.size());
                events_title += ")";
                if (ImGui::BeginTabItem((char*)events_title.c_str()))
                {
                    static ImGuiTableFlags flags =
                        ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable |
                        ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                    int selected_row = -1; // Variable pour suivre la ligne sélectionnée
                    ImGui::Text("Registered events");
                    if (ImGui::BeginTable("qsdf", 1, flags))
                    {
                        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableHeadersRow();
                        for (int row = 0; row < m_currentMatrixForPannel->available_events.size(); row++)
                        {
                            ImGui::TableNextRow();

                            for (int column = 0; column < 1; column++)
                            {
                                ImGui::TableSetColumnIndex(column);

                                if (column == 0)
                                {
                                    ImGui::Text((char *)m_currentMatrixForPannel->available_events[row].c_str());
                                }

                                // Rendre la ligne sélectionnable
                                if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
                                {
                                    selected_row = row; // Enregistrer la ligne sélectionnée
                                }
                            }
                        }
                        ImGui::EndTable();
                    }

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Logs"))
                {
                    ImGui::Text("All events On?? & event flags described here");
                    ImGui::EndTabItem();
                }

                        hArgs args;
                hVector<HToolkitChannel *> activechans = GetActiveChans(m_ctx, m_currentMatrixForPannel);
                std::string channels_title = "Channels (";
                channels_title += std::to_string(m_currentMatrixForPannel->get_registered_channels(&args).size());
                channels_title += ")";
                if (ImGui::BeginTabItem((char*)channels_title.c_str()))
                {

                    static ImGuiTableFlags flags =
                        ImGuiTableFlags_ScrollY |
                        ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter |
                        ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable |
                        ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                    int selected_row = -1; // Variable pour suivre la ligne sélectionnée

                    if (ImGui::BeginTable("tableofsentcommunications", 7, flags))
                    {
                        ImGui::TableSetupColumn("Overview", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Total data received", ImGuiTableColumnFlags_WidthStretch);
                        ImGui::TableSetupColumn("Buffer data received", ImGuiTableColumnFlags_WidthStretch);
                        ImGui::TableSetupColumn("Data sent", ImGuiTableColumnFlags_WidthStretch);
                        ImGui::TableSetupColumn("State", ImGuiTableColumnFlags_WidthStretch);
                        ImGui::TableHeadersRow();

                        for(auto channel : m_currentMatrixForPannel->get_registered_channels(&args))
                        {
                            ImGui::TableNextRow();
                            int i = 0;

                            for (int column = 0; column < 7; column++)
                            {
                                int numberparam = 0;
                                ImGui::TableSetColumnIndex(column);

                                if (column == 0)
                                {
                                    if(m_ctx->IO.GetAllActiveChannels().contains(channel)){
                                        
                                    
                                    if (ImGui::Button("Open"))
                                    {
                                        this->ShowChannelPannel(activechans[i]);
                                    };

                                        }
                                        else{

                                    if (ImGui::Button("Ask to join"))
                                    {
                                        for(auto element : m_currentMatrixForPannel->loaded_elements){
                                            if(element->role == "initializator"){
                                                HToolkit::AskToJoinChannel(m_currentMatrixForPannel->get_registered_channels(&args)[i], element, m_currentMatrixForPannel->loaded_self);
                                            }
                                        }
                                        //this->ShowChannelPannel(activechans[i]);
                                    };
                                        
                                    }
                                }
                                if (column == 1)
                                {
                                    ImGui::TextColored(ImVec4(0.8f, 0.5f, 1.0f, 1.0f), "<Channel>");
                                    ImGui::SameLine();
                                    ImGui::Text((char *)channel->channelType.c_str());
                                }
                                if (column == 2)
                                {
                                    ImGui::Text((char *)channel->id.c_str());
                                }
                                if (column == 3)
                                {
                                    ImGui::Text("%i bytes", channel->dataReceived);
                                }
                                if (column == 4)
                                {
                                    ImGui::Text("%i bytes", channel->bufferReceived);
                                }
                                if (column == 5)
                                {
                                    ImGui::Text("%i bytes", channel->metricsDataSent);
                                }
                                if (column == 6)
                                {
                                    ImGui::Text((char *)channel->state.c_str());
                                }

                                i++;
                            }
                        }
                        ImGui::EndTable();
                    }
                    ImGui::EndTabItem();
                }
                
                
                std::string nodes_title = "Nodes (";
                //channels_title += std::to_string(m_currentMatrixForPannel->nod.size());
                nodes_title += ")";
                if (ImGui::BeginTabItem((char*)nodes_title.c_str()))
                {

                    static ImGuiTableFlags flags =
                        ImGuiTableFlags_ScrollY |
                        ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter |
                        ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable |
                        ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                    int selected_row = -1; // Variable pour suivre la ligne sélectionnée

                    if (ImGui::BeginTable("nodestab", 5, flags))
                    {
                        ImGui::TableSetupColumn("Overview", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Low Level Technology", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Node Identifier", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("A", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("B", ImGuiTableColumnFlags_WidthStretch);
                        ImGui::TableHeadersRow();

                        for (auto node : m_currentMatrixForPannel->loaded_nodes)
                        {
                            ImGui::TableNextRow();
                            int i = 0;

                            for (int column = 0; column < 5; column++)
                            {
                                ImGui::TableSetColumnIndex(column);

                                if (column == 0)
                                {
                                    if (ImGui::Button("Open"))
                                    {
                                     //   this->ShowChannelPannel(activechans[i]);
                                    };
                                }
                                if (column == 1)
                                {
                                    if(node->a->interface.GetInterfaceVariable("protocol") == node->b->interface.GetInterfaceVariable("protocol")){
                                        
                                    ImGui::Text((char *)node->b->interface.GetInterfaceVariable("protocol").c_str());
                                    }
                                    else{
                                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "mixed...");
                                    }
                                }
                                if (column == 2)
                                {
                                    ImGui::TextColored(ImVec4(0.8f, 1.0f, 1.0f, 1.0f), "<Node>");
                                    ImGui::SameLine();
                                    ImGui::Text((char *)node->nodeID.c_str());
                                }
                                if (column == 3)
                                {
                                    ImGui::Text((char *)node->a->id.c_str());
                                }
                                if (column == 4)
                                {
                                    ImGui::Text((char *)node->b->id.c_str());
                                }

                            }
                        }
                        ImGui::EndTable();
                    }
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
        }
        ImGui::End();
    }

    void UI_ShowRegisteredProviders()
    {
        if (!m_UI_ShowRegisteredProviders)
            return;

        if (ImGui::Begin("Registered providers"))
        {
            // Colonne 1 (Enfant 1)

            static ImGuiTableFlags flags =
                ImGuiTableFlags_ScrollY |
                ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter |
                ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable |
                ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

            int selected_row = -1; // Variable pour suivre la ligne sélectionnée

            if (ImGui::BeginTable("table1", 3, flags))
            {
                ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Tag", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Reference", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableHeadersRow();
                for (auto handler : m_ctx->registeredProviders->creatorMap)
                {
                    ImGui::TableNextRow();

                    for (int column = 0; column < 2; column++)
                    {
                        ImGui::TableSetColumnIndex(column);

                        if (column == 0)
                        {
                            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "(NH)");
                            ImGui::SameLine();
                            ImGui::Text("Providers");
                        }
                        if (column == 1)
                        {
                            ImGui::Text((char *)handler.key.c_str());
                        }
                        if (column == 2)
                        {
                            ImGui::Text((char *)&handler.value);
                        }
                    }
                }
                ImGui::EndTable();
            }

            // Vérifier si une ligne a été sélectionnée et afficher la popup
            if (selected_row != -1)
            {
                if (ImGui::BeginPopup("my_select_popup"))
                {
                    // Vous pouvez afficher le contenu de la popup ici
                    ImGui::Text("Selected Row: %d", selected_row);
                    ImGui::Separator();
                    // Ajoutez d'autres éléments à la popup si nécessaire
                    ImGui::EndPopup();
                }
            }

            // Redimensionnement automatique de la hauteur entre les deux enfants
            float minHeight = ImGui::GetContentRegionAvail().y;                       // Hauteur minimale
            float maxHeight = ImGui::GetIO().DisplaySize.y - ImGui::GetCursorPos().y; // Hauteur maximale

            if (minHeight > maxHeight)
            {
                minHeight = maxHeight;
            }
        }
        ImGui::End();
    }

    void UI_ShowRegisteredElements()
    {
        if (!m_UI_ShowRegisteredElements)
            return;

        if (ImGui::Begin("Registered Elements"))
        {
            // Colonne 1 (Enfant 1)

            static ImGuiTableFlags flags =
                ImGuiTableFlags_ScrollY |
                ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter |
                ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable |
                ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

            int selected_row = -1; // Variable pour suivre la ligne sélectionnée

            if (ImGui::BeginTable("table1", 3, flags))
            {
                ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Tag", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Reference", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableHeadersRow();
                for (auto handler : m_ctx->registeredElements->creatorMap)
                {
                    ImGui::TableNextRow();

                    for (int column = 0; column < 2; column++)
                    {
                        ImGui::TableSetColumnIndex(column);

                        if (column == 0)
                        {
                            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "(NH)");
                            ImGui::SameLine();
                            ImGui::Text("Providers");
                        }
                        if (column == 1)
                        {
                            ImGui::Text((char *)handler.key.c_str());
                        }
                        if (column == 2)
                        {
                            ImGui::Text((char *)&handler.value);
                        }
                    }
                }
                ImGui::EndTable();
            }

            // Vérifier si une ligne a été sélectionnée et afficher la popup
            if (selected_row != -1)
            {
                if (ImGui::BeginPopup("my_select_popup"))
                {
                    // Vous pouvez afficher le contenu de la popup ici
                    ImGui::Text("Selected Row: %d", selected_row);
                    ImGui::Separator();
                    // Ajoutez d'autres éléments à la popup si nécessaire
                    ImGui::EndPopup();
                }
            }

            // Redimensionnement automatique de la hauteur entre les deux enfants
            float minHeight = ImGui::GetContentRegionAvail().y;                       // Hauteur minimale
            float maxHeight = ImGui::GetIO().DisplaySize.y - ImGui::GetCursorPos().y; // Hauteur maximale

            if (minHeight > maxHeight)
            {
                minHeight = maxHeight;
            }
        }
        ImGui::End();
    }

    void UI_ShowRegisteredChannels()
    {
        if (!m_UI_ShowRegisteredChannels)
            return;

        if (ImGui::Begin("Registered channels"))
        {
            // Colonne 1 (Enfant 1)

            static ImGuiTableFlags flags =
                ImGuiTableFlags_ScrollY |
                ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter |
                ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable |
                ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

            int selected_row = -1; // Variable pour suivre la ligne sélectionnée

            if (ImGui::BeginTable("table1", 3, flags))
            {
                ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Tag", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Reference", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableHeadersRow();
                for (auto handler : m_ctx->registeredChannels->creatorMap)
                {
                    ImGui::TableNextRow();

                    for (int column = 0; column < 2; column++)
                    {
                        ImGui::TableSetColumnIndex(column);

                        if (column == 0)
                        {
                            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "(NH)");
                            ImGui::SameLine();
                            ImGui::Text("Channel");
                        }
                        if (column == 1)
                        {
                            ImGui::Text((char *)handler.key.c_str());
                        }
                        if (column == 2)
                        {
                            ImGui::Text((char *)&handler.value);
                        }
                    }
                }
                ImGui::EndTable();
            }

            // Vérifier si une ligne a été sélectionnée et afficher la popup
            if (selected_row != -1)
            {
                if (ImGui::BeginPopup("my_select_popup"))
                {
                    // Vous pouvez afficher le contenu de la popup ici
                    ImGui::Text("Selected Row: %d", selected_row);
                    ImGui::Separator();
                    // Ajoutez d'autres éléments à la popup si nécessaire
                    ImGui::EndPopup();
                }
            }

            // Redimensionnement automatique de la hauteur entre les deux enfants
            float minHeight = ImGui::GetContentRegionAvail().y;                       // Hauteur minimale
            float maxHeight = ImGui::GetIO().DisplaySize.y - ImGui::GetCursorPos().y; // Hauteur maximale

            if (minHeight > maxHeight)
            {
                minHeight = maxHeight;
            }
        }
        ImGui::End();
    }

    void UI_ShowRegisteredNotificationsHandlers()
    {
        if (!m_UI_ShowRegisteredNotificationsHandlers)
            return;

        if (ImGui::Begin("Registered notification handlers"))
        {
            // Colonne 1 (Enfant 1)

            static ImGuiTableFlags flags =
                ImGuiTableFlags_ScrollY |
                ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter |
                ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable |
                ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

            int selected_row = -1; // Variable pour suivre la ligne sélectionnée

            if (ImGui::BeginTable("table1", 3, flags))
            {
                ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Tag", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Reference", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableHeadersRow();
                for (auto handler : m_ctx->registeredNotificationHandlers.typeMap)
                {
                    ImGui::TableNextRow();

                    for (int column = 0; column < 2; column++)
                    {
                        ImGui::TableSetColumnIndex(column);

                        if (column == 0)
                        {
                            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "(NH)");
                            ImGui::SameLine();
                            ImGui::Text("Notification Handler");
                        }
                        if (column == 1)
                        {
                            ImGui::Text((char *)handler.key.c_str());
                        }
                        if (column == 2)
                        {
                            ImGui::Text((char *)&handler.value);
                        }
                    }
                }
                ImGui::EndTable();
            }

            // Vérifier si une ligne a été sélectionnée et afficher la popup
            if (selected_row != -1)
            {
                if (ImGui::BeginPopup("my_select_popup"))
                {
                    // Vous pouvez afficher le contenu de la popup ici
                    ImGui::Text("Selected Row: %d", selected_row);
                    ImGui::Separator();
                    // Ajoutez d'autres éléments à la popup si nécessaire
                    ImGui::EndPopup();
                }
            }

            // Redimensionnement automatique de la hauteur entre les deux enfants
            float minHeight = ImGui::GetContentRegionAvail().y;                       // Hauteur minimale
            float maxHeight = ImGui::GetIO().DisplaySize.y - ImGui::GetCursorPos().y; // Hauteur maximale

            if (minHeight > maxHeight)
            {
                minHeight = maxHeight;
            }
        }
        ImGui::End();
    }

    void UI_ShowRegisteredMatrixes()
    {
        if (!m_UI_ShowRegisteredMatrixes)
            return;

        if (ImGui::Begin("Registered matrixes"))
        {
            // Colonne 1 (Enfant 1)

            static ImGuiTableFlags flags =
                ImGuiTableFlags_ScrollY |
                ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter |
                ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable |
                ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

            int selected_row = -1; // Variable pour suivre la ligne sélectionnée

            if (ImGui::BeginTable("table1", 3, flags))
            {
                ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Tag", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Reference", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableHeadersRow();
                for (auto handler : m_ctx->registeredMatrixes->creatorMap)
                {
                    ImGui::TableNextRow();

                    for (int column = 0; column < 2; column++)
                    {
                        ImGui::TableSetColumnIndex(column);

                        if (column == 0)
                        {
                            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.3f, 1.0f), "(M)");
                            ImGui::SameLine();
                            ImGui::Text("Matrixes");
                        }
                        if (column == 1)
                        {
                            ImGui::Text((char *)handler.key.c_str());
                        }
                        if (column == 2)
                        {
                            ImGui::Text((char *)handler.value);
                        }
                    }
                }
                ImGui::EndTable();
            }

            // Vérifier si une ligne a été sélectionnée et afficher la popup
            if (selected_row != -1)
            {
                if (ImGui::BeginPopup("my_select_popup"))
                {
                    // Vous pouvez afficher le contenu de la popup ici
                    ImGui::Text("Selected Row: %d", selected_row);
                    ImGui::Separator();
                    // Ajoutez d'autres éléments à la popup si nécessaire
                    ImGui::EndPopup();
                }
            }

            // Redimensionnement automatique de la hauteur entre les deux enfants
            float minHeight = ImGui::GetContentRegionAvail().y;                       // Hauteur minimale
            float maxHeight = ImGui::GetIO().DisplaySize.y - ImGui::GetCursorPos().y; // Hauteur maximale

            if (minHeight > maxHeight)
            {
                minHeight = maxHeight;
            }
        }
        ImGui::End();
    }

    void UI_ShowNetworkSentCommunications()
    {
        if (!m_UI_ShowNetworkSentCommunications)
            return;

        if (ImGui::Begin("Sent communications"))
        {
            // Colonne 1 (Enfant 1)

            static ImGuiTableFlags flags =
                ImGuiTableFlags_ScrollY |
                ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter |
                ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable |
                ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

            int selected_row = -1; // Variable pour suivre la ligne sélectionnée

#ifdef USE_REGISTERING
            if (ImGui::BeginTable("tableofsentcommunications", 7, flags))
            {
                ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Lenght", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Tag", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("N. Params", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Timestamp", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Body", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();
                for (int row = 0; row < m_ctx->registeredNotificationSent.notifications.size(); row++)
                {
                    ImGui::TableNextRow();

                    for (int column = 0; column < 7; column++)
                    {
                        int numberparam = 0;
                        ImGui::TableSetColumnIndex(column);

                        if (column == 0)
                        {
                            ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "(S)");
                            ImGui::SameLine();
                            ImGui::Text("Notification");
                        }
                        if (column == 1)
                        {
                            ImU32 color = GetColorForValue(m_ctx->registeredNotificationSent.notifications[row]->lenght);
                            ImGui::TextColored(ImColor(color), "%d bytes", m_ctx->registeredNotificationSent.notifications[row]->lenght);
                        }
                        if (column == 2)
                        {
                            ImGui::Text((char *)m_ctx->registeredNotificationSent.notifications[row]->tag.c_str());
                        }
                        if (column == 3)
                        {
                            ImGui::Text((char *)m_ctx->registeredNotificationSent.notifications[row]->type.c_str());
                        }
                        if (column == 4)
                        {
                            std::string params;
                            bool first = true;
                            for (auto param : m_ctx->registeredNotificationSent.notifications[row]->params)
                            {
                                if (!first)
                                    params += ",";
                                params += param.key.c_str();
                                numberparam++;
                                first = false;
                            }

                            const char *pp;
                            pp += numberparam;

                            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "[ %i ]", pp);
                            ImGui::SameLine();
                            ImGui::Text((char *)params.c_str());
                        }
                        if (column == 5)
                        {
                            ImGui::Text((char *)m_ctx->registeredNotificationSent.notifications[row]->timestamp.c_str());
                        }
                        if (column == 6)
                        {
                            ImGui::Text((char *)m_ctx->registeredNotificationSent.notifications[row]->body.c_str());
                        }

                        // Rendre la ligne sélectionnable
                        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
                        {
                            selected_row = row; // Enregistrer la ligne sélectionnée
                        }
                    }
                }
                ImGui::EndTable();
            }
#endif
            // Vérifier si une ligne a été sélectionnée et afficher la popup
            if (selected_row != -1)
            {
                if (ImGui::BeginPopup("fghjfghj"))
                {
                    // Vous pouvez afficher le contenu de la popup ici
                    ImGui::Text("Selected Row: %d", selected_row);
                    ImGui::Separator();
                    // Ajoutez d'autres éléments à la popup si nécessaire
                    ImGui::EndPopup();
                }
            }

            // Redimensionnement automatique de la hauteur entre les deux enfants
            float minHeight = ImGui::GetContentRegionAvail().y;                       // Hauteur minimale
            float maxHeight = ImGui::GetIO().DisplaySize.y - ImGui::GetCursorPos().y; // Hauteur maximale

            if (minHeight > maxHeight)
            {
                minHeight = maxHeight;
            }
        }
        ImGui::End();
    }

    void UI_ShowNetworkReceivedCommunications()
    {
        if (!m_UI_ShowNetworkReceivedCommunications)
            return;

        if (ImGui::Begin("Received communications"))
        {
            // Colonne 1 (Enfant 1)

            static ImGuiTableFlags flags =
                ImGuiTableFlags_ScrollY |
                ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter |
                ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable |
                ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

            int selected_row = -1; // Variable pour suivre la ligne sélectionnée

#ifdef USE_REGISTERING
            if (ImGui::BeginTable("table1", 7, flags))
            {
                ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Lenght", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Tag", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("N. Params", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Timestamp", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Body", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();
                for (int row = 0; row < m_ctx->registeredNotificationReceived.notifications.size(); row++)
                {
                    ImGui::TableNextRow();

                    for (int column = 0; column < 7; column++)
                    {
                        ImGui::TableSetColumnIndex(column);

                        if (column == 0)
                        {
                            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "(R)");
                            ImGui::SameLine();
                            ImGui::Text("Notification");
                        }
                        if (column == 1)
                        {
                            ImU32 color = GetColorForValue(m_ctx->registeredNotificationReceived.notifications[row]->lenght);
                            ImGui::TextColored(ImColor(color), "%d bytes", m_ctx->registeredNotificationReceived.notifications[row]->lenght);
                        }
                        if (column == 2)
                        {
                            ImGui::Text((char *)m_ctx->registeredNotificationReceived.notifications[row]->tag.c_str());
                        }
                        if (column == 3)
                        {
                            ImGui::Text((char *)m_ctx->registeredNotificationReceived.notifications[row]->type.c_str());
                        }
                        if (column == 4)
                        {
                            std::string params;
                            bool first = true;
                            int numberparam = 0;
                            for (auto param : m_ctx->registeredNotificationReceived.notifications[row]->params)
                            {
                                if (!first)
                                    params += ",";
                                params += param.key.c_str();
                                numberparam++;
                                first = false;
                            }

                            const char *pp;
                            pp += numberparam;

                            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "[ %i", pp, "]");
                            ImGui::SameLine();
                            ImGui::Text((char *)params.c_str());
                        }
                        if (column == 5)
                        {
                            ImGui::Text((char *)m_ctx->registeredNotificationReceived.notifications[row]->timestamp.c_str());
                        }
                        if (column == 6)
                        {
                            ImGui::Text((char *)m_ctx->registeredNotificationReceived.notifications[row]->body.c_str());
                        }

                        // Rendre la ligne sélectionnable
                        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
                        {
                            selected_row = row; // Enregistrer la ligne sélectionnée
                        }
                    }
                }
                ImGui::EndTable();
            }
#endif
            // Vérifier si une ligne a été sélectionnée et afficher la popup
            if (selected_row != -1)
            {
                if (ImGui::BeginPopup("my_select_popup"))
                {
                    // Vous pouvez afficher le contenu de la popup ici
                    ImGui::Text("Selected Row: %d", selected_row);
                    ImGui::Separator();
                    // Ajoutez d'autres éléments à la popup si nécessaire
                    ImGui::EndPopup();
                }
            }

            // Redimensionnement automatique de la hauteur entre les deux enfants
            float minHeight = ImGui::GetContentRegionAvail().y;                       // Hauteur minimale
            float maxHeight = ImGui::GetIO().DisplaySize.y - ImGui::GetCursorPos().y; // Hauteur maximale

            if (minHeight > maxHeight)
            {
                minHeight = maxHeight;
            }
        }
        ImGui::End();
    }

    void UI_ShowNetworkReceivedTransmissions()
    {
        if (!m_UI_ShowNetworkReceivedTransmissions)
            return;

        if (ImGui::Begin("Received transmissions"))
        {
            // Colonne 1 (Enfant 1)

            static ImGuiTableFlags flags =
                ImGuiTableFlags_ScrollY |
                ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter |
                ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable |
                ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

            int selected_row = -1; // Variable pour suivre la ligne sélectionnée

#ifdef USE_REGISTERING
            if (ImGui::BeginTable("table1", 7, flags))
            {
                ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Lenght", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Channel", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Timestamp", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Buffer", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();
                for (int row = 0; row < m_ctx->registeredTransmissionsReceived.transmissions.size(); row++)
                {
                    ImGui::TableNextRow();

                    for (int column = 0; column < 5; column++)
                    {
                        ImGui::TableSetColumnIndex(column);

                        if (column == 0)
                        {
                            ImGui::TextColored(ImVec4(0.3f, 0.5f, 1.0f, 1.0f), "(T)");
                            ImGui::SameLine();
                            ImGui::Text("Transmission");
                        }
                        if (column == 1)
                        {
                            ImU32 color = GetColorForValue(m_ctx->registeredTransmissionsReceived.transmissions[row]->buffer.length());
                            ImGui::TextColored(ImColor(color), "%d bytes", m_ctx->registeredTransmissionsReceived.transmissions[row]->buffer.length());
                        }
                        if (column == 2)
                        {
                            ImGui::Text((char *)m_ctx->registeredTransmissionsReceived.transmissions[row]->channel.c_str());
                        }
                        if (column == 3)
                        {
                            ImGui::Text((char *)m_ctx->registeredTransmissionsReceived.transmissions[row]->timestamp.c_str());
                        }
                        if (column == 4)
                        {
                            ImGui::Text((char *)m_ctx->registeredTransmissionsReceived.transmissions[row]->buffer.c_str());
                        }

                        // Rendre la ligne sélectionnable
                        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
                        {
                            selected_row = row; // Enregistrer la ligne sélectionnée
                        }
                    }
                }
                ImGui::EndTable();
            }
#endif
            // Vérifier si une ligne a été sélectionnée et afficher la popup
            if (selected_row != -1)
            {
                if (ImGui::BeginPopup("my_select_popup"))
                {
                    // Vous pouvez afficher le contenu de la popup ici
                    ImGui::Text("Selected Row: %d", selected_row);
                    ImGui::Separator();
                    // Ajoutez d'autres éléments à la popup si nécessaire
                    ImGui::EndPopup();
                }
            }

            // Redimensionnement automatique de la hauteur entre les deux enfants
            float minHeight = ImGui::GetContentRegionAvail().y;                       // Hauteur minimale
            float maxHeight = ImGui::GetIO().DisplaySize.y - ImGui::GetCursorPos().y; // Hauteur maximale

            if (minHeight > maxHeight)
            {
                minHeight = maxHeight;
            }
        }
        ImGui::End();
    }

    void UI_ShowNetworkSentTransmissions()
    {
        if (!m_UI_ShowNetworkSentTransmissions)
            return;

        if (ImGui::Begin("Sent transmissions"))
        {
            // Colonne 1 (Enfant 1)

            ImGui::Text("Total sent transmissions : ");
            ImGui::SameLine();
#ifdef USE_REGISTERING
            ImGui::Text((char *)std::to_string(m_ctx->registeredTransmissionsSent.transmissions.size()).c_str());
#endif

            static ImGuiTableFlags flags =
                ImGuiTableFlags_ScrollY |
                ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter |
                ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable |
                ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

            int selected_row = -1; // Variable pour suivre la ligne sélectionnée

#ifdef USE_REGISTERING
            if (ImGui::BeginTable("table1", 7, flags))
            {
                ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Lenght", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Channel", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Timestamp", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Buffer", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();
                for (int row = 0; row < m_ctx->registeredTransmissionsSent.transmissions.size(); row++)
                {
                    ImGui::TableNextRow();

                    for (int column = 0; column < 5; column++)
                    {
                        ImGui::TableSetColumnIndex(column);

                        if (column == 0)
                        {
                            ImGui::TextColored(ImVec4(0.3f, 0.5f, 6.0f, 1.0f), "(ST)");
                            ImGui::SameLine();
                            ImGui::Text("Transmission");
                        }
                        if (column == 1)
                        {
                            ImU32 color = GetColorForValue(m_ctx->registeredTransmissionsSent.transmissions[row]->buffer.length());
                            ImGui::TextColored(ImColor(color), "%d bytes", m_ctx->registeredTransmissionsSent.transmissions[row]->buffer.length());
                        }
                        if (column == 2)
                        {
                            ImGui::Text((char *)m_ctx->registeredTransmissionsSent.transmissions[row]->channel.c_str());
                        }
                        if (column == 3)
                        {
                            ImGui::Text((char *)m_ctx->registeredTransmissionsSent.transmissions[row]->timestamp.c_str());
                        }
                        if (column == 4)
                        {
                            ImGui::Text((char *)m_ctx->registeredTransmissionsSent.transmissions[row]->buffer.c_str());
                        }

                        // Rendre la ligne sélectionnable
                        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
                        {
                            selected_row = row; // Enregistrer la ligne sélectionnée
                        }
                    }
                }
                ImGui::EndTable();
            }
#endif
            // Vérifier si une ligne a été sélectionnée et afficher la popup
            if (selected_row != -1)
            {
                if (ImGui::BeginPopup("my_select_popup"))
                {
                    // Vous pouvez afficher le contenu de la popup ici
                    ImGui::Text("Selected Row: %d", selected_row);
                    ImGui::Separator();
                    // Ajoutez d'autres éléments à la popup si nécessaire
                    ImGui::EndPopup();
                }
            }

            // Redimensionnement automatique de la hauteur entre les deux enfants
            float minHeight = ImGui::GetContentRegionAvail().y;                       // Hauteur minimale
            float maxHeight = ImGui::GetIO().DisplaySize.y - ImGui::GetCursorPos().y; // Hauteur maximale

            if (minHeight > maxHeight)
            {
                minHeight = maxHeight;
            }
        }
        ImGui::End();
    }

    void UI_ShowCachedElements()
    {
        if (!m_UI_ShowCachedElements)
            return;

        if (ImGui::Begin("Cached Other Elements"))
        {
            ImGui::Text("Obsolette");
        }
        ImGui::End();
    }

    void UI_ShowActiveReceivers()
    {
        if (!m_UI_ShowActiveReceivers)
            return;

        if (ImGui::Begin("Actives Receivers"))
        {
            ImGui::Columns(2, "MatrixColumns", true); // Divise la fenêtre en deux colonnes avec une largeur égale

            // Colonne 1 (Enfant 1)

            // Votre liste d'éléments
            static bool selectedItems[5] = {false, false, false, false, false};
            static std::string dd[5] = {"All active matrixes", "Active [waiting]", "Active [active]", "Active [sleeping]", "Active [invalid]"};

            static ImGuiTableFlags flags =
                ImGuiTableFlags_ScrollY |
                ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter |
                ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable |
                ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

            if (ImGui::BeginTable("dfg", 1, flags))
            {
                ImGui::TableSetupColumn("Search a state of matrix :");
                ImGui::TableHeadersRow();
                for (int i = 0; i < 5; i++)
                {
                    if (ImGui::Selectable((char *)dd[i].c_str()))
                    {
                        for (int j = 0; j < 5; j++)
                        {
                            selectedItems[j] = false;
                        }
                        selectedItems[i] = true;
                    }
                }
                ImGui::EndTable();
            }

            // Colonne 2 (Enfant 2)
            ImGui::NextColumn();
            if (selectedItems[0])
            {

                static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                if (ImGui::BeginTable("table1", 4, flags))
                {
                    ImGui::TableSetupColumn("Pannel", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Protocol", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Tag", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("State", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableHeadersRow();
                    ImGui::TableNextRow();
                    for (auto m : m_ctx->IO.GetAllActiveReceivers())
                    {
                        for (int column = 0; column < 3; column++)
                        {
                            ImGui::TableSetColumnIndex(column);

                            if (column == 0)
                            {
                                if (ImGui::Button("Open"))
                                {
                                };
                            }
                            if (column == 1)
                            {
                                ImGui::Text((char *)m->GetInterfaceVariable("protocol").c_str());
                            }
                        }
                    }

                    ImGui::EndTable();
                }
            }
            else if (selectedItems[1])
            {
            }
            else if (selectedItems[2])
            {
                ImGui::Text("2 panneau");
            }
            else if (selectedItems[3])
            {
                ImGui::Text("3 panneau");
            }

            ImGui::Columns(1); // Reviens à une seule colonne

            // Redimensionnement automatique de la hauteur entre les deux enfants
            float minHeight = ImGui::GetContentRegionAvail().y;                       // Hauteur minimale
            float maxHeight = ImGui::GetIO().DisplaySize.y - ImGui::GetCursorPos().y; // Hauteur maximale

            if (minHeight > maxHeight)
            {
                minHeight = maxHeight;
            }
        }
        ImGui::End();
    }

    void UI_ShowActiveChannels()
    {
        if (!m_UI_ShowActiveChannels)
            return;

        if (ImGui::Begin("Actives Channels"))
        {
            ImGui::Columns(2, "MatrixColumns", true); // Divise la fenêtre en deux colonnes avec une largeur égale

            // Colonne 1 (Enfant 1)

            // Votre liste d'éléments
            static bool selectedItems[5] = {false, false, false, false, false};
            static std::string dd[5] = {"All active matrixes", "Active [waiting]", "Active [active]", "Active [sleeping]", "Active [invalid]"};

            static ImGuiTableFlags flags =
                ImGuiTableFlags_ScrollY |
                ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter |
                ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable |
                ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

            if (ImGui::BeginTable("dfg", 1, flags))
            {
                ImGui::TableSetupColumn("Search a state of matrix :");
                ImGui::TableHeadersRow();
                for (int i = 0; i < 5; i++)
                {
                    if (ImGui::Selectable((char *)dd[i].c_str()))
                    {
                        for (int j = 0; j < 5; j++)
                        {
                            selectedItems[j] = false;
                        }
                        selectedItems[i] = true;
                    }
                }
                ImGui::EndTable();
            }

            // Colonne 2 (Enfant 2)
            ImGui::NextColumn();
            if (selectedItems[0])
            {

                static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                if (ImGui::BeginTable("ActiveChannels", 4, flags))
                {
                    ImGui::TableSetupColumn("Pannel", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Matrix", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("State", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableHeadersRow();
                    ImGui::TableNextRow();
                    for (auto m : m_ctx->IO.GetAllActiveChannels())
                    {
                        ImGui::TableNextRow();
                        for (int column = 0; column < 3; column++)
                        {
                            ImGui::TableSetColumnIndex(column);

                            if (column == 0)
                            {
                                if (ImGui::Button("Open"))
                                {
                                };
                            }
                            if (column == 1)
                            {
                                ImGui::Text((char *)m->id.c_str());
                            }
                            if (column == 2)
                            {
                                ImGui::Text((char *)m->matrixID.c_str());
                            }
                            if (column == 3)
                            {
                                ImGui::Text((char *)m->state.c_str());
                            }
                        }
                    }

                    ImGui::EndTable();
                }
            }
            else if (selectedItems[1])
            {
            }
            else if (selectedItems[2])
            {
                ImGui::Text("2 panneau");
            }
            else if (selectedItems[3])
            {
                ImGui::Text("3 panneau");
            }

            ImGui::Columns(1); // Reviens à une seule colonne

            // Redimensionnement automatique de la hauteur entre les deux enfants
            float minHeight = ImGui::GetContentRegionAvail().y;                       // Hauteur minimale
            float maxHeight = ImGui::GetIO().DisplaySize.y - ImGui::GetCursorPos().y; // Hauteur maximale

            if (minHeight > maxHeight)
            {
                minHeight = maxHeight;
            }
        }
        ImGui::End();
    }

    void UI_ShowContextRuntimeElements()
    {
        if (!m_UI_ShowRuntimeElements)
            return;

        if (ImGui::Begin("Actives Runtime Elements"))
        {

                static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                if (ImGui::BeginTable("jghjkkkk", 5, flags))
                {
                    ImGui::TableSetupColumn("Pannel", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Tag", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("State", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("D", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableHeadersRow();
                    ImGui::TableNextRow();
                    for (auto element : m_ctx->IO.activeRuntimeElements)
                    {

                        ImGui::TableNextRow();
                        for (int column = 0; column < 4; column++)
                        {
                            ImGui::TableSetColumnIndex(column);

                            if (column == 0)
                            {
                                std::string label = "Open ";
                            }
                            if (column == 1)
                            {
                                ImGui::Text((char *)element->id.c_str());
                            }
                            if (column == 2)
                            {
                                ImGui::Text((char *)element->tag.c_str());
                            }
                            if (column == 3)
                            {
                            }
                            if (column == 4)
                            {
                            }
                        }
                    }

                    ImGui::EndTable();
                }
          
        }
        ImGui::End();
    }


    void UI_ShowContextMatrixes()
    {
        if (!m_UI_ShowContextMatrixes)
            return;

        if (ImGui::Begin("Actives Matrixes"))
        {
            ImGui::Columns(2, "MatrixColumns", true); // Divise la fenêtre en deux colonnes avec une largeur égale

            // Colonne 1 (Enfant 1)

            // Votre liste d'éléments
            static bool selectedItems[5] = {false, false, false, false, false};
            static std::string dd[5] = {"All active matrixes", "Active [waiting]", "Active [active]", "Active [sleeping]", "Active [invalid]"};

            static ImGuiTableFlags flags =
                ImGuiTableFlags_ScrollY | ImGuiTableFlags_BordersOuter |
                ImGuiTableFlags_BordersV |
                ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

            if (ImGui::BeginTable("dfg", 1, flags))
            {
                ImGui::TableSetupColumn("Search a state of matrix :");
                ImGui::TableHeadersRow();
                for (int i = 0; i < 5; i++)
                {
                    if (ImGui::Selectable((char *)dd[i].c_str()))
                    {
                        for (int j = 0; j < 5; j++)
                        {
                            selectedItems[j] = false;
                        }
                        selectedItems[i] = true;
                    }
                }
                ImGui::EndTable();
            }

            // Colonne 2 (Enfant 2)
            ImGui::NextColumn();
            if (selectedItems[0])
            {

                static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                if (ImGui::BeginTable("gdfgh", 5, flags))
                {
                    ImGui::TableSetupColumn("Pannel", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Tag", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("State", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("D", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableHeadersRow();
                    ImGui::TableNextRow();
                    //int i = 0;
                    //for (auto m : m_ctx->IO.GetAllActiveMatrixes())
                    for (int i = 0; i < m_ctx->IO.GetAllActiveMatrixes().size(); i++)
                    {

                        ImGui::TableNextRow();
                        for (int column = 0; column < 4; column++)
                        {
                            ImGui::TableSetColumnIndex(column);

                            if (column == 0)
                            {
                                std::string label = "Open ";
                                label += m_ctx->IO.GetAllActiveMatrixes()[i]->GetId().c_str();

                                if (ImGui::Button((char*)label.c_str()))
                                {
                                    this->ShowMatrixPannel(m_ctx->IO.GetAllActiveMatrixes()[i]);
                                };
                            }
                            if (column == 1)
                            {
                                ImGui::Text((char *)m_ctx->IO.GetAllActiveMatrixes()[i]->GetId().c_str());
                            }
                            if (column == 2)
                            {
                                ImGui::Text((char *)m_ctx->IO.GetAllActiveMatrixes()[i]->GetTag().c_str());
                            }
                            if (column == 3)
                            {
                                ImGui::Text((char *)m_ctx->IO.GetAllActiveMatrixes()[i]->GetState().c_str());
                            }
                            if (column == 4)
                            {
                                ImGui::Text(std::to_string(i).c_str());
                            }
                        }
                    }

                    ImGui::EndTable();
                }
            }
            else if (selectedItems[1])
            {
            }
            else if (selectedItems[2])
            {
                ImGui::Text("2 panneau");
            }
            else if (selectedItems[3])
            {
                ImGui::Text("3 panneau");
            }

            ImGui::Columns(1); // Reviens à une seule colonne

            // Redimensionnement automatique de la hauteur entre les deux enfants
            float minHeight = ImGui::GetContentRegionAvail().y;                       // Hauteur minimale
            float maxHeight = ImGui::GetIO().DisplaySize.y - ImGui::GetCursorPos().y; // Hauteur maximale

            if (minHeight > maxHeight)
            {
                minHeight = maxHeight;
            }
        }
        ImGui::End();
    }

    void UI_JoinMatrix()
    {
        if (!m_RegisteredJoinMatrix)
            return;
        ImGui::Begin("Join a matrix");

        static char portal[4096] = "";
        ImGui::InputText("Matrix portal", portal, 4096);

        if (ImGui::Button("Join Matrix"))
        {
            // provider->TrySend(&i, mess);
        }

        if (ImGui::Button("Create Matrix"))
        {
            hString tag = "::decentralized_nodes";

            // HToolkitMatrix _m = HToolkit::CreateMatrix(tag);
            // std::cout << HToolkit::GetPortal((char *)_m.GetId().c_str()).c_str() << std::endl;

            // provider->TrySend(&i, mess);
        }
        ImGui::End();
    }

    void ShowContextMatrixes() { m_UI_ShowContextMatrixes = true; }
    void ShowNetworkReceivedCommunications() { m_UI_ShowNetworkReceivedCommunications = true; }
    void ShowNetworkReceivedTransmissions() { m_UI_ShowNetworkReceivedTransmissions = true; }
    void ShowNetworkSentTransmissions() { m_UI_ShowNetworkSentTransmissions = true; }
    void ShowRegisteredElements() { m_UI_ShowRegisteredElements = true; }
    void ShowNetworkSentCommunications() { m_UI_ShowNetworkSentCommunications = true; }
    void ShowActiveReceivers() { m_UI_ShowActiveReceivers = true; }
    void ShowActiveChannels() { m_UI_ShowActiveChannels = true; }
    void ShowCachedElements() { m_UI_ShowCachedElements = true; }
    void ShowRuntimeElements() { m_UI_ShowRuntimeElements = true; }

    
    void ShowRegisteredNotificationsHandlers() { m_UI_ShowRegisteredNotificationsHandlers = true; }
    void ShowRegisteredMatrixes() { m_UI_ShowRegisteredMatrixes = true; }
    void ShowRegisteredProviders() { m_UI_ShowRegisteredProviders = true; }
    void ShowRegisteredChannels() { m_UI_ShowRegisteredChannels = true; }

    void SHows() { m_RegisteredJoinMatrix = true; }

private:
    HtkContext *m_ctx;
    bool m_UI_ShowContextMatrixes = false;
    bool m_UI_ShowNetworkSentCommunications = false;
    bool m_UI_ShowNetworkReceivedCommunications = false;
    bool m_UI_ShowNetworkReceivedTransmissions = false;
    bool m_UI_ShowMatrixPannel = false;
    bool m_RegisteredJoinMatrix = false;
    bool m_UI_ShowChannelPannel = false;
    bool m_UI_ShowActiveReceivers = false;
    bool m_UI_ShowActiveChannels = false;
    bool m_UI_ShowRegisteredNotificationsHandlers = false;
    bool m_UI_ShowRegisteredMatrixes = false;
    bool m_UI_ShowRegisteredProviders = false;
    bool m_UI_ShowRegisteredChannels = false;
    bool m_UI_ShowRegisteredElements = false;
    bool m_UI_ShowNetworkSentTransmissions = false;
    bool m_UI_ShowCachedElements = false;
    bool m_UI_ShowRuntimeElements = false;
    

    HToolkitMatrix *m_currentMatrixForPannel;
    HToolkitChannel *m_currentChannelForPannel;
    HToolkitProvider *m_currentProviderForPannel;
};

Walnut::Application *Walnut::CreateApplication(int argc, char **argv, HtkContext *ctx)
{
    int port = atoi(argv[1]);

    Walnut::ApplicationSpecification spec;
    spec.Name = "Hypernet Devtools (HTK)";
    spec.CustomTitlebar = false;

    Walnut::Application *app = new Walnut::Application(spec);
    std::shared_ptr<ExampleLayer> exampleLayer = std::make_shared<ExampleLayer>(ctx);

    app->PushLayer(exampleLayer);
    app->SetMenubarCallback([app, exampleLayer]()
                            {
    if (ImGui::BeginMenu("O")) {
      if (ImGui::MenuItem("Exit")) {
        app->Close();
      }
      ImGui::EndMenu();
    }

		auto* fgDrawList = ImGui::GetForegroundDrawList();
		const ImVec2 windowPadding = ImGui::GetCurrentWindow()->WindowPadding;

const uint8_t g_image[] =
{
0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52, 
0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x0e, 0x08, 0x06, 0x00, 0x00, 0x00, 0x1f, 0x48, 0x2d, 
0xd1, 0x00, 0x00, 0x00, 0x77, 0x49, 0x44, 0x41, 0x54, 0x28, 0x91, 0xed, 0xd2, 0x21, 0x0e, 0x83, 
0x40, 0x10, 0x85, 0xe1, 0x8f, 0x86, 0x4b, 0x60, 0xab, 0xb9, 0x41, 0x91, 0x5c, 0x81, 0x13, 0xd6, 
0xf5, 0x0a, 0xf5, 0x75, 0xb8, 0xde, 0xa0, 0x49, 0x45, 0x65, 0x39, 0xc0, 0x90, 0x49, 0x40, 0x55, 
0xb0, 0x08, 0x5c, 0xff, 0x64, 0xb3, 0x2f, 0x9b, 0xfd, 0x33, 0x62, 0x9e, 0x88, 0xc8, 0xd3, 0xc5, 
0x4e, 0x6a, 0xf4, 0x68, 0xf1, 0xc6, 0xcd, 0x36, 0x57, 0x8c, 0x29, 0xde, 0xf1, 0x40, 0x60, 0xd8, 
0xd0, 0x1a, 0x7c, 0x33, 0xd4, 0xcb, 0xc3, 0xa5, 0x60, 0x52, 0xf2, 0x59, 0xc3, 0xa9, 0x50, 0xf8, 
0xe1, 0x2f, 0x1e, 0x21, 0x56, 0x59, 0xb4, 0xbc, 0x0b, 0xff, 0xe7, 0x1e, 0x27, 0x9c, 0xd7, 0x02, 
0x94, 0xf2, 0x5a, 0x2a, 0xf7, 0xcc, 0x82, 0xef, 0xa5, 0x8b, 0x08, 0x33, 0x0d, 0x1f, 0x7c, 0xf7, 
0xc7, 0x6d, 0xb0, 0xbe, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

		// Load images
		
			uint32_t w, h;
			void* data = Image::Decode(g_image, sizeof(g_image), w, h);
			auto image = std::make_shared<Walnut::Image>(w, h, ImageFormat::RGBA, data);
			free(data);
		

		// Logo
		{
			const int logoWidth = 12;// m_LogoTex->GetWidth();
			const int logoHeight = 12;// m_LogoTex->GetHeight();
			const ImVec2 logoOffset(16.0f + windowPadding.x, 5.0f + windowPadding.y);
			const ImVec2 logoRectStart = { ImGui::GetItemRectMin().x + logoOffset.x, ImGui::GetItemRectMin().y + logoOffset.y };
			const ImVec2 logoRectMax = { logoRectStart.x + logoWidth, logoRectStart.y + logoHeight };
			

    
fgDrawList->AddImage(image->GetDescriptorSet(), logoRectStart, logoRectMax);}

    if (ImGui::BeginMenu("Inspector")) {
      if (ImGui::MenuItem("Matrixes")) {
        exampleLayer->ShowContextMatrixes();
      }
      if (ImGui::MenuItem("Channels")) {
        exampleLayer->ShowActiveChannels();
      }
      
      if (ImGui::MenuItem("Elements")) {
        exampleLayer->ShowCachedElements();
      }
      if (ImGui::MenuItem("Receivers")) {
        exampleLayer->ShowActiveReceivers();
      }
      if (ImGui::MenuItem("Runtime elements")) {
        exampleLayer->ShowRuntimeElements();
      }

      ImGui::EndMenu();
    }


	
    
    if (ImGui::BeginMenu("Console")) {
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Contents")) {
      if (ImGui::MenuItem("Registered providers")) {
        exampleLayer->ShowRegisteredProviders();
      }
      if (ImGui::MenuItem("Registered matrixes")) {
        exampleLayer->ShowRegisteredMatrixes();
      }
      if (ImGui::MenuItem("Registered notifications")) {
      }
      if (ImGui::MenuItem("Registered channels")) {
        exampleLayer->ShowRegisteredChannels();
      }
      if (ImGui::MenuItem("Registered notifications handlers")) {
        exampleLayer->ShowRegisteredNotificationsHandlers();
      }
      if (ImGui::MenuItem("Registered elements types")) {
        exampleLayer->ShowRegisteredElements();
      }
      if (ImGui::MenuItem("Imported contents")) {
      }
      if (ImGui::MenuItem("Services")) {
      }
      ImGui::EndMenu();
    }


    if (ImGui::BeginMenu("Networking")) {
      if (ImGui::MenuItem("Timeline")) {
      }
      if (ImGui::MenuItem("Provider networking")) {
      }
      if (ImGui::MenuItem("Received communications")) {
        exampleLayer->ShowNetworkReceivedCommunications();
      }
      if (ImGui::MenuItem("Sent communications")) {
        exampleLayer->ShowNetworkSentCommunications();
      }
      if (ImGui::MenuItem("Received transmissions")) {
        exampleLayer->ShowNetworkReceivedTransmissions();
      }
      if (ImGui::MenuItem("Sent transmissions")) {
        exampleLayer->ShowNetworkSentTransmissions();
      }
      ImGui::EndMenu();
    }



    if (ImGui::BeginMenu("Application")) {
      if (ImGui::MenuItem("Environnement")) {
      }
      if (ImGui::MenuItem("Detected Matrixes")) {
      }
      if (ImGui::MenuItem("[GARBAGE Join matrix]")) {
        exampleLayer->SHows();
      }

      ImGui::EndMenu();
    }


    if (ImGui::BeginMenu("Security")) {
      if (ImGui::MenuItem("Channels Encryption")) {
      }
      if (ImGui::MenuItem("Self securities")) {
      }
      if (ImGui::MenuItem("My keys")) {
      }
      if (ImGui::MenuItem("Certifications")) {
      }
      if (ImGui::MenuItem("Signature & Hash")) {
      }
      ImGui::EndMenu();
    }



    if (ImGui::BeginMenu("Help")) {
      if (ImGui::MenuItem("About")) {
      }
      ImGui::EndMenu();
    } });

    return app;
}

// Faire plutot un combo Registered / Active / Innactive / Desactiverd / OnError
// et des types (matrix, provider etc)
// possibilité de filtrer