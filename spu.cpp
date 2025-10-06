#include <TXLib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>

#define STACK_TYPE int
#include "stack.h"
#include "calc.h"
#include "spu.h"


void stk_printf(unsigned int index, stack_type value)
{
    printf("[%u] = %d\n", index, value);
}

int main(void)
{
    struct processor cpu = {};

    cpu.cmd_array = get_commands("input.asm", &cpu.cmd_count);

    if (cpu.cmd_array == NULL)
    {
        printf("Error: failed to read commands\n");
        return 1;
    }

    cpu.ip = 0;
    stack_ctor(&cpu.stk, 2 * cpu.cmd_count);

    enum user_error last_err = do_commands(&cpu);

    if (last_err != NOT_ERRORS)
    {
        print_error_info(last_err);
    }

    stack_dtor(&cpu.stk);
    free(cpu.cmd_array);

    return 0;
}

enum user_error do_commands(struct processor* cpu)
{
    enum user_error last_err = NOT_ERRORS;

    while (cpu->ip < cpu->cmd_count)
    {
        enum cmd_code command = cpu->cmd_array[cpu->ip++];

        switch (command)
        {
            case HLT:
                return NOT_ERRORS;

            case CMD_OUT:
                last_err = out_cmd(&cpu->stk);
                break;

            case PUSH:
                if (cpu->ip >= cpu->cmd_count) {
                    return INCORRECT_N_ARG;
                }
                {
                    int value = cpu->cmd_array[cpu->ip++];
                    last_err = push_cmd(&cpu->stk, value);
                }
                break;

            case ADD:
                last_err = add_cmd(&cpu->stk);
                break;

            case SUB:
                last_err = sub_cmd(&cpu->stk);
                break;

            case MUL:
                last_err = mul_cmd(&cpu->stk);
                break;

            case DIV:
                last_err = div_cmd(&cpu->stk);
                break;

            case POW:
                last_err = pow_cmd(&cpu->stk);
                break;

            case SQRT:
                last_err = sqrt_cmd(&cpu->stk);
                break;

            default:
                return INCORRECT_COMMAND;
        }

        if (last_err != NOT_ERRORS) {
            return last_err;
        }
    }
    return NOT_ERRORS;
}

enum cmd_code* get_commands(const char* filename, int* cmd_count)
{

    FILE* file = fopen(filename, "r");
    if (!file)
    {
        printf("Reading error\n");
        return NULL;
    }

    long int file_size = check_file_size(file);
    if (file_size <= 0)
    {
        fclose(file);
        return NULL;
    }

    enum cmd_code* cmd_mass = (enum cmd_code*)calloc(file_size, sizeof(enum cmd_code));
    if (cmd_mass == NULL)
    {
        free(cmd_mass);
        fclose(file);
        return NULL;
    }

    *cmd_count = 0;

    int value;
    while (fscanf(file, "%d", &value) == 1)
    {
        cmd_mass[(*cmd_count)++] = (enum cmd_code)value;
    }

    fclose(file);
    return cmd_mass;
}


long int check_file_size(FILE* file)
{
    struct stat file_info = {};
    int fd = fileno(file);
    if (fstat(fd, &file_info) == -1)
    {
        printf("ERROR: check number of lines incorrect\n");
        return -1;
    }
    return file_info.st_size;
}
