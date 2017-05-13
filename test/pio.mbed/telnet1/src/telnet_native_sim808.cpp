#include "hayes.h"
#include "3gpp.h"
#include <target/simcom.h>

#include "fact_semihosting.h"

struct sim808 :
    public hayes::v250,
    public simcom::generic_at,
    public _3gpp::_27007
    {};

typedef sim808::ip ip;

ATCommander atc(icserial, ocserial);


void telnet_setup()
{

}


void telnet_get_input_char()
{

}

void telnet_get_site_input()
{

}


void telnet_send_site_output()
{
    //ip::receive::assign::request(atc);
    //atc.command<ip::receive>();
}


void telnet_loop()
{

}
