
void decode(void)
{
    switch (opcode)
    {
        /* END */
        case 0x00:
            break;

        /* REGISTER OPERATIONS */
        case 0x01:  /* ADD */
        case 0x02:  /* SUBTRACT */
        case 0x03:  /* MULTIPLY */
        case 0x04:  /* DIVIDE */

        /* MEMORY OPERATIONS */
        case 0x05:  /* READ */
        case 0x06:  /* WRITE */

        /* CONSTANT OPERATIONS */
        case 0x07:  /* LOAD CONSTANT */
        case 0x09:  /* ADD CONSTANT */
        case 0x0A:  /* SUBTRACT CONSTANT */
        case 0x0B:  /* MULTIPLY CONSTANT */
        case 0x0C:  /* DIVIDE CONSTANT */

        /* BRANCH OPERATIONS */
        case 0x10:  /* BEQ */
        case 0x11:  /* BNE */
        case 0x12:  /* BCS */
        case 0x13:  /* BCC */
        case 0x14:  /* BMI */
        case 0x15:  /* BPL */
        case 0x16:  /* BVS */
        case 0x17:  /* BVC */
        case 0x18:  /* BHI */
        case 0x19:  /* BLS */
        case 0x1A:  /* BGE */
        case 0x1B:  /* BLT */
        case 0x1C:  /* BGT */
        case 0x1D:  /* BLE */
        case 0x1E:  /* BAL */
            break;

        default:
            printf("Decode Error: Unknown opcode %02X\n", opcode);
            end_of_simulation = 1;
            break;
    }
}

gcc -Wall -Wextra -std=c11 main.c compiler.c memory.c processor.c -o simulator

./simulator

cp programs/sum.txt program.txt
CS527 Mini Computer Simulator
cd ~/minicomputerCS527_lab
cp programs/sum.txt program.txt
cp data/sum.byte data.byte
gcc -Wall -Wextra -std=c11 main.c compiler.c memory.c processor.c -o simulator
./simulator

## 1. Project Overview

This project implements a Mini Computer Simulator in C for the CS527 laboratory.

The simulator consists of:

- A user-level program compiler
- A processor simulator
- Instruction memory
- Data memory
- General-purpose registers
- Processor status flags
- Arithmetic operations
- Memory read/write operations
- Conditional and unconditional branches
- Labels and branch targets
- A dynamic Makefile for executing different programs

The project is developed using C, GCC/MinGW, and Visual Studio Code.

---

## 2. Development Environment

| Component | Details |
|---|---|
| Operating System | Windows |
| Programming Language | C |
| Compiler | GCC / MinGW |
| Build Tool | mingw32-make |
| IDE | Visual Studio Code |

---

## 3. Project Structure

```text
minicomputerCS527_lab/
│
├── Makefile
├── README.md
│
├── main.c
│
├── compiler.c
├── compiler.h
│
├── processor.c
├── processor.h
│
├── memory.c
├── memory.h
│
├── programs/
│   ├── sum.txt
│   ├── complex.txt
│   ├── determinant.txt
│   ├── branch.txt
│   ├── arithmetic.txt
│   ├── division.txt
│   ├── memory.txt
│   ├── negative.txt
│   ├── zero.txt
│   ├── overflow.txt
│   └── final_test.txt
│
└── data/
    ├── sum.byte
    ├── complex.byte
    ├── determinant.byte
    ├── branch.byte
    ├── arithmetic.byte
    ├── division.byte
    ├── memory.byte
    ├── negative.byte
    ├── zero.byte
    ├── overflow.byte
    └── final_test.byte
