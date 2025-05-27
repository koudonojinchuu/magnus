#ifndef BLACKBOX_FSATRIPLES_H
#define BLACKBOX_FSATRIPLES_H

#include "rws.h"

fsa * fsa_triples(fsa *waptr, fsa *diffptr, storage_type op_table_type, boolean destroy, char *tempfilename, reduction_equation *eqnptr, int maxeqns, boolean idlabel, boolean eqnstop, boolean readback);

#endif
