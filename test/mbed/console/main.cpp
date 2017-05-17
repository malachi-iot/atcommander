#include <mbed.h>
#include <mbed_events.h>

#include <BufferedSoftSerial.h>

//#define DEBUG_ATC_OUTPUT
//#define DEBUG_ATC_INPUT
#define DEBUG_ATC_MATCH
#define DEBUG_PEEK
#define FEATURE_MINIMAL
// FIX: Non-threaded mode doesn't work
// NOTE: Now it does... so I am thinking we have a pointer malfunction somehwere
//#define FEATURE_THREAD

//#define DEBUG_FEATURE_MINIMAL

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

static bool echo_loop()
{
#ifdef DEBUG_PEEK
    int c;

    // FIX: peek locks things up in context of BufferedSoftSerial operations
    // and possibly , but hopefully, not other operations
    /*
    if(icserial.rdbuf()->in_avail())
    {
        int c = icserial.get();
        cout.put(c);
    }*/
    // The trouble with peek is the xsgetn resolves for BufferedSoftSerial to
    // the NON buffered Stream::read, which in turn calls _getc, which in turn
    // goes to SoftSerial::_getc instead of BufferedSoftSerial::_getc, because
    // BufferedSoftSerial::_getc doesn't exit
    if(c = icserial.peek() != EOF)
    {
        clog << "Have some input II" << endl;

        // TODO: If non-character-acquiring-pointer-bumping-only flavor exists
        // use that instead
        int _c = icserial.get();
        cout.put(c);
    }
    else if(c = cin.peek() != EOF)
    {
        // this peek works fine
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
    else return false;

    return true;
}
    //,
    //bufferedsoftserial_sgetc);

static void echo_thread()
{
    clog << "Starting ECHO mode" << endl;

    static int counter = 0;

    for(;;)
    {
        if(!echo_loop())
            Thread::yield();
    }
}


static void echo_nonthread(EventQueue& eq)
{
    clog << "Starting ECHO [nonthread] mode" << endl;

    static int counter = 0;

    for(;;)
    {
        echo_loop();

        eq.dispatch(1);
    }
}


struct sim808 :
    public hayes::v250,
    public simcom::generic_at,
    public _3gpp::_27007
    {};


int main()
{
    clog << "Compiled at " __TIME__ "\r\n";
    //clog << "ciserial initialized as serial = " << icserial.rdbuf()->is_serial() << "\r\n";

    EventQueue queue;

    serial_setup();

    queue.call_every(1000, blinky);

    // FIX: Idenitfy why on FEATURE_MINIMAL if I don't define atc things don't work
    // (don't see Compiled at or Starting ECHO mode)
#ifdef DEBUG_FEATURE_MINIMAL
    ATCommander atc(icserial, ocserial);
#endif
#ifndef FEATURE_MINIMAL
#ifndef DEBUG_FEATURE_MINIMAL
    ATCommander atc(icserial, ocserial);
#endif

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
#endif

#ifdef FEATURE_THREAD
    Thread echoThread;

    echoThread.start(echo_thread);

    queue.dispatch();
#else
    echo_nonthread(queue);
#endif
}
