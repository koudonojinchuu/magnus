#ifndef BLACKBOX_TC5_SAVECT_H
#define BLACKBOX_TC5_SAVECT_H

void init_pointer(struct tc_str_vars *tc_pvar);
char *tc_save(struct tc_str_vars *tc_pvar);
void tc_restore(struct tc_str_vars *tc_pvar);

#endif
