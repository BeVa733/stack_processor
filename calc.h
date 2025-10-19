#ifndef CALC_H
#define CALC_H

const int SIGNATURAA = 0xBEBA;
const int VERSION    = 4;

const int N_REGISTERS = 6;
const int N_METOK     = 100;
const int N_RAM       = 10000;

const int BASIS_DRAW_SYMBOL = 42;

const int CMD_COUNT = 24;

enum cmd_code
{
    HLT      = 0,
    CMD_OUT  = 1,
    ADD      = 2,
    SUB      = 3,
    MUL      = 4,
    DIV      = 5,
    POW      = 6,
    SQRT     = 7,
    IN_CMD   = 8,
    BACK     = 9,
    DRAW     = 10,
    PUSH     = 11,
    PUSHREG  = 12,
    POPREG   = 13,
    JB       = 14,
    JBE      = 15,
    JA       = 16,
    JAE      = 17,
    JE       = 18,
    JNE      = 19,
    JMP      = 20,
    CALL     = 21,
    PUSHM    = 22,
    POPM     = 23,
    INC_FUNC = 24
};

#endif
