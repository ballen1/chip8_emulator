#include "emulator.h"

#include <iostream>

int main()
{
    emulator ch8;

    if (ch8.load_program("C:\\Users\\brandona\\Desktop\\white_noise\\games\\tetris.c8"))
    {
        std::string input;
        while (std::cin >> input)
        {
            ch8.cycle();
            ch8.print_opcode();
        }
    }

    return 0;
}
