#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "calc.h"

enum cmd_code get_command (void);
void code_writer(int* code_mass, const char* filename, int n_commands);

int main()
{
    enum cmd_code command = CMD_OUT;
    int max_commands = 20;
    int n_commands = 0;
    int push_arg = 0;

    int* code_mass = (int*)calloc(max_commands, sizeof(code_mass[0]));

    while(1) //TODO funk
    {
        command = get_command();
        IF (command == EOF) break;

        if (n_commands == max_commands - 2)
        {
            max_commands *= 2;
            int* new_code_mass = (int*)realloc(code_mass, max_commands * sizeof(int));

            if (new_code_mass == NULL)
            {
                printf("Error of relocation memory\n");
                return 1;
            }

            code_mass = new_code_mass;
        }

        if (command == INC_FUNC)
        {
            printf("incorrect name of function, please try again\n");
        }
        else
        {
            code_mass[n_commands++] = command;
        }

        if (command == HLT)
        {
            code_mass[n_commands++] = command;
            break;
        }

        if (command == PUSH)
        {
            scanf(" %d", &push_arg);
            code_mass[n_commands++] = push_arg;
        }
    }

//  code_mass[n_commands++] = HLT;

    code_writer (code_mass, "input.asm", n_commands);

    free(code_mass);

    return 0;
}

enum cmd_code get_command (void)
{
    const int MAX_LEN = 5;

    char input[MAX_LEN] = "";
    if (scanf("%4s", input) != 1)
        return INC_FUNC;

    if(strcmp(input, "HLT") == 0)
        return HLT;

    else if(strcmp(input, "OUT") == 0)
        return CMD_OUT;

    else if(strcmp(input, "PUSH") == 0)
        return PUSH;

    else if(strcmp(input, "ADD") == 0)
        return ADD;

    else if(strcmp(input, "SUB") == 0)
        return SUB;

    else if(strcmp(input, "MUL") == 0)
        return MUL;

    else if(strcmp(input, "DIV") == 0)
        return DIV;

    else if(strcmp(input, "POW") == 0)
        return POW;

    else if(strcmp(input, "SQRT") == 0)
        return SQRT;
    else
        return INC_FUNC;
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

    return;
}
// TODO SIGNATYRA & WERSIiiYAaaA

