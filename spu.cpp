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

    spu_dtor (&spu);

    return 0;
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

        SPU_VERIFY

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
                push_value = (int16_t)spu->cmd_array[spu->ip++];
                last_err = push_cmd(&spu->stk, push_value);
                break;

            case IN_CMD:
                last_err = in_cmd(&spu->stk);
                break;

            case PUSHREG:
                if (spu->ip >= spu->cmd_count)
                {
                    return INCORRECT_N_ARG;
                }
                pushreg_value = (int16_t)spu->cmd_array[spu->ip++];
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

            case CALL:
                last_err = call_cmd(spu);
                break;

            case BACK:
                last_err = back_cmd(spu);
                break;

            default:
                return INCORRECT_COMMAND;
                break;
        }

        // spu_dump(spu, last_err);            // ДЛЯ ДЕБАГА БЛЯТЬ (НЕ УДАЛЯТЬ)
        // getchar();

        if (last_err != NOT_ERRORS)
        {
            // printf("___________%d____________", last_err);
            spu_dump(spu, last_err);
            return last_err;
        }
    }

    return NOT_ERRORS;
}

int16_t* get_commands(const char* filename, int* cmd_count)
{

    FILE* file = fopen(filename, "rb");
    if (!file)
    {
        printf("Reading error\n");
        return NULL;
    }

    int16_t sign       = 0;
    int8_t ver         = 0;
    int16_t n_commands = 0;

    fread(&sign, sizeof(int16_t), 1, file);
    fread(&ver, sizeof(int8_t), 1, file);
    fread(&n_commands, sizeof(int16_t), 1, file);
    *cmd_count = n_commands;

    int16_t* cmd_mass = (int16_t*)calloc(n_commands, sizeof(int16_t));
    if (cmd_mass == NULL)
    {
        free(cmd_mass);
        fclose(file);
        return NULL;
    }

    fread(cmd_mass, sizeof(int16_t), n_commands, file);

    fclose(file);
    return cmd_mass;
}
