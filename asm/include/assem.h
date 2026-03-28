#ifndef ASSEM_H
#define ASSEM_H

enum type_arg
{
    NO_ARG   = 0,
    NUMBER   = 1,
    REGISTER = 2,
    RAM      = 3,
    METKA    = 4
};

struct cmd_data
{
    enum cmd_code enum_code;
    const char* str_code;
    enum type_arg type;
    int valid_hash;
};

struct label
{
    const char* name_ptr;
    int jump_arg;
    int hash;
};

struct comp_info
{
    char** ptr_mass;
    int* code_mass;
    label* labels_array;
    int labels_capacity;
    int labels_size;
    int n_commands;
    int n_lines;
    bool first_pass;
};

enum status
{
    SUCCESS = 0,
    FAILURE = 1
};

void assem_ctor      (comp_info* assem_data);
int make_hash        (const char* str);
int cmp_cmd_data_ptr (const void* a, const void* b);
int cmp_label        (const void* a, const void* b);
int cmp_label_bsearch(const void* key, const void* element);
void assem_dtor      (comp_info* assem_data);

void text_code_writer (int* code_mass, const char* filename, int n_commands);
void bin_code_writer  (int* code_mass, const char* filename, int* n_commands);

enum cmd_code get_command (char* str);
int* run_compile          (comp_info* assem_data);
enum status add_label     (comp_info* assem_data, const char* metka_str_name, int jump_arg);
int find_label            (comp_info* assem_data, const char* name);
enum status compile_arg   (comp_info* assem_data, int i, enum type_arg type);

char** read_text        (const char* filename, int* num_lines);
char** make_ptr_massive (char* buffer, int num_lines, size_t read_size);
long int chek_file_size (FILE* file);
int check_n_lines       (char* buffer);

#ifdef ASSEMBLER
cmd_data cmd_code_translate[CMD_COUNT] =
{
    {HLT,     "HLT",     NO_ARG,   0 },
    {CMD_OUT, "OUT",     NO_ARG,   0 },
    {OUTC,    "OUTC",    NO_ARG,   0 },
    {ADD,     "ADD",     NO_ARG,   0 },
    {MUL,     "MUL",     NO_ARG,   0 },
    {SUB,     "SUB",     NO_ARG,   0 },
    {DIV,     "DIV",     NO_ARG,   0 },
    {POW,     "POW",     NO_ARG,   0 },
    {SQRT,    "SQRT",    NO_ARG,   0 },
    {IN_CMD,  "IN",      NO_ARG,   0 },
    {BACK,    "BACK",    NO_ARG,   0 },
    {DRAW,    "DRAW",    NO_ARG,   0 },
    {WDRAW,   "WDRAW",   NO_ARG,   0 },
    {PUSH,    "PUSH",    NUMBER,   0 },
    {PUSHREG, "PUSHREG", REGISTER, 0 },
    {POPREG,  "POPREG",  REGISTER, 0 },
    {JB,      "JB",      METKA,    0 },
    {JBE,     "JBE",     METKA,    0 },
    {JA,      "JA",      METKA,    0 },
    {JAE,     "JAE",     METKA,    0 },
    {JE,      "JE",      METKA,    0 },
    {JNE,     "JNE",     METKA,    0 },
    {JMP,     "JMP",     METKA,    0 },
    {CALL,    "CALL",    METKA,    0 },
    {PUSHM,   "PUSHM",   RAM,      0 },
    {POPM,    "POPM",    RAM,      0 },
};

#else
extern cmd_data cmd_code_translate[CMD_COUNT];

#endif //ASSEMBLER


#endif //ASSEM_H
