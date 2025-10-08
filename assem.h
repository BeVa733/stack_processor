#ifndef ASSEM_H
#define ASSEM_H

enum cmd_code get_stdin_command(void);
void code_writer(int* code_mass, const char* filename, int n_commands);
void bin_code_writer(int* code_mass, const char* filename, int* n_commands);
// int* stdin_code_compile(int* n_commands); // мб нахуй вырезать / если не лень задебажить
int* file_code_compile(int* n_commands, const char* filename);
char** read_text(const char* filename, int* num_lines);
char** make_ptr_massive(char* buffer, int num_lines, size_t read_size);
long int chek_file_size(FILE* file);
int check_n_lines(char* buffer, size_t read_size);
enum cmd_code get_file_command(char* str);

#ifdef DEBUG
#define ON_DEBUG(code) code
#define ON_DEBUG_ELSE(code)
#else
#define ON_DEBUG(code)
#define ON_DEBUG_ELSE(code) code
#endif //DEBUG


#endif
