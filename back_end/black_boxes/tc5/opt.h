#ifndef BLACKBOX_TC5_OPT_H
#define BLACKBOX_TC5_OPT_H

#include "defs.h"

Int tc_cosrep(Int coseth, Int *order, struct tc_str_vars *tc_pvar);
void tc_mst(struct tc_str_vars *tc_pvar);
Int tc_normal(Int n, struct tc_str_vars *tc_pvar);
void tc_normcl(Int parmtr, struct tc_str_vars *tc_pvar);
void tc_rc(Int coset, Int stop, Int desire, struct tc_str_vars *tc_pvar);
Int tc_tracew(Int n, Int *beg, Int *end, struct tc_str_vars *tc_pvar);

#endif
