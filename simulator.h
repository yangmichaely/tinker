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

int add(uint64_t rd, uint64_t rs, uint64_t rt);

void addi(uint64_t rd, uint64_t l);

void sub(uint64_t rd, uint64_t rs, uint64_t rt);

void subi(uint64_t rd, uint64_t l);

void mul(uint64_t rd, uint64_t rs, uint64_t rt);

void divide(uint64_t rd, uint64_t rs, uint64_t rt);

void and(uint64_t rd, uint64_t rs, uint64_t rt);

void or(uint64_t rd, uint64_t rs, uint64_t rt);

void xor(uint64_t rd, uint64_t rs, uint64_t rt);

void not(uint64_t rd, uint64_t rs);

void shftr(uint64_t rd, uint64_t rs, uint64_t rt);

void shftri(uint64_t rd, uint64_t l);

void shftl(uint64_t rd, uint64_t rs, uint64_t rt);

void shftli(uint64_t rd, uint64_t l);

void br(uint64_t rd);

void brrReg(uint64_t rd);

void brrL(uint64_t l);

void brnz(uint64_t rd, uint64_t rs);

int call(uint64_t rd, uint64_t rs, uint64_t rt);

int ret();

void brgt(uint64_t rd, uint64_t rs, uint64_t rt);

int movReadMemStoreReg(uint64_t rd, uint64_t rs, uint64_t l);

void movReadRegStoreReg(uint64_t rd, uint64_t rs);

void movSetBits(uint64_t rd, uint64_t l);

int movReadRegStoreMem(uint64_t rd, uint64_t rs, uint64_t l);

void addf(uint64_t rd, uint64_t rs, uint64_t rt);

void subf(uint64_t rd, uint64_t rs, uint64_t rt);

void mulf(uint64_t rd, uint64_t rs, uint64_t rt);

void divf(uint64_t rd, uint64_t rs, uint64_t rt);

int in(uint64_t rd, uint64_t rs);

int out(uint64_t rd, uint64_t rs);

int interpret(uint64_t opcode, uint64_t rd, uint64_t rs, uint64_t rt, uint64_t l, uint64_t* pc);

int readBinary(FILE* f);

#endif