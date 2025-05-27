#ifndef BLACKBOX_FSACHECKMULT_H
#define BLACKBOX_FSACHECKMULT_H

#include "rws.h"

int fsa_checkmult(fsa *multptr, reduction_equation *eqnptr, int maxeqns);
int fsa_checkmult_short(fsa *multptr, reduction_equation *eqnptr, int maxeqns);
int fsa_checkmult_int(fsa *multptr, reduction_equation *eqnptr, int maxeqns);

#endif
