#include "emulator.h"

#include <iostream>
#include <fstream>

emulator::emulator()
{
    memset(mem, 0, EMULATOR_MEMORY_SIZE);
    memset(vx, 0, EMULATOR_DATA_REGISTERS);
    memset(stack, 0, EMULATOR_STACK_SIZE);
    
    ix = 0;

    pc = 512;
}

emulator::~emulator()
{

}

bool
emulator::load_program(std::string program)
{
    std::ifstream program_file(program, std::ios_base::in |
                                        std::ios_base::binary);

    if (program_file.is_open())
    {
        int mem_available = EMULATOR_MEMORY_SIZE - EMULATOR_MEMORY_PROGRAM_START;

        program_file.read((char*)&mem[EMULATOR_MEMORY_PROGRAM_START],
                            mem_available);

        return true;
    }

    std::cerr << program << " does not exist" << std::endl;
    return false;
}

void
emulator::fetch_opcode()
{
    opcode = (mem[pc] << 8) | mem[pc+1];
}
