#ifndef MEMORY_H
#define MEMORY_H

#define INSTRUCTION_MEMORY_SIZE 256
#define DATA_MEMORY_SIZE 4096

extern unsigned char Instruction[INSTRUCTION_MEMORY_SIZE];
extern unsigned char Data[DATA_MEMORY_SIZE];

void initialize(void);
void finalize(void);

#endif