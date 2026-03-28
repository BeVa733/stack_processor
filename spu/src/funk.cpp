#ifdef _WIN32
#include <TXLib.h>
#endif

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

#define STACK_TYPE int
#define SPU

#include "stack.h"
#include "calc.h"
#include "spu.h"

enum spu_error out_cmd (processor* spu)
{
    if (spu->stk.size > 0)
    {
        int last_value = 0;

        if (stack_pop(&(spu->stk), &last_value) == SUCCESS)
        {
            printf("%d", last_value);
            return NOT_ERRORS;
        }
        else return INCORRECT_COMMAND;
    }
    else
        return NO_ENOUGH_ELEMENTS;
}

enum spu_error outc_cmd (processor* spu)
{
    if (spu->stk.size > 0)
    {
        int last_value = 0;

        if (stack_pop(&(spu->stk), &last_value) == SUCCESS)
        {
            printf("%c", last_value);
            return NOT_ERRORS;
        }
        else return INCORRECT_COMMAND;
    }
    else
        return NO_ENOUGH_ELEMENTS;
}

enum spu_error push_cmd (processor* spu)
{
    if (spu->ip >= spu->cmd_count)
        return INCORRECT_N_ARG;

    int push_value = spu->cmd_array[spu->ip++];

    if (stack_push(&(spu->stk), push_value) == SUCCESS)
        return NOT_ERRORS;

    else
        return INCORRECT_COMMAND;
}

enum spu_error in_cmd (processor* spu)
{
    int push_value = -333;

    if(scanf("%d", &push_value) != 1)
        return INCORRECT_COMMAND;

    if (stack_push(&(spu->stk), push_value) == SUCCESS)
        return NOT_ERRORS;
    else
        return INCORRECT_COMMAND;
}

enum spu_error pushreg_cmd (processor* spu)
{
    int pushreg_value = spu->cmd_array[spu->ip++];

    if (pushreg_value < 0 || pushreg_value >= N_REGISTERS)
        return INCORRECT_COMMAND;

    int push_value = spu->registers[pushreg_value];

    if (stack_push(&(spu->stk), push_value) == SUCCESS)
        return NOT_ERRORS;

    else
        return INCORRECT_COMMAND;
}

enum spu_error popreg_cmd (processor* spu)
{
    int pop_value = 0;

    int popreg_value = spu->cmd_array[spu->ip++];

    if (stack_pop (&(spu->stk), &pop_value) == SUCCESS)
    {
        spu->registers[popreg_value] = pop_value;
        return NOT_ERRORS;
    }

    else
        return NO_ENOUGH_ELEMENTS;
}

#define GEN_FUNC(name_func, sign)                  \
enum spu_error name_func (processor* spu)          \
{                                                  \
    stack_type arg_1 = 0;                          \
    stack_type arg_2 = 0;                          \
                                                   \
    if (spu->stk.size > 1)                         \
    {                                              \
        stack_pop(&(spu->stk), &arg_1);            \
        stack_pop(&(spu->stk), &arg_2);            \
                                                   \
        stack_push(&(spu->stk), arg_2 sign arg_1); \
                                                   \
        return NOT_ERRORS;                         \
    }                                              \
    else                                           \
        return NO_ENOUGH_ELEMENTS;                 \
                                                   \
}                                                  \

GEN_FUNC (add_cmd, +)
GEN_FUNC (sub_cmd, -)
GEN_FUNC (mul_cmd, *)

#undef GEN_FUNC

enum spu_error div_cmd (processor* spu)
{
    stack_type arg_1 = 0;
    stack_type arg_2 = 0;

    if (spu->stk.size > 1)
    {
        stack_pop(&(spu->stk), &arg_1);
        stack_pop(&(spu->stk), &arg_2);

        if (arg_1 == 0)
            return DIV_ON_ZERO;

        stack_push(&(spu->stk), arg_2/arg_1);

        return NOT_ERRORS;
    }
    else
        return NO_ENOUGH_ELEMENTS;

}

enum spu_error pow_cmd (processor* spu)
{
    stack_type arg_1    = 0;
    stack_type arg_2    = 0;
    stack_type push_arg = 0;

    if (spu->stk.size > 1)
    {
        stack_pop(&(spu->stk), &arg_1);
        stack_pop(&(spu->stk), &arg_2);

        if (arg_1 == 0 && arg_2 == 0)
            return ZERO_POW_ZERO;

        push_arg = (stack_type)round(pow(arg_2, arg_1));

        stack_push(&(spu->stk), push_arg);

        return NOT_ERRORS;
    }
    else
        return NO_ENOUGH_ELEMENTS;
}

enum spu_error sqrt_cmd (processor* spu)
{
    stack_type arg = 0;

    if (spu->stk.size > 0)
    {
        stack_pop(&(spu->stk), &arg);

        if (arg < 0)
            return IRR_SQRT;

        stack_push(&(spu->stk), (stack_type)round(sqrt(arg)));

        return NOT_ERRORS;
    }
    else
        return NO_ENOUGH_ELEMENTS;
}


#define GEN_JUMP(name_funk, comp_sign)      \
enum spu_error name_funk (processor* spu)   \
{                                           \
                                            \
    if (spu->stk.size < 2)                  \
        return NO_ENOUGH_ELEMENTS;          \
                                            \
    int pop_value_1 = 0;                    \
    int pop_value_2 = 0;                    \
                                            \
    stack_pop (&(spu->stk), &pop_value_1);  \
    stack_pop (&(spu->stk), &pop_value_2);  \
                                            \
    if (pop_value_2 comp_sign pop_value_1)  \
        spu->ip = spu->cmd_array[spu->ip];  \
    else                                    \
        spu->ip++;                          \
                                            \
    return NOT_ERRORS;                      \
}

GEN_JUMP(jb_cmd,  < )
GEN_JUMP(jbe_cmd, <=)
GEN_JUMP(ja_cmd,  > )
GEN_JUMP(jae_cmd, >=)
GEN_JUMP(je_cmd,  ==)
GEN_JUMP(jne_cmd, !=)

#undef GEN_JUMP

enum spu_error jmp_cmd (processor* spu)
{
    spu->ip = spu->cmd_array[spu->ip];

    return NOT_ERRORS;
}

enum spu_error call_cmd (processor* spu)
{
    stack_push(&(spu->stk_back), spu->ip + 1);

    spu->ip = spu->cmd_array[spu->ip];

    return NOT_ERRORS;
}

enum spu_error back_cmd (processor* spu)
{
    int jump_value = 0;
    if (stack_pop(&(spu->stk_back), &jump_value) == FAILURE)
        return NO_ENOUGH_ELEMENTS;

    spu->ip = jump_value;

    return NOT_ERRORS;
}

enum spu_error pushm_cmd (processor* spu)
{
    int reg_index = spu->cmd_array[spu->ip++];

    if (spu->registers[reg_index] > N_RAM)
        return INCORRECT_COMMAND;

    int push_value = spu->ram[spu->registers[reg_index]];

    stack_push(&(spu->stk), push_value);

    return NOT_ERRORS;
}

enum spu_error popm_cmd (processor* spu)
{
    int reg_index = spu->cmd_array[spu->ip++];

    if (spu->registers[reg_index] > N_RAM)
        return INCORRECT_COMMAND;

    int pop_value = 0;

    if (stack_pop(&(spu->stk), &pop_value) == FAILURE)
        return NO_ENOUGH_ELEMENTS;

    spu->ram[spu->registers[reg_index]] = pop_value;

    return NOT_ERRORS;
}

enum spu_error draw_cmd (processor* spu)
{
    for (int i = 0; i < N_RAM; i++)
    {
        printf("%c", spu->ram[i]);

        if ((i + 1) % 100 == 0)
            printf("\n");
    }

    return NOT_ERRORS;
}

#ifdef _WIN32
enum spu_error window_draw_cmd (processor* spu)
{
    const int WINDOW_SIZE = (int)sqrt(N_RAM);
    const int WINDOW_SCALE = 10;

    int x = 0;
    int y = 0;
    int n_line = 0;

    txCreateWindow (WINDOW_SCALE * WINDOW_SIZE, WINDOW_SCALE * WINDOW_SIZE);

    for (int i = 0; i < N_RAM; i++)
        {
            x = i % WINDOW_SIZE;
            y = n_line;

        if (spu->ram[i] != BASIS_DRAW_SYMBOL)
            txCircle (x * WINDOW_SCALE, y * WINDOW_SCALE, WINDOW_SCALE / 2);

        if ((i + 1) % WINDOW_SIZE == 0)
            n_line++;
        }

    txEnd();
 
    return NOT_ERRORS;
}
#endif

void print_error_info(enum spu_error last_error)
{
    switch (last_error)
    {
        case NO_ENOUGH_ELEMENTS:
            printf("Error: not enough elements in stack\n");
            break;
        case DIV_ON_ZERO:
            printf("Error: division by zero\n");
            break;
        case ZERO_POW_ZERO:
            printf("Error: zero to the power of zero is undefined\n");
            break;
        case INCORRECT_N_ARG:
            printf("Error: incorrect number of arguments\n");
            break;
        case IRR_SQRT:
            printf("Error: square root of negative number\n");
            break;
        case INCORRECT_COMMAND:
            printf("Error: incorrect command\n");
            break;
        case NOT_ERRORS:
            printf("No error\n");
            break;
        case CREATION_ERROR:
            printf("reding error\n");
            break;
        default:
            printf("Unknown error\n");
            break;
    }
}
