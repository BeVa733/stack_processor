#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#define ASSEMBLER

#include "calc.h"
#include "assem.h"

cmd_data* cmd_code_sorted[CMD_COUNT] = {};

int main(int argc, char *argv[])
{
    comp_info assem_data ={};
    assem_ctor(&assem_data);

    if (argc != 2 && argc != 3)
    {
        printf("Usage: asm.out <sourse_asm_filename> <output_filename> (optional)\n");
        return 1;
    }

    assem_data.ptr_mass = read_text(argv[1], &(assem_data.n_lines));
    if (assem_data.ptr_mass == NULL)
        return 1;

    run_compile(&assem_data);

    assem_data.code_mass = (int*)calloc(assem_data.n_commands, sizeof(int));
    if (assem_data.code_mass == NULL)
    {
        assem_dtor(&assem_data);
        return 1;
    }

    assem_data.first_pass = false;
    assem_data.n_commands = 0;

    if (run_compile(&assem_data) == NULL)
    {
        assem_dtor(&assem_data);
        return 1;
    }

    printf("Compilation successful\n");
    if (argc == 3)
        bin_code_writer(assem_data.code_mass, argv[2], &(assem_data.n_commands));
    else
        bin_code_writer(assem_data.code_mass, "output_asm.bin", &(assem_data.n_commands));

    text_code_writer(assem_data.code_mass, "output_asm.txt", assem_data.n_commands);

    assem_dtor(&assem_data);

    return 0;
}

enum status add_label(comp_info* assem_data, const char* metka_str_name, int jump_arg)
{
    if (assem_data->labels_size >= assem_data->labels_capacity)
    {
        assem_data->labels_capacity *= 2;
        label* new_ptr = (label*)realloc(assem_data->labels_array, assem_data->labels_capacity * sizeof(label));

        if (new_ptr != NULL)
        {
            assem_data->labels_array = new_ptr;
        }

        else
        {
            printf("bad realocation\n");
            return FAILURE;
        }
    }


    label* new_label = &assem_data->labels_array[assem_data->labels_size++];
    new_label->name_ptr = metka_str_name;
    new_label->jump_arg = jump_arg;
    new_label->hash = make_hash(metka_str_name);

    return SUCCESS;
}

int find_label (comp_info* assem_data, const char* name)
{
    int hash = make_hash(name);

    label* found = (label*)bsearch (&hash, assem_data->labels_array,
                                    assem_data->labels_size, sizeof(label),
                                    cmp_label_bsearch);

    if (found == NULL)
        return -1;

    return found->jump_arg;
}

//asserts

int* run_compile(comp_info* assem_data)
{
    enum cmd_code command = HLT;

    const int MAX_LEN = 10;

    for (int i = 0; i < assem_data->n_lines; i++)
    {
        if (assem_data->ptr_mass[i][0] == '\0' || assem_data->ptr_mass[i][0] == '\n')
            continue;

        if (assem_data->ptr_mass[i][0] == ':')
        {
            if (assem_data->first_pass)
                add_label(assem_data, assem_data->ptr_mass[i] + 1, assem_data->n_commands);

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
            assem_data->code_mass[assem_data->n_commands++] = command;

        else
            assem_data->n_commands++;

        if (cmd_code_translate[command].type != NO_ARG)
        {
            if (compile_arg(assem_data, i, cmd_code_translate[command].type) == FAILURE)
            {
                printf("Error in argument compilation at line %d\n", i + 1);
                return NULL;
            }
        }
    }

    if (assem_data->first_pass)
        qsort(assem_data->labels_array, assem_data->labels_size, sizeof(label), cmp_label);

    return assem_data->code_mass;
}

enum status compile_arg (comp_info* assem_data, int i, enum type_arg type)
{
    int argument = 0;
    char metka_name[100] = "";

    if (type == NUMBER)
    {
        if (sscanf(assem_data->ptr_mass[i], "%*s %d", &argument) != 1)
        {
            printf("Expected number after command in line %d\n", i + 1);
            return FAILURE;
        }
    }

    else if (type == REGISTER)
    {
        char reg_char = 0;

        if (sscanf(assem_data->ptr_mass[i], "%*s %cX", &reg_char) != 1)
        {
            printf("Expected register after command in line %d\n", i + 1);
            return FAILURE;
        }

        argument = reg_char - 'A';

        if (argument >= N_REGISTERS || argument < 0)
        {
            printf("Invalid register %cX in line %d (max: %d)\n", reg_char, i + 1, N_REGISTERS);
            return FAILURE;
        }
    }

    else if (type == METKA)
    {
        if (sscanf(assem_data->ptr_mass[i], "%*s :%99s", metka_name) == 1)
        {
            if (assem_data->first_pass)
                argument = 0;

            else
            {
                int jump_arg = find_label(assem_data, metka_name);

                if (jump_arg != -1)
                    argument = jump_arg;

                else
                {
                    printf("Unknown label :%s in line %d\n", metka_name, i + 1);
                    return FAILURE;
                }
            }
        }

        else if (sscanf(assem_data->ptr_mass[i], "%*s %d", &argument) == 1)
        ;

        else
        {
            printf("Expected number or label after command in line %d\n", i + 1);
            return FAILURE;
        }
    }

    else if (type == RAM)
    {
        char reg_char = 0;
        if (sscanf(assem_data->ptr_mass[i], "%*s [%cX]", &reg_char) != 1)
        {
            printf("Expected register in RAM access in line %d\n", i + 1);
            return FAILURE;
        }

        argument = reg_char - 'A';

        if (argument >= N_REGISTERS || argument < 0)
        {
            printf("Invalid register [%cX] in line %d (max: %d)\n", reg_char, i + 1, N_REGISTERS);
            return FAILURE;
        }
    }

    if (assem_data->code_mass != NULL)
        assem_data->code_mass[assem_data->n_commands] = argument;

    assem_data->n_commands++;

    return SUCCESS;
}

enum cmd_code get_command(char* input)
{
    int input_hash = make_hash(input);

    int left_edge = 0;
    int right_edge = CMD_COUNT - 1;

    while (left_edge <= right_edge)
    {
        int mid = left_edge + (right_edge - left_edge) / 2;
        int mid_hash = cmd_code_sorted[mid]->valid_hash;

        if (mid_hash == input_hash)
            return cmd_code_sorted[mid]->enum_code;

        else if (mid_hash < input_hash)
            left_edge = mid + 1;

        else
            right_edge = mid - 1;
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

        if (code_mass[i] > 12)
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

