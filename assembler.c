#include "assembler.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <limits.h>
#include <regex.h>

char* VALID_COMMANDS[36] = {"add", "addi", "sub", "subi", "mul", "div", "and", "or", "xor", "not", "shftr", 
"shftri", "shftl", "shftli", "br", "brr", "brr", "brnz", "call", "return", "brgt", "mov", "mov", "mov", "mov", "addf", "subf", 
"mulf", "divf", "in", "out", "halt", "clr", "ld", "push", "pop"};

char* VALID_PARAMETERS[36] = {"^r([0-9]|1[0-9]|2[0-9]|3[0-1]), r([0-9]|1[0-9]|2[0-9]|3[0-1]), r([0-9]|1[0-9]|2[0-9]|3[0-1])$",
"^r([0-9]|1[0-9]|2[0-9]|3[0-1]), r([0-9]|1[0-9]|2[0-9]|3[0-1])$", "^r([0-9]|1[0-9]|2[0-9]|3[0-1])$", 
"^r([0-9]|1[0-9]|2[0-9]|3[0-1]), [0-9]+|:[a-zA-Z0-9_-]+$", "^[-+]?[0-9]+|:[a-zA-Z0-9_-]+$", "^r([0-9]|1[0-9]|2[0-9]|3[0-1]), \\(r([0-9]|1[0-9]|2[0-9]|3[0-1])\\)\\([-+]?[0-9]+|:[a-zA-Z0-9_-]+\\)$", 
"^\\(r([0-9]|1[0-9]|2[0-9]|3[0-1])\\)\\([-+]?[0-9]+|:[a-zA-Z0-9_-]+\\), r([0-9]|1[0-9]|2[0-9]|3[0-1])$", "^[a-zA-Z0-9_-]+$"};

int lines;
int avail;
addr* head = NULL;

int main(int argc, char** argv){
    FILE* fp = fopen(argv[1], "r\0");
    if(fp == NULL){
        fprintf(stderr, "%s", "Invalid tinker filepath\n");
        exit(1);
    }
    char k = argv[1][strlen(argv[1]) - 1];
    char t = argv[1][strlen(argv[1]) - 2];
    char dot = argv[1][strlen(argv[1]) - 3];
    if(k != 'k' || t != 't' || dot != '.'){
        fprintf(stderr, "%s", "Invalid tinker filepath\n");
        exit(1);
    }
    firstPass(fp);
    char* outFile = (char*) calloc(sizeof(char) * strlen(argv[1]) + 2, 1);
    for(int i = 0; i < strlen(argv[1]); i++){
        outFile[i] = argv[1][i];
    }
    outFile[strlen(argv[1])] = 'o';
    outFile[strlen(argv[1]) + 1] = '\0';
    read(fp, outFile);
    // free(outFile);
    // fclose(fp);
    // return 0;
}

void insert(char* name, int address, int nameLength){
    if(head != NULL){
        addr* lk = (addr*) malloc(sizeof(addr));
        lk -> address = address;
        lk -> name = (char*) malloc(sizeof(char) * (nameLength + 1));
        strncpy(lk -> name, name, nameLength);
        lk -> name[nameLength] = '\0';
        lk -> next = head;
        head = lk;
    }
    else{
        head = (addr*) malloc (sizeof(addr));
        head -> address = address;
        head -> name = (char*) malloc(sizeof(char) * (nameLength + 1));
        strcpy(head -> name, name);
        head -> next = NULL;
    }
}

int search(char* name){
    addr *temp = head;
    while(temp != NULL) {
        if (strcmp(temp -> name, name) == 0) {
            return temp -> address;
        }
        temp=temp->next;
    }
    return -1;
}

void freeList(addr* head){
    addr* tmp;
    while (head != NULL){
        tmp = head;
        head = head->next;
        free(tmp -> name);
        free(tmp);
    }
}

void firstPass(FILE* fp){
    avail = 0;
    lines = 0;
    int mem = 0;
    for (char d = getc(fp); d != EOF; d = getc(fp)){
        if (d == '\n'){
            lines++;
        }
    }
    fseek(fp, -1, SEEK_END);
    //char a = getc(fp);
    //printf("here: %c\n", a);
    if(getc(fp) != '\n'){
        fprintf(stderr, "%s%d\n", "Error on line ", lines + 1);
        exit(1);
    }
    rewind(fp);
    int data = 0;
    int code = 0;
    for(int i = 0; i < lines; i++){
        char* buffer = (char*) calloc(512 * sizeof(char), 1);
        fgets(buffer, 512, fp);
        // if(strchr(buffer, '\n') != NULL){
        //     fprintf(stderr, "%s%d\n", "Error on line ", i + 1);
        //     free(buffer);
        //     exit(1);
        // }
        buffer[strcspn(buffer, "\n")] = '\0';
        char c = buffer[0];
        if(c == '.'){
            char* buff = strtok(buffer, ".");
            if(strcmp(buff, "data") == 0){
                data = 1;
                code = 0;
            }
            else if(strcmp(buff, "code") == 0){
                code = 1;
                data = 0;
            }
            else{
                fprintf(stderr, "%s%d\n", "Error on line ", i + 1);
                freeList(head);
                free(buffer);
                exit(1);
            }
        }
        else if(c == '\t'){
            char* buff = strtok(buffer, "\t");
            if(code == 1){
                char* cmdName = (char*) calloc(256 * sizeof(char), 1);
                int j = 0;
                for(j = 0; buff[j] != ' ' && buff[j] != '\n' && buff[j] != '\0' && buff[j] != EOF; j++){
                    cmdName[j] = buff[j];
                }
                if(strcmp(cmdName, "ld") == 0){
                    mem += 48;
                }
                else if(strcmp(cmdName, "push") == 0 || strcmp(cmdName, "pop") == 0){
                    mem += 8;
                }
                else{
                    mem += 4;
                }
                free(cmdName);
            }
            else if(data == 1){
                mem += 8;
            }
        }
        else if(c == ':'){
            char* name = strtok(buffer, ":");
            int found = search(name);
            regex_t regex;
            regcomp(&regex, VALID_PARAMETERS[7], REG_EXTENDED);
            if(strlen(name) > 255 || regexec(&regex, name, 0, NULL, 0) != 0 || found != -1){
                fprintf(stderr, "%s%d\n", "Error on line ", i + 1);
                regfree(&regex);
                freeList(head);
                free(buffer);
                exit(1);
            }
            regfree(&regex);
            insert(name, mem, strlen(name));
        }
        else if(c != ';'){
            fprintf(stderr, "%s%d\n", "Error on line ", i + 1);
            freeList(head);
            free(buffer);
            exit(1);
        }
        free(buffer);
    }
}

int checkValid(char* cmdName, char* cmdParams, int emptyParams){
    if(strcmp(cmdName, "add") == 0 || strcmp(cmdName, "sub") == 0 || strcmp(cmdName, "mul") == 0 || strcmp(cmdName, "div") == 0 || 
        strcmp(cmdName, "and") == 0 || strcmp(cmdName, "or") == 0 || strcmp(cmdName, "xor") == 0 || strcmp(cmdName, "shftr") == 0 ||
        strcmp(cmdName, "shftl") == 0 || strcmp(cmdName, "brgt") == 0 || strcmp(cmdName, "addf") == 0 ||
        strcmp(cmdName, "subf") == 0 || strcmp(cmdName, "mulf") == 0 || strcmp(cmdName, "divf") == 0){
            regex_t regex;
            regcomp(&regex, VALID_PARAMETERS[0], REG_EXTENDED);
            if(regexec(&regex, cmdParams, 0, NULL, 0) == 0){
                regfree(&regex);
                return 1;
            }
            regfree(&regex);
            return 0;
    }
    else if(strcmp(cmdName, "addi") == 0 || strcmp(cmdName, "subi") == 0 || strcmp(cmdName, "shftri") == 0 || strcmp(cmdName, "shftli") == 0 || 
        strcmp(cmdName, "ld") == 0){
            regex_t regex;
            regcomp(&regex, VALID_PARAMETERS[3], REG_EXTENDED);
            if(regexec(&regex, cmdParams, 0, NULL, 0) == 0){
                regfree(&regex);
                return 1;
            }
            regfree(&regex);
            return 0;
    }
    else if(strcmp(cmdName, "not") == 0 || strcmp(cmdName, "brnz") == 0 || strcmp(cmdName, "in") == 0 || strcmp(cmdName, "out") == 0){
        regex_t regex;
        regcomp(&regex, VALID_PARAMETERS[1], REG_EXTENDED);
        if(regexec(&regex, cmdParams, 0, NULL, 0) == 0){
            regfree(&regex);
            return 1;
        }
        regfree(&regex);
        return 0;
    }
    else if(strcmp(cmdName, "br") == 0 || strcmp(cmdName, "clr") == 0 || strcmp(cmdName, "push") == 0 || strcmp(cmdName, "pop") == 0 || strcmp(cmdName, "call") == 0){
        regex_t regex;
        regcomp(&regex, VALID_PARAMETERS[2], REG_EXTENDED);
        if(regexec(&regex, cmdParams, 0, NULL, 0) == 0){
            regfree(&regex);
            return 1;
        }
        regfree(&regex);
        return 0;
    }
    else if(strcmp(cmdName, "brr") == 0){
        regex_t regex;
        regcomp(&regex, VALID_PARAMETERS[2], REG_EXTENDED);
        if(regexec(&regex, cmdParams, 0, NULL, 0) == 0){
            regfree(&regex);
            return 15;
        }
        regfree(&regex);
        regcomp(&regex, VALID_PARAMETERS[4], REG_EXTENDED);
        if(regexec(&regex, cmdParams, 0, NULL, 0) == 0){
            regfree(&regex);
            return 16;
        }
        regfree(&regex);
        return 0;
    }
    else if(strcmp(cmdName, "mov") == 0){
        regex_t regex;
        regcomp(&regex, VALID_PARAMETERS[5], REG_EXTENDED);
        if(regexec(&regex, cmdParams, 0, NULL, 0) == 0){
            regfree(&regex);
            return 21;
        }
        regfree(&regex);
        regcomp(&regex, VALID_PARAMETERS[1], REG_EXTENDED);
        if(regexec(&regex, cmdParams, 0, NULL, 0) == 0){
            regfree(&regex);
            return 22;
        }
        regfree(&regex);
        regcomp(&regex, VALID_PARAMETERS[3], REG_EXTENDED);
        if(regexec(&regex, cmdParams, 0, NULL, 0) == 0){
            regfree(&regex);
            return 23;
        }
        regfree(&regex);
        regcomp(&regex, VALID_PARAMETERS[6], REG_EXTENDED);
        if(regexec(&regex, cmdParams, 0, NULL, 0) == 0){
            regfree(&regex);
            return 24;
        }
        regfree(&regex);
        return 0;
    }
    else if(strcmp(cmdName, "halt") == 0 || strcmp(cmdName, "return") == 0){
        if(emptyParams == 0){
            return 0;
        }
        return 1;
    }
    else{
        return 0;
    }
}

void read(FILE* fp, char* outFile){
    int next = 0;
    rewind(fp);
    int data = 0;
    int code = 0;
    FILE* out = fopen(outFile, "wb");
    for(int i = 0; i < lines; i++){
        char* buffer = (char*) calloc (512 * sizeof(char), 1);
        fgets(buffer, 512, fp);
        buffer[strcspn(buffer, "\n")] = '\0';
        char c = buffer[0];
        if(c == '.'){
            char* buff = strtok(buffer, ".");
            if(strcmp(buff, "data") == 0){
                data = 1;
                code = 0;
            }
            else if(strcmp(buff, "code") == 0){
                code = 1;
                data = 0;
            }
        }
        else if(c == '\t'){
            char* buff = strtok(buffer, "\t");
            if(code == 1){
                char* cmdName = (char*) calloc(256 * sizeof(char), 1);
                int j = 0;
                for(j = 0; buff[j] != ' ' && buff[j] != '\n' && buff[j] != '\0' && buff[j] != EOF; j++){
                    cmdName[j] = buff[j];
                }
                int emptyParams = 1;
                char* cmdParams = (char*) calloc(strlen(buff) * sizeof(char) - strlen(cmdName) * sizeof(char), 1);
                for(int h = j + 1; h < strlen(buff) && buff[h] != '\n' && buff[h] != '\0'; h++){
                    emptyParams = 0;
                    cmdParams[h - j - 1] = buff[h];
                }
                int check = checkValid(cmdName, cmdParams, emptyParams);
                if(check == 0){
                    fprintf(stderr, "%s%d\n", "Error on line ", i + 1);
                    remove(outFile);
                    fclose(out);
                    free(cmdParams);
                    free(cmdName);
                    free(buffer);
                    freeList(head);
                    exit(1);
                }
                for(int h = 0; h < 36; h++){
                    if(strcmp(cmdName, VALID_COMMANDS[h]) == 0){
                        if((strcmp(cmdName, "brr") != 0 && strcmp(cmdName, "mov") != 0) || ((strcmp(cmdName, "brr") == 0 || strcmp(cmdName, "mov") == 0) && h == check)){
                            if(splitter(cmdParams, h, emptyParams, out) == -1){
                                fprintf(stderr, "%s%d\n", "Error on line ", i + 1);
                                remove(outFile);
                                fclose(out);
                                free(cmdParams);
                                free(cmdName);
                                freeList(head);
                                free(buffer);
                                exit(1);
                            }
                        }
                    }
                }
                free(cmdName);
                free(cmdParams);
            }
            else if(data == 1){
                for (int j = 0; j < strlen(buff); j++){
                    if (!isdigit(buff[j])){
                        fprintf(stderr, "%s%d\n", "Error on line ", i + 1);
                        fclose(out);
                        remove(outFile);
                        free(buffer);
                        freeList(head);
                        exit(1);
                    }
                }
                uint64_t max = 18446744073709551615ULL;
                char *ptr;
                uint64_t dataVal = strtoul(buff, &ptr, 10);
                if(dataVal == max && !(strcmp(buff, "18446744073709551615") == 0)){
                    fprintf(stderr, "%s%d\n", "Error on line ", i + 1);
                    fclose(out);
                    remove(outFile);
                    free(buffer);
                    freeList(head);
                    exit(1);
                }
                fwrite(&dataVal, sizeof(dataVal), 1, out);
            }
        }
        free(buffer);
    }
    fclose(out);
    freeList(head);
}

uint32_t calcShifts(uint64_t l, uint32_t rt, uint32_t rs, uint32_t rd, uint32_t opcode){
    uint32_t instr = 0;
    instr |= l;
    instr |= rt << 12;
    instr |= rs << 17;
    instr |= rd << 22;
    instr |= opcode << 27;
    return instr;
}

int splitter(char* cmdParams, uint16_t cmdNum, int emptyParams, FILE* out){
    int x = 0;
    uint32_t instruction[4] = {cmdNum, 0, 0, 0};
    int64_t l = 0;
    int nextReg = 1;
    if(emptyParams == 0){
        while(x < strlen(cmdParams)) {
            char* tmp = (char*) calloc(sizeof(cmdParams) * sizeof(char), 1);
            int k = 0;
            if(x < strlen(cmdParams) && cmdParams[x] == '('){
                x++;
            }
            while (x < strlen(cmdParams) && cmdParams[x] != '\0' && cmdParams[x] != ',' && cmdParams[x] != ')') {
                tmp[k++] = cmdParams[x++];
            }
            x+=2;
            if(cmdParams[x] == ' '){
                x++;
            }
            //printf("tmp: %s\n", tmp);
            if(strchr(tmp, 'r') != NULL){
                char* reg = strtok(tmp, "r");
                //printf("regnumber: %s\n", reg);
                char* ptr;
                uint32_t regNum = strtoul(reg, &ptr, 10);
                instruction[nextReg] = regNum;
                nextReg++;
            }
            else if(strchr(tmp, ':') != NULL){
                char* label = strtok(tmp, ":");
                uint64_t labelAddr = search(label);
                if(labelAddr == -1){
                    free(tmp);
                    return -1;
                }
                l = labelAddr;
            }
            else{
                // regex_t regex;
                // regcomp(&regex, "^[-+]?[0-9]+$", REG_EXTENDED);
                // if(regexec(&regex, cmdParams, 0, NULL, 0) == 0){
                //     regfree(&regex);
                //     return -1;
                // }
                // regfree(&regex);
                char *ptr;
                l = strtoul(tmp, &ptr, 10);
                if(cmdNum == 16 || cmdNum == 21 || cmdNum == 24){
                    if(atoi(tmp) > 2047 || atoi(tmp) < -2048){
                        return -1;
                    }
                    l = atoi(tmp);
                }
                else if(cmdNum == 33){
                    uint64_t max = 18446744073709551615ULL;
                    if(l == max && !(strcmp(tmp, "18446744073709551615") == 0)){
                        return -1;
                    }
                }
                else{
                    if(atoi(tmp) > 4095 || atoi(tmp) < 0){
                        return -1;
                    }
                    l = atoi(tmp);
                }
            }
            free(tmp);
        }
    }
    u_int32_t binInstruction = 0;
    if(cmdNum == 32){
        binInstruction = calcShifts(0, instruction[1], instruction[1], instruction[1], 8);
        fwrite(&binInstruction, sizeof(binInstruction), 1, out);
    }
    else if(cmdNum == 33){
        binInstruction = calcShifts(0, instruction[1], instruction[1], instruction[1], 8);
        fwrite(&binInstruction, sizeof(binInstruction), 1, out);
        uint64_t twelve1 = l >> 52;
        uint64_t twelve2 = (l >> 40) & 0xfff;
        uint64_t twelve3 = (l >> 28) & 0xfff;
        uint64_t twelve4 = (l >> 16) & 0xfff;
        uint64_t twelve5 = (l >> 4) & 0xfff;
        uint64_t four = l & 0xf;
        binInstruction = calcShifts(twelve1, 0, 0, instruction[1], 1);
        fwrite(&binInstruction, sizeof(binInstruction), 1, out);
        binInstruction = calcShifts(12, 0, 0, instruction[1], 13);
        fwrite(&binInstruction, sizeof(binInstruction), 1, out);
        binInstruction = calcShifts(twelve2, 0, 0, instruction[1], 1);
        fwrite(&binInstruction, sizeof(binInstruction), 1, out);
        binInstruction = calcShifts(12, 0, 0, instruction[1], 13);
        fwrite(&binInstruction, sizeof(binInstruction), 1, out);
        binInstruction = calcShifts(twelve3, 0, 0, instruction[1], 1);
        fwrite(&binInstruction, sizeof(binInstruction), 1, out);
        binInstruction = calcShifts(12, 0, 0, instruction[1], 13);
        fwrite(&binInstruction, sizeof(binInstruction), 1, out);
        binInstruction = calcShifts(twelve4, 0, 0, instruction[1], 1);
        fwrite(&binInstruction, sizeof(binInstruction), 1, out);
        binInstruction = calcShifts(12, 0, 0, instruction[1], 13);
        fwrite(&binInstruction, sizeof(binInstruction), 1, out);
        binInstruction = calcShifts(twelve5, 0, 0, instruction[1], 1);
        fwrite(&binInstruction, sizeof(binInstruction), 1, out);
        binInstruction = calcShifts(4, 0, 0, instruction[1], 13);
        fwrite(&binInstruction, sizeof(binInstruction), 1, out);
        binInstruction = calcShifts(four, 0, 0, instruction[1], 1);
        fwrite(&binInstruction, sizeof(binInstruction), 1, out);
    }
    else if(cmdNum == 34){
        l = -8;
        l &= 0xfff;
        binInstruction = calcShifts(l, 0, instruction[1], 31, 24);
        fwrite(&binInstruction, sizeof(binInstruction), 1, out);
        binInstruction = calcShifts(8, 0, 0, 31, 3);
        fwrite(&binInstruction, sizeof(binInstruction), 1, out);
    }
    else if(cmdNum == 35){
        binInstruction = calcShifts(0, 0, 31, instruction[1], 21);
        fwrite(&binInstruction, sizeof(binInstruction), 1, out);
        binInstruction = calcShifts(8, 0, 0, 31, 1);
        fwrite(&binInstruction, sizeof(binInstruction), 1, out);
    }
    else{
        if(instruction[0] == 16 || instruction[0] == 21 || instruction[0] == 24){
            l &= 0xfff;
        }
        binInstruction = calcShifts(l, instruction[3], instruction[2], instruction[1], instruction[0]);
        fwrite(&binInstruction, sizeof(binInstruction), 1, out);
    }
}