//#define DEBUG_ATC_OUTPUT
#define DEBUG_ATC_INPUT
#define DEBUG_ATC_MATCH

#include "hayes.h"
#include "3gpp.h"
#include <target/simcom.h>

#include "fact_semihosting.h"

#include <Timer.h>
//#include <mbed.h> // only need this for Thread::yield

struct sim808 :
    public hayes::v250,
    public simcom::generic_at,
    public _3gpp::_27007
    {};

typedef sim808::ip ip;

ATCommander atc(icserial, ocserial);

mbed::Timer timer;


void sim808_setup()
{
    uint8_t power_level = 1;

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


    //atc.command<sim808::bearer_settings>(1, 1);

    if(power_level != 1)
    {
        // FIX: Need to fix SFINAE response dispatcher
        // It seems it doesn't like a reset right off the bat
        //atc.command<sim808::phone_functionality>(1);
        //atc.command<sim808::phone_functionality>(1, 1);
    }

}

void telnet_setup()
{
    clog << "sim808 setup" << endl;

    sim808_setup();

    clog << "IP setup" << endl;

    atc.command<ip::shutdown>(); // ensure IP is in initial state (critical)
    atc.command<ip::mux>(true); // turn on multiconnection mode
    atc.command<ip::receive_mode>('1'); // 1 = manual mode

    // Odd, but this seems to be necessary after setting IPmux = 1
    atc.command<sim808::apn_credentials>("CMNET");
    atc.command<sim808::bringup_wireless>();

    char ip_address[60];

    sim808::get_local_ip_address::command::request(atc);
    sim808::get_local_ip_address::command::response(atc, ip_address);
    //atc.command<sim808::get_local_ip_address>(); // Docs say we need this but I'm thinking and hoping we don't

    //atc.command<ip::start>("TCP", "rainmaker.wunderground.com", 23, 1);
    ip::start::command::request(atc, sim808::TCP, "rainmaker.wunderground.com", 23, 1);
    ip::start::command::response(atc, true); // process response in multiconnection mode

    // FIX: still need response handler for ip::start
    timer.start();
}


void telnet_get_input_char()
{

}

uint16_t telnet_get_site_input(uint8_t* input, uint16_t _request_length, uint16_t* _remaining_length = nullptr)
{
    //clog << "Input phase" << endl;

    int mux = 1;

    uint16_t request_length = _request_length;
    uint16_t response_length = 0;
    uint16_t remaining_length = 0;

    ip::receive::command::request(atc, '2', mux, request_length);
    ip::receive::command::response(atc, 2, mux, &response_length, &remaining_length);

    //clog << "response_length (2): " << response_length;
    //clog << "remaining_length (2): " << remaining_length;

    auto return_length = response_length;

    if(response_length)
    {
        //response_length = 10;
        //clog << "Pulling in response length of: " << response_length << endl;
        // TODO: implement a timeout for blocking read operation (hopefully
        // hidden within istream)
        while(response_length--)
        {
            int ch = atc.cin.get();

            if(ch >= 0)
                *input++ = ch;
        }

        //atc.cin.read((char*)input, response_length);
    }

    //clog << "Checking for OK" << endl;

    atc.check_for_ok();

    return return_length;
    //atc.command<ip::receive>();
}


void telnet_send_site_output(char c)
{
    ip::send::assign::request(atc, 1, 1);

    atc.ignore_whitespace_and_newlines();
    atc >> "> ";

    //Thread::yield();
    atc << c;

    atc.ignore_whitespace_and_newlines();

    //Thread::yield();
    ip::send::assign::response(atc, true); // true = mux mode = on

    /*
    ip::send::command::request(atc);
    ip::send::command::response(atc, ) */
}

void telnet_loop()
{
    if(cin.rdbuf()->in_avail())
    {
        int c = cin.get();
        telnet_send_site_output(c);
    }

    if(timer.read_ms() > 5000)
    {
        // FIX: If we make this input buffer 256, we error out grabbing remaining_length after response_length
        uint8_t input[128];

        uint16_t length;
        uint16_t remaining_length;

        // TODO: make this if a while once things get moving
        while((length = telnet_get_site_input(input, sizeof(input), &remaining_length)))
        {
            cout.write((char*)input, length);

            // If length read was less than a full buffer, then we
            // expect we've read everything currently available
            // and abort the loop
        }

        // we specifically reset at the end, we want to elongate delays
        // if it's time consuming to read , otherwise we'll be wacking the
        // read constantly
        timer.reset();
    }
}
