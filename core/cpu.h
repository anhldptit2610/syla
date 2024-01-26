#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

struct i8080 {
    /* memory */
    uint8_t mem[0xffff];

    /* passed cycles */
    unsigned long cycles;

    /* registers */
    union {
        uint16_t af;
        struct {
            union {
                uint8_t f;
                struct {
                    uint8_t CARRY : 1;
                    uint8_t UNUSED_1 : 1;
                    uint8_t PARITY : 1;
                    uint8_t UNUSED_2 : 1;
                    uint8_t HALFC : 1;
                    uint8_t UNUSED_3 : 1;
                    uint8_t ZERO : 1;
                    uint8_t SIGN : 1;
                };
            };
            uint8_t a;
        };
    };
    union {
        uint16_t bc;
        struct {
            uint8_t c;
            uint8_t b;
        };
    };
    union {
        uint16_t de;
        struct {
            uint8_t e;
            uint8_t d;
        };
    };
    union {
        uint16_t hl;
        struct {
            uint8_t l;
            uint8_t h;
        };
    };
    uint16_t sp;
    uint16_t pc;

    // for decoding 
    uint8_t opcode;
    uint8_t operand[2];

    bool test_finished;
};

void map_roms_to_memory(struct i8080 *cpu, char *rom_path, uint16_t start_addr);
void execute_instruction(struct i8080 *cpu);
void cpu_init(struct i8080 *cpu);

#ifdef __cplusplus
}
#endif
