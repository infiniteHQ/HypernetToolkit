// A very simple chat application.
#include "../../../htk.h"

// Include Basic matrix class (directly register in the runtime)
#include "../../../stacks/hstd/matrixes/base/M_Base.h"

// Include Basic channel class (directly register in the runtime)
#include "../../../stacks/hstd/channels/base/C_StringSession.h"

// Include TCP/IP provider (directly include in the runtime)
#include "../../../stacks/hstd/providers/ip/P_TcpIp.h" // TCP/IP

// Include Matrix Buffer portal for bootstrapping (directly include in the runtime)
#include "../../../stacks/hstd/portals/matrix/base/P_MatrixBuffer.h"

struct E_ChatUser : public HToolkitElement {
    virtual void constructor(hArgs* args) override {
        this->id = hRandom::Generate6CharID();
        this->SetProp("username", args->get<hString>("prop:username", "unknow"));
    }

    virtual void init(hArgs* args) override{ 
        this->id = args->get<hString>("identifier", "unknow");
        this->SetProp("username", args->get<hString>("prop:username", "unknow"));
    }
};

struct C_ChatRoom : public C_StringSession {};


ADD_ELEMENT_TO_FACTORY(HSTD, E_ChatUser);
ADD_CHANNEL_TO_FACTORY(HSTD, C_ChatRoom);


int main(int argc, char **argv)
{
    HTK_CHECKVERSION();


    // Create a new runtime context
    HtkContext *ctx = HToolkit::CreateContext();

    // Configure & start providers
    hArgs selfelement_args;

    // Ajouter les différentes possibilités de création d'éléments.
    ctx->IO.currentElementTypes.push_back("::HSTD::E_ChatUser");

    int port = atoi(argv[1]); 
    char* ip = "127.0.0.122";

    // Configure and start a TCP/IP provider
    HToolkit::hstd::Providers::P_TcpIp::configure(ip, port);
    HToolkit::StartProviders();
    
    std::string userinput;
    std::cout << ">";
    std::getline(std::cin, userinput);

    if (userinput == "0")
    {
        hArgs matrix_args;

        // Add supported protals types
        hVector<hString> portals = {"::HSTD::P_MatrixBuffer"};
        matrix_args.add("portals_tag", &portals);

        // Add supported channels types
        hVector<hString> attempted_channels = {"::HSTD::C_Base"};
        matrix_args.add("attempted_channels", &attempted_channels);

        // Add supported elements types
        hVector<hString> attempted_elements = {"::HSTD::E_ChatUser"};
        matrix_args.add("attempted_elements", &attempted_elements);

        // Create a new matrix
        hArgs element_args;
        element_args.add<hString>("prop:username", "Diego");
        matrix_args.add("element_args", &element_args);

        HToolkitMatrix *_m = HToolkit::CreateMatrix("::HSTD::M_Base", "::HSTD::E_ChatUser", &matrix_args); // We create as a "::HSTD::E_Base" element.
        
        for(auto portal : _m->current_portals){
            if(portal.key == "::HSTD::P_MatrixBuffer"){
                std::cout << "==== PORTAL BEGIN ====" << std::endl;
                std::cout << portal.value->value.c_str() << std::endl;
                std::cout << "==== PORTAL END ====" << std::endl;
            }
        }

        // TODO : Add possibility to join and talk
        while(true){};
    }
    else{
        hArgs matrix_args;
        matrix_args.add<hString>("portal_type", "::HSTD::P_MatrixBuffer");
        matrix_args.add<hString>("buffer", userinput.c_str());
        HToolkit::HandleMatrixPortal(&matrix_args);


        // TODO : Add possibility to join and talk
        while(true){};

    }


    //
    // App runtime here.
    //


    HToolkit::DestroyContext();
    return 0;
}