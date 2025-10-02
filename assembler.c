/**
 * Project 1
 * Assembler code fragment for LC-2K
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

//Every LC2K file will contain less than 1000 lines of assembly.
#define MAXLINELENGTH 1000
#define UNDEFINED INT_MIN

int readAndParse(FILE *, char *, char *, char *, char *, char *);
static void checkForBlankLinesInCode(FILE *inFilePtr);
static inline int isNumber(char *);
static inline void printHexToFile(FILE *, int);
bool checkInstructionErrors(char*, char*, char*, char*, char labels[MAXLINELENGTH][MAXLINELENGTH], int refAddr);
int instructionToBinary(char *, int, int, int);
int resolveLabel(char labels[MAXLINELENGTH][MAXLINELENGTH], char*);

int main(int argc, char **argv) {
    // I/O
    char *inFileString, *outFileString;
    FILE *inFilePtr, *outFilePtr;
    // Reading and parsing
    char label[MAXLINELENGTH],
         opcode[MAXLINELENGTH],
         arg0[MAXLINELENGTH],
         arg1[MAXLINELENGTH],
         arg2[MAXLINELENGTH];
    // Label management
    char labels[MAXLINELENGTH][MAXLINELENGTH];

    if (argc != 3) {
        printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
            argv[0]);
        exit(1);
    }

    inFileString = argv[1];
    outFileString = argv[2];

    inFilePtr = fopen(inFileString, "r");
    if (inFilePtr == NULL) {
        printf("error in opening %s\n", inFileString);
        exit(1);
    }

    // Check for blank lines in the middle of the code.
    checkForBlankLinesInCode(inFilePtr);

    outFilePtr = fopen(outFileString, "w");
    if (outFilePtr == NULL) {
        printf("error in opening %s\n", outFileString);
        exit(1);
    }

    // First pass through file contents: Locate and store addresses of all labels
    int lineAddr = 0;
    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
        if (strcmp(label, "")) {
            // Check if label hasn't been defined before
            for (int i = 0; i < MAXLINELENGTH; ++i) {
                if (!strcmp(labels[i], label)) {
                    exit(1);
                }
            }
            strcpy(labels[lineAddr], label);
        }
        ++lineAddr;
    }
    rewind(inFilePtr);

    // Second pass through file contents: Convert to machine code
    int currAddr = 0;
    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
        int arg0i = (isNumber(arg0)) ? atoi(arg0) : resolveLabel(labels, arg0);
        int arg1i = (isNumber(arg1)) ? atoi(arg1) : resolveLabel(labels, arg1);
        int arg2i = (isNumber(arg2)) ? atoi(arg2) :
                        (!strcmp(opcode, "beq")) ? (resolveLabel(labels, arg2) - currAddr - 1) & 0xFFFF
                                                 : resolveLabel(labels, arg2);
        // Check For Errors
        if (checkInstructionErrors(opcode, arg0, arg1, arg2, labels, currAddr)) {
            exit(1);
        }
        // Convert instruction to binary
        printHexToFile(outFilePtr, instructionToBinary(opcode, arg0i, arg1i, arg2i));
        ++currAddr;
    }

    return(0);
}

// Returns non-zero if the line contains only whitespace.
static int lineIsBlank(char *line) {
    char whitespace[4] = {'\t', '\n', '\r', ' '};
    int nonempty_line = 0;
    for(int line_idx=0; line_idx < strlen(line); ++line_idx) {
        int line_char_is_whitespace = 0;
        for(int whitespace_idx = 0; whitespace_idx < 4; ++ whitespace_idx) {
            if(line[line_idx] == whitespace[whitespace_idx]) {
                line_char_is_whitespace = 1;
                break;
            }
        }
        if(!line_char_is_whitespace) {
            nonempty_line = 1;
            break;
        }
    }
    return !nonempty_line;
}

// Exits 2 if file contains an empty line anywhere other than at the end of the file.
// Note calling this function rewinds inFilePtr.
static void checkForBlankLinesInCode(FILE *inFilePtr) {
    char line[MAXLINELENGTH];
    int blank_line_encountered = 0;
    int address_of_blank_line = 0;
    rewind(inFilePtr);

    for(int address = 0; fgets(line, MAXLINELENGTH, inFilePtr) != NULL; ++address) {
        // Check for line too long
        if (strlen(line) >= MAXLINELENGTH-1) {
            printf("error: line too long\n");
            exit(1);
        }

        // Check for blank line.
        if(lineIsBlank(line)) {
            if(!blank_line_encountered) {
                blank_line_encountered = 1;
                address_of_blank_line = address;
            }
        } else {
            if(blank_line_encountered) {
                printf("Invalid Assembly: Empty line at address %d\n", address_of_blank_line);
                exit(1);
            }
        }
    }
    rewind(inFilePtr);
}


// CUSTOM
bool checkInstructionErrors(char* opcode, char* arg0, char* arg1, char* arg2, char labels[MAXLINELENGTH][MAXLINELENGTH], int refAddr) {
    int arg0i = (isNumber(arg0)) ? atoi(arg0) : resolveLabel(labels, arg0);
    int arg1i = (isNumber(arg1)) ? atoi(arg1) : resolveLabel(labels, arg1);
    int arg2i = (isNumber(arg2)) ? atoi(arg2) : resolveLabel(labels, arg2);
    // ERROR CHECKING START =============================================================================
    // --- R-type Instructions
    if (!strcmp(opcode, "add") || !strcmp(opcode, "nor")) {
        return (
        (arg0i == UNDEFINED || arg1i == UNDEFINED || arg2i == UNDEFINED)                   ||                  
        (!isNumber(arg0) || !isNumber(arg1) || !isNumber(arg2))                            ||             
        ((arg0i < 0 || arg0i > 7) || (arg1i < 0 || arg1i > 7) || (arg2i < 0 || arg2i > 7))
        );                                                                                 
    }
    // --- I-type Instructions
    else if (!strcmp(opcode, "lw") || !strcmp(opcode, "sw") || !strcmp(opcode, "beq")) {
        return (
        (arg0i == UNDEFINED || arg1i == UNDEFINED || arg2i == UNDEFINED)                   ||
        (!isNumber(arg0) || !isNumber(arg1))                                               ||
        ((arg0i < 0 || arg0i > 7) || (arg1i < 0 || arg1i > 7))                             ||
        // (x <= 0xFFFF) iff x \in [-32768, 32767] as a 16-bit 2's complement int
        (!isNumber(arg2))
            ? (arg2i - refAddr - 1 <= 0xFFFF)
            : arg2i <= 0xFFFF                                                                           
        );
    }
    // --- J-type Instructions
    else if (!strcmp(opcode, "jalr")) {
        return (
        (arg0i == UNDEFINED || arg1i == UNDEFINED)                                         ||
        (!isNumber(arg0) || !isNumber(arg1))                                               ||
        ((arg0i < 0 || arg0i > 7) || (arg1i < 0 || arg1i > 7))                             
        );
    }
    // --- O-type Instructions
    else if (!strcmp(opcode, "halt") || !strcmp(opcode, "noop")) {
        // Nothing to check :)
        return false;
    }
    // -- .fill Instruction
    else if (!strcmp(opcode, ".fill")) {
        return arg0i == UNDEFINED;
    }
    // --- Unrecognized Instructions
    return true;
}
int instructionToBinary(char *opcode, int arg0, int arg1, int arg2) {
    return 
    // R-type Instructions =====================================================================
    //                              opcode     |     regA     |     regB     |    destReg
    (!strcmp(opcode, "add"))   ? (0b000 << 22) | (arg0 << 19) | (arg1 << 16) | arg2            :
    (!strcmp(opcode, "nor"))   ? (0b001 << 22) | (arg0 << 19) | (arg1 << 16) | arg2            :
    // I-type Instructions =====================================================================
    //                              opcode     |     regA     |     regB     |  offsetField
    (!strcmp(opcode, "lw"))    ? (0b010 << 22) | (arg0 << 19) | (arg1 << 16) | (arg2 & 0xFFFF) :
    (!strcmp(opcode, "sw"))    ? (0b011 << 22) | (arg0 << 19) | (arg1 << 16) | (arg2 & 0xFFFF) :
    (!strcmp(opcode, "beq"))   ? (0b100 << 22) | (arg0 << 19) | (arg1 << 16) | (arg2 & 0xFFFF) :
    // J-type Instructions =====================================================================
    //                              opcode     |     regA     |     regB     |     unused
    (!strcmp(opcode, "jalr"))  ? (0b101 << 22) | (arg0 << 19) | (arg1 << 16)                   :
    // O-type Instructions =====================================================================
    //                              opcode     |                    unused          
    (!strcmp(opcode, "halt"))  ? (0b110 << 22)                                                 :
    (!strcmp(opcode, "noop"))  ? (0b111 << 22)                                                 :
    // .fill Directive =========================================================================
    //                             opcode     |                     unused          
    (!strcmp(opcode, ".fill")) ? arg0                                                          :
    -1;
}
int resolveLabel(char labels[MAXLINELENGTH][MAXLINELENGTH], char* label) {
    for (int currAddr = 0; currAddr < MAXLINELENGTH; ++currAddr) {
        if (labels[currAddr] != NULL && !strcmp(labels[currAddr], label)) {
            return currAddr;
        }
    }
    return UNDEFINED;
}

/*
* NOTE: The code defined below is not to be modifed as it is implimented correctly.
*/

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
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

    /* check for line too long */
    if (strlen(line) == MAXLINELENGTH-1) {
	printf("error: line too long\n");
	exit(1);
    }

    // Ignore blank lines at the end of the file.
    if(lineIsBlank(line)) {
        return 0;
    }

    /* is there a label? */
    ptr = line;
    if (sscanf(ptr, "%[^\t\n ]", label)) {
	/* successfully read label; advance pointer over the label */
        ptr += strlen(label);
    }

    /*
     * Parse the rest of the line.  Would be nice to have real regular
     * expressions, but scanf will suffice.
     */
    sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]",
        opcode, arg0, arg1, arg2);

    return(1);
}

static inline int isNumber(char *string)
{
    int num;
    char c;
    return((sscanf(string, "%d%c",&num, &c)) == 1);
}

// Prints a machine code word in the proper hex format to the file
static inline void 
printHexToFile(FILE *outFilePtr, int word) {
    fprintf(outFilePtr, "0x%08X\n", word);
}
