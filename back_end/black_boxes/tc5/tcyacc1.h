/* Include file which is included by tcyacc source file.
 */
#include "stdio.h"
extern FILE *fout;
#ifndef val_stack 
#define val_stack 
typedef struct { int *numbs; int lengh;  } Num_gen_word;
typedef union  {
        int   vali;  /* for integer value such as integer power */
	char  gen[2];
        char  *words; /* char pointer for letter-gen word */
	Num_gen_word   *nums;  /* int pointer for number-gen word  */  
	} YYSTYPE;
extern YYSTYPE yylval; 
# define YYSTYPE_IS_DECLARED 1
#endif 

#ifndef ___DEF__
#define ___DEF__
/* define a macro for inverse */  
#define	INVERSE(c)	(islower((c))? toupper((c)): tolower((c))) 
#define BUF_LEN		100	

/*-------define stacks for group generators relators------*/ 
typedef struct {char gen; int gen_inv; } Gen_stk_type;
typedef struct rel_stk_type {char *rel; /* original and reduced rel.(in letters) A represents inverse of a, starting at rel[0] */ 
		int *rel_1; /* original rel (in numbers), starting at rel_1[1]  */
		int len_1; /*  length of *rel_1  */
		int *rel_n; /* rel (in gens numbers,-i represents inverse of i. starting rel_[0]*/
                int len; /* relator length of *rel(reduced) and *rel_n*/ 
		int exp; /* exponent of rel. */
                struct rel_stk_type *next; 
        } Rel_stk_type;  

/*--enum_def is used to communication between user interface and TC program--*/ 
typedef struct enum_def { /*enumeration definition table */ 
	char name[BUF_LEN];	/* enumeration name */
	Gen_stk_type *gen;	/* group generators */ 
	int  gen_no;	/* the number of group generators */ 
	Rel_stk_type *relators;  /* group relators */
	int rel_no;	/* the number of group relations */
	int rel_len;
	char s_name[BUF_LEN];	/* subgroup name */ 
	Rel_stk_type *s_gens;  /* subgroup generators */ 
	int sgen_no; 
	int sgn_len;
	int compact;
	int max_coset;
        int max[3]; 
	int ct[3];
	int f_type;
	int rt[3];
	int a_factor;
        float  time; 
        int fill_factor; 
        int fi[3]; 
        int rel_in_sgp; 
        int rel_in_sg[3]; 
	int diag[10]; 
        int message_interval; /* according to 0,1,2,3,... print whole or parts 
                      * procedures of enumerations 
                      */ 
	int workspace; 
	int *space;
	int page_strategy;
	int unsort;
/*  definitions for options  */
	int restart;
        int sr_l;
	int sa_l;
	char in_file[50];
	int res_l;
	int newrel;
	int newsg;
	int pr_l;
	int pr[3];
	int cc_l;
	int cc;
	int sc_l;
	int scn;
	int del_l;
	int tw_l;
	int twn;
	int nc_l;
	int ncn;
	int cy_l;
	int oo_l;
	int oon;
	int rc_l;
	int rcn;
	int rcstop;
	int rcdesire;
       } Enum_def; 
#endif 


