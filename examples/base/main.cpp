// A null exemple for API checking.
#include "../../htk.h"

// Contents
#include "../../stacks/hstd/matrixes/base/M_Base.h"
#include "../../stacks/hstd/channels/base/C_Base.h"
#include "../../stacks/hstd/elements/base/E_Base.h"


// Low level providers
#include "../../stacks/hstd/providers/ip/P_TcpIp.h" // TCP/IP

// Portals
#include "../../stacks/hstd/portals/matrix/base/P_MatrixBuffer.h" // Matrix code portal

int main(int argc, char **argv){
    HTK_CHECKVERSION();

    std::cout << "===================================================================" << std::endl;
    std::cout << "|  Hypernet Toolkit v." << HTOOLKIT_VERSION << std::endl;
    std::cout << "|  This is a exemple of the implementation of HypernetToolkit runtime." << std::endl;
    std::cout << "===================================================================" << std::endl << std::endl;

    std::cout << " - Create context with \"CreateContext()\"" << std::endl;
    HtkContext* ctx = HToolkit::CreateContext();


    HToolkit::hstd::Providers::P_TcpIp::configure("172.0.0.1", 3000);
    HToolkit::StartProviders();

    std::cout << " - Create a new matrix with \"CreateMatrix()\"" << std::endl;
    hArgs matrix_args;

    hVector<hString> portals = {"::HSTD::P_MatrixBuffer"};
    matrix_args.add("portals_tag", &portals); // Add supported protals types

    hVector<hString> attempted_channels = {"::HSTD::C_Base"};
    matrix_args.add("attempted_channels", &attempted_channels); // Add supported channels types

    hVector<hString> attempted_elements = {"::HSTD::E_Base"};
    matrix_args.add("attempted_elements", &attempted_elements); // Add supported elements types

    HToolkitMatrix* _m = HToolkit::CreateMatrix("::HSTD::M_Base", "::HSTD::E_Base", &matrix_args);
    std::cout << "(Our matrix ID is \""<< _m->GetId().c_str() <<"\")" << std::endl;
    std::cout << "(Our element ID is \""<< _m->loaded_self->id.c_str() <<"\")" << std::endl;
    std::cout << "(Our element type is \""<< _m->loaded_self->tag.c_str() <<"\")" << std::endl;

    std::cout << " - Create a new channel in this new matrix with \"constructor_channel()\" function" << std::endl;
    hArgs channel_args;
    channel_args.add("attempted_elements", &attempted_elements);
    channel_args.add<hString>("mode", "static");
    channel_args.add<hString>("tag", "::HSTD::C_Base");
    channel_args.add<hString>("type", "::HSTD::T_String");
    HToolkitChannel* channel = _m->constructor_channel(&channel_args);
    std::cout << "\n Our channel ID is \""<< channel->GetId().c_str() <<"\"" << std::endl;

    // App runtime here.

    printf("We destroy the context with \"DestroyContext()\"\n");
    HToolkit::DestroyContext();
    return 0;
}