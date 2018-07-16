#include "emulator.h"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>

emulator::emulator()
{
    memset(mem, 0, EMULATOR_MEMORY_SIZE);
    memset(vx, 0, EMULATOR_DATA_REGISTERS);
    memset(stack, 0, EMULATOR_STACK_SIZE);
    memset(display, 0, EMULATOR_DISPLAY_WIDTH * EMULATOR_DISPLAY_HEIGHT);
    
    ix = 0;
    sp = 0;
    pc = 512;

    dt = 0;
    st = 0;

    error = false;
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
emulator::tick_timers()
{
    if (dt > 0)
    {
        dt--;
    }

    if (st > 0)
    {
        st--;
    }
}

bool
emulator::error_flag()
{
    return error;
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
            else
            {
                std::cerr << "0 case not implemented" << std::endl;
                error = true;
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
            uint8_t reg = (opcode & 0x0F00) >> 8;
            if (vx[reg] != (opcode & 0x00FF))
            {
                increment_pc();
            }
        } break;
        case 5:
        {
            std::cerr << "Case 5 not implemented" << std::endl;
            error = true;
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
            std::cerr << "Case 8 not implemented" << std::endl;
            error = true;
        } break;
        case 9:
        {
            std::cerr << "Case 9 not implemented" << std::endl;
            error = true;
        } break;
        case 10:
        {
            ix = opcode & 0x0FFF;
        } break;
        case 11:
        {
            std::cerr << "Case B not implemented" << std::endl;
            error = true;
        } break;
        case 12:
        {
            uint8_t reg = (opcode & 0x0F00) >> 8; 
            std::srand(std::time(nullptr));
            uint8_t rand_num = static_cast<uint8_t>(std::rand());
            uint8_t kk = opcode & 0xFF;
            vx[reg] = rand_num & kk;
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
                    uint8_t prev = display[x + col_num][y + r];
                    display[x + col_num][y + r] = (display[x + col_num][y + r]) ^ ((bool)(col & mask));
                    if (prev == 1 && display[x + col_num][y + r] == 0)
                    {
                        vx[EMULATOR_VF_REGISTER] = 1;
                    }
                    mask *= 2;
                }
            }
        } break;
        case 14:
        {
            std::cerr << "Case E not implemented" << std::endl;
            error = true;
        } break;
        case 15:
        {
            if ((opcode & 0xFF) == 0x15)
            {
                uint8_t reg = (opcode & 0x0F00) >> 8;
                dt = vx[reg];
            }
            else if ((opcode & 0xFF) == 0x1E)
            {
                uint8_t reg = (opcode & 0x0F00) >> 8;
                ix += vx[reg];
            }
            else
            {
                std::cerr << "Case F not implemented" << std::endl;
                error = true;
            }
        } break;
        default:
        {
            std::cerr << "Unsupported opcode fetched" << std::endl; 
        } break;
    }
}


