#include "emulator.h"

int main()
{
    emulator ch8;

    if (ch8.load_program("C:\\Users\\brandona\\Desktop\\white_noise\\games\\tetris.c8"))
    {
        while (!ch8.error_flag())
        {
            ch8.cycle();
            ch8.print_opcode();
            ch8.print_display();
        }
    }

    return 0;
}
