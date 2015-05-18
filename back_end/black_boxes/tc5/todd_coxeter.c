/* File todd_coxeter.c */
#include "todd_coxeter.h"

Void
tc_todd_coxeter(restart,tc_pvar)
Int 	restart;
struct  tc_str_vars     *tc_pvar;
{
#include "tc_void.h"
register Int	*y;
register Int	spsize;
register Float	sttime;
Float		utl_second();
register Int	MAXTAB;
register Int	nmax;
register Int	relator_space;
register Int	subg_gen_space;
register Int	i;
/*
* todd-coxeter version 4.05 March 1992
* designed by george havas, key center for software technologe
* the department of computer science, UQ
* programmed by kate j. x. lian based on version 2.3 in Fortran
* this program provides a package for the enumeration of cosets
* of a subgroup of a finitely presented group and for manipulation
* of partial and complete coset tables
* for further reference see: cannon, dimino, havas, watson,
* "implementation and analysis of the todd-coxeter algorithm",
* mathematics of computation, 27(123), 1973, pp. 463-490.
* also see: cannon, havas,
* "implementation and application of the todd-coxeter algorithm",
* in preparation.
* y -- array set aside for the column translation table for each
* 	generator, (GEN_TO_COL, COL_TO_GEN, INV_TO_COL), subgroup 
*	generators, group relators, the columns for each essentially 
*	different position in each group relator for each group 
*  	generator, and the coset table. note that the relator is 
*	translated into the associated column numbers in the coset 
*	table. the coset table is stored physically at the front
* 	of y and all other information in y is initially assembled at the
* 	front of y and then shifted to the back of y.
* FRONTY, BACKY --  position within y of the beginning and ending of
* 	available space in y for use by the algorithms.
* NSGPG -- number of subgroup generators.
* NDREL -- number of group defining relators.
* NDGEN -- number of group defining generators.
* ncol -- number of columns needed for the group defining generators.
* 	note that an involution occupies only 1 column whilst a
* 	non-involutary generator occupies 2 columns in the coset table.
* GEN_TO_COL -- pointer to the middle of the column translation table for
* 	each generator, which is set up in such a way that 
*	GEN_TO_COL(generator) gives us the column in the coset table of a
* 	generator, whilst GEN_TO_COL(-generator) gives us the column in the
* 	coset table of the inverse of a generator. this table is of length
* 	2*NDGEN+1.
* COL_TO_GEN -- table of length ncol which translates each column of the
* 	coset table back into its generator number (or its inverse).
* INV_TO_COL -- table of length ncol which translates a column number of the
* 	coset table into its inverse column number of the coset table.
* RELATORS --group defining relators in full length and every relator is
*         twice for convenience of scan word.
* RELATOR_LENGTH---length of relators. RELATOR_LENGTH[i] gives length of i
*         relator (original one , not two times).
* RELATOR_INDEX---index of relators. RELATOR_INDEX[i] gives the
*         beginning position of ith relator in array RELATORS.
* EDP_BEG---index of essentially different positions for each generator.
*         EDP_BEG[i] gives beginning positions in array edp for generator i.
* EDP---essentially different positions for each relator for generators.
* STDCT -- standard table flag. it is TRUE when cosets in the coset 
*	table are represented by themsevles, FALSE cosets are represented by
*	their addresses in the coset table. during an enumeration
*	processing cosets are represented by their addresses, and are
*	converted to themselves when necessary - such as a requirement
*	of printing out the coset table.
* MSTFLG -- minimum spanning tree flag. TRUE a minimum spanning tree
*	has been built for the current coset table, otherwise FALSE.
* CTFLG -- coset table flag. TRUE there is a coset table, otherwise
*	FALSE.
*/
	if (restart && STDCT) {
		tc_unrenumber(tc_pvar);
	}
	STDCT = FALSE; 
	MSTFLG = FALSE;
	CTFLG = TRUE;
	Trans_Parameter;
        sttime = utl_second();
/* compute the size of the room needed by this enumeration. */
        spsize = Other_Space + Pdl_Space + Edp_Space;
         if (Rel_no != 0)
                spsize = spsize + (relator_space);
	if (Sgen_no != 0)
                spsize = spsize + (subg_gen_space);
        if ( spsize > Workspace) {
         fprintf(fout,"Workspace= %d is too small for this enumeration\n",Workspace);
                exit(0);
       } else if (MAXTAB != 0) {
         spsize += 2 * NDGEN * iabs(MAXTAB);
         if (spsize > Workspace) { 
		fprintf(fout,"Workspace= %d is inadequate for Max cosets= %d\n",Workspace, MAXTAB);
		exit(0);
	 }
       } 
        spsize = Workspace;
/* Workspace is the size of room, and Space is an integer pointer that points 
*  to the room. in the Trans_Parameter, Y = Space;
*/
        FRONTY = 1;
        BACKY = spsize - 1;
	y = Y;
	Assign_space;
/* put relators and subgroup gens into y array. */
	tc_set_relators(tc_pvar);
/* set COL_TO_GEN, GEN_TO_COL and INV_TO_COL   */ 
	tc_assemble_gens(tc_pvar);
/* convert relators and subgroup gens represented by group gens into
*  a internal format in which group gens are represented by
*  associated colunm mumbers in the coset table.
*/
	tc_translate(tc_pvar);
/* assemble EDP and set EDP_BEG and EDP_END. */
	tc_assemble_edp(tc_pvar);
	if(FILL_FACTOR == 0) 
		FILL_FACTOR = (5 * (NCOL +2)) / 4;
/* compute the max rows fit into the current condition - space and 
*  user definitions. 
*/
	Set_maxrow;
/* apply current cosets to new relators and subgroup gens 
*  if necessary.  
*/
	if (restart && (NEWREL || NEWSG)) { 
		tc_newrel_newsg(tc_pvar);
		NEWREL = 0;
		NEWSG = 0;
		if (INDEX)
			return;
	}
/* call tc_enum() to compute coset table.  */
	tc_enum(restart,tc_pvar);
/* overflow?  */
	Check_overflow;
/* output execution time.  */
	Execution_time;
	return;
}
/* End File  */
