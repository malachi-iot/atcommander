#include <mbed.h>
#include <mbed_events.h>

#include <BufferedSoftSerial.h>

#include <fact/iostream.h>

#define DEBUG_ATC_OUTPUT
// TODO: consider moving these includes into an atcommander folder
#include "hayes.h"

#define MAL_LED LED1

static void blinky(void) {
    static DigitalOut led(MAL_LED);
    led = !led;
    //printf("LED = %d \r\n",led.read());
}

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
    return ((BufferedSoftSerial*)ctx)->readable();
}

int bufferedsoftserial_getc(void* ctx)
{
    auto stream = (BufferedSoftSerial*)ctx;

    while(!stream->readable()) { Thread::wait(10); }

    return stream->getc();
}

ostream ocserial(serial);
istream icserial(serial, bufferedsoftserial_is_avail, bufferedsoftserial_getc);

static void echo2()
{
    static int counter = 0;

    for(;;)
    {
        if(++counter % 1000 == 0)
        {
            cout << "Heartbeat: " << counter << "\r\n";
        }

        if(icserial.rdbuf()->in_avail())
        {
            int c = icserial.get();
            cout.put(c);
        }
        else if(cin.rdbuf()->in_avail())
        {
            int c = cin.get();
            ocserial.put(c);
        }
        else
        {
            // 10 ms
            Thread::wait(10);
        }
    }
}



int main()
{
    Thread echoThread;

    EventQueue queue;

    clog << "Compiled at " __TIME__ "\r\n";
    clog << "ciserial initialized as serial = " << icserial.rdbuf()->is_serial() << "\r\n";

    serial.baud(9600);

    //echoThread.start(echo2);

    queue.call_every(1000, blinky);

    ATCommander atc(icserial, ocserial);

    char buf[128];

    atc << "ATI";
    atc.send();

    for(;;)
    {
        atc.getline(buf, 128);

        clog << "Got result: " << buf << "\r\n";
        Thread::wait(5000);
    }

    //atc.command<hayes::standard_at::reset>();

    //hayes::standard_at::information(atc, 0, buf, 128);

    queue.dispatch();
}
