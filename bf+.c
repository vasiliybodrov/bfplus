/* ************************************************************************** */
/* Program name: Brainfuck Interpreter Plus (bf+)                             */
/* Description: Brainfuck interpreter with extensions                         */
/* Author: Vasiliy V. Bodrov aka Bodro (e-mail: bodro-mail at list.ru)        */
/* Date: 2014-09-08                                                           */
/*                                                                            */
/* Programming language: ISO/IEC 9899:1990 (C90)                              */
/* Commenting language: English                                               */
/* ************************************************************************** */
/* Functions:                                                                 */
/*     atexit_func                                                            */
/*     print_preamble                                                         */
/*     print_use_help                                                         */
/*     print_license                                                          */
/*     print_help                                                             */
/*     print_author                                                           */
/*     print_version                                                          */
/*     print_show_information                                                 */
/*     read_config                                                            */
/*     is_command                                                             */
/*     control                                                                */
/*     work                                                                   */
/*     main                                                                   */
/* ************************************************************************** */
/* The MIT License (MIT)                                                      */
/*                                                                            */
/* Copyright (c) 2014 IPB Software (Vasiliy V. Bodrov)                        */
/*                                                                            */
/* Permission is hereby granted, free of charge, to any person obtaining a    */
/* copy of this software and associated documentation files (the "Software"), */
/* to deal in the Software without restriction, including without limitation  */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense,   */
/* and/or sell copies of the Software, and to permit persons to whom the      */
/* Software is furnished to do so, subject to the following conditions:       */
/*                                                                            */
/* The above copyright notice and this permission notice shall be included    */
/* in all copies or substantial portions of the Software.                     */
/*                                                                            */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS    */
/* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF                 */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.     */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY       */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT  */
/* OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR   */
/* THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                 */
/* ************************************************************************** */

#if defined(__cplusplus)
/*extern "C" {*/
#endif /* defined(__cplusplus) */

/* ************************************************************************** */
/* INCLUDES                                                                   */
/* ************************************************************************** */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include <ctype.h>

/* ************************************************************************** */
/* DEFINITIONS */
/* ************************************************************************** */
#define LICENSE1 \
"The MIT License (MIT)\n\
\n\
Copyright (c) 2014 IPB Software (Vasiliy V. Bodrov)\n\
\n"

#define LICENSE2 \
"Permission is hereby granted, free of charge, to any person obtaining a copy \
of this software and associated documentation files (the \"Software\"), to \
deal in the Software without restriction, including without limitation the \
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or \
sell  copies of the Software, and to permit persons to whom the Software is \
furnished to do so, subject to the following conditions:\n\
\n"
#define LICENSE3 \
"The above copyright notice and this permission notice shall be included in \
all copies or substantial portions of the Software.\n\
\n"
#define LICENSE4 \
"THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR \
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, \
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE \
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER \
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING \
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS \
IN THE SOFTWARE.\n"

#define PROGRAM_VERSION                     "0.9.0"
#define PROGRAM_AUTHORS                     "Vasiliy V. Bodrov aka Bodro"
#define PROGRAM_AUTHORS_COMPANY             "IPB Software"

#define MAX_FILE_NAME_LENGTH                 2048
#define STATIC_CELL_COUNT                    2048

#define PARAM_NAME_USE_COMMENT_TYPE1         "use_comment_type1"
#define PARAM_NAME_USE_COMMENT_TYPE2         "use_comment_type2"
#define PARAM_NAME_USE_COMMENT_TYPE3         "use_comment_type3"
#define PARAM_NAME_USE_COMMENT_TYPE4         "use_comment_type4"
#define PARAM_NAME_USE_INFINITE_CELLS        "use_infinite_cells"
#define PARAM_NAME_USE_INFINITE_NESTED_LOOPS "use_infinite_nested_loops"
#define PARAM_NAME_USE_NEGATIVE_VALUE        "use_negative_value"
#define PARAM_NAME_USE_LARGE_CELL_SIZE       "use_large_cell_size"
#define PARAM_NAME_USE_FAST_INPUT            "use_fast_input"
#define PARAM_NAME_USE_PROCEDURE             "use_procedure"
#define PARAM_NAME_USE_SYMBOL_EQUAL          "use_symbol_equal"
#define PARAM_NAME_USE_SYMBOL_UNDER          "use_symbol_under"
#define PARAM_NAME_USE_SYNTAX_HQ9PLUS        "use_syntax_hq9plus"
#define PARAM_NAME_USE_MOD255                "use_mod255"
#define PARAM_NAME_USE_FORCE_RN              "use_force_rn"

/* ************************************************************************** */
/* USER TYPES */
/* ************************************************************************** */

/* Program options (for interpreter) */
struct program_options_s {
    char config_filename[MAX_FILE_NAME_LENGTH];
    char source_filename[MAX_FILE_NAME_LENGTH];
    unsigned char verbose;
    unsigned char show_info;
    unsigned char quiet_exit;
    unsigned char print_preamble;
    unsigned char print_license;
    unsigned char print_help;
    unsigned char print_version;
    unsigned char print_author;

	union {
        struct comment_flag_s {
            unsigned int use_type1 : 1;
            unsigned int use_type2 : 1;
            unsigned int use_type3 : 1;
            unsigned int use_type4 : 1;
        } comment_flags;
        unsigned int use_comment;
    } comment;

    unsigned char use_infinite_cells;
    unsigned char use_infinite_nested_loops;
    unsigned char use_negative_value;
    unsigned char use_large_cell_size;
    unsigned char use_fast_input;
    unsigned char use_procedure; /* not support (next version) */
    unsigned char use_symbol_equal;
    unsigned char use_symbol_under;
    unsigned char use_syntax_hq9plus;
    unsigned char use_mod255;
    unsigned char use_force_rn;
};

/* Modes of interpretation */
enum modes {
	unknown_mode,     /* Unknown mode (initialization) */
	command_mode,     /* Command mode (work)           */
	comment_mode      /* Comment mode (comments)       */
};

 /* Loop: information item */
struct loop_position_s {
    int id_pos;
    fpos_t begin_pos;
    fpos_t end_pos;
};

typedef struct program_options_s program_options_t, *program_options_p;
typedef struct loop_position_s loop_position_t, *loop_position_p;
typedef enum modes mode_t;
typedef signed long int cell_t, *cell_p;
typedef signed int code_t, *code_p;
typedef signed int index_t, *index_p;

/* Main data struct aka class */
struct main_data_s {
    union main_data_cells_u {
        struct final_cells_s {
            cell_p cells;
            cell_p current_cell;
        }* final_cells;

        struct infinite_cells_s {
            struct infinite_cells_list_s {
                cell_p cell;
                struct infinite_cells_s* next_cell;
            }* current_cell;
            struct infinite_cells_list_s* start_cells;
        }* infinite_cells;
    }* cells;

    union main_data_loops_u {
        struct final_loops_s {
            loop_position_p position;
        }* final_loops;

        struct infinite_loops_s {
            loop_position_p loop_position;
            struct infinite_loops_s* next;
        }* infinite_loops;
    }* loops;
/*
    cell_p cells = NULL;
    cell_p current_cell = NULL;
    index_t current_index = 0;
    loop_position_t loops_pos[1024];
    int loops_pos_index = -1;
    */
    void (*cell_add)(struct main_data_s*);
    void (*cell_sub)(struct main_data_s*);
    void (*cell_prev)(struct main_data_s*);
    void (*cell_next)(struct main_data_s*);
    void (*data_input)(struct main_data_s*);
    void (*data_output)(struct main_data_s*);
    void (*loop_begin)(struct main_data_s*, FILE*);
    void (*loop_end)(struct main_data_s*, FILE*);
    void (*hq9plus_h_output)(void);
    void (*hq9plus_q_output)(FILE*);
    void (*hq9plus_9_output)(void);
    void (*done)(void);
};

/* ************************************************************************** */
/* PROTOTYPES */
/* ************************************************************************** */
struct main_data_s* constructor_main_data(struct main_data_s** main_data);
void destructor_main_data(struct main_data_s** main_data);
/*
void method_cell_add_fin(struct main_data_s*);
void method_cell_add_inf(struct main_data_s*);
void method_;
void method_;
void method_;
void method_;
*/
static void method_hq9plus_h_output_dummy(void);
static void method_hq9plus_q_output_dummy(FILE* f);
static void method_hq9plus_9_output_dummy(void);
static void method_hq9plus_h_output_real(void);
static void method_hq9plus_q_output_real(FILE* f);
static void method_hq9plus_9_output_real(void);

static void atexit_func(void);
static void print_preamble(void);
static void print_use_help(void);
static void print_license(void);
static void print_help(void);
static void print_author(void);
static void print_version(void);
static void print_show_information(void);
static void read_config(void);
static int is_command(code_t ch);
static int control(void);
static int work(void);
int main(const int argc, char* const* argv);

/* ************************************************************************** */
/* GLOBAL VARIABLE */
/* ************************************************************************** */
program_options_t options;

/* ************************************************************************** */
/* FUNCTIONS */
/* ************************************************************************** */

/* Выделение памяти */
struct main_data_s* constructor_main_data(struct main_data_s** main_data) {
    *main_data = (struct main_data_s*) calloc(1, sizeof(struct main_data_s));
    if(*main_data) {
        (*main_data)->cells = (union main_data_cells_u*) calloc(1, sizeof(union main_data_cells_u));
        if(!(*main_data)->cells) {
            goto allocation_memory;
        }

        (*main_data)->loops = (union main_data_loops_u*) calloc(1, sizeof(union main_data_loops_u));
        if(!(*main_data)->loops) {
            goto allocation_memory;
        }

        if(options.use_infinite_cells) {

        }
        else {

        }

        if(options.use_infinite_nested_loops) {

        }
        else {

        }

        if(options.use_syntax_hq9plus) {
            (*main_data)->hq9plus_h_output = method_hq9plus_h_output_real;
            (*main_data)->hq9plus_q_output = method_hq9plus_q_output_real;
            (*main_data)->hq9plus_9_output = method_hq9plus_9_output_real;
        }
        else {
            (*main_data)->hq9plus_h_output = method_hq9plus_h_output_dummy;
            (*main_data)->hq9plus_q_output = method_hq9plus_q_output_dummy;
            (*main_data)->hq9plus_9_output = method_hq9plus_9_output_dummy;
        }
    }

    goto ok;

allocation_memory:
    destructor_main_data(main_data);

ok:
    return *main_data;
}

/* Освобождение памяти */
void destructor_main_data(struct main_data_s** main_data) {

}

/* -------------------------------------------------------------------------- */
/* Function: method_hq9plus_h_output_dummy                                    */
/* Description: */
/* Parameters: */
/* Return: */
/* Note: */
/* -------------------------------------------------------------------------- */
void method_hq9plus_h_output_dummy(void) {
}

/* -------------------------------------------------------------------------- */
/* Function: method_hq9plus_q_output_dummy                                    */
/* Description: */
/* Parameters: */
/* Return: */
/* Note: */
/* -------------------------------------------------------------------------- */
void method_hq9plus_q_output_dummy(FILE* f) {
    (void) f;
}

/* -------------------------------------------------------------------------- */
/* Function: method_hq9plus_9_output_dummy                                    */
/* Description: */
/* Parameters: */
/* Return: */
/* Note: */
/* -------------------------------------------------------------------------- */
void method_hq9plus_9_output_dummy(void) {
}

/* -------------------------------------------------------------------------- */
/* Function: method_hq9plus_h_output_real                                     */
/* Description: */
/* Parameters: */
/* Return: */
/* Note: */
/* -------------------------------------------------------------------------- */
void method_hq9plus_h_output_real(void) {
    (void) printf("Hello world!\n");
}

/* -------------------------------------------------------------------------- */
/* Function: method_hq9plus_q_output_real                                     */
/* Description: */
/* Parameters: */
/* Return: */
/* Note: */
/* -------------------------------------------------------------------------- */
void method_hq9plus_q_output_real(FILE* f) {
    int ch = 0;
    fpos_t pos;

    fgetpos(f, &pos);
    rewind(f);

    while((ch = fgetc(f)) != EOF) {
        fputc(ch, stdout);
    }

    fsetpos(f, &pos);
}

/* -------------------------------------------------------------------------- */
/* Function: method_hq9plus_9_output_real                                     */
/* Description: */
/* Parameters: */
/* Return: */
/* Note: */
/* -------------------------------------------------------------------------- */
void method_hq9plus_9_output_real(void) {
    int i = 0;

    for(i = 99; i > 1; i--) {
        (void) printf("%i bottles of beer on the wall, %i bottles of beer.\n", i, i);
        (void) printf("Take one down and pass it around, %i bottles of beer on the wall.\n\n", i - 1);
    }

    (void) printf("1 bottle of beer on the wall, 1 bottle of beer.\n");
    (void) printf("Take one down and pass it around, no more bottles of beer on the wall.\n\n");
    (void) printf("No more bottles of beer on the wall, no more bottles of beer.\n");
    (void) printf("Go to the store and buy some more, 99 bottles of beer on the wall.\n");
}

/* -------------------------------------------------------------------------- */
/* Function: atexit_func                                                      */
/* Description: */
/* Parameters: */
/* Return: */
/* Note: */
/* -------------------------------------------------------------------------- */
void atexit_func(void) {
    if(!options.quiet_exit) {
        (void) printf("Bye!\n");
    }
}

/* -------------------------------------------------------------------------- */
/* Function: print_preamble                                                   */
/* Description: */
/* Parameters: */
/* Return: */
/* Note: */
/* -------------------------------------------------------------------------- */
void print_preamble(void) {
    (void) printf("Brainfuck Interpreter Plus (bf+) %s\n", PROGRAM_VERSION);
    (void) printf("Brainfuck interpreter with extensions\n");
    (void) printf("This is free software; see the license agreement (MIT License)\n");
}

/* -------------------------------------------------------------------------- */
/* Function: print_use_help                                                   */
/* Description: */
/* Parameters: */
/* Return: */
/* Note: */
/* -------------------------------------------------------------------------- */
void print_use_help(void) {
	print_preamble();
    (void) printf("\nFor help try -h or --help\n");
}

/* -------------------------------------------------------------------------- */
/* Function: print_help                                                       */
/* Description: */
/* Parameters: */
/* Return: */
/* Note: */
/* -------------------------------------------------------------------------- */
void print_license(void) {
    (void) printf("%s", LICENSE1);
    (void) printf("%s", LICENSE2);
    (void) printf("%s", LICENSE3);
    (void) printf("%s", LICENSE4);
}

/* -------------------------------------------------------------------------- */
/* Function: print_help                                                       */
/* Description: */
/* Parameters: */
/* Return: */
/* Note: */
/* -------------------------------------------------------------------------- */
void print_help(void) {
	print_preamble();
}

/* -------------------------------------------------------------------------- */
/* Function: print_author                                                     */
/* Description: */
/* Parameters: */
/* Return: */
/* Note: */
/* -------------------------------------------------------------------------- */
void print_author(void) {
	print_preamble();
    (void) printf("Authors: %s (%s)\n",
                  PROGRAM_AUTHORS,
                  PROGRAM_AUTHORS_COMPANY);
}

/* -------------------------------------------------------------------------- */
/* Function: print_version                                                    */
/* Description: */
/* Parameters: */
/* Return: */
/* Note: */
/* -------------------------------------------------------------------------- */
void print_version(void) {
	print_preamble();
	(void) printf("Version: %s\n", PROGRAM_VERSION);
}

/* -------------------------------------------------------------------------- */
/* Function: print_show_information                                           */
/* Description: */
/* Parameters: */
/* Return: */
/* Note: */
/* -------------------------------------------------------------------------- */
void print_show_information(void) {
	print_preamble();

    (void) printf("Show information:\n");
    (void) printf("\tconfig filename: %s\n",
                  options.config_filename);
    (void) printf("\tsource filename: %s\n",
                  options.source_filename);
    (void) printf("\tverbose mode: %d\n",
                  options.verbose);
    (void) printf("\tshow info: %d\n",
                  options.show_info);
    (void) printf("\tcomment flags.use_type1: %d\n",
                  options.comment.comment_flags.use_type1);
    (void) printf("\tcomment flags.use_type2: %d\n",
                  options.comment.comment_flags.use_type2);
    (void) printf("\tcomment flags.use_type3: %d\n",
                  options.comment.comment_flags.use_type3);
    (void) printf("\tcomment flags.use_type4: %d\n",
                  options.comment.comment_flags.use_type4);
    (void) printf("\tuse comment: 0x%04X\n",
                  options.comment.use_comment);
    (void) printf("\tuse infinite cells: %d\n",
                  options.use_infinite_cells);
    (void) printf("\tuse infinite nested loops: %d\n",
                  options.use_infinite_nested_loops);
    (void) printf("\tuse negative value: %d\n",
                  options.use_negative_value);
    (void) printf("\tuse large cell size: %d\n",
                  options.use_large_cell_size);
    (void) printf("\tuse fast input: %d\n",
                  options.use_fast_input);
    (void) printf("\tuse procedure: %d\n",
                  options.use_procedure);
    (void) printf("\tuse symbol equal: %d\n",
                  options.use_symbol_equal);
    (void) printf("\tuse symbol under: %d\n",
                  options.use_symbol_under);
    (void) printf("\tuse syntax hq9plus: %d\n",
                  options.use_syntax_hq9plus);
    (void) printf("\tuse mod255: %d\n",
                  options.use_mod255);
    (void) printf("\tuse force rn: %d\n",
                  options.use_force_rn);
    (void) printf("\tMAX FILENAME LENGTH: %d\n",
                  MAX_FILE_NAME_LENGTH);
    (void) printf("\tSTATIC CELL COUNT: %d\n",
                  STATIC_CELL_COUNT);
}

/* -------------------------------------------------------------------------- */
/* Function: read_config                                                      */
/* Description: */
/* Parameters: */
/* Return: */
/* Note: */
/* -------------------------------------------------------------------------- */
void read_config(void) {
    FILE* cfg_file = NULL;
    char str_param[1024];
    char* lexem;

    if(strlen(options.config_filename)) {
        if((cfg_file = fopen(options.config_filename, "r")) == NULL) {
            perror("File not open");
            return;
        }

        while(!feof(cfg_file)) {
            if(fgets(str_param, 1024, cfg_file)) {
                if(strlen(str_param)) {
                    if(*str_param != '#') {
                        lexem = strtok(str_param, ": ");
                        if(!strcmp(lexem, PARAM_NAME_USE_COMMENT_TYPE1)) {
                            lexem = strtok('\0', " \r\n");
                            if(lexem) {
                                if(!strcmp(lexem, "true")) {
                                    options.comment.comment_flags.use_type1 = 1;
                                }
                                else {
                                    options.comment.comment_flags.use_type1 = 0;
                                }
                            }
                        }
                        else if(!strcmp(lexem, PARAM_NAME_USE_COMMENT_TYPE2)) {
                            lexem = strtok('\0', " \r\n");
                            if(lexem) {
                                if(!strcmp(lexem, "true")) {
                                    options.comment.comment_flags.use_type2 = 1;
                                }
                                else {
                                    options.comment.comment_flags.use_type2 = 0;
                                }
                            }
                        }
                        else if(!strcmp(lexem, PARAM_NAME_USE_COMMENT_TYPE3)) {
                            lexem = strtok('\0', " \r\n");
                            if(lexem) {
                                if(!strcmp(lexem, "true")) {
                                    options.comment.comment_flags.use_type3 = 1;
                                }
                                else {
                                    options.comment.comment_flags.use_type3 = 0;
                                }
                            }
                        }
                        else if(!strcmp(lexem, PARAM_NAME_USE_COMMENT_TYPE4)) {
                            lexem = strtok('\0', " \r\n");
                            if(lexem) {
                                if(!strcmp(lexem, "true")) {
                                    options.comment.comment_flags.use_type4 = 1;
                                }
                                else {
                                    options.comment.comment_flags.use_type4 = 0;
                                }
                            }
                        }
                        else if(!strcmp(lexem, PARAM_NAME_USE_INFINITE_CELLS)) {
                            lexem = strtok('\0', " \r\n");
                            if(lexem) {
                                if(!strcmp(lexem, "true")) {
                                    options.use_infinite_cells = 1;
                                }
                                else {
                                    options.use_infinite_cells = 0;
                                }
                            }
                        }
                        else if(!strcmp(lexem, PARAM_NAME_USE_INFINITE_NESTED_LOOPS)) {
                            lexem = strtok('\0', " \r\n");
                            if(lexem) {
                                if(!strcmp(lexem, "true")) {
                                    options.use_infinite_nested_loops = 1;
                                }
                                else {
                                    options.use_infinite_nested_loops = 0;
                                }
                            }
                        }
                        else if(!strcmp(lexem, PARAM_NAME_USE_NEGATIVE_VALUE)) {
                            lexem = strtok('\0', " \r\n");
                            if(lexem) {
                                if(!strcmp(lexem, "true")) {
                                    options.use_negative_value = 1;
                                }
                                else {
                                    options.use_negative_value = 0;
                                }
                            }
                        }
                        else if(!strcmp(lexem, PARAM_NAME_USE_LARGE_CELL_SIZE)) {
                            lexem = strtok('\0', " \r\n");
                            if(lexem) {
                                if(!strcmp(lexem, "true")) {
                                    options.use_large_cell_size = 1;
                                }
                                else {
                                    options.use_large_cell_size = 0;
                                }
                            }
                        }
                        else if(!strcmp(lexem, PARAM_NAME_USE_FAST_INPUT)) {
                            lexem = strtok('\0', " \r\n");
                            if(lexem) {
                                if(!strcmp(lexem, "true")) {
                                    options.use_fast_input = 1;
                                }
                                else {
                                    options.use_fast_input = 0;
                                }
                            }
                        }
                        else if(!strcmp(lexem, PARAM_NAME_USE_PROCEDURE)) {
                            lexem = strtok('\0', " \r\n");
                            if(lexem) {
                                if(!strcmp(lexem, "true")) {
                                    options.use_procedure = 1;
                                }
                                else {
                                    options.use_procedure = 0;
                                }
                            }
                        }
                        else if(!strcmp(lexem, PARAM_NAME_USE_SYMBOL_EQUAL)) {
                            lexem = strtok('\0', " \r\n");
                            if(lexem) {
                                if(!strcmp(lexem, "true")) {
                                    options.use_symbol_equal = 1;
                                }
                                else {
                                    options.use_symbol_equal = 0;
                                }
                            }
                        }
                        else if(!strcmp(lexem, PARAM_NAME_USE_SYMBOL_UNDER)) {
                            lexem = strtok('\0', " \r\n");
                            if(lexem) {
                                if(!strcmp(lexem, "true")) {
                                    options.use_symbol_under = 1;
                                }
                                else {
                                    options.use_symbol_under = 0;
                                }
                            }
                        }
                        else if(!strcmp(lexem, PARAM_NAME_USE_SYNTAX_HQ9PLUS)) {
                            lexem = strtok('\0', " \r\n");
                            if(lexem) {
                                if(!strcmp(lexem, "true")) {
                                    options.use_syntax_hq9plus = 1;
                                }
                                else {
                                    options.use_syntax_hq9plus = 0;
                                }
                            }
                        }
                        else if(!strcmp(lexem, PARAM_NAME_USE_MOD255)) {
                            lexem = strtok('\0', " \r\n");
                            if(lexem) {
                                if(!strcmp(lexem, "true")) {
                                    options.use_mod255 = 1;
                                }
                                else {
                                    options.use_mod255 = 0;
                                }
                            }
                        }
                        else if(!strcmp(lexem, PARAM_NAME_USE_FORCE_RN)) {
                            lexem = strtok('\0', " \r\n");
                            if(lexem) {
                                if(!strcmp(lexem, "true")) {
                                    options.use_force_rn = 1;
                                }
                                else {
                                    options.use_force_rn = 0;
                                }
                            }
                        }
                    }
                }
            }
        }

        fclose(cfg_file);
    }
}

/* -------------------------------------------------------------------------- */
/* Function: is_command                                                       */
/* Description: */
/* Parameters: */
/* Return: */
/* Note: */
/* -------------------------------------------------------------------------- */
int is_command(code_t ch) {
	return (ch == '>' ||
			ch == '<' ||
			ch == '+' ||
			ch == '-' ||
			ch == '.' ||
			ch == ',' ||
			ch == '[' ||
			ch == ']');
}

/* -------------------------------------------------------------------------- */
/* Function: control                                                          */
/* Description: */
/* Parameters: */
/* Return: */
/* Note: */
/* -------------------------------------------------------------------------- */
int control(void) {
	if(!options.source_filename[0]) {
		return -1;
	}

	return 0;
}

/* -------------------------------------------------------------------------- */
/* Function: work                                                             */
/* Description: */
/* Parameters: */
/* Return: */
/* Note: */
/* -------------------------------------------------------------------------- */
int work(void) {
	FILE* file_code = NULL;
	code_t code = 0;
	mode_t mode = unknown_mode;
	cell_p cells = NULL;
	cell_p current_cell = NULL;
	index_t current_index = 0;
    loop_position_t loops_pos[1024];
    int loops_pos_index = -1;
    int local_loops_pos_index = 0;

    (void) memset(loops_pos, 0, 1024 * sizeof(loop_position_t));

    if(options.show_info) {
        print_show_information();
    }

    cells = (cell_p) calloc(STATIC_CELL_COUNT, sizeof(cell_t));
	if(!cells) {
		perror("Memory error");
		return EXIT_FAILURE;
	}

	if((file_code = fopen(options.source_filename, "r")) == NULL) {
		perror("File not open");
		return EXIT_FAILURE;
	}

	if(options.verbose) {
		(void) printf("Verbose mode!\n");
		(void) printf("* - command\n");
		(void) printf("symbol - current readable symbol (HEX)\n");
		(void) printf("ccn - current cell number (HEX)\n");
		(void) printf("ccv - current cell value (HEX/OCT/DEC)\n");
		(void) printf("----------------------------------------\n");
	}

	current_cell = cells;
	current_index = 0;
	mode = command_mode;
	while((code = fgetc(file_code)) != EOF) {
		if(options.verbose) {
			(void) printf("%c \'%c\' symbol=0x%02X; ccn=%ld; ccv=0x%04lX; ccv=0%05lo; ccv=%li;\n",
                          ((comment_mode != mode && is_command(code)) ? '*' : ' '),
						  (isprint(code) ? code : ' '),
						  (int) code,
						  (unsigned long int)(current_cell - cells),
						  (unsigned long int) *current_cell,
						  (unsigned long int) *current_cell,
						  (signed long int) *current_cell);
		}

		if(comment_mode == mode) {
            if((options.comment.comment_flags.use_type1 && code == '|') ||
               (options.comment.comment_flags.use_type2 && code == '}') ||
               (options.comment.comment_flags.use_type3 && code == '*') ||
               (options.comment.comment_flags.use_type4 && code == '#')) {
				mode = command_mode;
                continue;
			}
		}
		if(command_mode == mode) {
			switch(code) {
			case '|':
                if(options.comment.comment_flags.use_type1) {
					mode = comment_mode;
				}
				break;
			case '}':
                if(options.comment.comment_flags.use_type2) {
					mode = comment_mode;
				}
				break;
			case '*': 
                if(options.comment.comment_flags.use_type3) {
					mode = comment_mode;
				}
				break;
			case '#':
                if(options.comment.comment_flags.use_type4) {
					mode = comment_mode;
				}
				break;
			case '>':
				++current_cell;
				++current_index; /* for verbose mode */
				break;
			case '<':
				--current_cell;
				--current_index; /* for verbose mode */
				break;
			case '+':
				++*current_cell;
				break;
			case '-':
				--*current_cell;
				break;
			case '.':
				if(options.verbose) {
					fputc('O', stdout);
					fputc('>', stdout);
					fputc(' ', stdout);
				}

				(void) fputc(*current_cell, stdout);

				if(options.verbose) {
					fputc('\n', stdout);
				}
				break;
			case ',':
				if(options.verbose) {
					fputc('I', stdout);
					fputc('>', stdout);
					fputc(' ', stdout);
				}

				*current_cell = (int) fgetc(stdin);
				break;
			case '[':
				if(!*current_cell) {
                    if(loops_pos_index >= 0 && loops_pos[loops_pos_index].id_pos == ftell(file_code)) {
                        fsetpos(file_code, &loops_pos[loops_pos_index].end_pos);
                        loops_pos_index--;
                        break;
                    }

                    local_loops_pos_index = 0;

                    while((code = fgetc(file_code)) != ']' || local_loops_pos_index) {
                        if(code == '[') {
                            local_loops_pos_index++;
                        }
                        else if(code == ']') {
                            local_loops_pos_index--;
                        }
					}
				}
                else {
                    /* Save */
                    if(loops_pos_index < 0 || loops_pos[loops_pos_index].id_pos != ftell(file_code)) {
                        loops_pos_index++;
                        fgetpos(file_code, &loops_pos[loops_pos_index].begin_pos);
                        loops_pos[loops_pos_index].id_pos = ftell(file_code);
                    }
                }
				break;
			case ']':
				if(*current_cell) {
                    fgetpos(file_code, &loops_pos[loops_pos_index].end_pos);
                    fsetpos(file_code, &loops_pos[loops_pos_index].begin_pos);
                    fseek(file_code, -1, SEEK_CUR);
				}
                else {
                    loops_pos_index--;
                }
				break;
			default:
				break;
			}
		}
	}

	fclose(file_code);

	if(cells) {
		free(cells);
		cells = NULL;
	}

	return EXIT_SUCCESS;
}

/* -------------------------------------------------------------------------- */
/* Function: main                                                             */
/* Description: main function                                                 */
/* Parameters: argc - */
/*             argv - */
/* Return: EXIT_SUCCESS - success; EXIT_FAILURE - failure                     */
/* Note: none                                                                 */
/* -------------------------------------------------------------------------- */
int main(const int argc, char* const* argv) {
    const char* short_options = "c:f:svqplhVa";
	const struct option long_options[] = {
        { "config",         required_argument, NULL, 'c' },
        { "file",           required_argument, NULL, 'f' },
        { "show-info",      no_argument,       NULL, 's' },
        { "verbose",        no_argument,       NULL, 'v' },
        { "quiet-exit",     no_argument,       NULL, 'q' },
        { "print-preamble", no_argument,       NULL, 'p' },
        { "license",        no_argument,       NULL, 'l' },
        { "help",           no_argument,       NULL, 'h' },
        { "version",        no_argument,       NULL, 'V' },
        { "authors",        no_argument,       NULL, 'a' },
        { NULL,             0,                 NULL,  0  }
	};
	int result_option = 0;
	int index_option = 0;
	extern char* optarg; /* in getopt.h */

	if(atexit(atexit_func)) {
		perror("atexit error");
		return EXIT_FAILURE;
	}	

    (void) memset(&options, 0, sizeof(program_options_t));

	if(argc > 1) {
		while((result_option = getopt_long(argc, argv, short_options, long_options, &index_option)) != -1) {
			switch(result_option) {
            case 'c':
                (void) strncpy(options.config_filename, optarg, MAX_FILE_NAME_LENGTH);
                break;
            case 'f':
                (void) strncpy(options.source_filename, optarg, MAX_FILE_NAME_LENGTH);
                break;
            case 's':
                options.show_info = 1;
                break;
            case 'v':
                options.verbose = 1;
                break;
            case 'q':
                options.quiet_exit = 1;
                break;
            case 'p':
                options.print_preamble = 1;
                break;
            case 'l':
                options.print_license = 1;
                break;
			case 'h':
                options.print_help = 1;
                break;
			case 'V':
                options.print_version = 1;
                break;
			case 'a':
                options.print_author = 1;
                break;
			case '?':
			default:
				print_use_help();
				return EXIT_SUCCESS;
			}
		}
	}
	else {
        print_use_help();
		return EXIT_SUCCESS;
	}

    if(options.print_license) {
        print_license();
        return EXIT_SUCCESS;
    }
    else if(options.print_help) {
        print_help();
        return EXIT_SUCCESS;
    }
    else if(options.print_author) {
        print_author();
        return EXIT_SUCCESS;
    }
    else if(options.print_version) {
        print_version();
        return EXIT_SUCCESS;
    }

    if(options.print_preamble) {
        print_preamble();
    }

    read_config();

	if(control()) {
		return EXIT_FAILURE;
	}

	return work();
}

#if defined(__cplusplus)
/*}*/
#endif /* defined(__cplusplus) */

/* ************************************************************************** */
/* End of file                                                                */
/* ************************************************************************** */
