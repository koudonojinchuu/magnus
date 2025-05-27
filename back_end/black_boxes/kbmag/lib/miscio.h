#ifndef BLACKBOX_MISCIO_H
#define BLACKBOX_MISCIO_H

boolean isdelim(int c);
boolean isvalid(int c);
int read_char(FILE *rfile);
void check_next_char(FILE * rfile, int c); // Added prototype
void read_delim(FILE *rfile, int *delim);
void read_token(FILE *rfile, char *token, int *delim);
void skip_gap_expression(FILE *rfile, int *delim);
void read_ident(FILE *rfile, char *ident, int *delim, boolean inv);
void read_int(FILE *rfile, int *integ, int *delim);
void read_string(FILE *rfile, char *string, int *delim);
void process_names(char **name, int num_names);
void read_word(FILE *rfile, char *gen_word, char *end_word, int *delim, char **name, int num_names, boolean check);
void printbuffer(FILE *wfile);
void add_to_buffer(int n, char *w);
int add_word_to_buffer(FILE *wfile, char *word, char **symbols);
int add_iword_to_buffer(FILE *wfile, int *word, char **symbols);
int int_len(int n);
boolean is_int(char *x);

#endif
