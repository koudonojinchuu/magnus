#ifndef BLACKBOX_FSAWA_H
#define BLACKBOX_FSAWA_H

fsa *fsa_wa(fsa *fsaptr, storage_type op_table_type, boolean destroy, char *tempfilename);
fsa *fsa_wa_short(fsa *fsaptr, storage_type op_table_type, boolean destroy, char *tempfilename);
fsa *fsa_wa_int(fsa *fsaptr, storage_type op_table_type, boolean destroy, char *tempfilename); // Changed table_struc to storage_type

#endif
