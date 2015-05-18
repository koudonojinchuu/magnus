/*
 * Include file which MUST be included by every tc source file. 
 */
#include "stdio.h"
#ifndef __TC_VARS__
#define __TC_VARS__
#include "defs.h"
extern FILE *fout;

struct tc_str_vars {
/*---points[]---*/
Int	*y_array;
Int	veryfirst;
Int	*coset_table;
Int	ncol;
Int	fronty;
Int 	kn_fel;
Int 	kn_hlt;
Int 	nextdf;
Int	msglve;
Int	lve;
Int	*ded_pt;
Int	*ded_end;
Int	*gencol;
Int	*colgen;
Int	*invcol;
Int	deadde;
Int	backy;
Int	ndgen;
Int	nsgpg;
Int	ndrel;
Int	holes;
Int	nalive;
Int	maxcos;
Int	totcos;
Int	oldnal;
Int	intnal;
Int	stdct;
Int	maxrow;
Int	page_strategy;
Logical	index1;
Int	dedwng;
Logical	overfl;
Logical	index;
Int	strategy;
Logical	compct;
/*---look[]----*/
Int	relators_in_subgroup;
Int	fill_factor;
Float	ltime;
Int	chead;
Int	ctail;
/*---ermflg[]--*/
Float	fntime;
/*---new-------*/
Int	*relator_index;
Int	*relator_exponent;
Int	*relator_length;
Int	*relators;
Int	*edp;
Int	*edp_end;
Int	*edp_beg;
Int	*subgroup_gen;
Int	*subg_index;
Int	*subg_length;
Int 	*pdl_col;
Int 	*pdl_row;
Int	rel_len;
Int	sgen_len;
Int	comp_pct;
Int	f_type_def;
Int	a_factor;
char	*status;
Int	diag[10];
/* definitions for options */
Int	newrel;
Int	newsg;
Int	mstflg;
Int	mst_level;
Int 	*mst;
Int	*cosrep;
char 	*cosrep_a;
Int	ctflg;
char	in_f[50];
Int	tabrow;
};
#endif  

#ifndef __TC_DEFINES__
#define __TC_DEFINES__

/*------ structure tc_str_vars-------------------*/
#define Y	     tc_pvar->y_array /* pointer to workspace. */
#define COSET_TABLE  tc_pvar->coset_table /* points to the beginning of CT */
#define VeryFirst	tc_pvar->veryfirst /* flag for straight HLT(TRUE,or 1)*/
#define NCOL		tc_pvar->ncol  /* the number of columns in CT */
#define	FRONTY		tc_pvar->fronty /* first position being used in Y  */
/* (for felsch type) coset in the coset table at which the
* search for an undefined coset table entry is to begin. */
#define KN_Fel		tc_pvar->kn_fel 
/* (for HLT type) coset in the coset table at which the
* search for an undefined coset table entry is to begin. */
#define KN_Hlt		tc_pvar->kn_hlt
/* next sequentially available coset. */
#define NEXTDF          tc_pvar->nextdf 
/* the interval of message.  */
#define MSGLVE          tc_pvar->msglve  
#define LVE		tc_pvar->lve
/* the number of dead deductions.  */
#define DEADDE		tc_pvar->deadde
/* the top of the deduction stack  */
#define DED_PT 	      	tc_pvar->ded_pt
/* the bottom of the deduction stack  */
#define DED_END         tc_pvar->ded_end 
/* table of mapping gens to columns */
#define GEN_TO_COL          tc_pvar->gencol  
/* table of mapping columns to gens */
#define COL_TO_GEN          tc_pvar->colgen  
/* table of mapping inverses to columns */
#define INV_TO_COL          tc_pvar->invcol  
/* the last position in Y  */
#define BACKY           tc_pvar->backy 
/* the number of gens */
#define NDGEN           tc_pvar->ndgen  
/* the number of subgroup gens.  */
#define NSGPG           tc_pvar->nsgpg  
/* the number of relators */ 
#define NDREL           tc_pvar->ndrel 
#define HOLES		tc_pvar->holes
/* the number of alive cosets  */
#define NALIVE          tc_pvar->nalive   
/* maximum number of cosets active at any time. */
#define MAXCOS          tc_pvar->maxcos 
/* TOTCOS total number of cosets defined. */
#define TOTCOS          tc_pvar->totcos  
/*  OLDNAL INTNAL for message output. */
#define OLDNAL          tc_pvar->oldnal  
#define INTNAL          tc_pvar->intnal   
/* STDCT flag for standard coset table (TRUE or 1), 
* means CT contains coset's number insdead of coset's address in the coset
* table. 
*/
#define STDCT		tc_pvar->stdct
#define MAXROW          tc_pvar->maxrow   
/* flag for get index 1(TRUE or 1)  */
#define INDEX1          tc_pvar->index1  
/* flag for waning deductions being discarded   */
#define DEDWNG          tc_pvar->dedwng  
/* page stategy */
#define PAGE_STRATEGY   tc_pvar->page_strategy   
#define OVERFL          tc_pvar->overfl /* flag for over flow */
#define INDEX           tc_pvar->index  /* flag for finding index */ 
#define COMPCT          tc_pvar->compct  /* flag if CT compacted */ 
#define RELATORS_IN_SUBGROUP           tc_pvar->relators_in_subgroup   
#define FILL_FACTOR          tc_pvar->fill_factor    
#define LTIME		tc_pvar->ltime /* time limit */
#define CHEAD           tc_pvar->chead /* coinc queue haed */ 
#define CTAIL		tc_pvar->ctail /* coinc queue tail  */
#define FNTIME          tc_pvar->fntime  /* final time  */ 
/*******************************************/
#define RELATOR_EXPONENT	tc_pvar->relator_exponent
#define RELATOR_LENGTH	tc_pvar->relator_length
#define RELATOR_INDEX	tc_pvar->relator_index
#define RELATORS 	tc_pvar->relators
#define EDP 		tc_pvar->edp /* essentially different positions */
#define EDP_END 	tc_pvar->edp_end /* EDP end */	
#define EDP_BEG 	tc_pvar->edp_beg /* EDP beginning */	
#define SUBGROUP_GEN 	tc_pvar->subgroup_gen /* number of subgroup gens */
#define SUBG_INDEX 	tc_pvar->subg_index
#define SUBG_LENGTH 	tc_pvar->subg_length
/* preferred definition list for columns */
#define PDL_COL 	tc_pvar->pdl_col 
/* preferred definition list for rows */
#define PDL_ROW 	tc_pvar->pdl_row
#define REL_LEN 	tc_pvar->rel_len /* total length of relators */
#define SGEN_LEN 	tc_pvar->sgen_len /* total length of subg gens */
#define COMP_PCT 	tc_pvar->comp_pct /* compact %  */
#define F_TYPE_DEF 	tc_pvar->f_type_def /* CT factor */
#define A_FACTOR 	tc_pvar->a_factor  /* RT factor */
#define STATUS		tc_pvar->status /* the status of RT or CT */
#define DIAG		tc_pvar->diag /* diagnostic array */
/* definitions for options */
#define NEWREL		tc_pvar->newrel /* number of new relators */
#define NEWSG		tc_pvar->newsg /* number of new subg gens */
#define MST_T		tc_pvar->mst /* minimal spanning tree */
#define MSTFLG		tc_pvar->mstflg /* mst flag */	
#define MST_LEVEL	tc_pvar->mst_level /* levels of mst. */
/* CTFLG is a flag=TRUE(=1) tells there is a coset table, otherwise
* there is no coset table.
*/
#define CTFLG		tc_pvar->ctflg 
#define COSREP		tc_pvar->cosrep /* coset representatives */
#define COSREP_A	tc_pvar->cosrep_a /* cosrep by letters */
#define FILE_NAME		tc_pvar->in_f
#define TABROW		tc_pvar->tabrow
#endif


