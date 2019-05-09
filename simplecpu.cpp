/**
 *  This file is the basis for the SimpleComputerVM.
 *
 *  @author Christopher Cerne (cernec1999)
 *  @version 05/08/2019
 */

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string>

#include "simplecpu.hpp"

using namespace std;

/**
 *  Entry point for the program
 *
 *  @param argc The number of arguments in the program
 *  @param argv The character array holding the arguments
 *  @return If the program ran successfully
 */
int main(int argc, char **argv) {

    if (argc != 3) {
        printf("%s: Illegal arguments\n", argv[0]);
        printf("Usage: %s instr.txt data.txt\n", argv[0]);
        return 0;
    }

    printf("Simple computer initializing...\n");

    // Read in instruction memory and data memory
    int res_instr = read_file_write_mem(instr, argv[1]);
    int res_memory = read_file_write_mem(memory, argv[2]);

    if (!res_instr || !res_memory) {
        printf("There was a problem reading the instruciton or data memory files.\n");
        return 0;
    }

    while (running) {
        branch_control();

        // get the current instruction
        reg[IR] = instr[reg[PC]];

        instruction_decoder();

        uint16_t operand_A = reg[AA];
        uint16_t operand_B = reg[BA];

        // if MB, set operand_B to a constant value
        if (MB)
            operand_B = BA;

        // if MW, store in memory
        if (MW) {
            memory[reg[AA]] = reg[BA];
        }

        uint16_t out;

        if (MD) {
            out = memory[reg[AA]];
        } else {
            out = function_unit(operand_A, operand_B);
        }

        if (RW) {
            reg[DA] = out;
        }
    }

    print_memory(64);

    return 0;
}


/**
 *  Gets a range of bits for the instruction decoder
 *
 *  @param num The number to use
 *  @param high The higher bound of the range
 *  @param low The lower bound of the range
 *  @return The result of the operation (in the least significant part)
 */
uint16_t bit_range (uint16_t num, int high, int low) {
    return (num >> low) & ~(0b1111111111111111 << (high - low + 1));
}


/**
 *  Write to instruction memory or data memory
 *
 *  @param arr The data array to write to
 *  @param file The file to read from
 *  @return int If the write was successful
 */
int read_file_write_mem (uint16_t arr[], char *file) {
    ifstream in(file);

    if (!in)
        return 0;

    string line;
    uint16_t count = 0;

    // Iterate through each line in the file
    while (getline(in, line)) {
        // Read hex string as integer
        uint16_t curinstr = stoul (line, nullptr, 16);

        // Save integer to the data memory array and increment
        arr[count] = curinstr;
        count ++;
    }

    return 1;
}


/**
 *  Controls program branching
 */
void branch_control() {
    int8_t branch = (DA << 3) | BA;
    uint16_t jmp = reg[AA];

    // the branch is negative. set variable accordingly.
    if (DA > 0b100) {
        branch |= 0b11000000;
    }

    if (reset) {
        reg[PC] = 0;
        reset = false;
    }
    else {
        // jmp
        if (PL && JB)
            reg[PC] = jmp;
        // branch on negative
        else if (PL && !JB && BC) {
            if (reg[STATUS] & FL_N) {
                reg[PC] = reg[PC] + branch;
            }
            else
                reg[PC] = reg[PC] + 1;
        }
        // branch on zero
        else if (PL && !JB && !BC) {
            if (reg[STATUS] & FL_Z)
                reg[PC] += branch;
            else
                reg[PC] = reg[PC] + 1;
        }
        else {
            // increment
            reg[PC] = reg[PC] + 1;
        }
    }
}


/**
 *  Interprets instructions
 */
void instruction_decoder() {
    // DEBUG: print current instr


    // set registers
    DA = bit_range(reg[IR], 8, 6);
    AA = bit_range(reg[IR], 5, 3);
    BA = bit_range(reg[IR], 2, 0);

    // get bit values
    MB = bit_range(reg[IR], 15, 15);
    MD = bit_range(reg[IR], 13, 13);
    RW = !((bool) bit_range(reg[IR], 14, 14));
    MW = !((bool) bit_range(reg[IR], 15, 15)) && ((bool) bit_range(reg[IR], 14, 14));
    PL = bit_range(reg[IR], 15, 15) & bit_range(reg[IR], 14, 14);
    JB = MD;
    BC = bit_range(reg[IR], 9, 9);

    // Take care of function select
    FS = bit_range(reg[IR], 12, 9);

    // If PL, clear the LSB
    if (PL) {
        FS &= ~1;
    }

}

/**
 *  Prints the variables processed by the instruction decoder
 */
void print_decoder_output() {
    printf("DA: %d, AA: %d, BA: %d\n", DA, AA, BA);
    printf("MB: %d, MD: %d, RW: %d, MW: %d, PL: %d, JB: %d, BC: %d\n", MB, MD, RW, MW, PL, JB, BC);
    printf("FS: %d\n", FS);
}

/**
 *  Prints the memory of the program in its current execution state
 *
 *  @param num How many addresses we want to print
 */
void print_memory(int num) {
    printf("Printing %d pieces of memory...\n", num);
    for (int i = 0; i < num; i++) {
        printf("0x%x:\t0x%x\n", i, memory[i]);
    }
}


/**
 *  Prints the registers of the program in its current execution state
 */
void print_reg() {
    printf("Printing registers\n");
    for (int i = 0; i < 8; i++) {
        printf("R%d:\t0x%x\n", i, reg[i]);
    }
}



/**
 *  Processes the function unit instructions
 *
 *  @param A The A operand
 *  @param B The B operand
 *  @return The result of the operation
 */
uint16_t function_unit(uint16_t A, uint16_t B) {
   uint16_t out = 0;

    switch (FS) {
        case 0:
            out = A;
            break;
        case 1:
            out = A + 1;
            break;
        case 2:
            out = A + B;
            break;
        case 3:
            out = A + B + 1;
            break;
        case 4:
            out = A + ~B;
            break;
        case 5:
            out = A + ~B + 1;
            break;
        case 6:
            out = A - 1;
            break;
        case 7:
            out = A;
            break;
        case 8:
            out = A & B;
            break;
        case 9:
            out = A | B;
            break;
        case 10:
            out = A ^ B;
            break;
        case 11:
            out = ~A;
            break;
        case 12:
            out = B;
            break;
        case 13:
            out = B >> 1;
            break;
        case 14:
            out = B << 1;
            break;
        case 15:
            running = false;
            out = A;
            break;
        default:
            out = A;
            break;
    }

    reg[STATUS] = 0;

    // check MSB to see if it is negative
    if (bit_range (out, 15, 15))
        reg[STATUS] |= FL_N;

    if (out == 0)
        reg[STATUS] |= FL_Z;

    // store the number in the register
    return out;
}
