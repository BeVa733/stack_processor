#ifndef CALC_H
#define CALC_H

const int SIGNATURAA  = 0xBEBA;
const int VERSION     = 4;

const int N_REGISTERS = 6;
const int N_METOK     = 100;
const int N_RAM       = 10000;

const int CMD_COUNT   = 26;

enum cmd_code
{
    HLT      = 0,
    CMD_OUT  = 1,
    OUTC     = 2,
    ADD      = 3,
    SUB      = 4,
    MUL      = 5,
    DIV      = 6,
    POW      = 7,
    SQRT     = 8,
    IN_CMD   = 9,
    BACK     = 10,
    DRAW     = 11,
    WDRAW    = 12,
    PUSH     = 13,
    PUSHREG  = 14,
    POPREG   = 15,
    JB       = 16,
    JBE      = 17,
    JA       = 18,
    JAE      = 19,
    JE       = 20,
    JNE      = 21,
    JMP      = 22,
    CALL     = 23,
    PUSHM    = 24,
    POPM     = 25,
    INC_FUNC = 26
};

#endif
