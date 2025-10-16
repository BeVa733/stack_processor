#ifndef SPU_H
#define SPU_H

struct processor
{
    int16_t* cmd_array;
    int registers[N_REGISTERS];
    int cmd_count;
    int ip;
    stack_t stk;
    stack_t stk_back;
};

#define SPU_VERIFY                                     \
    if(!IS_WAS_DUMP)                                   \
    {                                                  \
        unsigned int spu_verif_code = spu_verif(spu);  \
        if(spu_verif_code != NOT_ERRORS)               \
        {                                              \
            spu_dump(spu, spu_verif_code);             \
            IS_WAS_DUMP = true;                        \
            return CREATION_ERROR;                     \
        }                                              \
    }


enum spu_error spu_ctor (processor* spu);
void spu_dtor (processor* spu);
void spu_dump(processor* spu, unsigned int error_code);

int16_t* get_commands (const char* filename, int* cmd_count);
enum spu_error do_commands(struct processor* cpu);

enum spu_error out_cmd (stack_t* stk);
enum spu_error push_cmd (stack_t* stk, int push_value);
enum spu_error in_cmd (stack_t* stk);
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
enum spu_error jmp_cmd (processor* spu);

enum spu_error back_cmd (processor* spu);
enum spu_error call_cmd (processor* spu);

void print_error_info (enum spu_error last_error);
unsigned int spu_verif(processor* spu);

#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_RESET   "\033[0m"

const ssize_t SPU_VERY_BIG_NUMBER = 1000000000;

#endif
