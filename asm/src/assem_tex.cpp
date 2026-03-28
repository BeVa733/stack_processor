#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#include "calc.h"
#include "assem.h"

extern cmd_data* cmd_code_sorted[CMD_COUNT];

void assem_ctor(comp_info* assem_data)
{
    assem_data->code_mass  = NULL;
    assem_data->first_pass = true;
    assem_data->ptr_mass   = NULL;
    assem_data->n_commands = 0;

    assem_data->labels_capacity = 20;
    assem_data->labels_size = 0;
    assem_data->labels_array = (label*)calloc(assem_data->labels_capacity, sizeof(label));

    for (int i = 0; i < CMD_COUNT; i++)
    {
        cmd_code_translate[i].valid_hash = make_hash(cmd_code_translate[i].str_code);
        cmd_code_sorted[i] = &cmd_code_translate[i];
    }

    qsort(cmd_code_sorted, CMD_COUNT, sizeof(cmd_data*), cmp_cmd_data_ptr);
}

int make_hash (const char* str)
{
    int hash = 5381;

    for (int i = 0; true; i++)
    {
        if (str[i] == '\0')
            break;

        hash *= 33;
        hash += str[i];
    }

    return hash;
}

int cmp_cmd_data_ptr(const void* a, const void* b)
{
    const cmd_data* cmd_1 = *(const cmd_data**)a;
    const cmd_data* cmd_2 = *(const cmd_data**)b;

    if (cmd_1->valid_hash < cmd_2->valid_hash)
        return -1;
    if (cmd_1->valid_hash > cmd_2->valid_hash)
        return 1;

    return 0;
}

int cmp_label(const void* a, const void* b)
{
    const label* label_a = (const label*)a;
    const label* label_b = (const label*)b;

    if (label_a->hash < label_b->hash) return -1;
    if (label_a->hash > label_b->hash) return 1;
    return 0;
}

int cmp_label_bsearch(const void* key, const void* element)
{
    int key_hash = *(int*)key;
    const label* lbl = (const label*)element;

    if (key_hash < lbl->hash) return -1;
    if (key_hash > lbl->hash) return 1;
    return 0;
}

void assem_dtor(comp_info* assem_data)
{
    free(assem_data->labels_array);
    free(assem_data->code_mass);
    free(assem_data->ptr_mass[0]);
    free(assem_data->ptr_mass);
}
