#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>

#include "calc.h"
#include "assem.h"

int main(int argc, char *argv[])
{
    int n_commands = 0;
    int* code_mass = NULL;

    if (argc != 2)
    {
        printf("ERROR: incorrect number of arguments (must be 2)\n");
        return 1;
    }

    code_mass = file_code_compile(&n_commands, argv[1]);

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

    uint16_t signature = (uint16_t)SIGNATURAA;
    uint8_t version = (uint8_t)VERSION;
    uint16_t command_count = (uint16_t)(*n_commands);

    fwrite(&signature, sizeof(signature), 1, file);
    fwrite(&version, sizeof(version), 1, file);
    fwrite(&command_count, sizeof(command_count), 1, file);

    for (int i = 0; i < *n_commands; i++)
    {
        uint16_t cmd = (uint16_t)code_mass[i];
        fwrite(&cmd, sizeof(cmd), 1, file);
    }

    fclose(file);
}

int* file_code_compile(int* n_commands, const char* filename)
{
    int n_lines           = 0;
    enum cmd_code command = HLT;
    int push_arg          = 0;
    int jump_arg          = 0;
    char reg_lit          = 'a';

    char** ptr_mass = read_text(filename, &n_lines);
    if (ptr_mass == NULL)
    {
        return NULL;
    }
    int max_commands = n_lines * 2;

    int* code_mass = (int*)calloc(max_commands, sizeof(int));
    if (code_mass == NULL)
    {
        free(ptr_mass[0]);
        free(ptr_mass);
        return NULL;
    }

    for (int i = 0; i < n_lines; i++)
    {
        command = get_file_command(ptr_mass[i]);

        if (command == INC_FUNC)
        {
            printf("incorrect name of function in line: %d\n", i + 1);
            break;
        }

        code_mass[(*n_commands)++] = command;

        if (command == HLT)
            break;

        if (command == PUSH)
        {
            if (sscanf(ptr_mass[i], "PUSH %d", &push_arg) != 1)
            {
                printf("Expected number after PUSH in line %d\n", i + 1);
                break;
            }

            code_mass[(*n_commands)++] = push_arg;
        }

        if (command == PUSHREG || command == POPREG)
        {
            if (sscanf(ptr_mass[i], "%*s %cX", &reg_lit) != 1)
            {
                printf("Expected register after command in the line %d\n", i + 1);
                break;
            }
            code_mass[(*n_commands)++] = reg_lit - 'A';
        }

        if (command == JB || command == JBE || command == JA || command == JAE || command == JE || command == JNE)
        {
            if (sscanf(ptr_mass[i], "%*s %d", &jump_arg) != 1)
            {
                printf("Expected number after JUMP in line %d\n", i + 1);
                break;
            }

            code_mass[(*n_commands)++] = jump_arg;
        }

        if (command == IN_CMD)
        {
            if (scanf("%d", &push_arg) != 1)
            {
                printf("expected arg before IN\n");
                break;
            }

            code_mass[(*n_commands)++] = push_arg;
        }

    }

    free(ptr_mass[0]);
    free(ptr_mass);

    return code_mass;
}


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
    // printf("%s:%d: %s(): file_size = %d\n", __FILE__, __LINE__, __func__, *num_lines);

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
