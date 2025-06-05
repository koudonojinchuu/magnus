#ifndef BLACKBOX_TCVOID_H
#define BLACKBOX_TCVOID_H

#include "defs.h"

//apply.c
void tc_apply(int hcoset, int *beg, int *end, Logical defflg, struct tc_str_vars *tc_pvar);
//assemble_edp.c
void tc_assemble_edp(struct tc_str_vars *tc_pvar);
//assemble_gens.c
void tc_assemble_gens(struct tc_str_vars *tc_pvar);
//check_index1.c
void tc_check_index1(struct tc_str_vars *tc_pvar);
//coinc.c
void tc_coinc(Int lows, Int highs, struct tc_str_vars *tc_pvar);
//coinc_cols12.c
void tc_coinc_cols12(int lows, int highs, struct tc_str_vars *tc_pvar);
//compact.c
void tc_compact(struct tc_str_vars *tc_pvar);
//ctrenumber.c
void tc_ctrenumber(struct tc_str_vars *tc_pvar);
void tc_unrenumber(struct tc_str_vars *tc_pvar);
//enum.c
void tc_enum(Int restart, struct tc_str_vars *tc_pvar);
//norelator.c
void tc_norelator(struct tc_str_vars *tc_pvar);
//print_ct.c
void tc_o(Int o, struct tc_str_vars *tc_pvar;
void tc_cycles(struct tc_str_vars *tc_pvar);
void tc_print_ct(Int n1, Int n2, Int n3, struct tc_str_vars *tc_pvar);
void tc_sc(Int n, struct tc_str_vars *tc_pvar);
//proc_ded.c
void tc_proc_ded(struct tc_str_vars *tc_pvar);
//set_relators.c
void tc_set_relators(struct tc_str_vars *tc_pvar);
//subg_proc.c
void tc_subg_proc(struct tc_str_vars *tc_pvar);
void tc_newrel_newsg(struct tc_str_vars *tc_pvar);
//text.c
void tc_text(Int arg, struct tc_str_vars *tc_pvar);
//time.c
void tc_time(int end_enum);
float utl_second();
//translate.c
void tc_translate(struct tc_str_vars *tc_pvar);
//todd_coxeter.c
void tc_todd_coxeter(Int restart, struct tc_str_vars *tc_pvar);

#endif
