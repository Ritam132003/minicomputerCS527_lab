#include <stdio.h>

#include "compiler.h"
#include "processor.h"
#include "memory.h"


int main(void)
{
    /*
     * Step 1:
     * Compile program.txt into program.byte
     */
    compile();


    /*
     * Step 2:
     * Load program.byte and data.byte
     * into instruction and data memory.
     */
    initialize();


    /*
     * Step 3:
     * Reset processor registers and flags.
     */
    reset();


    /*
     * Step 4:
     * Fetch -> Decode -> Execute
     * until the program reaches opcode 0x00.
     */
    while (!end_of_simulation)
    {
        fetch();

        decode();

        execute();
    }


    /*
     * Display registers for testing.
     */
    printf("\nRegister values:\n");

    printf("x1 = %d\n", Register[1]);

    printf("x2 = %d\n", Register[2]);

    printf("x3 = %d\n", Register[3]);

    printf("x4 = %d\n", Register[4]);

    printf("x5 = %d\n", Register[5]);

    printf("\nFlags:\n");
    
    printf("N = %d\n", N);
    printf("Z = %d\n", Z);
    printf("C = %d\n", C);
    printf("V = %d\n", V);


    /*
     * Step 5:
     * Write final data memory back to data.byte.
     */
    finalize();


    return 0;
}