#include "simulator.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
#include <limits.h>
#include <regex.h>

CPU cpu;

int main(int argc, char** argv){
    printf("FILE: %s\n", argv[1]);
    FILE* f = fopen(argv[1], "rb\0");
    if(f == NULL){
        fprintf(stderr, "%s", "Invalid tko filepath\n");
        return -1;
    }
    if(readBinary(f) != 0){
        fclose(f);
        return -1;
    }
    fclose(f);
    return 0;
}

int memCheck(uint64_t index){
    if(index < 0 || index >= MEM_SIZE){
        return -1;
    }
    return 0;
}

int instrCheck(uint64_t opcode, uint64_t rd, uint64_t rs, uint64_t rt, uint64_t l){
    if(opcode < 0 || opcode > 31 || rd < 0 || rd > 31 || rs < 0 || rs > 31 || rt < 0 || rt > 31 || l < 0 || l > 4095){
        return -1;
    }
    return 0;
}

int add(uint64_t rd, uint64_t rs, uint64_t rt){
    cpu.regs[rd].sinteger64 = cpu.regs[rs].sinteger64 + cpu.regs[rt].sinteger64;
    cpu.pc += 4;
}

void addi(uint64_t rd, uint64_t l){
    cpu.regs[rd].uinteger64 += l;
    cpu.pc += 4;
}

void sub(uint64_t rd, uint64_t rs, uint64_t rt){
    cpu.regs[rd].sinteger64 = cpu.regs[rs].sinteger64 - cpu.regs[rt].sinteger64;
    cpu.pc += 4;
}

void subi(uint64_t rd, uint64_t l){
    cpu.regs[rd].uinteger64 -= l;
    cpu.pc += 4;
}

void mul(uint64_t rd, uint64_t rs, uint64_t rt){
    cpu.regs[rd].sinteger64 = cpu.regs[rs].sinteger64 * cpu.regs[rt].sinteger64;
    cpu.pc += 4;
}

void divide(uint64_t rd, uint64_t rs, uint64_t rt){
    cpu.regs[rd].sinteger64 = cpu.regs[rs].sinteger64 / cpu.regs[rt].sinteger64;
    cpu.pc += 4;
}

void and(uint64_t rd, uint64_t rs, uint64_t rt){
    cpu.regs[rd].uinteger64 = cpu.regs[rs].uinteger64 & cpu.regs[rt].uinteger64;
    cpu.pc += 4;
}

void or(uint64_t rd, uint64_t rs, uint64_t rt){
    cpu.regs[rd].uinteger64 = cpu.regs[rs].uinteger64 | cpu.regs[rt].uinteger64;
    cpu.pc += 4;
}

void xor(uint64_t rd, uint64_t rs, uint64_t rt){
    cpu.regs[rd].uinteger64 = cpu.regs[rs].uinteger64 ^ cpu.regs[rt].uinteger64;
    cpu.pc += 4;
}

void not(uint64_t rd, uint64_t rs){
    cpu.regs[rd].uinteger64 = ~cpu.regs[rs].uinteger64;
    cpu.pc += 4;
}

void shftr(uint64_t rd, uint64_t rs, uint64_t rt){
    cpu.regs[rd].uinteger64 = cpu.regs[rs].uinteger64 >> cpu.regs[rt].uinteger64;
    cpu.pc += 4;
}

void shftri(uint64_t rd, uint64_t l){
    cpu.regs[rd].uinteger64 = cpu.regs[rd].uinteger64 >> l;
    cpu.pc += 4;
}

void shftl(uint64_t rd, uint64_t rs, uint64_t rt){
    cpu.regs[rd].uinteger64 = cpu.regs[rs].uinteger64 << cpu.regs[rt].uinteger64;
    cpu.pc += 4;
}

void shftli(uint64_t rd, uint64_t l){
    cpu.regs[rd].uinteger64 = cpu.regs[rd].uinteger64 << l;
    cpu.pc += 4;
}

void br(uint64_t rd){
    cpu.pc = cpu.regs[rd].uinteger64;
}

void brrReg(uint64_t rd){
    cpu.pc += cpu.regs[rd].uinteger64;
}

void brrL(uint64_t l){
    cpu.pc += l;
}

void brnz(uint64_t rd, uint64_t rs){
    if(cpu.regs[rs].uinteger64 == 0){
        cpu.pc += 4;
    }
    else{
        cpu.pc = cpu.regs[rd].uinteger64;
    }
}

int call(uint64_t rd, uint64_t rs, uint64_t rt){
    if(memCheck(cpu.regs[31].uinteger64 - 8) == 0){
        uint32_t value = (uint32_t) cpu.pc + 4;
        cpu.mem[cpu.regs[31].uinteger64 - 8] = (0xff000000 & value) >> 24;
        cpu.mem[cpu.regs[31].uinteger64 - 7] = (0x00ff0000 & value) >> 16;
        cpu.mem[cpu.regs[31].uinteger64 - 6] = (0x0000ff00 & value) >> 8;
        cpu.mem[cpu.regs[31].uinteger64 - 5] = 0xff & value;
        cpu.pc = cpu.regs[rd].uinteger64;
        return 0;
    }
    return -1;
}

int ret(){
    if(memCheck(cpu.regs[31].uinteger64 - 8) == 0){
        uint64_t val = 0;
        val |= (uint64_t) cpu.mem[cpu.regs[31].uinteger64 - 4] << 56;
        val |= (uint64_t) cpu.mem[cpu.regs[31].uinteger64 - 3] << 48;
        val |= (uint64_t) cpu.mem[cpu.regs[31].uinteger64 - 2] << 40;
        val |= (uint64_t) cpu.mem[cpu.regs[31].uinteger64 - 1] << 32;
        val |= cpu.mem[cpu.regs[31].uinteger64 - 8] << 24;
        val |= cpu.mem[cpu.regs[31].uinteger64 - 7] << 16;
        val |= cpu.mem[cpu.regs[31].uinteger64 - 6] << 8;
        val |= cpu.mem[cpu.regs[31].uinteger64 - 5];
        cpu.pc = val;
        return 0;
    }
    return -1;
}

void brgt(uint64_t rd, uint64_t rs, uint64_t rt){
    if(cpu.regs[rs].sinteger64 <= cpu.regs[rt].sinteger64){
        cpu.pc += 4;
    }
    else{
        cpu.pc = cpu.regs[rd].uinteger64;
    }
}

int movReadMemStoreReg(uint64_t rd, uint64_t rs, uint64_t l){
    if(memCheck(cpu.regs[rs].uinteger64 + l) == 0){
        uint64_t val = 0;
        val |= (uint64_t) cpu.mem[cpu.regs[rs].uinteger64 + l + 4] << 56;
        val |= (uint64_t) cpu.mem[cpu.regs[rs].uinteger64 + l + 5] << 48;
        val |= (uint64_t) cpu.mem[cpu.regs[rs].uinteger64 + l + 6] << 40;
        val |= (uint64_t) cpu.mem[cpu.regs[rs].uinteger64 + l + 7] << 32;
        val |= cpu.mem[cpu.regs[rs].uinteger64 + l] << 24;
        val |= cpu.mem[cpu.regs[rs].uinteger64 + l + 1] << 16;
        val |= cpu.mem[cpu.regs[rs].uinteger64 + l + 2] << 8;
        val |= cpu.mem[cpu.regs[rs].uinteger64 + l + 3];
        cpu.regs[rd].uinteger64 = val;
        cpu.pc += 4;
        return 0;
    }
    return -1;
}

void movReadRegStoreReg(uint64_t rd, uint64_t rs){
    cpu.regs[rd].uinteger64 = cpu.regs[rs].uinteger64;
    cpu.pc += 4;
}

void movSetBits(uint64_t rd, uint64_t l){
    cpu.regs[rd].uinteger64 &= 0xfffffffffffff000;
    cpu.regs[rd].uinteger64 |= l;
    cpu.pc += 4;
}

int movReadRegStoreMem(uint64_t rd, uint64_t rs, uint64_t l){
    if(memCheck(cpu.regs[rd].uinteger64 + l) == 0){
        uint64_t value = cpu.regs[rs].uinteger64;
        cpu.mem[cpu.regs[rd].uinteger64 + l] = (0xff00000000000000 & value) >> 56;
        cpu.mem[cpu.regs[rd].uinteger64 + l + 1] = (0x00ff000000000000 & value) >> 48;
        cpu.mem[cpu.regs[rd].uinteger64 + l + 2] = (0x0000ff0000000000 & value) >> 40;
        cpu.mem[cpu.regs[rd].uinteger64 + l + 3] = 0x000000ff00000000 & value >> 32;
        cpu.mem[cpu.regs[rd].uinteger64 + l] = (0x00000000ff000000 & value) >> 24;
        cpu.mem[cpu.regs[rd].uinteger64 + l + 1] = (0x0000000000ff0000 & value) >> 16;
        cpu.mem[cpu.regs[rd].uinteger64 + l + 2] = (0x000000000000ff00 & value) >> 8;
        cpu.mem[cpu.regs[rd].uinteger64 + l + 3] = 0xff & value;
        cpu.pc += 4;
        return 0;
    }
    return -1;
}

void addf(uint64_t rd, uint64_t rs, uint64_t rt){
    cpu.regs[rd].floatingPoint = cpu.regs[rs].floatingPoint + cpu.regs[rt].floatingPoint;
    cpu.pc += 4;
}

void subf(uint64_t rd, uint64_t rs, uint64_t rt){
    cpu.regs[rd].floatingPoint = cpu.regs[rs].floatingPoint - cpu.regs[rt].floatingPoint;
    cpu.pc += 4;
}

void mulf(uint64_t rd, uint64_t rs, uint64_t rt){
    cpu.regs[rd].floatingPoint = cpu.regs[rs].floatingPoint * cpu.regs[rt].floatingPoint;
    cpu.pc += 4;
}

void divf(uint64_t rd, uint64_t rs, uint64_t rt){
    cpu.regs[rd].floatingPoint = cpu.regs[rs].floatingPoint / cpu.regs[rt].floatingPoint;
    cpu.pc += 4;
}

int in(uint64_t rd, uint64_t rs){
    if(cpu.regs[rs].uinteger64 == 0){
        scanf("%lu", &cpu.regs[rd].uinteger64);
        cpu.pc += 4;
        return 0;
    }
    return -1;
}

int out(uint64_t rd, uint64_t rs){
    if(cpu.regs[rd].uinteger64 == 1){
        printf("%ld\n", cpu.regs[rs].uinteger64);
        cpu.pc += 4;
        return 0;
    }
    return -1;
}

int interpret(uint64_t opcode, uint64_t rd, uint64_t rs, uint64_t rt, uint64_t l){
    switch (opcode){
        case 0:
            add(rd, rs, rt);
            return 0;
        case 1:
            addi(rd, l);
            return 0;
        case 2:
            sub(rd, rs, rt);
            return 0;
        case 3:
            subi(rd, l);
            return 0;
        case 4:
            mul(rd, rs, rt);
            return 0;
        case 5:
            divide(rd, rs, rt);
            return 0;
        case 6:
            and(rd, rs, rt);
            return 0;
        case 7:
            or(rd, rs, rt);
            return 0;
        case 8:
            xor(rd, rs, rt);
            return 0;
        case 9:
            not(rd, rs);
            return 0;
        case 10:
            shftr(rd, rs, rt);
            return 0;
        case 11:
            shftri(rd, l);
            return 0;
        case 12:
            shftl(rd, rs, rt);
            return 0;
        case 13:
            shftli(rd, l);
            return 0;
        case 14:
            br(rd);
            return 0;
        case 15:
            brrReg(rd);
            return 0;
        case 16:
            brrReg(l);
            return 0;
        case 17:
            brnz(rd, rs);
            return 0;
        case 18:
            if(call(rd, rs, rt) == 0){
                return 0;
            }
            return -1;
        case 19:
            if(ret() == 0){
                return 0;
            }
            return -1;
        case 20:
            brgt(rd, rs, rt);
            return 0;
        case 21:
            if(movReadMemStoreReg(rd, rs, l) == 0){
                return 0;
            }
            return -1;
        case 22:
            movReadRegStoreReg(rd, rs);
            return 0;
        case 23:
            movSetBits(rd, l);
            return 0;
        case 24:
            if(movReadRegStoreMem(rd, rs, l) == 0){
                return 0;
            }
            return -1;
        case 25:
            addf(rd, rs, rt);
            return 0;
        case 26:
            subf(rd, rs, rt);
            return 0;
        case 27:
            mulf(rd, rs, rt);
            return 0;
        case 28:
            divf(rd, rs, rt);
            return 0;
        case 29:
            if(in(rd, rs) == 0){
                return 0;
            }
            return -1;
        case 30:
            if(out(rd, rs) == 0){
                return 0;
            }
            return -1;
        case 31:
            return 1;
        default:
            return -1;
    }
    return 0;
}

int readBinary(FILE* f){
    for(int i = 0; i < MEM_SIZE; i++){
        cpu.mem[i] = 0;
    }
    for(int i = 0; i < NUM_REGS; i++){
        cpu.regs[i].sinteger64 = 0;
        cpu.regs[i].uinteger64 = 0;
        cpu.regs[i].floatingPoint = 0;
    }
    fseek(f, 0, SEEK_END);
    int length = ftell(f);
    uint64_t binary = 0;
    cpu.regs[31].sinteger64 = MEM_SIZE;
    cpu.regs[31].uinteger64 = MEM_SIZE;
    cpu.regs[31].floatingPoint = MEM_SIZE;
    int check = 0;
    cpu.pc = 0;
    rewind(f);
    int nextMem = 0;
    while(fread(&binary, 4, 1, f) == 1){
        cpu.mem[nextMem++] = (0xff000000 & binary) >> 24;
        cpu.mem[nextMem++] = (0x00ff0000 & binary) >> 16;
        cpu.mem[nextMem++] = (0x0000ff00 & binary) >> 8;
        cpu.mem[nextMem++] = 0xff & binary;
    }
    while(cpu.pc < length){
        uint64_t instr = 0;
        instr |= cpu.mem[cpu.pc] << 24;
        instr |= cpu.mem[cpu.pc + 1] << 16;
        instr |= cpu.mem[cpu.pc + 2] << 8;
        instr |= cpu.mem[cpu.pc + 3];
        uint64_t opcode = (instr >> 27) & 0x1f;
        uint64_t rd = (instr >> 22) & 0x1f;
        uint64_t rs = (instr >> 17) & 0x1f;
        uint64_t rt = (instr >> 12) & 0x1f;
        uint64_t l = instr & 0xfff;
        // printf("opcode: %ld\n", opcode);
        // printf("rd: %ld\n", rd);
        // printf("rs: %ld\n", rs);
        // printf("rt: %ld\n", rt);
        // printf("l: %ld\n", l);
        check = interpret(opcode, rd, rs, rt, l);
        if(instrCheck(opcode, rd, rs, rt, l) == -1 || check == -1){
            fprintf(stderr, "%s\n", "Simulation error");
            return -1;
        }
        if(check == 1){
            return 0;
        }
    }
    return 0;
}