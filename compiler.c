#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "compiler.h"

#define MAX_LINE 256
#define MAX_LABELS 256

/*
 * A label stores:
 *
 * name  -> label name
 * address -> byte address in program.byte
 */
typedef struct
{
    char name[64];
    int address;
} Label;


/*
 * Global label table.
 */
Label labels[MAX_LABELS];

int label_count = 0;


/*
 * =========================================================
 * GET REGISTER NUMBER
 * =========================================================
 */

int get_register_number(char *name)
{
    int reg;

    if (sscanf(name, "x%d", &reg) != 1)
    {
        return -1;
    }

    if (reg < 0 || reg > 255)
    {
        return -1;
    }

    return reg;
}


/*
 * =========================================================
 * REMOVE COMMENT
 * =========================================================
 */

void remove_comment(char *line)
{
    char *p;

    p = strchr(line, '%');

    if (p != NULL)
    {
        *p = '\0';
    }
}


/*
 * =========================================================
 * REMOVE NEWLINE
 * =========================================================
 */

void remove_newline(char *line)
{
    line[strcspn(line, "\r\n")] = '\0';
}


/*
 * =========================================================
 * TRIM SPACES
 * =========================================================
 */

void trim(char *str)
{
    int start = 0;
    int end;

    while (isspace((unsigned char)str[start]))
    {
        start++;
    }

    if (start > 0)
    {
        memmove(
            str,
            str + start,
            strlen(str + start) + 1
        );
    }

    end = strlen(str) - 1;

    while (end >= 0 &&
           isspace((unsigned char)str[end]))
    {
        str[end] = '\0';
        end--;
    }
}


/*
 * =========================================================
 * FIND LABEL
 * =========================================================
 */

int find_label(char *name)
{
    int i;

    for (i = 0; i < label_count; i++)
    {
        if (strcmp(labels[i].name, name) == 0)
        {
            return labels[i].address;
        }
    }

    return -1;
}


/*
 * =========================================================
 * ADD LABEL
 * =========================================================
 */

void add_label(char *name, int address)
{
    if (label_count >= MAX_LABELS)
    {
        printf("Error: Too many labels\n");
        exit(1);
    }

    /*
     * Check duplicate label.
     */
    if (find_label(name) != -1)
    {
        printf("Error: Duplicate label: %s\n", name);
        exit(1);
    }

    strcpy(labels[label_count].name, name);

    labels[label_count].address = address;

    label_count++;
}


/*
 * =========================================================
 * CHECK WHETHER LINE IS A LABEL
 * =========================================================
 */

int is_label(char *line)
{
    if (line[0] == '.')
    {
        return 1;
    }

    return 0;
}


/*
 * =========================================================
 * FIRST PASS
 *
 * Find label addresses.
 * =========================================================
 */

void first_pass(void)
{
    FILE *input;

    char line[MAX_LINE];

    int address = 0;


    input = fopen("program.txt", "r");

    if (input == NULL)
    {
        printf("Error: Cannot open program.txt\n");
        exit(1);
    }


    while (fgets(line, sizeof(line), input) != NULL)
    {
        remove_comment(line);

        remove_newline(line);

        trim(line);


        /*
         * Ignore empty lines.
         */
        if (strlen(line) == 0)
        {
            continue;
        }


        /*
         * Label.
         *
         * A label does NOT generate bytecode.
         */
        if (is_label(line))
        {
            add_label(line, address);

            continue;
        }


        /*
         * Every normal instruction occupies
         * four bytes.
         */
        address += 4;
    }


    fclose(input);
}


/*
 * =========================================================
 * GET BRANCH CONDITION CODE
 * =========================================================
 */

int get_branch_code(char *instruction)
{
    if (strcmp(instruction, "BEQ") == 0)
        return 0x00;

    if (strcmp(instruction, "BNE") == 0)
        return 0x01;

    if (strcmp(instruction, "BCS") == 0)
        return 0x02;

    if (strcmp(instruction, "BCC") == 0)
        return 0x03;

    if (strcmp(instruction, "BMI") == 0)
        return 0x04;

    if (strcmp(instruction, "BPL") == 0)
        return 0x05;

    if (strcmp(instruction, "BVS") == 0)
        return 0x06;

    if (strcmp(instruction, "BVC") == 0)
        return 0x07;

    if (strcmp(instruction, "BHI") == 0)
        return 0x08;

    if (strcmp(instruction, "BLS") == 0)
        return 0x09;

    if (strcmp(instruction, "BGE") == 0)
        return 0x0A;

    if (strcmp(instruction, "BLT") == 0)
        return 0x0B;

    if (strcmp(instruction, "BGT") == 0)
        return 0x0C;

    if (strcmp(instruction, "BLE") == 0)
        return 0x0D;

    if (strcmp(instruction, "BAL") == 0)
        return 0x0E;

    return -1;
}


/*
 * =========================================================
 * SECOND PASS
 *
 * Generate program.byte.
 * =========================================================
 */

void second_pass(void)
{
    FILE *input;
    FILE *output;

    char line[MAX_LINE];

    char dest_name[32];
    char src1_name[32];
    char src2_name[32];

    char instruction[32];
    char label_name[64];

    char operator;

    int dest;
    int src1;
    int src2;

    int constant;

    int branch_code;

    int label_address;

    int current_address;

    int next_address;

    int offset;


    input = fopen("program.txt", "r");

    if (input == NULL)
    {
        printf("Error: Cannot open program.txt\n");
        exit(1);
    }


    output = fopen("program.byte", "w");

    if (output == NULL)
    {
        printf("Error: Cannot create program.byte\n");

        fclose(input);

        exit(1);
    }


    current_address = 0;


    while (fgets(line, sizeof(line), input) != NULL)
    {
        remove_comment(line);

        remove_newline(line);

        trim(line);


        /*
         * Ignore empty lines.
         */
        if (strlen(line) == 0)
        {
            continue;
        }


        /*
         * Label does not generate an instruction.
         */
        if (is_label(line))
        {
            continue;
        }


        /*
         * =================================================
         * BRANCH
         *
         * Example:
         *
         * BEQ .loop
         * BAL .exit
         * =================================================
         */

        if (sscanf(line,
                   "%31s %63s",
                   instruction,
                   label_name) == 2)
        {
            branch_code =
                get_branch_code(instruction);


            if (branch_code >= 0)
            {
                label_address =
                    find_label(label_name);


                if (label_address < 0)
                {
                    printf(
                        "Error: Unknown label: %s\n",
                        label_name
                    );

                    fclose(input);
                    fclose(output);

                    exit(1);
                }


                /*
                 * After fetch(), PC points to
                 * the next instruction.
                 */
                next_address =
                    current_address + 4;


                /*
                 * Relative offset:
                 *
                 * target - next instruction
                 */
                offset =
                    label_address - next_address;


                /*
                 * Offset must fit in signed byte.
                 */
                if (offset < -128 ||
                    offset > 127)
                {
                    printf(
                        "Error: Branch offset out of range\n"
                    );

                    fclose(input);
                    fclose(output);

                    exit(1);
                }


                fprintf(
                    output,
                    "%02X 00 00 %02X\n",
                    0x10 + branch_code,
                    (unsigned char)offset
                );


                current_address += 4;

                continue;
            }
        }


        /*
         * =================================================
         * x1 = 10
         * =================================================
         */

        if (sscanf(line,
                   "%31s = %d",
                   dest_name,
                   &constant) == 2)
        {
            dest =
                get_register_number(dest_name);


            if (dest >= 0 &&
                constant >= 0 &&
                constant <= 255)
            {
                fprintf(
                    output,
                    "07 %02X %02X 00\n",
                    dest,
                    constant
                );

                current_address += 4;

                continue;
            }
        }


        /*
         * =================================================
         * x1 = [0]
         * =================================================
         */

        if (sscanf(line,
                   "%31s = [%d]",
                   dest_name,
                   &constant) == 2)
        {
            dest =
                get_register_number(dest_name);


            if (dest >= 0 &&
                constant >= 0 &&
                constant <= 255)
            {
                fprintf(
                    output,
                    "05 %02X %02X 00\n",
                    dest,
                    constant
                );

                current_address += 4;

                continue;
            }
        }


        /*
         * =================================================
         * x1 = [x2]
         * =================================================
         */

        if (sscanf(line,
                   "%31s = [%31[^]]]",
                   dest_name,
                   src1_name) == 2)
        {
            dest =
                get_register_number(dest_name);

            src1 =
                get_register_number(src1_name);


            if (dest >= 0 &&
                src1 >= 0)
            {
                fprintf(
                    output,
                    "05 %02X %02X 00\n",
                    dest,
                    src1
                );

                current_address += 4;

                continue;
            }
        }


        /*
         * =================================================
         * [x1] = x2
         * =================================================
         */

        if (sscanf(line,
                   "[%31[^]]] = %31s",
                   src1_name,
                   dest_name) == 2)
        {
            src1 =
                get_register_number(src1_name);

            dest =
                get_register_number(dest_name);


            if (src1 >= 0 &&
                dest >= 0)
            {
                fprintf(
                    output,
                    "06 %02X %02X 00\n",
                    dest,
                    src1
                );

                current_address += 4;

                continue;
            }
        }


        /*
         * =================================================
         * Register arithmetic
         *
         * x1 = x2 + x3
         * =================================================
         */

        if (sscanf(line,
                   "%31s = %31s %c %31s",
                   dest_name,
                   src1_name,
                   &operator,
                   src2_name) == 4)
        {
            dest =
                get_register_number(dest_name);

            src1 =
                get_register_number(src1_name);


            if (dest < 0 ||
                src1 < 0)
            {
                printf(
                    "Invalid register:\n%s\n",
                    line
                );

                current_address += 4;

                continue;
            }


            /*
             * Register-register arithmetic.
             */

            src2 =
                get_register_number(src2_name);


            if (src2 >= 0)
            {
                switch (operator)
                {
                    case '+':

                        fprintf(
                            output,
                            "01 %02X %02X %02X\n",
                            dest,
                            src1,
                            src2
                        );

                        break;


                    case '-':

                        fprintf(
                            output,
                            "02 %02X %02X %02X\n",
                            dest,
                            src1,
                            src2
                        );

                        break;


                    case '*':

                        fprintf(
                            output,
                            "03 %02X %02X %02X\n",
                            dest,
                            src1,
                            src2
                        );

                        break;


                    case '/':

                        fprintf(
                            output,
                            "04 %02X %02X %02X\n",
                            dest,
                            src1,
                            src2
                        );

                        break;


                    default:

                        printf(
                            "Unknown operator: %c\n",
                            operator
                        );

                        break;
                }


                current_address += 4;

                continue;
            }


            /*
             * Constant arithmetic.
             */

            if (sscanf(src2_name,
                       "%d",
                       &constant) == 1)
            {
                if (constant < 0 ||
                    constant > 255)
                {
                    printf(
                        "Constant out of range: %d\n",
                        constant
                    );

                    current_address += 4;

                    continue;
                }


                switch (operator)
                {
                    case '+':

                        fprintf(
                            output,
                            "09 %02X %02X %02X\n",
                            dest,
                            src1,
                            constant
                        );

                        break;


                    case '-':

                        fprintf(
                            output,
                            "0A %02X %02X %02X\n",
                            dest,
                            src1,
                            constant
                        );

                        break;


                    case '*':

                        fprintf(
                            output,
                            "0B %02X %02X %02X\n",
                            dest,
                            src1,
                            constant
                        );

                        break;


                    case '/':

                        fprintf(
                            output,
                            "0C %02X %02X %02X\n",
                            dest,
                            src1,
                            constant
                        );

                        break;


                    default:

                        printf(
                            "Unknown operator: %c\n",
                            operator
                        );

                        break;
                }


                current_address += 4;

                continue;
            }
        }


        /*
         * =================================================
         * LEGACY READ
         * =================================================
         */

        if (sscanf(line,
                   "Read %31s %d",
                   dest_name,
                   &constant) == 2)
        {
            dest =
                get_register_number(dest_name);


            if (dest >= 0 &&
                constant >= 0 &&
                constant <= 255)
            {
                fprintf(
                    output,
                    "05 %02X %02X 00\n",
                    dest,
                    constant
                );
            }

            current_address += 4;

            continue;
        }


        /*
         * =================================================
         * LEGACY WRITE
         * =================================================
         */

        if (sscanf(line,
                   "Write %31s %d",
                   dest_name,
                   &constant) == 2)
        {
            dest =
                get_register_number(dest_name);


            if (dest >= 0 &&
                constant >= 0 &&
                constant <= 255)
            {
                fprintf(
                    output,
                    "06 %02X %02X 00\n",
                    dest,
                    constant
                );
            }

            current_address += 4;

            continue;
        }


        /*
         * Unknown instruction.
         */

        printf(
            "Warning: Could not compile line:\n%s\n",
            line
        );

        current_address += 4;
    }


    /*
     * END OF PROGRAM
     */

    fprintf(
        output,
        "00 00 00 00\n"
    );


    fclose(input);

    fclose(output);
}


/*
 * =========================================================
 * MAIN COMPILER FUNCTION
 * =========================================================
 */

void compile(void)
{
    /*
     * Reset label table.
     */
    label_count = 0;


    /*
     * PASS 1
     *
     * Find labels.
     */
    first_pass();


    /*
     * PASS 2
     *
     * Generate bytecode.
     */
    second_pass();
}