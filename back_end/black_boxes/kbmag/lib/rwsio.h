#ifndef BLACKBOX_RWSIO_H
#define BLACKBOX_RWSIO_H

void initialise_reduction_fsa();
void initialise_eqns();
void read_eqns(FILE *rfile, boolean check);
void read_done(FILE *rfile);
void read_kbinput(FILE *rfile, boolean check);
void read_extra_kbinput(FILE *rfile, boolean check);
void print_kboutput(FILE *wfile);
void print_wdoutput(FILE *wfile, char *suffix);

#endif
