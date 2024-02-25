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

int memCheck(uint64_t index);

int add(uint64_t rd, uint64_t rs, uint64_t rt, CPU* cpu);

void addi(uint64_t rd, uint64_t l, CPU* cpu);

void sub(uint64_t rd, uint64_t rs, uint64_t rt, CPU* cpu);

void subi(uint64_t rd, uint64_t l, CPU* cpu);

void mul(uint64_t rd, uint64_t rs, uint64_t rt, CPU* cpu);

void divide(uint64_t rd, uint64_t rs, uint64_t rt, CPU* cpu);

void and(uint64_t rd, uint64_t rs, uint64_t rt, CPU* cpu);

void or(uint64_t rd, uint64_t rs, uint64_t rt, CPU* cpu);

void xor(uint64_t rd, uint64_t rs, uint64_t rt, CPU* cpu);

void not(uint64_t rd, uint64_t rs, CPU* cpu);

void shftr(uint64_t rd, uint64_t rs, uint64_t rt, CPU* cpu);

void shftri(uint64_t rd, uint64_t l, CPU* cpu);

void shftl(uint64_t rd, uint64_t rs, uint64_t rt, CPU* cpu);

void shftli(uint64_t rd, uint64_t l, CPU* cpu);

void br(uint64_t rd, CPU* cpu);

void brrReg(uint64_t rd, CPU* cpu);

void brrL(uint64_t l, CPU* cpu);

void brnz(uint64_t rd, uint64_t rs, CPU* cpu);

int call(uint64_t rd, uint64_t rs, uint64_t rt, CPU* cpu);

int ret(CPU* cpu);

void brgt(uint64_t rd, uint64_t rs, uint64_t rt, CPU* cpu);

int movReadMemStoreReg(uint64_t rd, uint64_t rs, uint64_t l, CPU* cpu);

void movReadRegStoreReg(uint64_t rd, uint64_t rs, CPU* cpu);

void movSetBits(uint64_t rd, uint64_t l, CPU* cpu);

int movReadRegStoreMem(uint64_t rd, uint64_t rs, uint64_t l, CPU* cpu);

void addf(uint64_t rd, uint64_t rs, uint64_t rt, CPU* cpu);

void subf(uint64_t rd, uint64_t rs, uint64_t rt, CPU* cpu);

void mulf(uint64_t rd, uint64_t rs, uint64_t rt, CPU* cpu);

void divf(uint64_t rd, uint64_t rs, uint64_t rt, CPU* cpu);

int in(uint64_t rd, uint64_t rs, CPU* cpu);

int out(uint64_t rd, uint64_t rs, CPU* cpu);

int interpret(uint64_t opcode, uint64_t rd, uint64_t rs, uint64_t rt, uint64_t l, CPU* cpu);

int readBinary(FILE* f);

#endif