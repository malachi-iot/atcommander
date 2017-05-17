#include <mbed.h>
#include <mbed_events.h>

#include <BufferedSoftSerial.h>

//#define DEBUG_ATC_OUTPUT
//#define DEBUG_ATC_INPUT
#define DEBUG_ATC_MATCH
#define DEBUG_PEEK

// TODO: consider moving these includes into an atcommander folder
#include "hayes.h"
#include "simcom.h"
#include "3gpp.h"

#include "secrets.h"

#include "fact_semihosting.h"

#define MAL_LED LED1

static void blinky(void) {
    static DigitalOut led(MAL_LED);
    led = !led;
    //printf("LED = %d \r\n",led.read());
}



using namespace FactUtilEmbedded::std;

    //,
    //bufferedsoftserial_sgetc);

static void echo2()
{
    static int counter = 0;

    for(;;)
    {
        /*
        if(++counter % 1000 == 0)
        {
            cout << "Heartbeat: " << counter << "\r\n";
        }*/
#ifdef DEBUG_PEEK
        int c;

        // FIX: peek locks things up in context of BufferedSoftSerial operations
        // and possibly , but hopefully, not other operations
        if(c = icserial.peek() != EOF)
        {
            // TODO: If non-character-acquiring-pointer-bumping-only flavor exists
            // use that instead
            int _c = icserial.get();
            cout.put(c);
        }
        else if(c = cin.peek() != EOF)
        {
            int _c = cin.get();
            ocserial.put(c);
        }
#else
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
#endif
        else
        {
            Thread::yield();
            // 10 ms
            //Thread::wait(10);
        }
    }
}


struct sim808 :
    public hayes::v250,
    public simcom::generic_at,
    public _3gpp::_27007
    {};


int main()
{
    Thread echoThread;

    EventQueue queue;

    clog << "Compiled at " __TIME__ "\r\n";
    //clog << "ciserial initialized as serial = " << icserial.rdbuf()->is_serial() << "\r\n";

    serial_setup();

    queue.call_every(1000, blinky);

    ATCommander atc(icserial, ocserial);

    uint8_t power_level = 1;

    //power_level = atc.status<sim808::phone_functionality>();

    //echoThread.start(echo2);
    //queue.dispatch();

    sim808::experimental::reset(atc);

    // turn off echo mode
    atc.command_with_echo<sim808::echo>(0);

    // make sure radios are on
    power_level = atc.status<sim808::phone_functionality>();

    char buf[128];

    sim808::information::command::request(atc, 0);
    sim808::information::command::response(atc, buf, 128);

    // attach to network
    atc.command<sim808::attach>(true);
    // register on network
    atc.command<sim808::registration>(1);
    // bringup wireless connection to GPRS
    //atc.command<simcom::generic_at::bringup_wireless>();
    atc.command<sim808::bearer_settings>(1, 1);

    if(power_level != 1)
    {
        // FIX: Need to fix SFINAE response dispatcher
        // It seems it doesn't like a reset right off the bat
        //atc.command<sim808::phone_functionality>(1);
        //atc.command<sim808::phone_functionality>(1, 1);
    }

    typedef sim808::http http;

    atc.error.reset();
    atc.command<http::init>();
    if(atc.error.at_result())
    {
        atc.error.reset();
        // Then it's probably because http was already initialized, so ignore it
    }

    atc.command<http::para>("CID", 1);
    atc.command<http::para>("URL", SLACK_WEBHOOK);
    atc.command<http::para>("CONTENT", "application/json");
    atc.command<http::ssl>(true);

    char notify[128];
    // TODO: optimize so we can do atc << commands here instead
    sprintf(notify, "{\"text\": \"%s\"}", "ATCommander ONLINE (built @ " __DATE__ " "  __TIME__ ")");
    atc.command<http::data>(notify);

    if(power_level == 1)
        http::action::experimental::post(atc);
    else
        clog << "Radios off, not sending POST" << endl;

    // leave initialized for now, since we are developing still
    //atc.command<http::term>();

    echoThread.start(echo2);

    queue.dispatch();
}
