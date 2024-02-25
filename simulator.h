#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>

#ifndef SIMULATOR_H
#define SIMULATOR_H
#define NUM_REGS 32
#define MEM_SIZE 512 * 1024

typedef union reg {
    int64_t sinteger64;
    uint64_t uinteger64;
    double floatingPoint;
}reg;

typedef struct CPU {
    reg regs[NUM_REGS];
    uint8_t mem[MEM_SIZE];
    uint64_t pc;
}CPU;

CPU cpu;

int memCheck(uint64_t index);

int interpret(uint64_t opcode, uint64_t rd, uint64_t rs, uint64_t rt, uint64_t l);

int readBinary(FILE* f);

#endif