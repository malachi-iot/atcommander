#include <iostream>
#include <fstream>

// copying basic concepts for newb thread: http://forum.arduino.cc/index.php?topic=185031.0
// this didn't help: stty -f /dev/tty.usbserial-AH00R97V sane raw pass8 -echo -hupcl clocal 115200

// http://stackoverflow.com/questions/11677639/two-way-c-communication-over-serial-connection
// http://stackoverflow.com/questions/109449/getting-a-file-from-a-stdfstream


#include <unistd.h>

using namespace std;

void raw_console()
{
    char ch;

    // hacky way to get FD for the ifstream.  Not proven to work
    // but hopefully we can use it to set usb/baud rate parameters
    auto fd = dup(0);
    close(fd);

    ifstream f;

    //f.open("/dev/ttyACM0");
    f.open("/dev/tty.usbserial-AH00R97V");

    cout << "USB opened" << endl;

    while (f.get(ch))
    {
      cout << ch;
    }

    cout << "Done" << endl;
}

int main(int argc, char *argv[])
{
    cout << "Hello World!" << endl;
    raw_console();
    return 0;
}
