#ifndef __H_EMULATOR__
#define __H_EMULATOR__

#include <cstdint>
#include <string>

#define EMULATOR_MEMORY_SIZE 4096
#define EMULATOR_MEMORY_PROGRAM_START 512

class emulator
{
public:
    emulator();
    ~emulator();

    bool load_program(std::string program);

private:
    uint8_t mem[4096];
};

#endif
