#ifndef BLACKBOX_RWSIO2_H
#define BLACKBOX_RWSIO2_H

void read_kbinput_simple(FILE *rfile, boolean check);
void read_gens(FILE *rfile);
void read_inverses(FILE *rfile);
void read_eqns_simple(FILE *rfile, boolean check);
void rws_clear(rewriting_system *rwsptr);

#endif
