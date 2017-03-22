#include <iostream>
#include <libusb.h>


using namespace std;

int main(int argc, char *argv[])
{
    libusb_context* ctx;

    libusb_init(&ctx);
    cout << "Hello World!" << endl;
    return 0;

    libusb_exit(ctx);
}
