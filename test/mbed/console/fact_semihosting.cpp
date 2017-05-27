#include <mbed.h>
#include <mbed_events.h>

#include <BufferedSoftSerial.h>

#include "fact_semihosting.h"

//#define DEBUG_IS_AVAIL

class BufferedSoftSerialWrapper : public BufferedSoftSerial
{
public:
    BufferedSoftSerialWrapper(PinName tx, PinName rx) : BufferedSoftSerial(tx, rx) {}

    // The trouble with peek is the xsgetn resolves for BufferedSoftSerial to
    // the NON buffered Stream::read, which in turn calls _getc, which in turn
    // goes to SoftSerial::_getc instead of BufferedSoftSerial::_getc, because
    // BufferedSoftSerial::_getc doesn't exit
    // luckily read is virtual so we can properly override it here as BufferedSoftSerial should
    // have done
    virtual ssize_t read(void* buffer, size_t length) override
    {
        auto b = (uint8_t*) buffer;

        while(length--)
        {
            while (!readable());

            *b++ = getc();
        }

        return b - (uint8_t *)buffer;
    }
};

BufferedSoftSerialWrapper serial(PA_10, PB_3);
Serial usb(USBTX, USBRX);

namespace FactUtilEmbedded { namespace std {

ostream cout(usb);
istream cin(usb);
ostream& clog = cout;
ostream& cerr = cout;

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
