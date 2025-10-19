#ifndef SPU_H
#define SPU_H

struct processor
{
    int16_t* cmd_array;
    int registers[N_REGISTERS];
    int ram[N_RAM];
    int cmd_count;
    int ip;
    stack_t stk;
    stack_t stk_back;
};

enum spu_error
{
    NOT_ERRORS          = 0,
    NO_ENOUGH_ELEMENTS  = 1,
    DIV_ON_ZERO         = 2,
    ZERO_POW_ZERO       = 3,
    INCORRECT_N_ARG     = 4,
    IRR_SQRT            = 5,
    INCORRECT_COMMAND   = 6,
    CREATION_ERROR      = 7
};

enum spu_verif_error
{
    SPU_NO_ERRORS        = 0b000000,
    SPU_BAD_MAIN_PTR     = 0b100000,
    SPU_BAD_CMD_ARRAY    = 0b010000,
    SPU_BAD_CMD_COUNT    = 0b001000,
    SPU_BAD_IP           = 0b000100,
    SPU_BAD_REGISTERS    = 0b000010,
    INCORRECT_CMD_QUEUE  = 0b000001
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
enum spu_error do_commands(processor* spu);

enum spu_error out_cmd     (processor* spu);
enum spu_error push_cmd    (processor* spu);
enum spu_error in_cmd      (processor* spu);
enum spu_error pushreg_cmd (processor* spu);
enum spu_error popreg_cmd  (processor* spu);

enum spu_error add_cmd  (processor* spu);
enum spu_error sub_cmd  (processor* spu);
enum spu_error mul_cmd  (processor* spu);
enum spu_error div_cmd  (processor* spu);
enum spu_error pow_cmd  (processor* spu);
enum spu_error sqrt_cmd (processor* spu);

enum spu_error jb_cmd  (processor* spu);
enum spu_error jbe_cmd (processor* spu);
enum spu_error ja_cmd  (processor* spu);
enum spu_error jae_cmd (processor* spu);
enum spu_error je_cmd  (processor* spu);
enum spu_error jne_cmd (processor* spu);
enum spu_error jmp_cmd (processor* spu);

enum spu_error back_cmd (processor* spu);
enum spu_error call_cmd (processor* spu);

enum spu_error popm_cmd  (processor* spu);
enum spu_error pushm_cmd (processor* spu);

enum spu_error draw_cmd (processor* spu);

void print_error_info (enum spu_error last_error);
unsigned int spu_verif(processor* spu);

#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_RESET   "\033[0m"

const ssize_t SPU_VERY_BIG_NUMBER = 1000000000;

struct spu_cmd_data
{
    enum cmd_code enum_code;
    const char* str_code;
    enum spu_error (*funk)(processor* spu);
};

#endif
