#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#include "calc.h"
#include "assem.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("ERROR: incorrect number of arguments (must be 3)\n");
        printf("Usage: %s <input.bin> <output.txt>\n", argv[0]);
        return 1;
    }

    FILE* bin_file = fopen(argv[1], "rb");
    if (!bin_file)
    {
        printf("ERROR: cannot open binary file %s\n", argv[1]);
        return 1;
    }

    FILE* out_file = fopen(argv[2], "w");
    if (!out_file)
    {
        printf("ERROR: cannot create output file %s\n", argv[2]);
        fclose(bin_file);
        return 1;
    }

    uint16_t signature = 0;
    uint8_t version = 0;
    uint16_t command_count = 0;

    fread(&signature, sizeof(signature), 1, bin_file);
    fread(&version, sizeof(version), 1, bin_file);
    fread(&command_count, sizeof(command_count), 1, bin_file);

    if (signature != SIGNATURAA)
    {
        printf("ERROR: invalid signature 0x%X\n", signature);
        fclose(bin_file);
        fclose(out_file);
        return 1;
    }

    if (version != VERSION)
    {
        printf("ERROR: unsupported version %d\n", version);
        fclose(bin_file);
        fclose(out_file);
        return 1;
    }

    uint16_t* code_mass = (uint16_t*)calloc(command_count, sizeof(uint16_t));
    if (!code_mass)
    {
        printf("ERROR: memory allocation failed\n");
        fclose(bin_file);
        fclose(out_file);
        return 1;
    }

    for (int i = 0; i < command_count; i++)
    {
        fread(&code_mass[i], sizeof(uint16_t), 1, bin_file);
    }

    fclose(bin_file);

    for (int i = 0; i < command_count; i++)
    {
        enum cmd_code cmd = (enum cmd_code)code_mass[i];

        switch (cmd)
        {
            case HLT:
                fprintf(out_file, "HLT\n");
                break;

            case CMD_OUT:
                fprintf(out_file, "OUT\n");
                break;

            case ADD:
                fprintf(out_file, "ADD\n");
                break;

            case SUB:
                fprintf(out_file, "SUB\n");
                break;

            case MUL:
                fprintf(out_file, "MUL\n");
                break;

            case DIV:
                fprintf(out_file, "DIV\n");
                break;

            case POW:
                fprintf(out_file, "POW\n");
                break;

            case SQRT:
                fprintf(out_file, "SQRT\n");
                break;

            case PUSH:
                if (i + 1 < command_count)
                {
                    fprintf(out_file, "PUSH %d\n", code_mass[++i]);
                }
                else
                {
                    printf("ERROR: PUSH without argument\n");
                }
                break;

            case PUSHREG:
                if (i + 1 < command_count)
                {
                    fprintf(out_file, "PUSHREG %cX\n", 'A' + code_mass[++i]);
                }
                else
                {
                    printf("ERROR: PUSHREG without argument\n");
                }
                break;

            case POPREG:
                if (i + 1 < command_count)
                {
                    fprintf(out_file, "POPREG %cX\n", 'A' + code_mass[++i]);
                }
                else
                {
                    printf("ERROR: POPREG without argument\n");
                }
                break;

            case JB:
                if (i + 1 < command_count)
                {
                    fprintf(out_file, "JB %d\n", code_mass[++i]);
                }
                else
                {
                    printf("ERROR: JB without argument\n");
                }
                break;

            case JBE:
                if (i + 1 < command_count)
                {
                    fprintf(out_file, "JBE %d\n", code_mass[++i]);
                }
                else
                {
                    printf("ERROR: JBE without argument\n");
                }
                break;

            case JA:
                if (i + 1 < command_count)
                {
                    fprintf(out_file, "JA %d\n", code_mass[++i]);
                }
                else
                {
                    printf("ERROR: JA without argument\n");
                }
                break;

            case JAE:
                if (i + 1 < command_count)
                {
                    fprintf(out_file, "JAE %d\n", code_mass[++i]);
                }
                else
                {
                    printf("ERROR: JAE without argument\n");
                }
                break;

            case JE:
                if (i + 1 < command_count)
                {
                    fprintf(out_file, "JE %d\n", code_mass[++i]);
                }
                else
                {
                    printf("ERROR: JE without argument\n");
                }
                break;

            case JNE:
                if (i + 1 < command_count)
                {
                    fprintf(out_file, "JNE %d\n", code_mass[++i]);
                }
                else
                {
                    printf("ERROR: JNE without argument\n");
                }
                break;

            case IN_CMD:
                if (i + 1 < command_count)
                {
                    fprintf(out_file, "IN\n");
                    i++;
                }
                else
                {
                    printf("ERROR: IN without argument\n");
                }
                break;

            default:
                printf("ERROR: unknown command code %d at position %d\n", cmd, i);
                break;
        }
    }

    fclose(out_file);
    free(code_mass);

    return 0;
}
