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

static void echo()
{
    for(;;)
    {
        if(serial.readable())
        {
            int c = serial.getc();
            usb.putc(c);
        }
        else if(usb.readable())
        {
            int c = usb.getc();
            serial.putc(c);
        }
        else
        {
            // 10 ms
            Thread::wait(10);
        }
    }
}


namespace FactUtilEmbedded { namespace std {

ostream cout(usb);
istream cin(usb);
ostream& clog = cout;

}}

using namespace FactUtilEmbedded::std;

ostream ocserial(serial);
istream icserial(serial);

static void echo2()
{
    static int counter = 0;

    for(;;)
    {
        if(++counter % 1000 == 0)
        {
            cout << "Heartbeat: " << counter << "\r\n";
        }

        // cuz SoftSerial "readable" isn't the same as Serial "readable" (fantastic then!)
        if(serial.readable())
        //if(icserial.rdbuf()->in_avail())
        {
            // unsure why icserial.get doesn't work though, it should still route
            // through stream interface
            //int c = icserial.get();
            int c = serial.getc();
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

    echoThread.start(echo2);

    queue.call_every(1000, blinky);

    ATCommander atc(icserial, ocserial);

    char buf[128];

    //hayes::standard_at::information(atc, 0, buf, 128);

    queue.dispatch();
}
