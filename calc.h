#ifndef CALC_H
#define CALC_H

const int SIGNATURAA = 0xBEBA;
const int VERSION    = 4;

const int N_REGISTERS = 6;
const int N_METOK     = 100;
const int N_RAM       = 10000;

const int CMD_COUNT = 25;

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
    WDRAW    = 11,
    PUSH     = 12,
    PUSHREG  = 13,
    POPREG   = 14,
    JB       = 15,
    JBE      = 16,
    JA       = 17,
    JAE      = 18,
    JE       = 19,
    JNE      = 20,
    JMP      = 21,
    CALL     = 22,
    PUSHM    = 23,
    POPM     = 24,
    INC_FUNC = 25
};

#endif
