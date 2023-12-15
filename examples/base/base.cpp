// A very simple chat application over TCP/IP

#include "../../../../htk.h"

#include "../../../../stacks/hstd/matrixes/base/M_BaseMatrix.h"
#include "../../../../stacks/hstd/channels/base/C_BaseChannel.h"
#include "../../../../stacks/hstd/elements/base/E_BaseElement.h"

#include "../../../../stacks/hstd/providers/ip/P_TcpIp.h"
#include "../../../../stacks/hstd/portals/matrix/base/P_MatrixBuffer.h"

#include "../../../../tools/devtools/app/src/devtools.h"

#include <thread> 

int main(int argc, char **argv){

    std::thread receiveThread;

    int port = atoi(argv[1]); 
    char* ip = "127.0.0.122";

    HtkContext* ctx = HToolkit::CreateContext();

    // Configure and start a TCP/IP provider
    HToolkit::hstd::Providers::P_TcpIp::configure(ip, port);
    HToolkit::StartProviders();

    hArgs selfelement_args;
    //HToolkitElement* element = HToolkit::CreateElementType("::HSTD::E_BaseElement", &selfelement_args);
    //ctx->IO.AttacheElementToIO(element);


    hArgs matrix_args;
    hVector<hString> portals;
    hVector<hString> attempted_channels;
    hVector<hString> attempted_elements;

    portals.push_back("::HSTD::P_MatrixBuffer");
    attempted_channels.push_back("::HSTD::C_BaseChannel");
    attempted_elements.push_back("::HSTD::E_BaseElement");

    matrix_args.add("portals_tag", &portals);
    //matrix_args.add("current_element", element);
    matrix_args.add("attempted_channels", &attempted_channels);
    matrix_args.add("attempted_elements", &attempted_elements);
    //matrix_args.add("initiator", &attempted_elements); // Ajouter son propre ID d'element et son provider interface

    HToolkitMatrix* _m = HToolkit::CreateMatrix("::HSTD::M_BaseMatrix", "::HSTD::E_BaseElement", &matrix_args);

    // When we receive & accept joining request
    hArgs channel_args;
    hVector<HToolkitNode*> chan_nodes;


    std::thread Thread([&]() {HToolkit::DebugTools(argc, argv, ctx);});
    receiveThread.swap(Thread);
 
    while(true){
    std::string userInput;

    std::cout << "What do you want to do ?" << std::endl;    
    std::cout << ">";
    std::getline(std::cin, userInput);

    if(userInput == "/matrix"){
        std::cout << "Enter portal >";
        std::getline(std::cin, userInput);

        hString le = userInput.c_str();
        hArgs adrgs;
        hString pp = "::HSTD::P_MatrixBuffer";
        adrgs.add("portal_type", pp);
        adrgs.add("buffer", le);

        HToolkit::HandleMatrixPortal(&adrgs);
    }   
    if(userInput == "/channel"){
        std::cout << "Enter portal : ";
        std::getline(std::cin, userInput);
    }
       
    if(userInput == "/send"){
        std::cout << "Enter a ::HSTD::String to send in every channels : ";
        std::getline(std::cin, userInput);
    }

    if(userInput == "/create"){

        std::cout << "What do you want to create ? : ";
        std::getline(std::cin, userInput);
        if(userInput == "channel"){
            hString mode = "static";
            hString tag = "::HSTD::C_BaseChannel";
            hString type = "::HSTD::T_String";
            channel_args.add("mode", mode); // Create to another element
            channel_args.add("tag", tag);
            channel_args.add("type", type);
            _m->constructor_channel(&channel_args);
        }

        //_m->CreateChannel("::HSTD::C_BaseChannel", &channel_args);

        /*
        En mode element : auto deduction des nodes et creation. (the element need to be loaded !)
        En mode node : creation "bete" sur les nodes impliqués avec une securité de la technologie low level.
        */
    }

   };


    receiveThread.join();

    return 0;
}