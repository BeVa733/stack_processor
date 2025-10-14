//#include <TXLib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

#define STACK_TYPE int
#include "stack.h"
#include "calc.h"
#include "spu.h"


void stk_printf (unsigned int index, stack_type value)
{
    printf("[%u] = %d\n", index, value);
}

int main (void)
{
    enum spu_error last_err = NOT_ERRORS;

    processor spu = {};
    last_err = spu_ctor(&spu);

    if (last_err == NOT_ERRORS)
    {
        last_err = do_commands(&spu);
    }

    spu_dump (&spu, 0);

    spu_dtor (&spu);

    return 0;
}

enum spu_error spu_ctor (processor* spu)
{
    spu->cmd_array = get_commands("output_asm.bin", &(spu->cmd_count));
    stack_ctor(&(spu->stk), 20);
    if (spu->cmd_array == NULL)
    {
        printf("Error: failed to read commands\n");
        return CREATION_ERROR;
    }
    else
        return NOT_ERRORS;
}

void spu_dtor (processor* spu)
{
    stack_dtor(&(spu->stk));
    free(spu->cmd_array);
}

unsigned int spu_verif(processor* spu)
{
    unsigned int error_code = SPU_NO_ERRORS;

    if (spu == NULL)
    {
        error_code |= SPU_BAD_MAIN_PTR;
        return error_code;
    }

    else if (spu->cmd_array == NULL)
        error_code |= SPU_BAD_CMD_ARRAY;

    else if (spu->cmd_count < 0 || spu->cmd_count > SPU_VERY_BIG_NUMBER)
        error_code |= SPU_BAD_CMD_COUNT;

    else if (spu->ip < 0)
        error_code |= SPU_BAD_IP;

    for (int i = 0; i < N_REGISTERS; i++)
    {
        if (spu->registers[i] < -SPU_VERY_BIG_NUMBER || spu->registers[i] > SPU_VERY_BIG_NUMBER)
        {
            error_code |= SPU_BAD_REGISTERS;
            break;
        }
    }

    return error_code;
}

void spu_dump(processor* spu, unsigned int error_code)
{
    unsigned int verif_code = spu_verif(spu);

    printf("PROCESSOR_DUMP()\n");

    printf("Verification status: ");
    if (verif_code == NOT_ERRORS)
        printf(COLOR_GREEN "OK" COLOR_RESET "\n");
    else
    {
        printf(COLOR_RED "ERRORS:" COLOR_RESET);
        if (verif_code & SPU_BAD_MAIN_PTR)
            printf(" BAD_MAIN_PTR");

        if (verif_code & SPU_BAD_CMD_ARRAY)
            printf(" BAD_CMD_ARRAY");

        if (verif_code & SPU_BAD_CMD_COUNT)
            printf(" BAD_CMD_COUNT");

        if (verif_code & SPU_BAD_IP)
            printf(" BAD_IP");

        if (verif_code & SPU_BAD_REGISTERS)
            printf(" BAD_REGISTERS");

        printf("\n");
    }

    printf("1) Stack:\n");
    stack_dump(&(spu->stk), stack_verif(&spu->stk));

    printf("2) Registers [%p]:\n", spu->registers);
    for (int i = 0; i < 6; i++)
    {
        printf("  %cX: %d (0x%08X)\n", 'A' + i, spu->registers[i], spu->registers[i]);
    }

    printf("3) Command pointer (IP): %d (0x%04X)\n", spu->ip, spu->ip);

    printf("4) Command array [%p]:\n", spu->cmd_array);
    if (spu->cmd_array == NULL)
    {
        printf("  cmd_array pointer == NULL\n");
        return;
    }

    printf("Command memory:\n");

    int print_count = 0;
    printf("[0x0000]    ");

    for (int i = 0; i < spu->cmd_count; i++)
    {
        if (print_count == 8)
        {
            printf("\n");
            print_count = 0;
            printf("[0x%04X]    ", i);
        }

        if (i == spu->ip)
            printf(COLOR_GREEN "0x%04X  " COLOR_RESET, spu->cmd_array[i]);

        else
            printf("0x%04X  ", spu->cmd_array[i]);

        print_count++;
    }
    printf("\n");

    print_error_info((enum spu_error)error_code);
}

enum spu_error do_commands (processor* spu)
{

    SPU_VERIFY

    enum spu_error last_err = NOT_ERRORS;

    int pop_value     = 0;
    int pushreg_value = 0;
    int push_value    = 0;

    while (spu->ip < spu->cmd_count)
    {
        enum cmd_code command = (enum cmd_code)spu->cmd_array[spu->ip++];

        switch (command)
        {
            case HLT:
                return NOT_ERRORS;

            case CMD_OUT:
                last_err = out_cmd(&spu->stk);
                break;

            case PUSH:
                if (spu->ip >= spu->cmd_count)
                {
                    return INCORRECT_N_ARG;
                }
                push_value = spu->cmd_array[spu->ip++];
                last_err = push_cmd(&spu->stk, push_value);
                break;

            case IN_CMD:
                if (spu->ip >= spu->cmd_count)
                {
                    return INCORRECT_N_ARG;
                }
                push_value = spu->cmd_array[spu->ip++];
                last_err = in_cmd(&spu->stk, push_value);
                break;

            case PUSHREG:
                if (spu->ip >= spu->cmd_count)
                {
                    return INCORRECT_N_ARG;
                }
                pushreg_value = spu->cmd_array[spu->ip++];
                last_err = pushreg_cmd(spu, pushreg_value);
                break;

            case POPREG:
                if (spu->ip >= spu->cmd_count)
                {
                    return INCORRECT_N_ARG;
                }
                pop_value = spu->cmd_array[spu->ip++];
                last_err = popreg_cmd(spu, pop_value);
                break;

            case ADD:
                last_err = add_cmd(&spu->stk);
                break;

            case SUB:
                last_err = sub_cmd(&spu->stk);
                break;

            case MUL:
                last_err = mul_cmd(&spu->stk);
                break;

            case DIV:
                last_err = div_cmd(&spu->stk);
                break;

            case POW:
                last_err = pow_cmd(&spu->stk);
                break;

            case SQRT:
                last_err = sqrt_cmd(&spu->stk);
                break;

            case JB:
                last_err = jb_cmd(spu);
                break;

            case JBE:
                last_err = jbe_cmd(spu);
                break;

            case JA:
                last_err = ja_cmd(spu);
                break;

            case JAE:
                last_err = jae_cmd(spu);
                break;

            case JE:
                last_err = je_cmd(spu);
                break;

            case JNE:
                last_err = jne_cmd(spu);
                break;

            case JMP:
                last_err = jmp_cmd(spu);
                break;

            default:
                return INCORRECT_COMMAND;
                break;
        }

        if (last_err != NOT_ERRORS)
        {
            return last_err;
        }
    }

    return NOT_ERRORS;
}

uint16_t* get_commands(const char* filename, int* cmd_count)
{

    FILE* file = fopen(filename, "rb");
    if (!file)
    {
        printf("Reading error\n");
        return NULL;
    }

    uint16_t sign       = 0;
    uint8_t ver         = 0;
    uint16_t n_commands = 0;

    fread(&sign, sizeof(uint16_t), 1, file);
    fread(&ver, sizeof(uint8_t), 1, file);
    fread(&n_commands, sizeof(uint16_t), 1, file);
    *cmd_count = n_commands;

    uint16_t* cmd_mass = (uint16_t*)calloc(n_commands, sizeof(uint16_t));
    if (cmd_mass == NULL)
    {
        free(cmd_mass);
        fclose(file);
        return NULL;
    }

    fread(cmd_mass, sizeof(uint16_t), n_commands, file);

    fclose(file);
    return cmd_mass;
}
