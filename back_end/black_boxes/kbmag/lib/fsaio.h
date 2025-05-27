#ifndef BLACKBOX_FSAIO_H
#define BLACKBOX_FSAIO_H

void srec_print(FILE *wfile, srec *srptr, char *name, int offset, char *endstring);
void table_print(FILE *wfile, table_struc *tableptr, char *name, int offset, char *endstring, int ns, int ne);
void fsa_print(FILE *wfile, fsa *fsaptr, char *name);
void srec_read(FILE *rfile, srec *srptr, int maxsize);
void table_read(FILE *rfile, table_struc *tableptr, storage_type table_storage_type, int dr, int ns, int maxstates, int ne);
void fsa_read(FILE *rfile, fsa *fsaptr, storage_type table_storage_type, int dr, int maxstates, boolean assignment, char *name);
void compressed_transitions_read(fsa *fsaptr, FILE *rfile);

#endif
