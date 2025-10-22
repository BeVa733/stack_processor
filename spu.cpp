#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

#define PROCESSOR
#define SPU
#define STACK_TYPE int

#include "stack.h"
#include "calc.h"
#include "spu.h"

void stk_printf (unsigned int index, stack_type value)
{
    printf("[%u] = %d; ", index, value);
}

// spu_cmd_data cmd_code_translate[CMD_COUNT] =
// {
//     {HLT,     "HLT",        0       },
//     {CMD_OUT, "OUT",     out_cmd    },
//     {ADD,     "ADD",     add_cmd    },
//     {SUB,     "SUB",     sub_cmd    },
//     {MUL,     "MUL",     mul_cmd    },
//     {DIV,     "DIV",     div_cmd    },
//     {POW,     "POW",     pow_cmd    },
//     {SQRT,    "SQRT",    sqrt_cmd   },
//     {IN_CMD,  "IN",      in_cmd     },
//     {BACK,    "BACK",    back_cmd   },
//     {DRAW,    "DRAW",    draw_cmd   },
//     {PUSH,    "PUSH",    push_cmd   },
//     {PUSHREG, "PUSHREG", pushreg_cmd},
//     {POPREG,  "POPREG",  popreg_cmd },
//     {JB,      "JB",      jb_cmd     },
//     {JBE,     "JBE",     jbe_cmd    },
//     {JA,      "JA",      ja_cmd     },
//     {JAE,     "JAE",     jae_cmd    },
//     {JE,      "JE",      je_cmd     },
//     {JNE,     "JNE",     jne_cmd    },
//     {JMP,     "JMP",     jmp_cmd    },
//     {CALL,    "CALL",    call_cmd   },
//     {PUSHM,   "PUSHM",   pushm_cmd  },
//     {POPM,    "POPM",    popm_cmd   },
// };

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

    while (spu->ip < spu->cmd_count)
    {
        SPU_VERIFY

        enum cmd_code command = (enum cmd_code)spu->cmd_array[spu->ip++];

        if (command == HLT)
            break;

        last_err = cmd_code_translate[command].funk(spu);

        //spu_dump(spu, last_err);            //ДЛЯ ДЕБАГА БЛЯТЬ (НЕ УДАЛЯТЬ)
        //getchar();

        if (last_err != NOT_ERRORS)
        {
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
