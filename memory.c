#include <stdio.h>
#include <stdlib.h>
#include "memory.h"

unsigned char Instruction[INSTRUCTION_MEMORY_SIZE];
unsigned char Data[DATA_MEMORY_SIZE];

void initialize(void)
{
    FILE *program_file;
    FILE *data_file;

    unsigned int value;
    int i = 0;

    /* Initialize instruction memory */
    for (i = 0; i < INSTRUCTION_MEMORY_SIZE; i++)
    {
        Instruction[i] = 0;
    }

    /* Initialize data memory */
    for (i = 0; i < DATA_MEMORY_SIZE; i++)
    {
        Data[i] = 0;
    }

    program_file = fopen("program.byte", "r");

    if (program_file == NULL)
    {
        printf("Error: Cannot open program.byte\n");
        exit(1);
    }

    i = 0;

    while (i < INSTRUCTION_MEMORY_SIZE &&
           fscanf(program_file, "%x", &value) == 1)
    {
        Instruction[i] = (unsigned char)value;
        i++;
    }

    fclose(program_file);


    data_file = fopen("data.byte", "r");

    if (data_file == NULL)
    {
        printf("Error: Cannot open data.byte\n");
        exit(1);
    }

    i = 0;

    while (i < DATA_MEMORY_SIZE &&
           fscanf(data_file, "%x", &value) == 1)
    {
        Data[i] = (unsigned char)value;
        i++;
    }

    fclose(data_file);
}

void finalize(void)
{
    FILE *data_file;
    int i;

    data_file = fopen("data.byte", "w");

    if (data_file == NULL)
    {
        printf("Error: Cannot open data.byte for writing\n");
        exit(1);
    }

    for (i = 0; i < DATA_MEMORY_SIZE; i++)
    {
        fprintf(data_file, "%02X", Data[i]);

        if ((i + 1) % 4 == 0)
        {
            fprintf(data_file, "\n");
        }
        else
        {
            fprintf(data_file, " ");
        }
    }

    fclose(data_file);
}