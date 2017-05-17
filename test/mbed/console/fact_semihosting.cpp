#include <mbed.h>
#include <mbed_events.h>

#include <BufferedSoftSerial.h>

#include "fact_semihosting.h"

//#define DEBUG_IS_AVAIL

BufferedSoftSerial serial(PA_10, PB_3);
Serial usb(USBTX, USBRX);

namespace FactUtilEmbedded { namespace std {

ostream cout(usb);
istream cin(usb);
ostream& clog = cout;

}}

using namespace FactUtilEmbedded::std;


streamsize bufferedsoftserial_is_avail(void* ctx)
{
    streamsize readable = ((BufferedSoftSerial*)ctx)->readable();

#ifdef DEBUG_IS_AVAIL
    if(readable > 0)
        clog << "Have some input" << endl;
#endif

    return readable;
}

int bufferedsoftserial_sbumpc(void* ctx)
{
    auto stream = (BufferedSoftSerial*)ctx;

    while(!stream->readable()) { Thread::yield(); }

    return stream->getc();
}


ostream ocserial(serial);
istream icserial(serial,
    bufferedsoftserial_is_avail,
    bufferedsoftserial_sbumpc);


void serial_setup()
{
    serial.baud(9600);
}
