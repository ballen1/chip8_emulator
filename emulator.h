#ifndef __H_EMULATOR__
#define __H_EMULATOR__

#include <cstdint>
#include <string>

#define EMULATOR_MEMORY_SIZE 4096
#define EMULATOR_MEMORY_PROGRAM_START 512
#define EMULATOR_DATA_REGISTERS 16
#define EMULATOR_STACK_SIZE 48
#define EMULATOR_DISPLAY_WIDTH 64 
#define EMULATOR_DISPLAY_HEIGHT 32
#define EMULATOR_VF_REGISTER 15

class emulator
{
public:
    emulator();
    ~emulator();

    bool load_program(std::string program);

    void cycle();
    void tick_timers();

    bool error_flag();

    void print_opcode();
    void print_display();

private:
    uint8_t mem[EMULATOR_MEMORY_SIZE];
    uint8_t vx[EMULATOR_DATA_REGISTERS];
    uint16_t ix; 
    uint16_t stack[EMULATOR_STACK_SIZE];
    uint8_t sp;
    uint16_t pc;
    uint16_t opcode;
    bool display[EMULATOR_DISPLAY_WIDTH][EMULATOR_DISPLAY_HEIGHT];
    uint8_t dt;
    uint8_t st;

    bool error;

    void increment_pc();

    void fetch_opcode();
    void execute_op();
};

#endif
