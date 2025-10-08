#ifndef SPU_H
#define SPU_H

struct processor
{
    uint16_t* cmd_array;
    int registers[4];
    int cmd_count;
    int ip;
    stack_t stk;
};

enum spu_error spu_ctor (processor* spu);
void spu_dtor (processor* spu);
void spu_dump(processor* spu, enum spu_error last_error);

uint16_t* get_commands (const char* filename, int* cmd_count);
enum spu_error do_commands(struct processor* cpu);

enum spu_error out_cmd (stack_t* stk);
enum spu_error push_cmd (stack_t* stk, int push_value);
enum spu_error in_cmd (stack_t* stk, int push_value);
enum spu_error pushreg_cmd (processor* spu, int value);
enum spu_error popreg_cmd (processor* spu, int value);
enum spu_error add_cmd (stack_t* stk);
enum spu_error sub_cmd (stack_t* stk);
enum spu_error mul_cmd (stack_t* stk);
enum spu_error div_cmd (stack_t* stk);
enum spu_error pow_cmd (stack_t* stk);
enum spu_error sqrt_cmd (stack_t* stk);
enum spu_error jb_cmd (processor* spu);
enum spu_error jbe_cmd (processor* spu);
enum spu_error ja_cmd (processor* spu);
enum spu_error jae_cmd (processor* spu);
enum spu_error je_cmd (processor* spu);
enum spu_error jne_cmd (processor* spu);

void print_error_info (enum spu_error last_error);

#endif
