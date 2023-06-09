#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "linkedlist.h"
#include "hashtable.h"
#include "riscv.h"

/************** BEGIN HELPER FUNCTIONS PROVIDED FOR CONVENIENCE ***************/
const int R_TYPE = 0;
const int I_TYPE = 1;
const int MEM_TYPE = 2;
const int U_TYPE = 3;
const int UNKNOWN_TYPE = 4;

/**
 * Return the type of instruction for the given operation
 * Available options are R_TYPE, I_TYPE, MEM_TYPE, UNKNOWN_TYPE
 */
static int get_op_type(char *op)
{
    const char *r_type_op[] = {"add", "sub", "and", "or", "xor", "slt", "sll", "sra"};
    const char *i_type_op[] = {"addi", "andi", "ori", "xori", "slti"};
    const char *mem_type_op[] = {"lw", "lb", "sw", "sb"};
    const char *u_type_op[] = {"lui"};
    for (int i = 0; i < (int)(sizeof(r_type_op) / sizeof(char *)); i++)
    {
        if (strcmp(r_type_op[i], op) == 0)
        {
            return R_TYPE;
        }
    }
    for (int i = 0; i < (int)(sizeof(i_type_op) / sizeof(char *)); i++)
    {
        if (strcmp(i_type_op[i], op) == 0)
        {
            return I_TYPE;
        }
    }
    for (int i = 0; i < (int)(sizeof(mem_type_op) / sizeof(char *)); i++)
    {
        if (strcmp(mem_type_op[i], op) == 0)
        {
            return MEM_TYPE;
        }
    }
    for (int i = 0; i < (int)(sizeof(u_type_op) / sizeof(char *)); i++)
    {
        if (strcmp(u_type_op[i], op) == 0)
        {
            return U_TYPE;
        }
    }
    return UNKNOWN_TYPE;
}
/*************** END HELPER FUNCTIONS PROVIDED FOR CONVENIENCE ****************/

registers_t *registers;
// TODO: create any additional variables to store the state of the interpreter
hashtable_t *memory;
void init(registers_t *starting_registers)
{
    registers = starting_registers;
    // TODO: initialize any additional variables needed for state
    memory = ht_init(1024);
}

// TODO: create any necessary helper functions

char *strsep(char **stringp, const char *delim)
{
    if (stringp == NULL || *stringp == NULL) {
        return NULL;
    }

    char *start = *stringp;
    char *end = strpbrk(start, delim);

    if (end != NULL) {
        *end = '\0';
        *stringp = end + 1;
    }
    else {
        *stringp = NULL;
    }
    return start;
}

int read_register(char* register_index) {
    int index = register_index[1] - '0';
    if (strlen(register_index) == 3) {
        index = index * 10 + register_index[2] - '0';
    }
    return registers->r[index];
}

void write_register(char* register_index, int value) {
    int index = register_index[1] - '0';
    if (strlen(register_index) == 3) {
        index = index * 10 + register_index[2] - '0';
    }
    registers->r[index] = value;
}

int sign_extended(int number) {
    number = number << 20;
    number = number >> 20;
    return number;
}

void step(char *instruction)
{
    // Extracts and returns the substring before the first space character,
    // by replacing the space character with a null-terminator.
    // `instruction` now points to the next character after the space
    // See `man strsep` for how this library function works
    char *op = strsep(&instruction, " ");
    // Uses the provided helper function to determine the type of instruction
    int op_type = get_op_type(op);
    // Skip this instruction if it is not in our supported set of instructions
    if (op_type == UNKNOWN_TYPE)
    {
        return;
    }
    // TODO: write logic for evaluating instruction on current interpreter state
    if (op_type == R_TYPE){
        char *rd = strsep(&instruction, ", ");
        while (strcmp(rd, "") == 0) {
            rd = strsep(&instruction, ", ");
        }

        char *rs1 = strsep(&instruction, ", ");
        while (strcmp(rs1, "") == 0) {
            rs1 = strsep(&instruction, ", ");
        }

        char *rs2 = strsep(&instruction, ", ");
        while (strcmp(rs2, "") == 0) {
            rs2 = strsep(&instruction, ", ");
        }

        int register1 = read_register(rs1);
        int register2 = read_register(rs2);

        if (strcmp(op, "add") == 0) {
            int result = register1 + register2;
            write_register(rd, result);
        } else if (strcmp(op, "sub") == 0){
            int result = register1 - register2;
            write_register(rd, result);
        } else if (strcmp(op, "and") == 0){
            int result = register1 & register2;
            write_register(rd, result);
        } else if (strcmp(op, "or") == 0){
            int result = register1 | register2;
            write_register(rd, result);
        } else if (strcmp(op, "xor") == 0){
            int result = register1 ^ register2;
            write_register(rd, result);
        } else if (strcmp(op, "nor") == 0){
            int result = !(register1 | register2);
            write_register(rd, result);
        } else if (strcmp(op, "slt") == 0){
            int result = (register1 < register2)?1:0;
            write_register(rd, result);
        } else if (strcmp(op, "sll") == 0){
            int result = register1 << register2;
            write_register(rd, result);
        } else if (strcmp(op, "sra") == 0){
            int result = register1 >> register2;
            write_register(rd, result);
        }
    }

    if (op_type == I_TYPE){
        char *rd = strsep(&instruction, ", ");
        while (strcmp(rd, "") == 0) {
            rd = strsep(&instruction, ", ");
        }
        
        char *rs1 = strsep(&instruction, ", ");
        while (strcmp(rs1, "") == 0) {
            rs1 = strsep(&instruction, ", ");
        }

        char *imm = strsep(&instruction, ", ");
        while (strcmp(imm, "") == 0) {
            imm = strsep(&instruction, ", ");
        }

        int register1 = read_register(rs1);
        int immediate = (int)strtol(imm, NULL, 0);
        immediate = sign_extended(immediate);

        if (strcmp(op, "addi") == 0) {
            int result = register1 + immediate;
            write_register(rd, result);
        } else if (strcmp(op, "andi") == 0){
            int result = register1 & immediate;
            write_register(rd, result);
        } else if (strcmp(op, "ori") == 0){
            int result = register1 | immediate;
            write_register(rd, result);
        } else if (strcmp(op, "xori") == 0){
            int result = register1 ^ immediate;
            write_register(rd, result);
        } else if (strcmp(op, "slti") == 0){
            int result = (register1 < immediate)?1:0;
            write_register(rd, result);
        }
    }

    if (op_type == MEM_TYPE){
        char *rd = strsep(&instruction, ", ()");
        while (strcmp(rd, "") == 0) {
            rd = strsep(&instruction, ", ");
        }

        char *imm = strsep(&instruction, ", ()");
        while (strcmp(imm, "") == 0) {
            imm = strsep(&instruction, ", ()");
        }

        char *rs1 = strsep(&instruction, ", ()");
        while (strcmp(rs1, "") == 0) {
            rs1 = strsep(&instruction, ", ()");
        }
        
        int register1 = read_register(rs1);
        int immediate = (int)strtol(imm, NULL, 0);
        immediate = sign_extended(immediate);

        int memory_address = register1 + immediate;
        if (strcmp(op, "lw") == 0) {
            int result = ht_get(memory, memory_address + 3);
            result = (result << 8) + ht_get(memory, memory_address + 2);
            result = (result << 8) + ht_get(memory, memory_address + 1);
            result = (result << 8) + ht_get(memory, memory_address);
            write_register(rd, result);
        } else if (strcmp(op, "lb") == 0){
            int result = ht_get(memory, memory_address);
            result += ((result & (0x00000080))?1:0) * (0xffffff00);
            write_register(rd, result);
        } else if (strcmp(op, "sw") == 0){
            int result = read_register(rd);
            ht_add(memory, memory_address, result & 0x000000ff);
            ht_add(memory, memory_address + 1, (result & 0x0000ff00) >> 8);
            ht_add(memory, memory_address + 2, (result & 0x00ff0000) >> 16);
            ht_add(memory, memory_address + 3, (result & 0xff000000) >> 24);
        } else if (strcmp(op, "sb") == 0){
            int result = read_register(rd);
            ht_add(memory, memory_address, result & 0x000000ff);
        }
    }

    if (op_type == U_TYPE){
        char *rd = strsep(&instruction, ", ");
        while (strcmp(rd, "") == 0) {
            rd = strsep(&instruction, ", ");
        }

        char *imm = strsep(&instruction, ", ");
        while (strcmp(imm, "") == 0) {
            imm = strsep(&instruction, ", ");
        }

        int immediate = (int)strtol(imm, NULL, 0);

        int result = immediate << 12;
        write_register(rd, result);
    }

    // x0 always equals 0
    write_register("x0", 0);
}
