//
// Created by Tyler on 2021-06-01.
//

/* Assembler code fragment for LC-2K */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define MAXLINELENGTH 1000

//datatype for labels
typedef struct Label {
    char name[7];
    char val[MAXLINELENGTH];
    int addr;
    struct Label *next; //ptr to next
} Label;

int numLabels;
Label labels;

int readAndParse(FILE *, char *, char *, char *,char *, char *);
int isNumber(char *);

void handleLabels(FILE *asFilePtr);
int isValidLabel(char *);



int main(int argc, char *argv[]) {
    char *inFileString, *outFileString;
    FILE *inFilePtr, *outFilePtr;
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
            arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];
    int flag = 1;
    int opcodeDec;  /* 24-22 */
    int regADec;    /* 21-19 */
    int regBDec;    /* 18-16 */
    int destRegDec; /* 2-0 */
    int offsetDec;  /* 15-0 16 bit 2's complement number with a range of -32768 to 32767 */
    if (argc != 3) {
        printf("error: usage: %s <assembly-code-file> <machine-code-file>\n", argv[0]);
        exit(1);
    }
    inFileString = argv[1];
    outFileString = argv[2];
    inFilePtr = fopen(inFileString, "r");
    if (inFilePtr == NULL) {
        printf("error in opening %s\n", inFileString);
        exit(1);
    }
    outFilePtr = fopen(outFileString, "w");
    if (outFilePtr == NULL) {
        printf("error in opening %s\n", outFileString);
        exit(1);
    }
    handleLabels(inFilePtr);

    int address = 0;

    while(flag) {
        int type;       /* type of opcode 1 r,2 i,3 j,4 o, or 5 . */
        int opcodeDec;  /* 24-22 */
        int regADec;    /* 21-19 */
        int regBDec;    /* 18-16 */
        int destRegDec; /* 2-0 */
        int offsetDec ;  /* 15-0 16 bit 2's complement number with a range of -32768 to 32767 */
        int machineCode = 0;/* holds the machine code value to print to output */
        if (!readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
            /* reached end of file */
            printf("reached end of file");
            flag = 0;
            break;
        }

        /* R TYPE INSTRUCTIONS */
        if (!strcmp(opcode, "add")) {
            opcodeDec = 0;
            type = 1;
        }
        else if (!strcmp(opcode, "nor")) {
            opcodeDec = 4194304;
            type = 1;
        }
        /* I TYPE INSTRUCTIONS */
        else if (!strcmp(opcode, "lw")) {
            opcodeDec = 8388608;
            type = 2;
        }
        else if (!strcmp(opcode, "sw")) {
            opcodeDec = 12582912;
            type = 2;
        }
        else if (!strcmp(opcode, "beq")) {
            opcodeDec = 16777216;
            type = 2;
        }
        /* J TYPE INSTRUCTIONS */
        else if (!strcmp(opcode, "jalr")) {
            opcodeDec = 20971520;
            type = 3;
        }
        /* O TYPE INSTRUCTIONS */
        else if (!strcmp(opcode, "halt")) {
            opcodeDec = 25165824;
            type = 4;
        }
        else if (!strcmp(opcode, "noop")) {
            opcodeDec = 29360128;
            type = 4;
        }
        else if (!strcmp(opcode, ".fill")) {
            opcodeDec = 0;
            type = 5;
        } else {
            printf("error: opcode not recognized: %s\n", opcode);
            exit(1);
        }


        switch (type) {
            case 1: /* r type */
                if (isNumber(arg0) && isNumber(arg1) && isNumber(arg2)){
                    regADec = atoi(arg0) * 524288;
                    regBDec = atoi(arg1) * 65536;
                    destRegDec = atoi(arg2);
                    machineCode = opcodeDec + regADec + regBDec + destRegDec;
                } else {
                    printf("error: args not valid");
                    exit(1);
                }
                break;
            case 2: /* i type */
            if (isNumber(arg0) && isNumber(arg1)) {
                regADec = atoi(arg0) * 524288;
                regBDec = atoi(arg1) * 65536;
                if (isNumber(arg2)) {
                    offsetDec = atoi(arg2);
                    if (offsetDec > 32767 || offsetDec < -32768) {
                        printf("error: offset too large");
                        exit(1);
                    }
                } else {
                    Label *curr;
                    for (curr = &labels; curr; curr = curr->next) {
                        if (!strcmp(curr->name, arg2)) {
                            offsetDec = curr->addr;
                            break;
                        }
                    }

                }
                //branch
                if (opcodeDec == 16777216 && !isNumber(arg2)) {
                    int branchto = offsetDec;
                    offsetDec = branchto - address - 1;
                    if(offsetDec > 32767 || offsetDec < -32768){
                        printf("error: offset too large");
                        exit(1);
                    }
                }
                //printf("offsetDec: %d\n", offsetDec);
                offsetDec &= 0xFFFF;
                machineCode = opcodeDec + regADec + regBDec + offsetDec;
            } else {
                printf("error: register args not valid");
                exit(1);
            }
                break;
            case 3: /* j type */
            if (isNumber(arg0) && isNumber(arg1)) {
                regADec = atoi(arg0) * 524288;
                regBDec = atoi(arg1) * 65536;
                machineCode = opcodeDec + regADec + regBDec;
            } else {
                printf("error: args not valid");
                exit(1);
            }
                break;
            case 4: /* o type */
                machineCode = opcodeDec;
                break;
            case 5: /* .fill */
                //if it is a number
                if (isNumber(arg0)) {
                    machineCode = atoi(arg0);
                }
                //if it is a label
                else {
                    Label *curr;
                    int found = 0;
                    for (curr = &labels; curr; curr = curr->next) {
                        //printf("    current label: %s\n    arg0: %s \n",curr->name, arg0);
                        if (!strcmp(curr->name, arg0)) {
                            machineCode = curr->addr;
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        printf("error: undefined label");
                        exit(1);
                    }
                }
                break;
            default:
                printf("error: invalid opcode");
                exit(1);
        }

        fprintf(outFilePtr, "%d\n", machineCode);
        printf("(Address %d): %d\n", address, machineCode);
        address++;
    }
    /* this is how to rewind the file ptr so that you start reading from the
         beginning of the file */
    rewind(inFilePtr);
    return (0);
}

/*
 * Read and parse a line of the assembly-language file. Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *  0 if reached end of file
 *  1 if all went well
 *
 * exit(1) if line is too long
 */
int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0, char *arg1, char *arg2) {
    char line[MAXLINELENGTH];
    char *ptr = line;
    /* delete prior values */
    label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

    /* read the line from the assembly-language file */
    if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
        /* reached end of file */
        return(0);
    }
    /* check for line too long (by looking for a \n) */
    if (strchr(line, '\n') == NULL) {
        /* line too long */
        printf("error: line too long\n");
        exit(1);
    }
    /* is there a label? */
    ptr = line;
    if (sscanf(ptr, "%[^\t\n\r ]", label)) {
        /* successfully read label; advance pointer over the label */
        ptr += strlen(label);
    }
    /*
     * Parse the rest of the line. Would be nice to have real regular
     * expressions, but sscanf will suffice.
     */
    sscanf(ptr, "%*[\t\n\r    ]%[^\t\n\r    ]%*[\t\n\r    ]%[^\t\n\r    ]%*[\t\n\r    ]%[^\t\n\r ]%*[\t\n\r  ]%[^\t\n\r  ]", opcode, arg0, arg1, arg2);
    //printf("%s %s %s %s %s\n", label, opcode, arg0, arg1, arg2);
    return(1);
}
void handleLabels(FILE *asFilePtr) {
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
            arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];
    int address = 0;
    numLabels = 0;
    int flag = 1;
    Label *curr;

    while (flag) {
        //end of file?
        if (!readAndParse(asFilePtr, label, opcode, arg0, arg1, arg2)) {
            /* reached end of file */
            flag = 0;
            break;
        }

        //is there a label?
        if (!strcmp(label, "")) {
            address++;
            continue;
        }
        //is the label valid?
        if (!isValidLabel(label)) {
            printf("error: invalid label");
            exit(1);
        }
        //does the label already exist? run through existing labels
        for (curr = &labels; curr; curr = curr->next) {
            if (!strcmp(curr->name, label))
            {
                printf("error: multiple labels with same name");
                exit(1);
            }
        }
        //adjust pointer and set name
        for (curr = &labels; curr->next; curr = curr->next)
            ;
        curr->next = malloc(sizeof(Label));
        curr = curr->next;
        strcpy(curr->name, label);
        curr->addr = address++;

        // if it is a fill command
        if (!strcmp(opcode, ".fill")) {
            strcpy(curr->val, arg0);
        } else {
            //isnt fill
            curr->val[0] = '\0';
        }
        numLabels += 1;
    }
    rewind(asFilePtr);
}

int isNumber(char *string) {
    /* return 1 if string is a number */
    int i;
    return ((sscanf(string, "%d", &i)) == 1);
}

int isValidLabel(char *label) {
    /*  RULES:
     *  1. Max 6 characters
     *  2. Can consist of letters and numbers
     *  3. Must start with a letter
    */
    //check 6 chars and if start with letter
    if (strlen(label) > 6 || isNumber(&label[0])) {
        return 0;
    }
    for (int i=0; label[i]; i++) {
        if ((label[i] >= 'a' && label[i] <= 'z' || label[i] >= 'A' && label[i] <= 'Z') || label[i] >= '0' && label[i] <= '9') {
            continue;
        } else {
            printf("error: label %s does not contain only letters or numbers", label);
            exit(1);
        }
    }
    return 1;
}