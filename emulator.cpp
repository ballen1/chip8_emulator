#include "emulator.h"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>

uint8_t font[80] =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0,
    0x20, 0x60, 0x20, 0x20, 0x70,
    0xF0, 0x10, 0xF0, 0x80, 0xF0,
    0xF0, 0x10, 0xF0, 0x10, 0xF0,
    0x90, 0x90, 0xF0, 0x10, 0x10,
    0xF0, 0x80, 0xF0, 0x10, 0xF0,
    0xF0, 0x80, 0xF0, 0x90, 0xF0,
    0xF0, 0x10, 0x20, 0x40, 0x40,
    0xF0, 0x90, 0xF0, 0x90, 0xF0,
    0xF0, 0x90, 0xF0, 0x10, 0xF0,
    0xF0, 0x90, 0xF0, 0x90, 0x90,
    0xE0, 0x90, 0xE0, 0x90, 0xE0,
    0xF0, 0x80, 0x80, 0x80, 0xF0,
    0xE0, 0x90, 0x90, 0x90, 0xE0,
    0xF0, 0x80, 0xF0, 0x80, 0xF0,
    0xF0, 0x80, 0xF0, 0x80, 0x80
};

emulator::emulator()
{
    memset(mem, 0, EMULATOR_MEMORY_SIZE);
    memset(vx, 0, EMULATOR_DATA_REGISTERS);
    memset(stack, 0, EMULATOR_STACK_SIZE);
    memset(display, 0, EMULATOR_DISPLAY_WIDTH * EMULATOR_DISPLAY_HEIGHT);
    memset(inputs, 0, EMULATOR_INPUT_TOTAL);
    
    ix = 0;
    sp = 0;
    pc = 512;

    dt = 0;
    st = 0;

    error = false;
    draw_flag = false;
    waiting_for_input = false;
    input_slot = 0;

    count = 0;

    // Load fonts into memory
    for (int i = 0; i < 80; i++)
    {
        mem[i] = font[i]; 
    }
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
    if (!waiting_for_input)
    {
        draw_flag = false;
        fetch_opcode();
        execute_op();

        count++;

        if (count == 10)
        {
            tick_timers();
            count = 0;
        }
    }
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

void
emulator::set_key_down(uint8_t key)
{
    inputs[key] = 1;

    if (waiting_for_input)
    {
        waiting_for_input = false;
        vx[input_slot] = key;
    }
}

void
emulator::set_key_up(uint8_t key)
{
    inputs[key] = 0;
}

bool
emulator::get_display_pixel(int x, int y)
{
    return display[x][y];
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

bool
emulator::needs_draw()
{
    return draw_flag;
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
            if (opcode == 0x00E0)
            {
                for (int w = 0; w < EMULATOR_DISPLAY_WIDTH; w++)
                {
                    for (int h = 0; h < EMULATOR_DISPLAY_HEIGHT; h++)
                    {
                        display[w][h] = 0;
                    }
                }
            }
            else if (opcode == 0x00EE)
            {
                sp -= 1;
                pc = stack[sp];
            }
            else
            {
                std::cerr << "Case 0 not implemented" << std::endl;
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
            uint8_t cmd = (opcode & 0xF);
            if (cmd == 0)
            {
                uint8_t reg_x = (opcode & 0x0F00) >> 8;
                uint8_t reg_y = (opcode & 0x00F0) >> 4;
                vx[reg_x] = vx[reg_y];
            }
            else if (cmd == 1)
            {
                uint8_t reg_x = (opcode & 0x0F00) >> 8;
                uint8_t reg_y = (opcode & 0x00F0) >> 4;
                vx[reg_x] = (vx[reg_x] | vx[reg_y]);
            }
            else if (cmd == 2)
            {
                uint8_t reg_x = (opcode & 0x0F00) >> 8;
                uint8_t reg_y = (opcode & 0x00F0) >> 4;
                vx[reg_x] = vx[reg_x] & vx[reg_y];
            }
            else if (cmd == 3)
            {
                uint8_t reg_x = (opcode & 0x0F00) >> 8;
                uint8_t reg_y = (opcode & 0x00F0) >> 4;

                vx[reg_x] = (vx[reg_x] ^ vx[reg_y]);
            }
            else if (cmd == 4)
            {
                uint8_t reg_x = (opcode & 0x0F00) >> 8;
                uint8_t reg_y = (opcode & 0x00F0) >> 4;
                uint16_t sum = vx[reg_x] + vx[reg_y];
                vx[reg_x] = vx[reg_x] + vx[reg_y];
                
                if (sum > 0xFF)
                {
                    vx[15] = 1;
                }
                else
                {
                    vx[15] = 0;
                }
            }
            else if (cmd == 5)
            {
                uint8_t reg_x = (opcode & 0x0F00) >> 8;
                uint8_t reg_y = (opcode & 0x00F0) >> 4;
                
                if (vx[reg_x] > vx[reg_y])
                {
                    vx[15] = 1;
                }
                else
                {
                    vx[15] = 0;
                }

                vx[reg_x] -= vx[reg_y];
            }
            else if (cmd == 6)
            {
                uint8_t reg_x = (opcode & 0x0F00) >> 8;

                vx[0xF] = (vx[reg_x] & 0x1);
                vx[reg_x] >>= 1;
            }
            else if (cmd == 0xE)
            {
                uint8_t reg_x = (opcode & 0x0F00) >> 8;

                vx[0xF] = (vx[reg_x] & 0x80);
                vx[reg_x] <<= 1;
            }
            else
            {
                std::cerr << "Case 8 not implemented" << std::endl;
                error = true;
            }
        } break;
        case 9:
        {
            uint8_t reg_x = (opcode & 0x0F00) >> 8;
            uint8_t reg_y = (opcode & 0x00F0) >> 4;
            if (vx[reg_x] != vx[reg_y])
            {
                increment_pc();
            }
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
            draw_flag = true;
            uint8_t bytes = opcode & 0xF;
            uint8_t x = vx[(opcode & 0x0F00) >> 8];
            uint8_t y = vx[(opcode & 0x00F0) >> 4];

            bool collision = false;

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
                        collision = true;
                    }
                    mask *= 2;
                }
            }

            vx[EMULATOR_VF_REGISTER] = collision;
        } break;
        case 14:
        {
            if ((opcode & 0xFF) == 0x9E)
            {
                uint8_t reg = (opcode & 0x0F00) >> 8;
                if (inputs[vx[reg]] == 1)
                {
                    increment_pc();
                }
            }
            else if ((opcode & 0xFF) == 0xA1)
            {
                uint8_t reg = (opcode & 0x0F00) >> 8;
                if (inputs[vx[reg]] == 0)
                {
                    increment_pc();
                }
            }
            else
            {
                std::cerr << "Case E not implemented" << std::endl;
                error = true;
            }
        } break;
        case 15:
        {
            uint8_t reg = (opcode & 0x0F00) >> 8;
            if ((opcode & 0xFF) == 0x07)
            {
                vx[reg] = dt;
            }
            else if ((opcode & 0xFF) == 0x0A)
            {
                waiting_for_input = true; 
                input_slot = reg;
            }
            else if ((opcode & 0xFF) == 0x15)
            {
                dt = vx[reg];
            }
            else if ((opcode & 0xFF) == 0x18)
            {
                st = vx[reg]; 
            }
            else if ((opcode & 0xFF) == 0x1E)
            {
                ix += vx[reg];
            }
            else if ((opcode & 0xFF) == 0x29)
            {
                ix = vx[reg] * EMULATOR_SPRITE_FONT_BYTES;
            }
            else if ((opcode & 0xFF) == 0x33)
            {
                uint8_t val = vx[reg];
                mem[ix] = val / 100;
                val = val % 100;
                mem[ix + 1] = val / 10;
                val = val % 10;
                mem[ix + 2] = val;
            }
            else if ((opcode & 0xFF) == 0x55)
            {
                for (int i = 0; i <= reg; i++)
                {
                    mem[ix++] = vx[i];
                }
            }
            else if ((opcode & 0xFF) == 0x65)
            {
                for (int i = 0; i <= reg; i++)
                {
                    vx[i] = mem[ix++]; 
                }
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
            error = true;
        } break;
    }
}


