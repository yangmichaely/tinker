#include "simulator.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
#include <limits.h>
#include <regex.h>

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
    if(index < 0 || index >= MEM_SIZE){
        return -1;
    }
    return 0;
}

int interpret(uint64_t opcode, uint64_t rd, uint64_t rs, uint64_t rt, uint64_t l){
    switch (opcode){
        case 0:
            cpu.regs[rd].sinteger64 = cpu.regs[rs].sinteger64 + cpu.regs[rt].sinteger64;
            cpu.pc += 4;
            return 0;
        case 1:
            cpu.regs[rd].uinteger64 += l;
            cpu.pc += 4;
            return 0;
        case 2:
            cpu.regs[rd].sinteger64 = cpu.regs[rs].sinteger64 - cpu.regs[rt].sinteger64;
            cpu.pc += 4;
            return 0;
        case 3:
            cpu.regs[rd].sinteger64 -= l;
            cpu.pc += 4;
            return 0;
        case 4:
            cpu.regs[rd].sinteger64 = cpu.regs[rs].sinteger64 * cpu.regs[rt].sinteger64;
            cpu.pc += 4;
            return 0;
        case 5:
            if(rt == 0){
                return -1;
            }
            cpu.regs[rd].sinteger64 = cpu.regs[rs].sinteger64 / cpu.regs[rt].sinteger64;
            cpu.pc += 4;
            return 0;
        case 6:
            cpu.regs[rd].uinteger64 = cpu.regs[rs].uinteger64 & cpu.regs[rt].uinteger64;
            cpu.pc += 4;
            return 0;
        case 7:
            cpu.regs[rd].uinteger64 = cpu.regs[rs].uinteger64 | cpu.regs[rt].uinteger64;
            cpu.pc += 4;
            return 0;
        case 8:
            cpu.regs[rd].uinteger64 = cpu.regs[rs].uinteger64 ^ cpu.regs[rt].uinteger64;
            cpu.pc += 4;
            return 0;
        case 9:
            cpu.regs[rd].uinteger64 = ~cpu.regs[rs].uinteger64;
            cpu.pc += 4;
            return 0;
        case 10:
            cpu.regs[rd].uinteger64 = cpu.regs[rs].uinteger64 >> cpu.regs[rt].uinteger64;
            cpu.pc += 4;
            return 0;
        case 11:
            cpu.regs[rd].uinteger64 = cpu.regs[rd].uinteger64 >> l;
            cpu.pc += 4;
            return 0;
        case 12:
            cpu.regs[rd].uinteger64 = cpu.regs[rs].uinteger64 << cpu.regs[rt].uinteger64;
            cpu.pc += 4;
            return 0;
        case 13:
            cpu.regs[rd].uinteger64 = cpu.regs[rd].uinteger64 << l;
            cpu.pc += 4;
            return 0;
        case 14:
            cpu.pc = cpu.regs[rd].uinteger64;
            return 0;
        case 15:
            cpu.pc += cpu.regs[rd].uinteger64;
            return 0;
        case 16:
            cpu.pc += l;
            return 0;
        case 17:
            if(cpu.regs[rs].uinteger64 == 0){
                cpu.pc += 4;
            }
            else{
                cpu.pc = cpu.regs[rd].uinteger64;
            }
            return 0;
        case 18:
            if(memCheck(cpu.regs[31].uinteger64 - 8) != 0){
                return -1;
            }
            cpu.mem[cpu.regs[31].uinteger64 - 8] = cpu.pc + 4;
            cpu.pc = cpu.regs[rd].uinteger64;
            return 0;
        case 19:
            if(memCheck(cpu.regs[31].uinteger64 - 8) != 0){
                return -1;
            }
            cpu.pc = cpu.mem[cpu.regs[31].uinteger64 - 8];
            return 0;
        case 20:
            if(cpu.regs[rs].sinteger64 <= cpu.regs[rt].sinteger64){
                cpu.pc += 4;
            }
            else{
                cpu.pc = cpu.regs[rd].uinteger64;
            }
            return 0;
        case 21:
            for(int i = 0; i < 8; i++){
                if(memCheck(cpu.regs[rs].uinteger64 + l + i) != 0){
                    return -1;
                }
            }
            uint64_t val = 0;
            for(int i = 0; i < 8; i++){
                val |= (uint64_t) cpu.mem[cpu.regs[rs].uinteger64 + l + i] << (i * 8);
            }
            cpu.regs[rd].uinteger64 = val;
            cpu.pc += 4;
            return 0;
        case 22:
            cpu.regs[rd].uinteger64 = cpu.regs[rs].uinteger64;
            cpu.pc += 4;
            return 0;
        case 23:
            cpu.regs[rd].uinteger64 &= 0xfffffffffffff000;
            cpu.regs[rd].uinteger64 |= l;
            cpu.pc += 4;
            return 0;
        case 24:
            for(int i = 0; i < 8; i++){
                if(memCheck(cpu.regs[rd].uinteger64 + l + i) != 0){
                    return -1;
                }
            }
            uint64_t value = cpu.regs[rs].uinteger64;
            for(int i = 0; i < 8; i++){
                cpu.mem[cpu.regs[rd].uinteger64 + l + i] = (((long)0xff << i * 8) & value) >> i * 8;
            }
            cpu.pc += 4;
            return 0;
        case 25:
            cpu.regs[rd].floatingPoint = cpu.regs[rs].floatingPoint + cpu.regs[rt].floatingPoint;
            cpu.pc += 4;
            return 0;
        case 26:
            cpu.regs[rd].floatingPoint = cpu.regs[rs].floatingPoint - cpu.regs[rt].floatingPoint;
            cpu.pc += 4;
            return 0;
        case 27:
            cpu.regs[rd].floatingPoint = cpu.regs[rs].floatingPoint * cpu.regs[rt].floatingPoint;
            cpu.pc += 4;
            return 0;
        case 28:
            if(rt == 0){
                return -1;
            }
            cpu.regs[rd].floatingPoint = cpu.regs[rs].floatingPoint / cpu.regs[rt].floatingPoint;
            cpu.pc += 4;
            return 0;
        case 29:
            if(cpu.regs[rs].uinteger64 == 0){
                scanf("%lu\n", &cpu.regs[rd].uinteger64);
                cpu.pc += 4;
            }
            return 0;
        case 30:
            if(cpu.regs[rd].uinteger64 == 1){
                printf("%lu", cpu.regs[rs].uinteger64);
                cpu.pc += 4;
            }
            return 0;
        case 31:
            cpu.pc += 4;
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
        cpu.regs[i].uinteger64 = 0;
    }
    uint8_t binary = 0;
    cpu.regs[31].uinteger64 = MEM_SIZE;
    int check = 0;
    cpu.pc = 0;
    nextMem = 0;
    while(fread(&binary, sizeof(binary), 1, f) == 1){
        cpu.mem[nextMem] = binary;
        nextMem++;
    }
    while(cpu.pc < MEM_SIZE && cpu.pc >= 0){
        for(int i = 0; i < 4; i++){
            if(memCheck(cpu.pc + i) != 0){
                fprintf(stderr, "%s\n", "Simulation error");
                exit(-1);
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
        // printf("opcode: %ld\n", opcode);
        // printf("rd: %ld\n", rd);
        // printf("rs: %ld\n", rs);
        // printf("rt: %ld\n", rt);
        // printf("l: %ld\n", l);
        if(opcode == 16 || opcode == 21 || opcode == 24){
            uint64_t sign = l & (1 << 11);
            sign = sign >> 11;
            if(sign == 1){
                l = -(~l + 4097);
            }
        }
        check = interpret(opcode, rd, rs, rt, l);
        if(check == -1){
            fprintf(stderr, "%s\n", "Simulation error");
            exit(-1);
        }
        if(check == 1){
            exit(0);
        }
    }
    fprintf(stderr, "%s\n", "Simulation error");
    exit(1);
}