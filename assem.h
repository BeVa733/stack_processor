#ifndef ASSEM_H
#define ASSEM_H

struct cmd_data
{
    enum cmd_code enum_code;
    const char* str_code;
};

struct comp_info
{
    char** ptr_mass;
    int* code_mass;
    int metki_array[N_METOK];
    int n_commands;
    int n_lines;
    bool first_pass;
};
void assem_ctor(comp_info* assem_data);

void text_code_writer (int* code_mass, const char* filename, int n_commands);
void bin_code_writer (int* code_mass, const char* filename, int* n_commands);

int* run_compile(comp_info* assem_data);
int* compile_arg(comp_info* assem_data, int i, enum cmd_code command);

char** read_text (const char* filename, int* num_lines);
char** make_ptr_massive (char* buffer, int num_lines, size_t read_size);
long int chek_file_size (FILE* file);
int check_n_lines (char* buffer);
enum cmd_code get_command (char* str);

void assem_dtor(comp_info* assem_data);

// #ifdef DEBUG
// #define ON_DEBUG(code) code
// #define ON_DEBUG_ELSE(code)
// #else
// #define ON_DEBUG(code)
// #define ON_DEBUG_ELSE(code) code
// #endif //DEBUG


#endif
