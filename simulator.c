#include "simulator.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
#include <limits.h>
#include <regex.h>

CPU cpu;
int nextMem;

int main(int argc, char** argv){
    FILE* f = fopen(argv[1], "rb\0");
    if(f == NULL){
        fprintf(stderr, "%s", "Invalid tko filepath\n");
        exit(-1);
    }
    char o = argv[1][strlen(argv[1]) - 1];
    char k = argv[1][strlen(argv[1]) - 2];
    char t = argv[1][strlen(argv[1]) - 3];
    char dot = argv[1][strlen(argv[1]) - 4];
    if(k != 'k' || t != 't' || dot != '.' || o != 'o'){
        fprintf(stderr, "%s", "Invalid tinker filepath\n");
        exit(-1);
    }
    if(readBinary(f) != 0){
        fclose(f);
        exit(-1);
    }
    fclose(f);
    return 0;
}

int memCheck(uint64_t index){
    if(index < 0 || index >= MEM_SIZE || index >= nextMem){
        return -1;
    }
    return 0;
}

int add(uint64_t rd, uint64_t rs, uint64_t rt){
    cpu.regs[rd].sinteger64 = cpu.regs[rs].sinteger64 + cpu.regs[rt].sinteger64;
    cpu.pc += 4;
    return cpu.pc;
}

int addi(uint64_t rd, uint64_t l){
    cpu.regs[rd].uinteger64 += l;
    cpu.pc += 4;
    return cpu.pc;
}

int sub(uint64_t rd, uint64_t rs, uint64_t rt){
    cpu.regs[rd].sinteger64 = cpu.regs[rs].sinteger64 - cpu.regs[rt].sinteger64;
    cpu.pc += 4;
    return cpu.pc;
}

int subi(uint64_t rd, uint64_t l){
    cpu.regs[rd].sinteger64 -= l;
    cpu.pc += 4;
    return cpu.pc;
}

int mul(uint64_t rd, uint64_t rs, uint64_t rt){
    cpu.regs[rd].sinteger64 = cpu.regs[rs].sinteger64 * cpu.regs[rt].sinteger64;
    cpu.pc += 4;
    return cpu.pc;
}

int divide(uint64_t rd, uint64_t rs, uint64_t rt){
    cpu.regs[rd].sinteger64 = cpu.regs[rs].sinteger64 / cpu.regs[rt].sinteger64;
    cpu.pc += 4;
    return cpu.pc;
}

int and(uint64_t rd, uint64_t rs, uint64_t rt){
    cpu.regs[rd].uinteger64 = cpu.regs[rs].uinteger64 & cpu.regs[rt].uinteger64;
    cpu.pc += 4;
    return cpu.pc;
}

int or(uint64_t rd, uint64_t rs, uint64_t rt){
    cpu.regs[rd].uinteger64 = cpu.regs[rs].uinteger64 | cpu.regs[rt].uinteger64;
    cpu.pc += 4;
    return cpu.pc;
}

int xor(uint64_t rd, uint64_t rs, uint64_t rt){
    cpu.regs[rd].uinteger64 = cpu.regs[rs].uinteger64 ^ cpu.regs[rt].uinteger64;
    cpu.pc += 4;
    return cpu.pc;
}

int not(uint64_t rd, uint64_t rs){
    cpu.regs[rd].uinteger64 = ~cpu.regs[rs].uinteger64;
    cpu.pc += 4;
    return cpu.pc;
}

int shftr(uint64_t rd, uint64_t rs, uint64_t rt){
    cpu.regs[rd].uinteger64 = cpu.regs[rs].uinteger64 >> cpu.regs[rt].uinteger64;
    cpu.pc += 4;
    return cpu.pc;
}

int shftri(uint64_t rd, uint64_t l){
    cpu.regs[rd].uinteger64 = cpu.regs[rd].uinteger64 >> l;
    cpu.pc += 4;
    return cpu.pc;
}

int shftl(uint64_t rd, uint64_t rs, uint64_t rt){
    cpu.regs[rd].uinteger64 = cpu.regs[rs].uinteger64 << cpu.regs[rt].uinteger64;
    cpu.pc += 4;
    return cpu.pc;
}

int shftli(uint64_t rd, uint64_t l){
    cpu.regs[rd].uinteger64 = cpu.regs[rd].uinteger64 << l;
    cpu.pc += 4;
    return cpu.pc;
}

int br(uint64_t rd){
    cpu.pc = cpu.regs[rd].uinteger64;
    return cpu.pc;
}

int brrReg(uint64_t rd){
    cpu.pc += cpu.regs[rd].uinteger64;
    return cpu.pc;
}

int brrL(uint64_t l){
    cpu.pc += l;
    return cpu.pc;
}

int brnz(uint64_t rd, uint64_t rs){
    if(cpu.regs[rs].uinteger64 == 0){
        cpu.pc += 4;
    }
    else{
        cpu.pc = cpu.regs[rd].uinteger64;
    }
    return cpu.pc;
}

int call(uint64_t rd, uint64_t rs, uint64_t rt){
    //for(int i = 0; i < 4; i++){
        if(memCheck(cpu.regs[31].uinteger64 - 8) != 0){
            return -1;
        }
    //}
    // uint32_t value = (uint32_t) cpu.pc + 4;
    // cpu.mem[cpu.regs[31].uinteger64 - 8] = (0xff000000 & value) >> 24;
    // cpu.mem[cpu.regs[31].uinteger64 - 7] = (0x00ff0000 & value) >> 16;
    // cpu.mem[cpu.regs[31].uinteger64 - 6] = (0x0000ff00 & value) >> 8;
    // cpu.mem[cpu.regs[31].uinteger64 - 5] = 0xff & value;
    //uint32_t value = (uint32_t) cpu.pc + 4;
    cpu.mem[cpu.regs[31].uinteger64 - 8] = cpu.pc + 4;
    // for(int i = 0; i < 4; i++){
    //     cpu.mem[cpu.regs[31].uinteger64 - 8 + i] = (((long)0xff << i * 8) & value) >> i * 8;
    // }
    cpu.pc = cpu.regs[rd].uinteger64;
    return cpu.pc;
}

int ret(){
    //for(int i = 0; i < 8; i++){
        if(memCheck(cpu.regs[31].uinteger64 - 8) != 0){
            return -1;
        }
    //}
    // uint64_t val = 0;
    // for(int i = 0; i < 8; i++){
    //     val |= (uint64_t) cpu.mem[cpu.regs[31].uinteger64 - 8 + i] << (i * 8);
    // }
    // val |= (uint64_t) cpu.mem[cpu.regs[31].uinteger64 - 4] << 56;
    // val |= (uint64_t) cpu.mem[cpu.regs[31].uinteger64 - 3] << 48;
    // val |= (uint64_t) cpu.mem[cpu.regs[31].uinteger64 - 2] << 40;
    // val |= (uint64_t) cpu.mem[cpu.regs[31].uinteger64 - 1] << 32;
    // val |= cpu.mem[cpu.regs[31].uinteger64 - 8] << 24;
    // val |= cpu.mem[cpu.regs[31].uinteger64 - 7] << 16;
    // val |= cpu.mem[cpu.regs[31].uinteger64 - 6] << 8;
    // val |= cpu.mem[cpu.regs[31].uinteger64 - 5];
    // cpu.pc = val;
    cpu.pc = cpu.mem[cpu.regs[31].uinteger64 - 8];
    return cpu.pc;
}

int brgt(uint64_t rd, uint64_t rs, uint64_t rt){
    if(cpu.regs[rs].sinteger64 <= cpu.regs[rt].sinteger64){
        cpu.pc += 4;
    }
    else{
        cpu.pc = cpu.regs[rd].uinteger64;
    }
    return cpu.pc;
}

int movReadMemStoreReg(uint64_t rd, uint64_t rs, uint64_t l){
    for(int i = 0; i < 8; i++){
        if(memCheck(cpu.regs[rd].uinteger64 + l + i) != 0){
            return -1;
        }
    }
    uint64_t val = 0;
    for(int i = 0; i < 8; i++){
        val |= (uint64_t) cpu.mem[cpu.regs[rs].uinteger64 + l + i] << (i * 8);
    }
    // val |= (uint64_t) cpu.mem[cpu.regs[rs].uinteger64 + l + 4] << 56;
    // val |= (uint64_t) cpu.mem[cpu.regs[rs].uinteger64 + l + 5] << 48;
    // val |= (uint64_t) cpu.mem[cpu.regs[rs].uinteger64 + l + 6] << 40;
    // val |= (uint64_t) cpu.mem[cpu.regs[rs].uinteger64 + l + 7] << 32;
    // val |= cpu.mem[cpu.regs[rs].uinteger64 + l] << 24;
    // val |= cpu.mem[cpu.regs[rs].uinteger64 + l + 1] << 16;
    // val |= cpu.mem[cpu.regs[rs].uinteger64 + l + 2] << 8;
    // val |= cpu.mem[cpu.regs[rs].uinteger64 + l + 3];
    cpu.regs[rd].uinteger64 = val;
    cpu.pc += 4;
    return cpu.pc;
}

int movReadRegStoreReg(uint64_t rd, uint64_t rs){
    cpu.regs[rd].uinteger64 = cpu.regs[rs].uinteger64;
    cpu.pc += 4;
    return cpu.pc;
}

int movSetBits(uint64_t rd, uint64_t l){
    cpu.regs[rd].uinteger64 &= 0xfffffffffffff000;
    cpu.regs[rd].uinteger64 |= l;
    cpu.pc += 4;
    return cpu.pc;
}

int movReadRegStoreMem(uint64_t rd, uint64_t rs, uint64_t l){
    for(int i = 0; i < 8; i++){
        if(memCheck(cpu.regs[rd].uinteger64 + l + i) != 0){
            return -1;
        }
    }
    uint64_t value = cpu.regs[rs].uinteger64;
    for(int i = 0; i < 8; i++){
        cpu.mem[cpu.regs[rd].uinteger64 + l + i] = (((long)0xff << i * 8) & value) >> i * 8;
    }
    // cpu.mem[cpu.regs[rd].uinteger64 + l] = (0x00000000ff000000 & value) >> 24;
    // cpu.mem[cpu.regs[rd].uinteger64 + l + 1] = (0x0000000000ff0000 & value) >> 16;
    // cpu.mem[cpu.regs[rd].uinteger64 + l + 2] = (0x000000000000ff00 & value) >> 8;
    // cpu.mem[cpu.regs[rd].uinteger64 + l + 3] = 0xff & value;
    // cpu.mem[cpu.regs[rd].uinteger64 + l + 4] = (0xff00000000000000 & value) >> 56;
    // cpu.mem[cpu.regs[rd].uinteger64 + l + 5] = (0x00ff000000000000 & value) >> 48;
    // cpu.mem[cpu.regs[rd].uinteger64 + l + 6] = (0x0000ff0000000000 & value) >> 40;
    // cpu.mem[cpu.regs[rd].uinteger64 + l + 7] = 0x000000ff00000000 & value >> 32;
    cpu.pc += 4;
    return cpu.pc;
}

int addf(uint64_t rd, uint64_t rs, uint64_t rt){
    cpu.regs[rd].floatingPoint = cpu.regs[rs].floatingPoint + cpu.regs[rt].floatingPoint;
    cpu.pc += 4;
    return cpu.pc;
}

int subf(uint64_t rd, uint64_t rs, uint64_t rt){
    cpu.regs[rd].floatingPoint = cpu.regs[rs].floatingPoint - cpu.regs[rt].floatingPoint;
    cpu.pc += 4;
    return cpu.pc;
}

int mulf(uint64_t rd, uint64_t rs, uint64_t rt){
    cpu.regs[rd].floatingPoint = cpu.regs[rs].floatingPoint * cpu.regs[rt].floatingPoint;
    cpu.pc += 4;
    return cpu.pc;
}

int divf(uint64_t rd, uint64_t rs, uint64_t rt){
    cpu.regs[rd].floatingPoint = cpu.regs[rs].floatingPoint / cpu.regs[rt].floatingPoint;
    cpu.pc += 4;
    return cpu.pc;
}

int in(uint64_t rd, uint64_t rs){
    if(cpu.regs[rs].uinteger64 == 0){
        scanf("%lu", &cpu.regs[rd].uinteger64);
        cpu.pc += 4;
    }
    return cpu.pc;
}

int out(uint64_t rd, uint64_t rs){
    if(cpu.regs[rd].uinteger64 == 1){
        printf("%lu\n", cpu.regs[rs].uinteger64);
        cpu.pc += 4;
    }
    return cpu.pc;
}

int interpret(uint64_t opcode, uint64_t rd, uint64_t rs, uint64_t rt, uint64_t l){
    int ans = 0;
    switch (opcode){
        case 0:
            return add(rd, rs, rt);
        case 1:
            return addi(rd, l);
        case 2:
            return sub(rd, rs, rt);
        case 3:
            return subi(rd, l);
        case 4:
            return mul(rd, rs, rt);
        case 5:
            if(rt == 0){
                return -1;
            }
            return divide(rd, rs, rt);
        case 6:
            return and(rd, rs, rt);
        case 7:
            return or(rd, rs, rt);
        case 8:
            return xor(rd, rs, rt);
        case 9:
            return not(rd, rs);
        case 10:
            return shftr(rd, rs, rt);
        case 11:
            return shftri(rd, l);
        case 12:
            return shftl(rd, rs, rt);
        case 13:
            return shftli(rd, l);
        case 14:
            return br(rd);
        case 15:
            return brrReg(rd);
        case 16:
            return brrReg(l);
        case 17:
            return brnz(rd, rs);
        case 18:
            ans = call(rd, rs, rt);
            if(ans != -1){
                return ans;
            }
            return -1;
        case 19:
            ans = ret();
            if(ans != -1){
                return ans;
            }
            return -1;
        case 20:
            return brgt(rd, rs, rt);
        case 21:
            ans = movReadMemStoreReg(rd, rs, l);
            if(ans != -1){
                return ans;
            }
            return -1;
        case 22:
            return movReadRegStoreReg(rd, rs);
        case 23:
            return movSetBits(rd, l);
        case 24:
            ans = movReadRegStoreMem(rd, rs, l);
            if(ans != -1){
                return ans;
            }
            return -1;
        case 25:
            return addf(rd, rs, rt);
        case 26:
            return subf(rd, rs, rt);
        case 27:
            return mulf(rd, rs, rt);
        case 28:
            if(rt == 0){
                return -1;
            }
            return divf(rd, rs, rt);
        case 29:
            return in(rd, rs);
        case 30:
            return out(rd, rs);
        case 31:
            exit(0);
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
        cpu.regs[i].uinteger64 = 0;
    }
    // fseek(f, 0, SEEK_END);
    // int length = ftell(f);
    uint8_t binary = 0;
    cpu.regs[31].uinteger64 = MEM_SIZE;
    int check = 0;
    cpu.pc = 0;
    // rewind(f);
    nextMem = 0;
    while(fread(&binary, sizeof(binary), 1, f) == 1){
        cpu.mem[nextMem] = binary;
        nextMem++;
    }
    while(cpu.pc < nextMem){
        for(int i = 0; i < 4; i++){
            if(memCheck(cpu.pc + i) != 0){
                return -1;
            }
        }
        uint64_t instr = 0;
        instr |= cpu.mem[cpu.pc];
        instr |= cpu.mem[cpu.pc + 1] << 8;
        instr |= cpu.mem[cpu.pc + 2] << 16;
        instr |= cpu.mem[cpu.pc + 3] << 24;
        uint64_t opcode = (instr >> 27) & 0x1f;
        uint64_t rd = (instr >> 22) & 0x1f;
        uint64_t rs = (instr >> 17) & 0x1f;
        uint64_t rt = (instr >> 12) & 0x1f;
        uint64_t l = instr & 0xfff;
        if(opcode == 16 || opcode == 21 || opcode == 24){
            uint64_t sign = l & (1 << 11);
            sign = sign >> 11;
            if(sign == 1){
                l = -(~l + 4097);
            }
        }
        // printf("opcode: %ld\n", opcode);
        // printf("rd: %ld\n", rd);
        // printf("rs: %ld\n", rs);
        // printf("rt: %ld\n", rt);
        // printf("l: %ld\n", l);
        check = interpret(opcode, rd, rs, rt, l);
        if(check == -1){
            fprintf(stderr, "%s\n", "Simulation error");
            exit(-1);
        }
        cpu.pc = check;
    }
    return 0;
}