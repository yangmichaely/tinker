#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>

#ifndef ASSEMBLER_H
#define ASSEMBLER_H

typedef struct addr{
    char* name;
    int address;
    struct addr* next;
} addr;

void insert(char* name, int address, int nameLength);

int search(char* name);

void freeList(addr* head);

int firstPass(FILE* fp);

int checkValid(char* cmdName, char* cmdParams, int emptyParams);

int read(FILE* fp, char* outFile);

uint32_t calcShifts(uint64_t l, uint32_t rt, uint32_t rs, uint32_t rd, uint32_t opcode);

int splitter(char* cmdParams, uint16_t cmdNum, int emptyParams, FILE* out);

#endif