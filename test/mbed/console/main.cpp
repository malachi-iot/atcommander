#include <mbed.h>
#include <mbed_events.h>

#include <BufferedSoftSerial.h>

#include <fact/iostream.h>
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

int main()
{
    Thread echoThread;

    EventQueue queue;

    clog << "Compiled at " __TIME__ "\r\n";

    serial.baud(9600);

    //echoThread.start(echo);

    queue.call_every(1000, blinky);

    ATCommander atc(icserial, ocserial);

    char buf[128];

    hayes::standard_at::information(atc, 0, buf, 128);

    queue.dispatch();
}
