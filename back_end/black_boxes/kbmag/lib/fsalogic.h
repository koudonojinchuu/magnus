#ifndef BLACKBOX_FSALOGIC_H
#define BLACKBOX_FSALOGIC_H

typedef enum {AND, OR, AND_NOT} binop;

/* Functions defined in this file: */
boolean srec_equal(srec *srptr1, srec *srptr2);
boolean table_equal(table_struc *tableptr1, table_struc *tableptr2, int ne, int ns);
boolean fsa_equal(fsa *fsaptr1, fsa *fsaptr2);
fsa *fsa_and(fsa *fsaptr1, fsa *fsaptr2, storage_type op_table_type, boolean destroy, char *tempfilename);
fsa *fsa_or(fsa *fsaptr1, fsa *fsaptr2, storage_type op_table_type, boolean destroy, char *tempfilename);
fsa *fsa_and_not(fsa *fsaptr1, fsa *fsaptr2, storage_type op_table_type, boolean destroy, char *tempfilename);
fsa *fsa_binop(fsa *fsaptr1, fsa *fsaptr2, storage_type op_table_type, boolean destroy, char *tempfilename, binop op);
fsa *fsa_not(fsa *fsaptr, storage_type op_table_type);
fsa *fsa_exists(fsa *fsaptr, storage_type op_table_type, boolean destroy, char *tempfilename);
fsa *fsa_exists_short(fsa *fsaptr, storage_type op_table_type, boolean destroy, char *tempfilename);
fsa *fsa_exists_int(fsa *fsaptr, storage_type op_table_type, boolean destroy, char *tempfilename);
fsa *fsa_greater_than(srec *alphptr);

#endif
