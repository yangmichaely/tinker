#include "assembler.h"
#include "simulator.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>

int main(int argc, char** argv){
    FILE* fp = fopen(argv[1], "r\0");
    //printf("\nFILENAME: %s\n", argv[1]);
    if(fp == NULL){
        fprintf(stderr, "%s", "Invalid tinker filepath\n");
        return -1;
    }
    if(firstPass(fp) != 1){
        //printf("successful first pass\n");
    // }
    // else{
        //printf("failed first pass\n");
        fclose(fp);
        return -1;
    }
    char* outFile = (char*) calloc(sizeof(char) * strlen(argv[1]) + 2, 1);
    for(int i = 0; i < strlen(argv[1]); i++){
        outFile[i] = argv[1][i];
    }
    outFile[strlen(argv[1])] = 'o';
    outFile[strlen(argv[1]) + 1] = '\0';
    if(read(fp, outFile) != 1){
        //printf("successful write\n");
    // }
    // else{
        //printf("failed write\n");
        fclose(fp);
        return -1;
    }
    fclose(fp);
    FILE* f = fopen(outFile, "rb");
    if(f == NULL){
        fprintf(stderr, "%s", "Invalid tko filepath\n");
        return -1;
    }
    if (readBinary(f) != 0){
        //printf("successful binary read\n");
    // }
    // else{
        //printf("failed binary read\n");
        fclose(f);
        return -1;
    }
    free(outFile);
    fclose(f);
    return 0;
}