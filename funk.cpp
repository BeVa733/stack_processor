#include <TXLib.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define STACK_TYPE int

#include "stack.h"
#include "calc.h"
#include "spu.h"

enum spu_error out_cmd (stack_t* stk)
{
    if (stk->size > 0)
    {
        stack_type last_value = 0;

        if (stack_pop(stk, &last_value) == SUCCESS)
        {
            printf("OUT: %d\n", last_value);
            return NOT_ERRORS;
        }
        else return INCORRECT_COMMAND;
    }

    else
        return NO_ENOUGH_ELEMENTS;
}

enum spu_error push_cmd (stack_t* stk, int push_value)
{
    if (stack_push(stk, push_value) == SUCCESS)
        return NOT_ERRORS;
    else
        return INCORRECT_COMMAND;
}

enum spu_error in_cmd (stack_t* stk, int push_value)
{
    if (stack_push(stk, push_value) == SUCCESS)
        return NOT_ERRORS;
    else
        return INCORRECT_COMMAND;
}

enum spu_error pushreg_cmd (processor* spu, int value)
{
    if (value < 0 || value >3)
        return INCORRECT_COMMAND;

    int push_value = spu->registers[value];

    if (stack_push(&(spu->stk), push_value) == SUCCESS)
        return NOT_ERRORS;
    else
        return INCORRECT_COMMAND;
}

enum spu_error popreg_cmd (processor* spu, int value)
{
    int pop_value = 0;

    if (stack_pop (&(spu->stk), &pop_value) == SUCCESS)
        {
        spu->registers[value] = pop_value;
        return NOT_ERRORS;
    }
    else
        return NO_ENOUGH_ELEMENTS;
}

enum spu_error add_cmd (stack_t* stk)
{
    stack_type arg_1 = 0;
    stack_type arg_2 = 0;

    if (stk->size > 1)
    {
        stack_pop(stk, &arg_1);
        stack_pop(stk, &arg_2);

        stack_push(stk, arg_1 + arg_2);

        return NOT_ERRORS;
    }
    else
        return NO_ENOUGH_ELEMENTS;

}

enum spu_error sub_cmd (stack_t* stk)
{
    stack_type arg_1 = 0;
    stack_type arg_2 = 0;

    if (stk->size > 1)
    {
        stack_pop(stk, &arg_1);
        stack_pop(stk, &arg_2);

        stack_push(stk, arg_2 - arg_1);

        return NOT_ERRORS;
    }
    else
        return NO_ENOUGH_ELEMENTS;

}

enum spu_error mul_cmd (stack_t* stk)
{
    stack_type arg_1 = 0;
    stack_type arg_2 = 0;

    if (stk->size > 1)
    {
        stack_pop(stk, &arg_1);
        stack_pop(stk, &arg_2);

        stack_push(stk, arg_2*arg_1);

        return NOT_ERRORS;
    }
    else
        return NO_ENOUGH_ELEMENTS;

}

enum spu_error div_cmd (stack_t* stk)
{
    stack_type arg_1 = 0;
    stack_type arg_2 = 0;

    if (stk->size > 1)
    {
        stack_pop(stk, &arg_1);
        stack_pop(stk, &arg_2);

        if (arg_1 == 0)
            return DIV_ON_ZERO;

        stack_push(stk, arg_2/arg_1);

        return NOT_ERRORS;
    }
    else
        return NO_ENOUGH_ELEMENTS;

}

enum spu_error pow_cmd (stack_t* stk)
{
    stack_type arg_1 = 0;
    stack_type arg_2 = 0;

    if (stk->size > 1)
    {
        stack_pop(stk, &arg_1);
        stack_pop(stk, &arg_2);

        if (arg_1 == 0 && arg_2 == 0)
            return ZERO_POW_ZERO;

        stack_push(stk, (stack_type)pow(arg_2, arg_1));

        return NOT_ERRORS;
    }
    else
        return NO_ENOUGH_ELEMENTS;
}

enum spu_error sqrt_cmd (stack_t* stk)
{
    stack_type arg = 0;

    if (stk->size > 0)
    {
        stack_pop(stk, &arg);

        if (arg < 0)
            return IRR_SQRT;

        stack_push(stk, (stack_type)sqrt(arg));

        return NOT_ERRORS;
    }
    else
        return NO_ENOUGH_ELEMENTS;
}

enum spu_error jb_cmd (processor* spu)
{
    int pop_value_1 = 0;
    int pop_value_2 = 0;

    stack_pop (&(spu->stk), &pop_value_1);
    stack_pop (&(spu->stk), &pop_value_2);

    if (pop_value_2 < pop_value_1)
        spu->ip = spu->cmd_array[spu->ip];
    else
        spu->ip++;

    return NOT_ERRORS;
}

enum spu_error jbe_cmd (processor* spu)
{
    int pop_value_1 = 0;
    int pop_value_2 = 0;

    stack_pop (&(spu->stk), &pop_value_1);
    stack_pop (&(spu->stk), &pop_value_2);
    if (pop_value_2 <= pop_value_1)
        spu->ip = spu->cmd_array[spu->ip];
    else
        spu->ip++;

    return NOT_ERRORS;
}

enum spu_error ja_cmd (processor* spu)
{
    int pop_value_1 = 0;
    int pop_value_2 = 0;

    stack_pop (&(spu->stk), &pop_value_1);
    stack_pop (&(spu->stk), &pop_value_2);
    if (pop_value_2 > pop_value_1)
        spu->ip = spu->cmd_array[spu->ip];
    else
        spu->ip++;

    return NOT_ERRORS;
}

enum spu_error jae_cmd (processor* spu)
{
    int pop_value_1 = 0;
    int pop_value_2 = 0;

    stack_pop (&(spu->stk), &pop_value_1);
    stack_pop (&(spu->stk), &pop_value_2);
    if (pop_value_2 >= pop_value_1)
        spu->ip = spu->cmd_array[spu->ip];
    else
        spu->ip++;

    return NOT_ERRORS;
}

enum spu_error je_cmd (processor* spu)
{
    int pop_value_1 = 0;
    int pop_value_2 = 0;

    stack_pop (&(spu->stk), &pop_value_1);
    stack_pop (&(spu->stk), &pop_value_2);
    if (pop_value_2 == pop_value_1)
        spu->ip = spu->cmd_array[spu->ip];
    else
        spu->ip++;

    return NOT_ERRORS;
}

enum spu_error jne_cmd (processor* spu)
{
    int pop_value_1 = 0;
    int pop_value_2 = 0;

    stack_pop (&(spu->stk), &pop_value_1);
    stack_pop (&(spu->stk), &pop_value_2);
    if (pop_value_2 != pop_value_1)
        spu->ip = spu->cmd_array[spu->ip];
    else
        spu->ip++;

    return NOT_ERRORS;
}

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
        default:
            printf("Unknown error\n");
            break;
    }
}
