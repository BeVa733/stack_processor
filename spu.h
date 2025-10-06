#ifndef SPU_H
#define SPU_H

struct processor {
    enum cmd_code* cmd_array;
    int cmd_count;
    int ip;
    stack_t stk;
};

enum cmd_code* get_commands (const char* filename, int* cmd_count);
long int check_file_size(FILE* file);

enum user_error do_commands(struct processor* cpu);

enum user_error out_cmd (stack_t* stk);
enum user_error push_cmd(stack_t* stk, int push_value);
enum user_error add_cmd (stack_t* stk);
enum user_error sub_cmd (stack_t* stk);
enum user_error mul_cmd (stack_t* stk);
enum user_error div_cmd (stack_t* stk);
enum user_error pow_cmd (stack_t* stk);
enum user_error sqrt_cmd (stack_t* stk);

void print_error_info(enum user_error last_error);

#endif
