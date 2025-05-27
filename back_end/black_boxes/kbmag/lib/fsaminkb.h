#ifndef BLACKBOX_FSAMINKB_H
#define BLACKBOX_FSAMINKB_H

fsa *fsa_minred(fsa *waptr, storage_type op_table_type, boolean destroy, char *tempfilename);
fsa *fsa_minkb(fsa *minredptr, fsa *waptr, fsa *diffptr, storage_type op_table_type, boolean destroy, char *tempfilename);
fsa *fsa_diff1c(fsa *minkbptr, storage_type op_table_type);

#endif
