#include <mbed.h>
#include <mbed_events.h>

#include <BufferedSoftSerial.h>

#include <fact/iostream.h>

#define MAL_LED LED1

static void blinky(void) {
    static DigitalOut led(MAL_LED);
    led = !led;
    //printf("LED = %d \r\n",led.read());
}


BufferedSoftSerial serial(PA_10, PB_3);
Serial usb(USBTX, USBRX);

/*
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
*/

int main()
{
    Thread echoThread;

    EventQueue queue;

    printf("Compiled at: " __TIME__ "\r\n");

    serial.baud(9600);

    //echoThread.start(echo);

    queue.call_every(1000, blinky);

    queue.dispatch();
}
