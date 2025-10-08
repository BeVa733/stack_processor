#ifndef CALC_H
#define CALC_H

const int SIGNATURAA = 0xBEBA;
const int VERSION    = 3;

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
    SQRT     = 8,
    PUSHREG  = 9,
    POPREG   = 10,
    JB       = 11,
    JBE      = 12,
    JA       = 13,
    JAE      = 14,
    JE       = 15,
    JNE      = 16,
    IN_CMD   = 17
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

#endif
