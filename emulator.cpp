#include "emulator.h"

#include <iostream>
#include <fstream>

emulator::emulator()
{
    memset(mem, 0, EMULATOR_MEMORY_SIZE);
    memset(vx, 0, EMULATOR_DATA_REGISTERS);
    memset(stack, 0, EMULATOR_STACK_SIZE);
    
    ix = 0;
    sp = 0;
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
emulator::cycle()
{
    fetch_opcode();
    execute_op();
}

void
emulator::print_opcode()
{
    std::cout << "Opcode : " << std::hex << opcode << std::dec << std::endl;
}

void
emulator::increment_pc()
{
    pc += 2;
}

void
emulator::fetch_opcode()
{
    opcode = (mem[pc] << 8) | mem[pc+1];
    increment_pc();
}

void
emulator::execute_op()
{
    switch (opcode >> 12)
    {
        case 0:
        {
            if (opcode == 0x00EE)
            {
                sp -= 1;
                pc = stack[sp];
            }
        } break;
        case 1:
        {

        } break;
        case 2:
        {
            stack[sp] = pc;
            sp += 1;
            pc = opcode & 0x0FFF;
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
            uint8_t reg = (opcode & 0x0F00) >> 8;
            vx[reg] = (opcode & 0xFF);
        } break;
        case 7:
        {
            uint8_t reg = (opcode & 0x0F00) >> 8;
            vx[reg] += (opcode & 0xFF);
        } break;
        case 8:
        {

        } break;
        case 9:
        {

        } break;
        case 10:
        {
            ix = opcode & 0x0FFF;
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
            std::cerr << "Unsupported opcode fetched" << std::endl; 
        } break;
    }
}


