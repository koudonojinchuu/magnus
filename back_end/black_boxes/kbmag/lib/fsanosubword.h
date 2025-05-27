#ifndef BLACKBOX_FSANOSUBWORD_H
#define BLACKBOX_FSANOSUBWORD_H

fsa *fsa_nosub_exists(fsa *fsaptr, storage_type op_table_type, boolean destroy, char *tempfilename);
fsa *fsa_nosub_exists_short(fsa *fsaptr, storage_type op_table_type, boolean destroy, char *tempfilename);
fsa *fsa_nosub_exists_int(fsa *fsaptr, storage_type op_table_type, boolean destroy, char *tempfilename); // Changed table_struc to storage_type as op_table_type is an enum

#endif
