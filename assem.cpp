#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>

#include "calc.h"
#include "assem.h"

int main(int argc, char *argv[])
{
    int n_commands = 0;
    enum type_input input = STDIN;
    int* code_mass = NULL;

    if (argc == 2)
        input = FILE_INP;
    else if (argc == 1)
        input = STDIN;
    else
    {
        printf("too many arguments\n");
        return 1;
    }

    if (input == FILE_INP)
        code_mass = file_code_compile(&n_commands, argv[1]);

    else
        code_mass = stdin_code_compile(&n_commands);

    code_writer(code_mass, "input.asm", n_commands);
    free(code_mass);
    return 0;
}

enum cmd_code get_stdin_command(void)
{
    const int MAX_LEN = 10;
    char input[MAX_LEN] = "";

    if (scanf("%9s", input) != 1)
        return INC_FUNC;

    if      (strcmp(input, "HLT")  == 0) return HLT;
    else if (strcmp(input, "OUT")  == 0) return CMD_OUT;
    else if (strcmp(input, "PUSH") == 0) return PUSH;
    else if (strcmp(input, "ADD")  == 0) return ADD;
    else if (strcmp(input, "SUB")  == 0) return SUB;
    else if (strcmp(input, "MUL")  == 0) return MUL;
    else if (strcmp(input, "DIV")  == 0) return DIV;
    else if (strcmp(input, "POW")  == 0) return POW;
    else if (strcmp(input, "SQRT") == 0) return SQRT;
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

int* stdin_code_compile(int* n_commands)
{
    enum cmd_code command = HLT;
    int max_commands = 20;
    int push_arg = 0;
    int* code_mass = (int*)calloc(max_commands, sizeof(int));

    while(1)
    {
        command = get_stdin_command();

        if (command == INC_FUNC)
            break;

        if (*n_commands >= max_commands - 2)
        {
            max_commands *= 2;
            int* new_code_mass = (int*)realloc(code_mass, max_commands * sizeof(int));
            if (new_code_mass == NULL)
            {
                printf("Memory reallocation error\n");
                free(code_mass);
                return NULL;
            }
            code_mass = new_code_mass;
        }

        code_mass[(*n_commands)++] = command;

        if (command == HLT)
            break;

        if (command == PUSH)
        {
            if (scanf("%d", &push_arg) != 1)
            {
                printf("Expected number after PUSH\n");
                break;
            }
            code_mass[(*n_commands)++] = push_arg;
        }
    }

    return code_mass;
}

int* file_code_compile(int* n_commands, const char* filename)
{
    int n_lines = 0;
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

    enum cmd_code command = HLT;
    int push_arg = 0;

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

enum cmd_code get_file_command(char* str)
{
    const int MAX_LEN = 10;
    char input[MAX_LEN] = "";

    if (sscanf(str, "%9s", input) != 1)
        return INC_FUNC;

    if      (strcmp(input, "HLT")  == 0) return HLT;
    else if (strcmp(input, "OUT")  == 0) return CMD_OUT;
    else if (strcmp(input, "PUSH") == 0) return PUSH;
    else if (strcmp(input, "ADD")  == 0) return ADD;
    else if (strcmp(input, "SUB")  == 0) return SUB;
    else if (strcmp(input, "MUL")  == 0) return MUL;
    else if (strcmp(input, "DIV")  == 0) return DIV;
    else if (strcmp(input, "POW")  == 0) return POW;
    else if (strcmp(input, "SQRT") == 0) return SQRT;
    else return INC_FUNC;
}
