#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>

#include "calc.h"
#include "assem.h"

cmd_data cmd_code_translate[CMD_COUNT] =
{
    {HLT,     "HLT"},
    {CMD_OUT, "OUT"},
    {ADD,     "ADD"},
    {MUL,     "MUL"},
    {SUB,     "SUB"},
    {DIV,     "DIV"},
    {POW,     "POW"},
    {SQRT,    "SQRT"},
    {IN_CMD,  "IN"},
    {BACK,    "BACK"},
    {DRAW,    "DRAW"},
    {PUSH,    "PUSH"},
    {PUSHREG, "PUSHREG"},
    {POPREG,  "POPREG"},
    {JB,      "JB"},
    {JBE,     "JBE"},
    {JA,      "JA"},
    {JAE,     "JAE"},
    {JE,      "JE"},
    {JNE,     "JNE"},
    {JMP,     "JMP"},
    {CALL,    "CALL"},
    {PUSHM,   "PUSHM"},
    {POPM,    "POPM"},
};

int main(int argc, char *argv[])
{
    comp_info assem_data ={};
    assem_ctor(&assem_data);

    if (argc != 2)
    {
        printf("ERROR: incorrect number of arguments (must be 2)\n");
        return 1;
    }

    assem_data.ptr_mass = read_text(argv[1], &(assem_data.n_lines));
    if (assem_data.ptr_mass == NULL)
        return 1;

    assem_data.n_commands = 0;

    run_compile(&assem_data);

    assem_data.code_mass = (int*)calloc(assem_data.n_commands, sizeof(int));
    if (assem_data.code_mass == NULL)
    {
        assem_dtor(&assem_data);
        return 1;
    }

    assem_data.n_commands = 0;

    if (run_compile(&assem_data) == NULL)
    {
        free(assem_data.code_mass);
        assem_dtor(&assem_data);
        return 1;
    }

    printf("Compilation successful\n");
    text_code_writer(assem_data.code_mass, "output_asm.txt", assem_data.n_commands);
    bin_code_writer(assem_data.code_mass, "output_asm.bin", &(assem_data.n_commands));

    assem_dtor(&assem_data);

    return 0;
}

void assem_ctor(comp_info* assem_data)
{
    assem_data->code_mass  = NULL;
    assem_data->first_pass = true;
    assem_data->ptr_mass   = NULL;

    for (int i = 0; i < N_METOK; i++)
        assem_data->metki_array[i] = -1;
}

void assem_dtor(comp_info* assem_data)
{
    free(assem_data->code_mass);
    free(assem_data->ptr_mass[0]);
    free(assem_data->ptr_mass);
}

int* run_compile(comp_info* assem_data)
{
    enum cmd_code command = HLT;
    int metka = -1;
    const int MAX_LEN = 10;

    for (int i = 0; i < assem_data->n_lines; i++)
    {
        if (assem_data->ptr_mass[i][0] == '\0' || assem_data->ptr_mass[i][0] == '\n')
            continue;

        if (sscanf(assem_data->ptr_mass[i], ":%d", &metka) == 1)
        {
            if (assem_data->first_pass && metka >= 0 && metka < N_METOK)
            {
                assem_data->metki_array[metka] = assem_data->n_commands;
            }
            continue;
        }

        char input[MAX_LEN] = "";
        if (sscanf(assem_data->ptr_mass[i], "%9s", input) != 1)
        {
            printf("incorrect name function in line %d", i);
            return NULL;
        }
        command = get_command(input);

        if (command == INC_FUNC)
        {
            printf("incorrect name of function in line: %d\n", i + 1);
            return NULL;
        }

        if (assem_data->code_mass != NULL)
            assem_data->code_mass[assem_data->n_commands ++] = command;
        else
            assem_data->n_commands ++;

        if (compile_arg(assem_data, i, command) == NULL)
        {
            printf("Error in argument compilation at line %d\n", i + 1);
            return NULL;
        }
    }

    return assem_data->code_mass;
}

int* compile_arg (comp_info* assem_data, int i, enum cmd_code command)
{
    char reg_lit = 'a';
    int push_arg = 0;
    int jump_arg = 0;

    if (command == PUSH)
    {
        if (sscanf(assem_data->ptr_mass[i], "PUSH %d", &push_arg) != 1)
        {
            printf("Expected number after PUSH in line %d\n", i + 1);
            return NULL;
        }

        if (assem_data->code_mass != NULL)
            assem_data->code_mass[assem_data->n_commands ++] = push_arg;
        else
            assem_data->n_commands ++;
    }
    else if (command == PUSHREG || command == POPREG)
    {
        if (sscanf(assem_data->ptr_mass[i], "%*s %cX", &reg_lit) != 1)
        {
            printf("Expected register after command in line %d\n", i + 1);
            return NULL;
        }

        if (reg_lit - 'A' >= N_REGISTERS)
        {
            printf("Too much registers used (max: %d)\n", N_REGISTERS);
            return NULL;
        }

        if (assem_data->code_mass != NULL)
            assem_data->code_mass[assem_data->n_commands ++] = reg_lit - 'A';
        else
            assem_data->n_commands ++;
    }

    else if (command >= JB && command <= CALL)
    {
        if (sscanf(assem_data->ptr_mass[i], "%*s :%d", &jump_arg) == 1)
        {
            if (jump_arg >= 0 && jump_arg < N_METOK && assem_data->metki_array[jump_arg] != -1)
            {
                if (assem_data->code_mass != NULL)
                    assem_data->code_mass[assem_data->n_commands ++] = assem_data->metki_array[jump_arg];
                else
                    assem_data->n_commands ++;
            }

            else if (!assem_data->first_pass)
            {
                printf("Unknown metka :%d in line %d\n", jump_arg, i + 1);
                return NULL;
            }

            else
            {
                assem_data->n_commands ++;
            }
        }

        else if (sscanf(assem_data->ptr_mass[i], "%*s %d", &jump_arg) == 1)
        {
            if (assem_data->code_mass != NULL)
                assem_data->code_mass[assem_data->n_commands ++] = jump_arg;

            else
                assem_data->n_commands ++;
        }

        else
        {
            printf("Expected number or metka after JUMP in line %d\n", i + 1);
            return NULL;
        }
    }

    else if (command == PUSHM || command == POPM)
    {
        if (sscanf(assem_data->ptr_mass[i], "%*s [%cX]", &reg_lit) != 1)
        {
            printf("Expected register after command in line %d\n", i + 1);
            return NULL;
        }

        if (reg_lit - 'A' >= N_REGISTERS)
        {
            printf("Too much registers used (max: %d)\n", N_REGISTERS);
            return NULL;
        }

        if (assem_data->code_mass != NULL)
            assem_data->code_mass[assem_data->n_commands ++] = reg_lit - 'A';

        else
            assem_data->n_commands ++;
    }

    return (assem_data->code_mass == NULL) ? &(assem_data->n_commands) : assem_data->code_mass;
}

enum cmd_code get_command(char* input)
{
    for (int i = 0; i < CMD_COUNT; i++)
    {
        if (strcmp (input, cmd_code_translate[i].str_code) == 0)
            return cmd_code_translate[i].enum_code;
    }

    return INC_FUNC;
}

void text_code_writer(int* code_mass, const char* filename, int n_commands)
{
    assert(code_mass != NULL);

    FILE* file = fopen(filename, "w");

    for (int i = 0; i < n_commands; i++)
    {
        fprintf(file, "%4d [%2d] - %s\n", i, code_mass[i], cmd_code_translate[code_mass[i]].str_code);

        if (code_mass[i] > 9)
            i++;
    }

    fclose(file);
}

void bin_code_writer(int* code_mass, const char* filename, int* n_commands)
{
    assert(code_mass != NULL);

    FILE* file = fopen(filename, "wb");
    if(!file)
    {
        printf("error opening file\n");
        return;
    }

    int16_t signature = (int16_t)SIGNATURAA;
    int8_t version = (int8_t)VERSION;
    int16_t command_count = (int16_t)(*n_commands);

    fwrite(&signature, sizeof(signature), 1, file);
    fwrite(&version, sizeof(version), 1, file);
    fwrite(&command_count, sizeof(command_count), 1, file);

    for (int i = 0; i < *n_commands; i++)
    {
        int16_t cmd = (int16_t)code_mass[i];
        fwrite(&cmd, sizeof(cmd), 1, file);
    }

    fclose(file);
}

void onegin_dtor(char** ptr_mass)
{
    free(ptr_mass[0]);
    free(ptr_mass);
}
