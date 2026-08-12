#include <stdio.h>
#include <stdint.h>

#include "processor.h"


/*
 * =========================================================
 * REGISTERS
 * =========================================================
 */

int Register[256];

int PC;

int opcode;
int dest;
int src1;
int src2;

int N;
int Z;
int C;
int V;

int end_of_simulation = 0;


/*
 * =========================================================
 * RESET
 * =========================================================
 */

void reset(void)
{
    int i;

    for (i = 0; i < 256; i++)
    {
        Register[i] = 0;
    }

    PC = 0;

    opcode = 0;
    dest = 0;
    src1 = 0;
    src2 = 0;

    N = 0;
    Z = 0;
    C = 0;
    V = 0;

    end_of_simulation = 0;
}


/*
 * =========================================================
 * FETCH
 * =========================================================
 */

void fetch(void)
{
    opcode = Instruction[PC];

    dest = Instruction[PC + 1];

    src1 = Instruction[PC + 2];

    src2 = Instruction[PC + 3];

    PC = PC + 4;
}


/*
 * =========================================================
 * DECODE
 * =========================================================
 */

void decode(void)
{
    /*
     * The instruction is already separated into:
     *
     * opcode
     * dest
     * src1
     * src2
     *
     * by fetch().
     *
     * Therefore no additional decoding is required here.
     */
}


/*
 * =========================================================
 * UPDATE ZERO AND NEGATIVE FLAGS
 * =========================================================
 */

void update_ZN(int32_t result)
{
    if (result == 0)
    {
        Z = 1;
    }
    else
    {
        Z = 0;
    }


    if (result < 0)
    {
        N = 1;
    }
    else
    {
        N = 0;
    }
}


/*
 * =========================================================
 * EXECUTE
 * =========================================================
 */

void execute(void)
{
    int32_t a;
    int32_t b;

    int64_t result64;

    int32_t result;

    uint32_t address;

    int32_t memory_value;


    switch (opcode)
    {

        /*
         * =================================================
         * 0x01
         *
         * ADD
         *
         * x1 = x2 + x3
         * =================================================
         */

        case 0x01:

            a = Register[src1];

            b = Register[src2];

            result64 =
                (int64_t)a +
                (int64_t)b;

            result = (int32_t)result64;

            Register[dest] = result;

            update_ZN(result);


            /*
             * Carry
             */
            if ((uint64_t)(uint32_t)a +
                (uint64_t)(uint32_t)b >
                0xFFFFFFFFULL)
            {
                C = 1;
            }
            else
            {
                C = 0;
            }


            /*
             * Signed overflow
             */
            if (((a >= 0) &&
                 (b >= 0) &&
                 (result < 0)) ||

                ((a < 0) &&
                 (b < 0) &&
                 (result >= 0)))
            {
                V = 1;
            }
            else
            {
                V = 0;
            }

            break;


        /*
         * =================================================
         * 0x02
         *
         * SUBTRACT
         *
         * x1 = x2 - x3
         * =================================================
         */

        case 0x02:

            a = Register[src1];

            b = Register[src2];

            result64 =
                (int64_t)a -
                (int64_t)b;

            result = (int32_t)result64;

            Register[dest] = result;

            update_ZN(result);


            /*
             * Carry / borrow
             */
            if ((uint32_t)a >=
                (uint32_t)b)
            {
                C = 1;
            }
            else
            {
                C = 0;
            }


            /*
             * Signed overflow
             */
            if (((a >= 0) &&
                 (b < 0) &&
                 (result < 0)) ||

                ((a < 0) &&
                 (b >= 0) &&
                 (result >= 0)))
            {
                V = 1;
            }
            else
            {
                V = 0;
            }

            break;


        /*
         * =================================================
         * 0x03
         *
         * MULTIPLY
         *
         * x1 = x2 * x3
         * =================================================
         */

        case 0x03:

            result =
                Register[src1] *
                Register[src2];

            Register[dest] = result;

            update_ZN(result);

            break;


        /*
         * =================================================
         * 0x04
         *
         * DIVIDE
         *
         * x1 = x2 / x3
         * =================================================
         */

        case 0x04:

            if (Register[src2] != 0)
            {
                result =
                    Register[src1] /
                    Register[src2];

                Register[dest] = result;

                update_ZN(result);
            }
            else
            {
                printf("Error: Division by zero\n");

                end_of_simulation = 1;
            }

            break;


        /*
         * =================================================
         * 0x05
         *
         * MEMORY READ
         *
         * x1 = [x2]
         *
         * IMPORTANT:
         *
         * src1 is the REGISTER NUMBER.
         *
         * Therefore:
         *
         * address = Register[src1]
         *
         * NOT:
         *
         * address = src1
         * =================================================
         */

        case 0x05:

            /*
             * Get actual memory address from register.
             *
             * Example:
             *
             * x1 = 10
             * x3 = [x1]
             *
             * src1 = 1
             *
             * Register[1] = 10
             *
             * Therefore address = 10
             */

            address =
                (uint32_t)Register[src1];


            /*
             * A 32-bit value requires four bytes.
             */

            if (address + 3 <
                DATA_MEMORY_SIZE)
            {
                memory_value =
                    ((int32_t)Data[address]) |

                    ((int32_t)Data[address + 1] << 8) |

                    ((int32_t)Data[address + 2] << 16) |

                    ((int32_t)Data[address + 3] << 24);


                Register[dest] =
                    memory_value;


                update_ZN(memory_value);
            }
            else
            {
                printf(
                    "Error: Memory read out of bounds\n"
                );

                end_of_simulation = 1;
            }

            break;


        /*
         * =================================================
         * 0x06
         *
         * MEMORY WRITE
         *
         * [x1] = x2
         *
         * IMPORTANT:
         *
         * src1 is the REGISTER containing the address.
         *
         * dest is the REGISTER containing the value.
         *
         * =================================================
         */

        case 0x06:

            /*
             * Get actual address.
             *
             * Example:
             *
             * x1 = 10
             * x2 = 20
             *
             * [x1] = x2
             *
             * address = Register[1] = 10
             *
             * value = Register[2] = 20
             */

            address =
                (uint32_t)Register[src1];


            /*
             * Make sure four bytes fit in memory.
             */

            if (address + 3 <
                DATA_MEMORY_SIZE)
            {
                memory_value =
                    Register[dest];


                /*
                 * Store the 32-bit value
                 * as four bytes.
                 */

                Data[address] =
                    (unsigned char)
                    (memory_value & 0xFF);


                Data[address + 1] =
                    (unsigned char)
                    ((memory_value >> 8) & 0xFF);


                Data[address + 2] =
                    (unsigned char)
                    ((memory_value >> 16) & 0xFF);


                Data[address + 3] =
                    (unsigned char)
                    ((memory_value >> 24) & 0xFF);
            }
            else
            {
                printf(
                    "Error: Memory write out of bounds\n"
                );

                end_of_simulation = 1;
            }

            break;


        /*
         * =================================================
         * 0x07
         *
         * LOAD CONSTANT
         *
         * x1 = 10
         * =================================================
         */

        case 0x07:

            Register[dest] = src1;

            update_ZN(Register[dest]);

            break;


        /*
         * =================================================
         * 0x09
         *
         * ADD CONSTANT
         *
         * x1 = x2 + 10
         * =================================================
         */

        case 0x09:

            a = Register[src1];

            b = src2;

            result64 =
                (int64_t)a +
                (int64_t)b;

            result = (int32_t)result64;

            Register[dest] = result;

            update_ZN(result);


            if ((uint64_t)(uint32_t)a +
                (uint64_t)(uint32_t)b >
                0xFFFFFFFFULL)
            {
                C = 1;
            }
            else
            {
                C = 0;
            }


            if (((a >= 0) &&
                 (b >= 0) &&
                 (result < 0)) ||

                ((a < 0) &&
                 (b < 0) &&
                 (result >= 0)))
            {
                V = 1;
            }
            else
            {
                V = 0;
            }

            break;


        /*
         * =================================================
         * 0x0A
         *
         * SUBTRACT CONSTANT
         *
         * x1 = x2 - 10
         * =================================================
         */

        case 0x0A:

            a = Register[src1];

            b = src2;

            result64 =
                (int64_t)a -
                (int64_t)b;

            result = (int32_t)result64;

            Register[dest] = result;

            update_ZN(result);


            if ((uint32_t)a >=
                (uint32_t)b)
            {
                C = 1;
            }
            else
            {
                C = 0;
            }


            if (((a >= 0) &&
                 (b < 0) &&
                 (result < 0)) ||

                ((a < 0) &&
                 (b >= 0) &&
                 (result >= 0)))
            {
                V = 1;
            }
            else
            {
                V = 0;
            }

            break;


        /*
         * =================================================
         * 0x0B
         *
         * MULTIPLY CONSTANT
         *
         * x1 = x2 * 10
         * =================================================
         */

        case 0x0B:

            result =
                Register[src1] *
                src2;

            Register[dest] = result;

            update_ZN(result);

            break;


        /*
         * =================================================
         * 0x0C
         *
         * DIVIDE CONSTANT
         *
         * x1 = x2 / 10
         * =================================================
         */

        case 0x0C:

            if (src2 != 0)
            {
                result =
                    Register[src1] /
                    src2;

                Register[dest] = result;

                update_ZN(result);
            }
            else
            {
                printf(
                    "Error: Division by zero\n"
                );

                end_of_simulation = 1;
            }

            break;
                    /*
         * =================================================
         * BRANCH INSTRUCTIONS
         * =================================================
         *
         * Branch bytecode:
         *
         * opcode 00 00 offset
         *
         * PC has already been advanced by fetch().
         *
         * Therefore:
         *
         * PC = PC + signed offset
         */


        /*
         * BEQ
         *
         * Branch if Z == 1
         */
        case 0x10:

            if (Z == 1)
            {
                PC += (int8_t)src2;
            }

            break;


        /*
         * BNE
         *
         * Branch if Z == 0
         */
        case 0x11:

            if (Z == 0)
            {
                PC += (int8_t)src2;
            }

            break;


        /*
         * BCS
         *
         * Branch if C == 1
         */
        case 0x12:

            if (C == 1)
            {
                PC += (int8_t)src2;
            }

            break;


        /*
         * BCC
         *
         * Branch if C == 0
         */
        case 0x13:

            if (C == 0)
            {
                PC += (int8_t)src2;
            }

            break;


        /*
         * BMI
         *
         * Branch if N == 1
         */
        case 0x14:

            if (N == 1)
            {
                PC += (int8_t)src2;
            }

            break;


        /*
         * BPL
         *
         * Branch if N == 0
         */
        case 0x15:

            if (N == 0)
            {
                PC += (int8_t)src2;
            }

            break;


        /*
         * BVS
         *
         * Branch if V == 1
         */
        case 0x16:

            if (V == 1)
            {
                PC += (int8_t)src2;
            }

            break;


        /*
         * BVC
         *
         * Branch if V == 0
         */
        case 0x17:

            if (V == 0)
            {
                PC += (int8_t)src2;
            }

            break;


        /*
         * BHI
         *
         * C == 1 and Z == 0
         */
        case 0x18:

            if (C == 1 && Z == 0)
            {
                PC += (int8_t)src2;
            }

            break;


        /*
         * BLS
         *
         * C == 0 or Z == 1
         */
        case 0x19:

            if (C == 0 || Z == 1)
            {
                PC += (int8_t)src2;
            }

            break;


        /*
         * BGE
         *
         * N == V
         */
        case 0x1A:

            if (N == V)
            {
                PC += (int8_t)src2;
            }

            break;


        /*
         * BLT
         *
         * N != V
         */
        case 0x1B:

            if (N != V)
            {
                PC += (int8_t)src2;
            }

            break;


        /*
         * BGT
         *
         * Z == 0 and N == V
         */
        case 0x1C:

            if (Z == 0 && N == V)
            {
                PC += (int8_t)src2;
            }

            break;


        /*
         * BLE
         *
         * Z == 1 or N != V
         */
        case 0x1D:

            if (Z == 1 || N != V)
            {
                PC += (int8_t)src2;
            }

            break;


        /*
         * BAL
         *
         * Always branch.
         */
        case 0x1E:

            PC += (int8_t)src2;

            break;


        /*
         * =================================================
         * 0x00
         *
         * END PROGRAM
         * =================================================
         */

        case 0x00:

            end_of_simulation = 1;

            break;


        /*
         * =================================================
         * UNKNOWN OPCODE
         * =================================================
         */

        default:

            printf(
                "Unknown opcode: %02X\n",
                opcode
            );

            end_of_simulation = 1;

            break;
    }
}