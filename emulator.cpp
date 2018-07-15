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

void
emulator::cycle()
{
    fetch_opcode();
    decode_opcode();
    execute_op();
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

void
emulator::decode_opcode()
{
    opcode_nibbles[3] = opcode >> 12;
    opcode_nibbles[2] = (opcode & 0x0FFF) >> 8;
    opcode_nibbles[1] = (opcode & 0x00FF) >> 4;
    opcode_nibbles[0] = (opcode & 0x000F);
}

void
emulator::execute_op()
{
    switch (opcode_nibbles[3])
    {
        case 0:
        {

        } break;
        case 1:
        {

        } break;
        case 2:
        {

        } break;
        case 3:
        {

        } break;
        case 4:
        {

        } break;
        case 5:
        {

        } break;
        case 6:
        {

        } break;
        case 7:
        {

        } break;
        case 8:
        {

        } break;
        case 9:
        {

        } break;
        case 10:
        {

        } break;
        case 11:
        {

        } break;
        case 12:
        {

        } break;
        case 13:
        {

        } break;
        case 14:
        {

        } break;
        case 15:
        {

        } break;
        default:
        {

        } break;
    }
}
