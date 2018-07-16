#include "emulator.h"

#include <iostream>
#include <fstream>

emulator::emulator()
{
    memset(mem, 0, EMULATOR_MEMORY_SIZE);
    memset(vx, 0, EMULATOR_DATA_REGISTERS);
    memset(stack, 0, EMULATOR_STACK_SIZE);
    memset(display, 0, EMULATOR_DISPLAY_WIDTH * EMULATOR_DISPLAY_HEIGHT);
    
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
emulator::print_display()
{
    for (int y = 0; y < EMULATOR_DISPLAY_HEIGHT; y++)
    {
        for (int x = 0; x < EMULATOR_DISPLAY_WIDTH; x++)
        {
            if (display[x][y])
            {
                std::cout << "*";
            }
            else
            {
                std::cout << "-";
            }
        }
        std::cout << std::endl;
    }
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
            pc = opcode & 0x0FFF;
        } break;
        case 2:
        {
            stack[sp] = pc;
            sp += 1;
            pc = opcode & 0x0FFF;
        } break;
        case 3:
        {
            uint8_t reg = (opcode & 0x0F00) >> 8;
            if (vx[reg] == (opcode & 0x00FF))
            {
                increment_pc();
            }
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
            // TODO:
            uint8_t reg = (opcode & 0x0F00) >> 8; 
        } break;
        case 13:
        {
            uint8_t bytes = opcode & 0xF;
            uint8_t x = vx[(opcode & 0x0F00) >> 8];
            uint8_t y = vx[(opcode & 0x00F0) >> 4];

            for (int r = 0; r < bytes; r++)
            {
                uint8_t col = mem[ix + r];
                int mask = 1;
                for (int col_num = 7; col_num >= 0; col_num--)
                {
                    display[x + col_num][y + r] = (col & mask);
                    mask *= 2;
                }
            }
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


