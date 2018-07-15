#ifndef __H_EMULATOR__
#define __H_EMULATOR__

#include <cstdint>
#include <string>

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
