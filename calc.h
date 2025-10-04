#ifndef CALC_H
#define CALC_H

enum cmd_code
{
    INC_FUNC = -1,
    HLT      = 0,
    CMD_OUT  = 1,
    PUSH     = 2,
    ADD      = 3,
    SUB      = 4,
    MUL      = 5,
    DIV      = 6,
    POW      = 7,
    SQRT     = 8
};

enum user_error
{
    NOT_ERRORS          = 0,
    NO_ENOUGH_ELEMENTS = 1,
    DIV_ON_ZERO        = 2,
    ZERO_POW_ZERO      = 3,
    INCORRECT_N_ARG    = 4,
    IRR_SQRT           = 5,
    INCORRECT_COMMAND  = 6,
};

struct processor {
    enum cmd_code* cmd_array;    // Массив команд
    int cmd_count;               // Количество команд
    int ip;                      // Инструкционный указатель
    stack_t stk;                 // Стек
};

enum user_error do_commands(struct processor* cpu);
void print_error_info(enum user_error last_error);

enum cmd_code* get_commands (const char* filename, int* cmd_count);

enum user_error out_cmd (stack_t* stk);
enum user_error push_cmd(stack_t* stk, int push_value);
enum user_error add_cmd (stack_t* stk);
enum user_error sub_cmd (stack_t* stk);
enum user_error mul_cmd (stack_t* stk);
enum user_error div_cmd (stack_t* stk);
enum user_error pow_cmd (stack_t* stk);
enum user_error sqrt_cmd (stack_t* stk);

long int check_file_size(FILE* file);

#endif
