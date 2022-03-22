//
// Created by Tyler on 2021-06-14.
//

/*  LC-2K  Instruction-level  simulator  */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUMMEMORY 65536  /*  maximum  number  of  words  in  memory  */
#define NUMREGS 8  /*  number  of  machine  registers  */
#define MAXLINELENGTH 1000

typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

void printState(stateType *);
int convertNum(int num);

int main(int argc, char *argv[]) {
    char line[MAXLINELENGTH];
    stateType state;
    //state.pc = 0;
    for (int i = 0; i < NUMREGS; i++) {
        state.reg[i] = 0;
    }
    FILE *filePtr;
    int instrsExecuted = 0;
    if (argc != 2) {
        printf("error: usage %s <machine-code file>\n", argv[0]);
        exit(1);
    }
    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s", argv[1]);
        perror("fopen");
        exit(1);
    }
    /*  read  in  the  entire  machine-code  file  into  memory  */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL; state.numMemory++)  {
        if (sscanf(line, "%d", state.mem + state.numMemory) != 1)  {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
        printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }

    printState(&state);
    while(1) {
        int v;
        int type;
        int opcode;
        int arg0;
        int arg1;
        int arg2;
        int halt = 0;
        int offset;
        int memoryAddress;

        stateType *sPtr = &state;

        //get instructions from memory
        v = sPtr->mem[sPtr->pc];
        opcode = (v >> 22) & 0b111;
        arg0 = (v >> 19) & 0b111;
        arg1 = (v >> 16) & 0b111;
        arg2 = (v & 0xFFFF);

        state.pc++;
        instrsExecuted++;

        switch(opcode) {
            case 0: // add
                sPtr->reg[arg2] = sPtr->reg[arg0] + sPtr->reg[arg1];
                break;
            case 1: // nor
                sPtr->reg[arg2] = ~(sPtr->reg[arg0] | sPtr->reg[arg1]);
                break;
            case 2: // lw
                offset = convertNum(arg2);
                if (offset > 32767 || offset < -32768) {
                    printf("error: invalid offset");
                    exit(1);
                }
                memoryAddress = sPtr->reg[arg0] + offset;
                sPtr->reg[arg1] = sPtr->mem[memoryAddress];
                break;
            case 3: // sw
                offset = convertNum(arg2);
                if (offset > 32767 || offset < -32768) {
                    printf("error: invalid offset");
                    exit(1);
                }
                memoryAddress = sPtr->reg[arg0] + offset;
                sPtr->mem[memoryAddress] = sPtr->reg[arg1];
                break;
            case 4: // beq
                offset = convertNum(arg2);
                if (offset > 32767 || offset < -32768) {
                    printf("error: invalid offset");
                    exit(1);
                }
                if (sPtr->reg[arg0] == sPtr->reg[arg1]) {
                    sPtr->pc += offset;
                }
                break;
            case 5: //  jalr
                sPtr->reg[arg1] = sPtr->pc;
                sPtr->pc = sPtr->reg[arg0];
                break;
            case 6: // halt
                halt = 1;
                break;
            case 7: // noop
                break;
            default:
                printf("error: incorrect opcode");
                exit(1);
                break;
        }

        if (halt) {
            break;
        }

        printState(&state);
    }
    printf("machine halted\ntotal of %d instructions executed\nfinal state of machine:\n", instrsExecuted);
    printState(&state);
    fclose(filePtr);
    exit(0);
}

void printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i = 0; i < statePtr->numMemory; i++) {
        printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
    }
    printf("\tregisters:\n");
    for (i = 0; i < NUMREGS; i++) {
        printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }
    printf("end state\n");
}

int convertNum(int num) {
    /* convert a 16-bit number into a 32 bit Linux integer */
    if(num & (1<<15)){
        num -= (1<<16);
    }
    return(num);
}