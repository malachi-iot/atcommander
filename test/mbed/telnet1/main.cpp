#include <mbed.h>

void telnet_setup();
void telnet_loop();

#include "fact_semihosting.h"

#include <fact/string_convert.h>

int main()
{
    serial_setup();
    telnet_setup();

    for(;;)
    {
        telnet_loop();
    }

    return 0;
}
