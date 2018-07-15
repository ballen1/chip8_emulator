#include "emulator.h"

int main()
{
    emulator ch8;

    if (ch8.load_program("C:\\Users\\brandona\\Desktop\\white_noise\\games\\tetris.c8"))
    {
        ch8.cycle();
    }

    return 0;
}
