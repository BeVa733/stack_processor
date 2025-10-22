#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>

#include "calc.h"
#include "assem.h"

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

    *num_lines = check_n_lines(buffer);

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

int check_n_lines(char* buffer)
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
