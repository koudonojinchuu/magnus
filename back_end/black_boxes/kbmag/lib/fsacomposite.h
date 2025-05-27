#ifndef BLACKBOX_FSACOMPOSITE_H
#define BLACKBOX_FSACOMPOSITE_H

fsa  *fsa_genmult2(fsa *genmultptr, storage_type op_table_type, boolean destroy, char *genmult2filename, boolean readback);
fsa  *fsa_genmult2_short(fsa *genmultptr, storage_type op_table_type, boolean destroy, char *genmult2filename, boolean readback);
fsa  *fsa_genmult2_int(fsa *genmultptr, storage_type op_table_type, boolean destroy, char *genmult2filename, boolean readback);
fsa  *fsa_genmult3(fsa *genmultptr, fsa *genmult2ptr, storage_type op_table_type, boolean destroy, char *genmult3filename, boolean readback);
fsa  *fsa_genmult3_short(fsa *genmultptr, fsa *genmult2ptr, storage_type op_table_type, boolean destroy, char *genmult3filename, boolean readback);
fsa  *fsa_genmult3_int(fsa *genmultptr, fsa *genmult2ptr, storage_type op_table_type, boolean destroy, char *genmult3filename, boolean readback);
void  fsa_makemult(fsa *genmultptr, int g);
void  fsa_makemult2(fsa *genmult2ptr, int g1, int g2);
fsa  *fsa_composite(fsa *mult1ptr, fsa *mult2ptr, storage_type op_table_type, boolean destroy, char *compfilename, boolean readback);
fsa  *fsa_composite_short(fsa *mult1ptr, fsa *mult2ptr, storage_type op_table_type, boolean destroy, char *compfilename, boolean readback);
fsa  *fsa_composite_int(fsa *mult1ptr, fsa *mult2ptr, storage_type op_table_type, boolean destroy, char *compfilename, boolean readback);

#endif
