/**
 *  This file is the basis for the SimpleComputerVM.
 *
 *  @author Christopher Cerne (cernec1999)
 *  @version 05/08/2019
 */

#include <stdio.h>

/**
 *  Entry point for the program
 *
 *  @param argc The number of arguments in the program
 *  @param argv The character array holding the arguments
 */
int main(int argc, char **argv) {
    if (argc != 3) {
        printf("%s: Illegal arguments\n", argv[0]);
        printf("Usage: %s instr.txt data.txt\n", argv[0]);
        return 0;
    }

    printf("Hello, world\n");
    return 0;
}
