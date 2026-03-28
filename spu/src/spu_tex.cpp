#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define SPU
#define STACK_TYPE int
#include "stack.h"
#include "calc.h"
#include "spu.h"

// extern spu_cmd_data cmd_code_translate[CMD_COUNT];

enum spu_error spu_ctor (processor* spu)
{
    spu->cmd_array = get_commands("output_asm.bin", &(spu->cmd_count));

    for (int i = 0; i < N_RAM; i++)
    {
        spu->ram[i] = BASIS_DRAW_SYMBOL;
    }

    stack_ctor(&(spu->stk), 10);
    stack_ctor(&(spu->stk_back), 10);

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
    stack_dtor(&(spu->stk_back));
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

    for (int i = 0; i < CMD_COUNT - 1; i++)
    {
        if ( i != cmd_code_translate[i].enum_code)
            error_code |= INCORRECT_CMD_QUEUE;
    }

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

        if (verif_code & INCORRECT_CMD_QUEUE)
            printf( "INVALID NUMERATION OF COMMANDS");

        printf("\n");
    }

    printf("1) Stack:\n");
    stack_dump(&(spu->stk), stack_verif(&spu->stk));

    printf("2) Registers [%p]:\n", spu->registers);
    for (int i = 0; i < N_REGISTERS; i++)
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

    printf("5) RAM [%p]:\n", spu->ram);
    printf("[");
    for (int i = 0; i < N_RAM; i++)
        printf("%d, ", spu->ram[i]);
    printf("]\n");

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
            printf(COLOR_RED "0x%08X  " COLOR_RESET, spu->cmd_array[i]);

        else
            printf("0x%08X  ", spu->cmd_array[i]);

        print_count++;
    }
    printf("\n");

    print_error_info((enum spu_error)error_code);
}
