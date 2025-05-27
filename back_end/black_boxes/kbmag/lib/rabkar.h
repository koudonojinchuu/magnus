#ifndef BLACKBOX_RABKAR_H
#define BLACKBOX_RABKAR_H

void rk_init(int maxeqns);
void rk_reset(int maxeqns);
void rk_clear();
void rk_add_lhs(int n);
void slow_rws_reduce_rk(char *w);
boolean slow_check_rws_reduce_rk(char *w, int i);

#endif
