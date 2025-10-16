#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>

#include "calc.h"
#include "assem.h"

const int N_METOK = 100;

int main(int argc, char *argv[])
{
    int n_commands = 0;
    int* code_mass = NULL;
    int metki_array[100] = {};
    memset(metki_array, -1, sizeof(metki_array));
    if (argc != 2)
    {
        printf("ERROR: incorrect number of arguments (must be 2)\n");
        return 1;
    }

    code_mass = file_code_compile(&n_commands, argv[1], metki_array);
    if (code_mass == NULL)
    {
        return 1;
    }

    ON_DEBUG (code_writer(code_mass, "output_asm.txt", n_commands);)
    ON_DEBUG_ELSE (bin_code_writer(code_mass, "output_asm.bin", &n_commands);)

    free(code_mass);
    return 0;
}

enum cmd_code get_file_command(char* str)
{
    const int MAX_LEN = 10;
    char input[MAX_LEN] = "";

    if (sscanf(str, "%9s", input) != 1)
        return INC_FUNC;

    if      (strcmp(input, "HLT")  == 0)    return HLT;
    else if (strcmp(input, "OUT")  == 0)    return CMD_OUT;
    else if (strcmp(input, "PUSH") == 0)    return PUSH;
    else if (strcmp(input, "ADD")  == 0)    return ADD;
    else if (strcmp(input, "SUB")  == 0)    return SUB;
    else if (strcmp(input, "MUL")  == 0)    return MUL;
    else if (strcmp(input, "DIV")  == 0)    return DIV;
    else if (strcmp(input, "POW")  == 0)    return POW;
    else if (strcmp(input, "SQRT") == 0)    return SQRT;
    else if (strcmp(input, "PUSHREG") == 0) return PUSHREG;
    else if (strcmp(input, "POPREG") == 0)  return POPREG;
    else if (strcmp(input, "JB") == 0)      return JB;
    else if (strcmp(input, "JBE") == 0)     return JBE;
    else if (strcmp(input, "JA") == 0)      return JA;
    else if (strcmp(input, "JAE") == 0)     return JAE;
    else if (strcmp(input, "JE") == 0)      return JE;
    else if (strcmp(input, "JNE") == 0)     return JNE;
    else if (strcmp(input, "IN") == 0)      return IN_CMD;
    else if (strcmp(input, "JMP") == 0)     return JMP;
    else if (strcmp(input, "CALL") == 0)    return CALL;
    else if (strcmp(input, "BACK") == 0)    return BACK;
    else return INC_FUNC;
}

void code_writer(int* code_mass, const char* filename, int n_commands)
{
    assert(code_mass != NULL);
    FILE* file = fopen(filename, "w");

    for (int i = 0; i < n_commands; i++)
    {
        fprintf(file, "%d ", code_mass[i]);
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

int* file_code_compile(int* n_commands, const char* filename, int* metki_array)
{
    int n_lines = 0;

    char** ptr_mass = read_text(filename, &n_lines);
    if (ptr_mass == NULL)
        return NULL;

    int total_commands = first_run_compile(ptr_mass, metki_array, n_lines);
    if (total_commands == -1)
        return NULL;

    int* code_mass = (int*)calloc(total_commands, sizeof(int));
    if (code_mass == NULL)
    {
        free(ptr_mass[0]);
        free(ptr_mass);
        return NULL;
    }

    return second_run_compile(ptr_mass, code_mass, metki_array, n_commands, n_lines);
}

int first_run_compile(char** ptr_mass, int* metki_array, int n_lines)
{
    enum cmd_code command = HLT;
    int total_commands = 0;
    int metka = -1;

    for (int i = 0; i < n_lines; i++)
    {
        if (ptr_mass[i][0] == '\0' || ptr_mass[i][0] == '\n')
            continue;

        if (sscanf(ptr_mass[i], ":%d", &metka) == 1)
        {
            metki_array[metka] = total_commands;
            continue;
        }

        command = get_file_command(ptr_mass[i]);
        if (command == INC_FUNC)
        {
            printf("incorrect name of function in line: %d\n", i + 1);
            free(ptr_mass[0]);
            free(ptr_mass);
            return -1;
        }

        total_commands++;

        if (command == PUSH)
        {
            total_commands++;
        }
        else if (command == PUSHREG || command == POPREG)
        {
            total_commands++;
        }
        else if (command == JB || command == JBE || command == JA  ||
                command == JAE || command == JE  || command == JNE ||
                command == JMP || command == CALL)
        {
            total_commands++;
        }
    }

    return total_commands;
}

#define VSE_NAXYI           free(code_mass);\
                            free(ptr_mass[0]);\
                            free(ptr_mass);

int* second_run_compile (char** ptr_mass, int* code_mass, int* metki_array, int* n_commands, int n_lines)
{
    enum cmd_code command = HLT;
    int push_arg = 0;
    int jump_arg = 0;
    char reg_lit = 'a';
    int metka = -1;

    *n_commands = 0;

    for (int i = 0; i < n_lines; i++)
    {
        if (ptr_mass[i][0] == '\0' || ptr_mass[i][0] == '\n')
            continue;

        if (sscanf(ptr_mass[i], ":%d", &metka) == 1)
        {
            continue;
        }

        command = get_file_command(ptr_mass[i]);

        if (command == INC_FUNC)
        {
            printf("incorrect name of function in line: %d\n", i + 1);
            VSE_NAXYI
            return NULL;
        }

        code_mass[(*n_commands)++] = command;

        if (command == PUSH)
        {
            if (sscanf(ptr_mass[i], "PUSH %d", &push_arg) != 1)
            {
                printf("Expected number after PUSH in line %d\n", i + 1);
                VSE_NAXYI
                return NULL;
            }

            code_mass[(*n_commands)++] = push_arg;
        }

        else if (command == PUSHREG || command == POPREG)
        {
            if (sscanf(ptr_mass[i], "%*s %cX", &reg_lit) != 1)
            {
                printf("Expected register after command in line %d\n", i + 1);
                VSE_NAXYI
                return NULL;
            }

            if (reg_lit - 'A' > N_REGISTERS)
            {
                printf("Too much registers used (max: %d)\n", N_REGISTERS);
                VSE_NAXYI
                return NULL;
            }

            code_mass[(*n_commands)++] = reg_lit - 'A';
        }

        else if (command == JB || command == JBE || command == JA  ||
                command == JAE || command == JE  || command == JNE ||
                command == JMP || command == CALL)
        {
            if (sscanf(ptr_mass[i], "%*s :%d", &jump_arg) == 1)
            {
                if (jump_arg >= 0 && jump_arg < N_METOK && metki_array[jump_arg] != -1)
                {
                    code_mass[(*n_commands)++] = metki_array[jump_arg];
                }

                else
                {
                    printf("Unknown metka :%d in line %d\n", jump_arg, i + 1);
                    VSE_NAXYI
                    return NULL;
                }
            }

            else if (sscanf(ptr_mass[i], "%*s %d", &jump_arg) == 1)
            {
                code_mass[(*n_commands)++] = jump_arg;
            }

            else
            {
                printf("Expected number or metka after JUMP in line %d\n", i + 1);
                VSE_NAXYI
                return NULL;
            }
        }
    }

    free(ptr_mass[0]);
    free(ptr_mass);

    return code_mass;

}

#undef VSE_NAXYI

char** read_text(const char* filename, int* num_lines)
{
    FILE* file = fopen(filename, "r");
    if (!file)
    {
        printf("Reading error\n");
        return 0;
    }

    long int file_size = chek_file_size(file);

    char* buffer = (char*)calloc(file_size + 1, sizeof(char));
    if (!buffer)
    {
        fclose(file);
        return NULL;
    }

    size_t read_size = fread(buffer, sizeof(char), file_size, file);

    fclose(file);

    buffer[read_size] = '\0';

    *num_lines = check_n_lines(buffer, read_size);

    return make_ptr_massive(buffer, *num_lines, read_size);

}

char** make_ptr_massive(char* buffer, int num_lines, size_t read_size)
{
    assert(buffer != NULL);

    char** lines = (char**)calloc(num_lines, sizeof(char*));
    if (!lines)
    {
        free(buffer);
        return NULL;
    }

    int line_index = 0;
    char* start_str = buffer;

    for (size_t i = 0; i < read_size; i++)
    {
        if (buffer[i] == '\n')
        {
            buffer[i] = '\0';
            lines[line_index++] = start_str;
            start_str = &buffer[i+1];
        }
    }

    return lines;
}

long int chek_file_size(FILE* file)
{
    struct stat file_info = {};
    int fd = fileno(file);
    if (fstat(fd, &file_info)== -1)
    {
        printf("ERROR: check number of lines incorrect\n");
        return -1;
    }

    return file_info.st_size;
}

int check_n_lines(char* buffer, size_t read_size)
{

    int n_lines = 0;
    char* buf_ptr = strchr(buffer, '\n');

    while (buf_ptr != NULL)
    {
        n_lines += 1;
        buf_ptr = strchr(buf_ptr + 1, '\n');
    }

    return n_lines;
}
