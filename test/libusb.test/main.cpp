#include <iostream>
//#include <libusb.h>
#include <libusb-1.0/libusb.h>
//#include <libusbpp.hpp>
#include <cassert>
#include <cstdio>

using namespace std;

int main(int argc, char *argv[])
{
    //auto devices = LibUSB::LibUSB::FindAllDevices();
    libusb_context* ctx;
    libusb_device** device_list;

    libusb_init(&ctx);
    auto count = libusb_get_device_list(ctx, &device_list);

    assert(count > 0);

    for (size_t idx = 0; idx < count; ++idx) {
        libusb_device *device = device_list[idx];
        libusb_device_descriptor desc = {0};

        auto rc = libusb_get_device_descriptor(device, &desc);
        assert(rc == 0);

        printf("Vendor:Device = %04x:%04x\n", desc.idVendor, desc.idProduct);
    }

/*
    if(device_list != nullptr)
    {
        libusb_device* device = device_list[0];
        if(device != nullptr)
        {
            libusb_device_descriptor device_descriptor;
            libusb_get_device_descriptor(device, &device_descriptor);
            auto idProduct = device_descriptor.idProduct;
            cout << idProduct;
        }
    }
    */
    cout << "Hello World!" << endl;
    return 0;

    libusb_free_device_list(device_list, 1);
    libusb_exit(ctx);
}
