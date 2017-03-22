#include <iostream>
#include <fstream>

// copying basic concepts for newb thread: http://forum.arduino.cc/index.php?topic=185031.0
// this didn't help: stty -f /dev/tty.usbserial-AH00R97V sane raw pass8 -echo -hupcl clocal 115200

using namespace std;

void raw_console()
{
    char ch;
    ifstream f;
    //f.open("/dev/ttyACM0");
    f.open("/dev/tty.usbserial-AH00R97V");

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
