#include "cpu.h"

int main(int argc, char *argv[])
{
    struct i8080 cpu;

    map_roms_to_memory(&cpu, argv[1], 0x0100);

    cpu.pc = 0x100;

  // inject "out 0,a" at 0x0000 (signal to stop the test)
    cpu.mem[0x0000] = 0xD3;
    cpu.mem[0x0001] = 0x00;

  // inject "out 1,a" at 0x0005 (signal to output some characters)
    cpu.mem[0x0005] = 0xD3;
    cpu.mem[0x0006] = 0x01;
    cpu.mem[0x0007] = 0xC9;

    cpu_init(&cpu);
    cpu.test_finished = 0;
    while (!cpu.test_finished) {
        execute_instruction(&cpu);
    }
    return 0;
}