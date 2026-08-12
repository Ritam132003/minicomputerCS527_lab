# CS527 Mini Computer Simulator

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