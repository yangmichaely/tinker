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
}CPU;

int memCheck(uint64_t index);

int add(uint64_t rd, uint64_t rs, uint64_t rt);

int addi(uint64_t rd, uint64_t l);

int sub(uint64_t rd, uint64_t rs, uint64_t rt);

int subi(uint64_t rd, uint64_t l);

int mul(uint64_t rd, uint64_t rs, uint64_t rt);

int divide(uint64_t rd, uint64_t rs, uint64_t rt);

int and(uint64_t rd, uint64_t rs, uint64_t rt);

int or(uint64_t rd, uint64_t rs, uint64_t rt);

int xor(uint64_t rd, uint64_t rs, uint64_t rt);

int not(uint64_t rd, uint64_t rs);

int shftr(uint64_t rd, uint64_t rs, uint64_t rt);

int shftri(uint64_t rd, uint64_t l);

int shftl(uint64_t rd, uint64_t rs, uint64_t rt);

int shftli(uint64_t rd, uint64_t l);

int br(uint64_t rd);

int brrReg(uint64_t rd);

int brrL(uint64_t l);

int brnz(uint64_t rd, uint64_t rs);

int call(uint64_t rd, uint64_t rs, uint64_t rt);

int ret();

int brgt(uint64_t rd, uint64_t rs, uint64_t rt);

int movReadMemStoreReg(uint64_t rd, uint64_t rs, uint64_t l);

int movReadRegStoreReg(uint64_t rd, uint64_t rs);

int movSetBits(uint64_t rd, uint64_t l);

int movReadRegStoreMem(uint64_t rd, uint64_t rs, uint64_t l);

int addf(uint64_t rd, uint64_t rs, uint64_t rt);

int subf(uint64_t rd, uint64_t rs, uint64_t rt);

int mulf(uint64_t rd, uint64_t rs, uint64_t rt);

int divf(uint64_t rd, uint64_t rs, uint64_t rt);

int in(uint64_t rd, uint64_t rs);

int out(uint64_t rd, uint64_t rs);

int interpret(uint64_t opcode, uint64_t rd, uint64_t rs, uint64_t rt, uint64_t l);

int readBinary(FILE* f);

#endif