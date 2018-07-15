#ifndef __H_EMULATOR__
#define __H_EMULATOR__

#include <cstdint>
#include <string>

#define EMULATOR_MEMORY_SIZE 4096
#define EMULATOR_MEMORY_PROGRAM_START 512
#define EMULATOR_DATA_REGISTERS 16
#define EMULATOR_STACK_SIZE 48

class emulator
{
public:
    emulator();
    ~emulator();

    bool load_program(std::string program);

    void cycle();

private:
    uint8_t mem[EMULATOR_MEMORY_SIZE];
    uint8_t vx[EMULATOR_DATA_REGISTERS];
    uint16_t ix; 
    uint8_t stack[EMULATOR_STACK_SIZE];
    uint16_t pc;
    uint16_t opcode;
    uint8_t opcode_nibbles[4];
    
    void fetch_opcode();
    void decode_opcode();
    void execute_op();
};

#endif
