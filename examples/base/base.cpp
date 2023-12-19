// A null exemple for API checking.
#include "../../htk.h"

int main(int argc, char **argv){
    HTK_CHECKVERSION();

    printf("Hypernet Toolkit v.");
    printf(HTOOLKIT_VERSION, "\n");
    printf("\nCreateContext()\n");
    HtkContext* ctx = HToolkit::CreateContext();

    // App runtime here.

    printf("DestroyContext()\n");
    HToolkit::DestroyContext();
    return 0;
}