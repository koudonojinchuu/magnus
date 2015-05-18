/* a change is made to compact, its default value is 20, instead of 0,
*  this means that under the default only redandent space is 
*  greater than 20% of the coset table, a compaction is carried out.
*  24/11/1993
*/ 
#include "ctype.h"
#include "tctime.h"
#include "tcstdlib.h"
#include "stdio.h" 
#include "tcyacc.h" 
#include "tc.h"
#include "sys/types.h"
#include "miscellaneous.h"
#include "parser.h"
extern Rel_stk_type *rel_pt;
static Word_pt  word_;
Word_pt *w_pt;
char *progname; 
int numb,lineno; 
Gen_stk_type gen_stk[53]; 
static Enum_def  enumd;
Enum_def *pt;
char 	*infile;	/* input file name */ 
FILE 	*fin; 		/* input file pointer */ 
FILE	*fout;		/* output file pointer  */
int	lineno = 1; 
char	*eemalloc();

/* the printing format of relators and subgens has been changed - add a ',' 
*  at the end of relators and subgens. 10/8/92.
*/
init() /* initiate structure enumb by default value */ 
{ 
#define	Init_(n)	for(i=0;i<2;i++) n[i]=0; n[2] = 1
int 	i;

	strcpy(Enum_name,"");
	Gen_st = (Gen_stk_type *) eemalloc(sizeof(Gen_stk_type));
	Relators = (Rel_stk_type *) 0;
	*Subg_name = ' '; 
	S_gens = (Rel_stk_type *) 0; 
	Sgen_no = 0; 
	Rel_no = 0;
	Pr_l = 0;
	Init_(Pr);
	Compact = 20; 
	Init_(Max);
	Init_(Ct);
	Init_(Rt);
	Time = 0.000000;
	Init_(Fi);
	Init_(RelInSg);
	Message_interval = 0; 
	Workspace = 200000;
	Space = (int *)eemalloc(200000 * sizeof(int)); 
	Restart = 0;
	Sr_l = 0;
	Sa_l = 0;
	Res_l = 0;
	Newrel = 0;
	Newsg = 0;
	Cc_l = 0;
	Cc = 0;
	Sc_l = 0;
	Scn = 0;
	Del_l = 0;
	Tw_l = 0;
	Twn = 0;
	Nc_l= 0;
	Ncn = 0;
	Cy_l = 0;
	Oo_l = 0;
	Oon = 0;
	Rc_l = 0;
	Rcn = 0;
	RcStop = 0;
	RcDesire = 0;
	Page_strategy = 's';
	for (i = 0; i <= 9; i++)
		Diag[i] = 0;
	Unsort = 's';
	fin = stdin;
	fout = stdout;
} 

main(argc, argv) /* enumerate cosets by todd coxeter algorithm */ 
char *argv[]; 
{	progname = argv[0];

	setlinebuf(stdout);
	pt = &enumd; 
	w_pt = &word_;
	init(); 
	tc_time(0);
        run();
	tc_time(1);
	return 0; 
} 

run() /* execute until EOF */ 
{
struct  tc_str_vars     tc_pvar;
int	i,j,k,l,m;

/* initialize pointers of minimal spanning tree and coset representative. */
	tc_pvar.mst = (int *)eemalloc(sizeof(int));
	tc_pvar.cosrep = (int *)eemalloc(sizeof(int));
	tc_pvar.mstflg = 0;
	tc_pvar.ctflg = 0;
	tc_pvar.stdct = 0;
	tc_pvar.status = "";
	tc_pvar.y_array = Space;
	for ( ; tc_parse(); )  {
/* deal with different options first
*  Rl_l: add new relators.
*  Sg_l: add new subg
*  Dr_l: delete relators
*  Ds_l: delete subgroup generators.
*  Cc_l: print out a coset representative and add it back into the
*      subgroup and do the resulting enumeration.
*  Sr_l: print out group name, generators, subg generators, alive cosets,
*      max cosets, total cosets.
*  Sa_l: save the whole enumeration information into a file named 
*      Enum_name.hour:minutes.
*  Res_l: restore an enumeration from a file In_file.
*  Pr_l: print out the coset table and coset representatives.
*  Cr_l: print out the coset representative of coset in Cc and adds it back
*      into the subgroup and does the resulting enumeration.
*  Sc_l: print out the stabilising cosets of the subgroup. 
*  Tw_l: trace a word in rel_pt through the coset table, start at coset Twn.
*      if Twn < 0, trace the word with all cosets and print which coset are 
*      not stabilized. (coset * word != coset)
*  Cy_l: print out the coset table in cycle.  
*  Rc_l: find non trival subgroups in the coset table with index a 
*      multiple of a desired index RcDesire by repeatedly putting cosets 
*      coincident with coset 1 and observing what happens. The first 
*      coset to be put coincident with coset 1 is Rcn and the 
*      previous coset to this coset is used if a favourable result 
*      doesn't occur and this process is repeated until a favourable 
*      result occurs or else we reach coset 1 or coset RcStop.
*      Note that at the end of this option the original coset table
*      will be current and if we wish to work in the coset table resulting
*      from the final repeated coincidence with a suitable finite index then
*      a CC option will have to be done using the last coset put coincidednt
*      with coset 1.
*  Cy_l: print out the coset table in cycle (permutation representation).
*  Oo_l: find a coset with order abs(Oon) mod the subgroup and print out 
*      its coset representative.
*  Nc_l: test for normal closure. if Ncn is not 0, also try to find the 
*      normal closure by adding a set excluded conjugate to the subgroup. 
*/
		if (Del_l != 0) {
/* user foget to end 'DS' or 'DL' with ';' we check if some of relator 
*  and subg gens should be taken off the chains.
*/
			del_r_s(&Relators);
			del_r_s(&S_gens);
			Del_l = 0;
		}
		if (Sr_l != 0) {
			Sr_l = 0;
			i = Message_interval;
			Message_interval = 1;	
			printt();
			tc_text(11, &tc_pvar);
			Message_interval = i;
			continue;
		}
		if (Sa_l) {
			Sa_l = 0;
			tc_compact(&tc_pvar);
			tc_save(&tc_pvar);
			continue;
		}
		if (Res_l) {
			Res_l= 0;
			strcpy(tc_pvar.in_f, In_file);
			tc_restore(&tc_pvar);
/* because of restoring the enumeration information, we have the coset
*  table, but it might not be a standard table and no minimal spanning tree.
*  so we set following flags.
*/
			tc_pvar.ctflg = 1;
			tc_pvar.stdct = 0;
			tc_pvar.mstflg = 0;
			continue;
		}
/* print out the coset table and coset representatives. */
		if (Pr_l) {
			Pr_l = 0;
			if (tc_pvar.ctflg == 0) { /* have coset table? */
				fprintf(fout,"NO COSET TABLE\n");
				continue;
			}
			if (tc_pvar.stdct == 0) { /* standard table?  */
				tc_compact(&tc_pvar);
				tc_ctrenumber(&tc_pvar);
			}
			if (Pr[0] < 0 && tc_pvar.mstflg == 0) 
				tc_mst(&tc_pvar); /* construct ms tree. */
			if (iabs(Pr[0]) > iabs(Pr[1])) {
				fprintf(fout,"WRONG ORDER OF THE FIRST COSET AND THE LAST COSET\n");
				continue;
			}
			tc_print_ct(Pr[0],Pr[1],Pr[2], &tc_pvar);	
			continue;
		}
/* print out the coset representative of coset in Cc and adds it back 
*  into the subgroup and does the resulting enumeration.
*/
		if (Cc_l) {
			Cc_l = 0;
			if (tc_pvar.ctflg == 0) {
                                fprintf(fout,"NO COSET TABLE\n");
                                continue;
                        }
			if (Cc > tc_pvar.nalive) {
				fprintf(fout,"THERE ARE ONLY %d COSETS\n", tc_pvar.nalive);
				continue;
			}
                        if (tc_pvar.stdct == 0) { /* standard table?  */
                                tc_compact(&tc_pvar);
                                tc_ctrenumber(&tc_pvar);
                        }
			if (tc_pvar.mstflg == 0)
                                tc_mst(&tc_pvar); /* construct ms tree. */
			tc_print_ct(-Cc,Cc,1, &tc_pvar);
			tc_add_cc(Cc, &tc_pvar);
			if (tc_pvar.index)
				continue;
			Unsort = 'u';
			Restart = 1;
		}
/*  Cy: print out the coset table in cycle.  */
		if (Cy_l) {
			Cy_l = 0;
			if (tc_pvar.ctflg == 0) {
                                fprintf(fout,"NO COSET TABLE\n");
                                continue;
                        }
			if (!tc_pvar.index) {
                                fprintf(fout,"INCOMPLETE COSET TABLE\n");
                                continue;
                        }
                        if (tc_pvar.stdct == 0) { /* standard table?  */
                                tc_compact(&tc_pvar);
                                tc_ctrenumber(&tc_pvar);
                        }
			tc_cycles(&tc_pvar);
			continue;
		}
/*  Sc: print out the stabilising cosets of the subgroup. */
		if (Sc_l) {
			Sc_l = 0;
			if (tc_pvar.ctflg == 0) {
                                fprintf(fout,"NO COSET TABLE\n");
                                continue;
                        }
			if (tc_pvar.stdct == 0) { /* standard table?  */
                                tc_compact(&tc_pvar);
                                tc_ctrenumber(&tc_pvar);
                        }
			if (tc_pvar.mstflg == 0 && Scn <= 0)
				tc_mst(&tc_pvar); /* construct ms tree. */
			tc_sc(Scn, &tc_pvar);
			continue;
		}
		if (Tw_l) { 
			int *word;
/* trace a word through the coset table , start at coset Tw. */
			if (tc_pvar.ctflg == 0) { /* have coset table? */
                                fprintf(fout,"NO COSET TABLE\n");
                                continue;
                        }
			if (Twn > tc_pvar.nalive) {
                                fprintf(fout,"THERE ARE ONLY %d COSETS\n", tc_pvar.nalive);
                                continue;
                        }
			if (tc_pvar.stdct == 0) { /* standard table?  */
                                tc_compact(&tc_pvar);
                                tc_ctrenumber(&tc_pvar);
                        }
			if (Twn == 0) 
				Twn = 1;
			check_relator(&Pt2Word); /* if there is nongen letter.*/
			relator_by_gennumb(&Pt2Word); 	
			/* allocate space for the word. */
			word = (int *) eemalloc(Pt2Word->len * sizeof(int));
			/* traslate gens into columns of the coset table. */
			for(i = 0; i < Pt2Word->len; i++)
				word[i] = tc_pvar.gencol[Pt2Word->rel_n[i]];
			/* trace out the word, start at coset Twn.  */
			if (Twn > 0) {
			j = tc_tracew(Twn,word,&word[Pt2Word->len-1],&tc_pvar);	
		/* print out 'Twn word j'.  */
			fprintf(fout,"%d*%s = %d\n",Twn,Pt2Word->rel,j);
			} else {
				for (i = 1; i <= tc_pvar.nalive; i++) {
					j = tc_tracew(i,word,&word[Pt2Word->len-1],&tc_pvar);
					if (j != i)
						fprintf(fout,"%d*%s = %d\n",i,Pt2Word->rel,j);
				}
			}
		/* release the space used by TW option.  */
			free(word);
			free(Pt2Word->rel_n);
			free(Pt2Word->rel);
			free(Pt2Word);
			Tw_l = 0;
			Twn = 0;
			continue;
		}
		if (Nc_l) {
			Nc_l = 0;
			if (tc_pvar.ctflg == 0) { /* have coset table? */
                                fprintf(fout,"NO COSET TABLE\n");
                                continue;
                        }
                        if (tc_pvar.stdct == 0) { /* standard table?  */
                                tc_compact(&tc_pvar);
                                tc_ctrenumber(&tc_pvar);
                        }
			tc_normcl(Ncn, &tc_pvar);
			if (Newsg == 0)
				continue;
			Restart = 1;
		}
		if (Oo_l) {
			Oo_l = 0;
			tc_o(Oon, &tc_pvar);
			Oon = 0;
			continue;
		}
		if (Rc_l) {
			Rc_l = 0;
			tc_rc(Rcn, RcStop, RcDesire, &tc_pvar);
			continue;
		}
	    if(Gen_st[1].gen == '0') {
		if (Unsort != 'u')
			{free_reduce_n(); sort();}
	    } else {
		if (Unsort != 'u') {
	 		free_reduce(); /* free reduce and cyclic reduce relators */ 
			sort(); /* sort relator chain in increasing order */ }
		check_relator(&Relators); 
		check_relator(&S_gens); 
			  /* check if there are letters in relators and 
			  * subgroup generators that are not generators 
			  * of this group */ 
		relator_by_gennumb(&Relators); /* replace letters in relators 
						by corresponding subsript
                        			in Gen_st[]. */
		relator_by_gennumb(&S_gens); /* Subg gens.  */ 
	   }
                check_involutory();/* check if a group generator is involutory*/
                get_exp(); /* work out the exponent for every relator and 
			    * subgroup generators */
                get_length(); /* get the total length of relators and
			       * subgrp generators */
                printt(); /* output this enumeration name */
		for (k=Max[0];k<=Max[1];k+=Max[2])
		for (j = RelInSg[0];j <= RelInSg[1];j += RelInSg[2])
		for(i=Fi[0];i<=Fi[1];i+=Fi[2])
		for (l=Ct[0]; l<=Ct[1]; l+=Ct[2])
		for (m=Rt[0]; m<=Rt[1]; m+=Rt[2]) {
			Max_coset = k;
			Rel_in_sgp = j;
			Fill_factor = i;
			F_type = l;
			A_factor = m;
			tc_todd_coxeter(Restart,&tc_pvar);
		}
		Restart = 0;
		if (Message_interval < 0) 
			tc_time(0);
		fprintf(fout,"\n");
	}
} 
free_space_r()
{
/* deallocate the space used by relators */
	Rel_stk_type *relp,*it;

	relp = Relators; 
	for (; relp;) { 
		it = relp;
		relp = relp->next; 
		free(it->rel_n);
		if (it->rel_1)
			free(it->rel_1);
		else 
			free(it->rel); 
		free(it); 
	} 
}
free_space_s()
{
/* deallocate the space used by subgroup generators */
        Rel_stk_type *relp,*it;
 
        relp = S_gens;
        for (; relp;) {
                it = relp;
                relp = relp->next;
                free(it->rel_n);
		if (it->rel_1)
                        free(it->rel_1);
                else  
                        free(it->rel);
                free(it);
        } 
}
print_rel_n(relpp, n)
Rel_stk_type *relpp;
int n;
{
	Rel_stk_type *relp;
	int     i,j,len;
	relp = relpp;
	j = n;
/* print out the first relator if there is one. */
                if (relp != (Rel_stk_type *) 0) {
                        len = relp->len_1 / relp->exp;
                        if (relp->exp != 1)  {
                                j += 3 + (4 + len*2);/* 4 for (..)^x */
                                fprintf(fout,"(");
                        } else  
                                j += len*2 + 3;
/* print first len - 1 charactors of this relator. */
                        for(i = 1; i<len; i++)
                                fprintf(fout,"%d*", relp->rel_1[i]);
/* print the last charactor and terminator - with power or without. */
                        if (relp->exp != 1)
                                fprintf(fout,"%d)^%d", relp->rel_1[i], relp->exp);
                        else 
                                fprintf(fout,"%d", relp->rel_1[i]);
                        relp = relp->next;
                }
/* print out the rest relators. */
                while (relp != (Rel_stk_type *) 0) {
                        len = relp->len_1 / relp->exp;
                        if (relp->exp != 1) { 
                                j += 3 + (4 + len*2);/* 4 for (..)^x */
                                if (j > 72) {
                                        j = 3 + (4 + len*2);
                                        fprintf(fout,",\n   (");
                                } else
					fprintf(fout,",    (");
                        } else { 
                                j += len*2 + 3;
                                if (j > 72) {
                                        fprintf(fout,",\n   ");
                                        j = len*2 + 3;
                                } else
					fprintf(fout,",   ");
                        }
/* print first len - 1 charactors of this relator. */
                        for(i = 1; i<len; i++)
                                fprintf(fout,"%d*", relp->rel_1[i]);
/* print the last charactor and terminator - with power or without. */
                        if (relp->exp != 1)
                                fprintf(fout,"%d)^%d", relp->rel_1[i], relp->exp);
                        else 
                                fprintf(fout,"%d", relp->rel_1[i]);
                        relp = relp->next;
                }
                fprintf(fout,";\n");
}
print_rel(relpp, n)
Rel_stk_type *relpp;
int n;
{
        Rel_stk_type *relp;
        int     i,j,len;
        relp = relpp;
        j = n;
/* print out the first relator if there is one. */
                if (relp != (Rel_stk_type *) 0) {
                        len = relp->len / relp->exp;
                        if (relp->exp != 1)  {
                                j += 3 + (4 + len);/* 4 for (..)^x */
                                fprintf(fout,"(");
                        } else  
                                j += len + 3;
/* print first len - 1 charactors of this relator. */
                        for(i = 0; i<len-1; i++)
                                fprintf(fout,"%c", relp->rel[i]);
/* print the last charactor and terminator - with power or without. */
                        if (relp->exp != 1)
                                fprintf(fout,"%c)^%d", relp->rel[i], relp->exp);
                        else 
                                fprintf(fout,"%c", relp->rel[i]);
			relp = relp->next;
                }
/* print out the rest relators. */
                while (relp != (Rel_stk_type *) 0) {
                        len = relp->len / relp->exp;
/* is this a base^power format?  */
			if (relp->exp != 1)  { /* base^power */
                                j += 3 + (4 + len);/* 4 for (..)^x */
/* too long to fit into this line?  */
				if (j > 72) {
                                        j = 3 + (4 + len);
                                        fprintf(fout,",\n   (");
                                } else
                                        fprintf(fout,", (");
                        } else { 
                                j += len + 3;
                                if (j > 72) {
                                        fprintf(fout,",\n   ");
                                        j = len + 3;
                                } else
                                        fprintf(fout,", ");
                        }
/* print first len - 1 charactors of this relator. */
                        for(i = 0; i<len-1; i++)
                                fprintf(fout,"%c", relp->rel[i]);
/* print the last charactor and terminator - with power or without. */
                        if (relp->exp != 1)
                                fprintf(fout,"%c)^%d", relp->rel[i], relp->exp);
                        else 
                                fprintf(fout,"%c", relp->rel[i]);
                        relp = relp->next;
                }
                fprintf(fout,";\n");
}

printt()
{
        Rel_stk_type *relp; 
        int     i,j,len; 
/* print out the information of the current enumeration  */
/* enumeration name. */
   fprintf(fout,"Enumeration: %s \n", Enum_name);
   if(Message_interval) {
/* group generators. */
	fprintf(fout,"Group Generators: ");
	if(Gen_st[1].gen == '0') { /* for number generators */
		fprintf(fout,"%d;\n",Gen_no);
/* group relators. */
		fprintf(fout,"Group Relators:   ");
		j = 16;
		print_rel_n(Relators, j);
/* subgroup generators */
		fprintf(fout,"Subgroup name: %s\n", Subg_name);
		fprintf(fout,"Subgroup Generators:");
		j = 21;
		print_rel_n(S_gens, j);
	} else { /* for letter generators  */ 
/* group generators and relators */
		fprintf(fout,"%c", Gen_st[1].gen);
        	for(i=2; i <= Gen_no; i++) 
			fprintf(fout,", %c",Gen_st[i].gen);
		fprintf(fout,";\n");
		fprintf(fout,"Group Relators:");
		j = 16;
		print_rel(Relators, j);
/* subgroup generators */
        	fprintf(fout,"Subgroup name: %s\n", Subg_name); 
		fprintf(fout,"Subgroup Generators:");
		j = 21;
		print_rel(S_gens, j);
	}
   }
	if(Message_interval<0) {
/* all parameters */
		fprintf(fout,"           Compact %%:%d;\n",Compact);
		fprintf(fout,"          Max cosets:%d,%d,%d;\n",Max[0],Max[1],Max[2]);
		fprintf(fout,"          CT factor :%d,%d,%d;\n",Ct[0],Ct[1],Ct[2]);
		fprintf(fout,"          RT factor :%d,%d,%d;\n",Rt[0],Rt[1],Rt[2]);
		fprintf(fout,"         Fill factor:%d,%d,%d;\n",Fi[0],Fi[1],Fi[2]);
		fprintf(fout,"Relators in Subgroup:%d,%d,%d;\n",RelInSg[0],RelInSg[1],RelInSg[2]);
		fprintf(fout,"                Time:%f;\n",Time);
		fprintf(fout,"            Messages:%d;\n",Message_interval);
		fprintf(fout,"           Workspace:%d;\n", Workspace);
	}
}  
int get_gen(s) 
char s[]; 
{  /* numb records the number of group generators */ 
	numb++; 
	gen_stk[numb].gen = s[0]; /* put generator in */  
	gen_stk[numb].gen_inv = 0; /* assume it is not involutory */ 
	return numb; 
} 
put_gen() /* allocate space for group generators and move them from 
           * gen_stk into Gen_st. 
           */ 
{ 
	int i; 

	free(Gen_st);
	Gen_st = (Gen_stk_type *) eemalloc((Gen_no + 1)*sizeof(Gen_stk_type));
	if (Gen_st == 0) {
	   fprintf(fout,"out of memory in allocation space for group generators\n");
		exit(1);
	}
	if(gen_stk[1].gen == '0') { 
		for (i = 1; i <= Gen_no; i++)
/* assume it is not involutory */
			Gen_st[i].gen_inv = -i; 
		Gen_st[1].gen = gen_stk[1].gen;
	} else 
		for (i = 1; i <= Gen_no; i++) {	
		      Gen_st[i].gen = gen_stk[i].gen; 
/* assume it is not involutory */  
		      Gen_st[i].gen_inv = -i; 
		}
} 
free_reduce() /* free reduction and cyclic reduction for relators  */ 
{ 
	Rel_stk_type 	*ptr; 
	char		ct[1000], *st; 
	int 		i; 

/* find the longest length of relators to allocate buffer ct. 
        if(Relators)
                i = Relators->len;
        for(ptr = Relators; ptr; ptr=ptr->next)
                if (ptr->len > i)
                        i = ptr->len;
	ct = (char *)eemalloc(i + 1);
	if (ct == 0) {
                fprintf(fout,"out of memory in allocate space for free_reduce()\n");
                exit(1);
        }
*/
	for (ptr = Relators; ptr != (Rel_stk_type *) 0; ptr = ptr->next) 
/* for every relators */  
	{ 
		st = ptr->rel; 
		while (*st != '\0' && INVERSE(*st) == *(st+1)) 
/* delete 'Aa' like things */ 
			st += 2; 
		i = 0;
		if (*st) { 
			ct[++i] = *st; 
			st++; 
			for( ; *st; st++) 	
				if (INVERSE(*st) == *(st+1))
					st++; /* delete *st and *(st+1) */ 
				else if (i > 0 && INVERSE(ct[i]) == *st) 
				 	i--; /* delete ct[i] and *st   */  
				else  
					ct[++i] = *st; 
		} /* end if(*st != '\0') */ 
/* copy ct into ptr->rel */ 
		if (i == 0) {  /* delete this relator and release the storage */ 
			Rel_stk_type *prior,*it; 

			for (prior = it = Relators;it != ptr;it = it->next) 
				prior = it; 	
			if (prior == it) 
				Relators = it->next; 
			else 
				prior->next = it->next; 
			free(it->rel); 
			free(it); 
			ptr = prior; /* as ptr has been feed at this stage. */
			Rel_no--; 
		} else {  /* reduce 'a' and 'A' in relator 'aRA' */ 
			st = ct; 
			for (;INVERSE(st[1]) == st[i] && i>1;i -= 2,st++); 
			st[++i] = '\0'; 
			st++;   /* skip ct[0] */ 
/* replace relator by reduced one */
			if (i-1 != ptr->len) { 
				ptr->rel = (char *) realloc(ptr->rel,i);  
				if (ptr->rel == 0) { 
				     fprintf(fout,"out of memory in realloc()\n");  
                        		exit(1);  
				} 
				strcpy(ptr->rel,st); 
				ptr->len = strlen(st);
			} 
		}
	} 
/* free reduce subg gens  */
	for (ptr = S_gens; ptr != (Rel_stk_type *) 0; ptr = ptr->next)
/* for every subg gens  */
        {
                st = ptr->rel;
                while (*st != '\0' && INVERSE(*st) == *(st+1))
/* delete 'Aa' like things */
                        st += 2;
                i = 0;
                if (*st) {
                        ct[++i] = *st;
                        st++;
                        for( ; *st; st++)
                                if (INVERSE(*st) == *(st+1))
                                        st++; /* delete *st and *(st+1) */
                                else if (i > 0 && INVERSE(ct[i]) == *st)
                                        i--; /* delete ct[i] and *st   */
                                else
                                        ct[++i] = *st;
                } /* end if(*st != '\0') */
/* copy ct into ptr->rel */
                if (i == 0) {  /* delete this subg gens  and release the storage */

                        Rel_stk_type *prior,*it;

                        for (prior = it = S_gens;it != ptr;it = it->next)
                                prior = it;
                        if (prior == it)
                                S_gens = it->next;
                        else
                                prior->next = it->next;
                        free(it->rel);
                        free(it);
                        ptr = prior; /* as ptr has been feed at this stage. */
                        Sgen_no--;
                } else {  
			st = ct;
			st[++i] = '\0';
   			st++;   /* skip ct[0] */
			/* replace subg gen by reduced one */
                        if (i-1 != ptr->len) {
                                ptr->rel = (char *) realloc(ptr->rel,i);
                                if (ptr->rel == 0) {
                                     fprintf(fout,"out of memory in realloc()\n"
);
                                        exit(1);
                                }
                                strcpy(ptr->rel,st);
                                ptr->len = strlen(st);
                        }
		}
	}
	free(ct); 
} 
sort_relators(c) /* sort relators chain in increasing order */ 
char c; 
{ 
	Rel_stk_type *this, *first; 
	Rel_stk_type *it,*prior, *st; 

/* we sort relators chain in the way that original relator chain is now 
 * pointed by 'first' and sorted chain is pointed by 'this'; every time 
 * we find the longest relator in 'first', pick it up, connected to 'this'; 
 * keep doing it until 'first' becomes empty. 
 */ 
	if (c == 'r') 
                first = Relators; 
	else if (c == 'g') 
                first = S_gens;
        else {  fprintf(fout,"error in call sort_relators()\n"); 
                exit(1); 
             } 
	this = (Rel_stk_type *) 0; 
	for(; first;) { /* find the longest one in 'first'. */ 		
		for(it=first,prior=it,st=it; st->next; st=st->next) 
			if(it->len <= st->next->len) { 
				prior = st; 
				it = st->next; 
			} 
		if (prior == it) /* pick it up */ 
			first = it->next; 
		else 
			prior->next = it->next; 
/* connect it to 'this'  */ 
		it->next = this; 
		this = it; 
	} 
	if (c == 'r')  
                Relators = this;
	else 
		S_gens = this; 
} 
sort()  /* sort relators and subgroup generators in increasing order */ 
{ 
	sort_relators('r'); 
	sort_relators('g'); 
} 
check_relator(ptt) 
Rel_stk_type **ptt;
{	
	Rel_stk_type *ptr; 
	char	     *ct; 
	ptr = *ptt;
	for(;ptr != (Rel_stk_type *) 0;ptr=ptr->next) /* for every relators */ 
                for(ct = ptr->rel; *ct != '\0'; ct++) 
			check_gen(*ct); /* check every letter */  
} 
check_gen(c) /* check c to tell if it is a generator of this group */ 
char c; 
{ 
	int i; 
	for(i = 0; i <= Gen_no; i++) 
		if (c == Gen_st[i].gen || INVERSE(c) == Gen_st[i].gen) 
			return; 
	fprintf(fout,"%c is not a generator\n",c);
	exit(1);
} 
relator_by_gennumb(ptt) 
Rel_stk_type **ptt;
{
/* replace letters in relators by corresponding subsript in Gen_st[]. */
        Rel_stk_type *ptr;
        char *ct;
        int  i,j;
	ptr = *ptt;
        for(; ptr != (Rel_stk_type *) 0;ptr=ptr->next) {
/* for every relators */
                ptr->rel_n = (int *) eemalloc(sizeof(int) * ptr->len);
                j = 0;
                for(ct = ptr->rel; *ct != '\0'; ct++) {
                        for(i = 1; i <= Gen_no; i++)
                                if(Gen_st[i].gen == *ct) {
                                        ptr->rel_n[j] = i; 
/* replace gen by i.  */                                       
                                        break;
                                } else if (Gen_st[i].gen == INVERSE(*ct)) {
                                        ptr->rel_n[j] = -i; 
/* -i is the inverse of i. */
                                        break;
                                }
                j++;
                }
        }
}

check_involutory()
{
        Rel_stk_type *ptr;
        int     i;

        for(ptr = Relators; ptr != (Rel_stk_type *) 0;ptr=ptr->next) /* for every relators */
        if(ptr->len == 2 && ptr->rel_n[0] == ptr->rel_n[1]) {
                i = ptr->rel_n[0];
                i = (i>0) ? i : -i;
                Gen_st[i].gen_inv = i;
        }
}
base_exp(ptr) /* get exponent of a relator  */
Rel_stk_type    *ptr;
{
        int     i;
        int     l,j;
        int     adr;
        int     nbasel;
        int     expm1;
        int     baslen;
        int     exp1;
        baslen = ptr->len;
        exp1 = baslen;
    for(;;) {
        if (exp1 <= 1) {
                ptr->exp = exp1;
                return;
        }
/*-exp1- is a possible exponent if it divides the length */
        nbasel = baslen / exp1;
        if (nbasel * exp1 == baslen) {
/*try to split the word into sections */
        	expm1 = exp1 - 1;
        	for (i = 0; i < nbasel; i++) {
                	l = ptr->rel_n[i];
                	adr = i;
                	for (j = 1; j <= expm1; j++) {
                        	adr = adr + nbasel;
                        	if (l != ptr->rel_n[adr])
                                	goto TryAnother;
                	}
        	}
/*found an exponent */
        	ptr->exp = exp1;
        	return;
	}
/* try another exponent */
TryAnother:   ;
        exp1 = exp1 - 1;
    }
}

get_exp() /* get the exponent of a relator.  */
{
        Rel_stk_type *ptr;
        for(ptr = Relators;ptr != (Rel_stk_type *) 0;ptr=ptr->next)
                base_exp(ptr);
	for(ptr = S_gens;ptr != (Rel_stk_type *) 0;ptr=ptr->next)
                base_exp(ptr);
}
get_length() /* get the total length of relators and subgrp generators
                and put they into Rel_len and Sgn_len respectively. */
{
        Rel_stk_type *ptr;
        int     i;

        i = 0;
        for(ptr = Relators;ptr != (Rel_stk_type *) 0;ptr=ptr->next)
                i += ptr->len;
        Rel_len = i;
        i = 0;
        for(ptr = S_gens;ptr != (Rel_stk_type *) 0;ptr=ptr->next)
                i += ptr->len;
        Sgn_len = i;
}
free_reduce_n()
{
        int *ct,*st,i,j;
/* find the longest length of relators to allocate buffer ct. */
	if(Relators)
		i = Relators->len_1;
	for(rel_pt = Relators; rel_pt; rel_pt=rel_pt->next)
		if (rel_pt->len_1 > i)
			i = rel_pt->len_1;
        ct = (int *)eemalloc(sizeof(int) * i + 1);
	if (ct == 0) {
                fprintf(fout,"out of memory in allocate space for free_reduce_n()\n");
                exit(1);
	}
	st = ct;
        for(rel_pt = Relators; rel_pt; rel_pt=rel_pt->next) {
                i = 0;
                for(j =1; j < rel_pt->len_1; j++)
                        if(rel_pt->rel_1[j] + rel_pt->rel_1[j+1])
                                ct[++i] = rel_pt->rel_1[j];
			else j++; /* delete '2*2^-1' like things */
		if(j == rel_pt->len_1)
                        ct[++i] = rel_pt->rel_1[j];
		if (i)
                        for(;ct[1] + ct[i] == 0 && i > 1; i -= 2,ct++);
/* delete 'n' and 'n^-1' in 'nRn^-1'  */
                if (i == 0) { /* delete this relator and release the storage */

                        Rel_stk_type *prior,*it;

                        for (prior = it = Relators;it != rel_pt;it = it->next)
                                prior = it;
                        if (prior == it)
                                Relators = it->next;
                        else
                                prior->next = it->next;
			rel_pt = prior; 
                        free(it->rel_1);
                        free(it);
/* as rel_pt has been freed at this stage, set rel_pt to be prior one */
			Rel_no--;
			ct = st;
			continue;
		}
		rel_pt->rel_n =(int *) eemalloc(sizeof(int) * (i + 1));
                for(j=1; j <= i; j++)
                        rel_pt->rel_n[j-1] = ct[j];
                rel_pt->len = i;
        }
	for(rel_pt = S_gens; rel_pt; rel_pt=rel_pt->next) {
		rel_pt->rel_n=(int *)eemalloc(sizeof(int)*(rel_pt->len_1 +1)); 
		for(j=1; j <= rel_pt->len_1; j++)
			rel_pt->rel_n[j-1] = rel_pt->rel_1[j];
		rel_pt->len = rel_pt->len_1;
	}
	free(ct);
}
add_relators()
{	
	Rel_stk_type *p;
	if (Relators) {
		for (p = Relators; p->next; p = p->next);
		p->next = rel_pt;
	} else
		Relators = rel_pt;
	return;
}
add_sg()
{
	Rel_stk_type *p;
	if (S_gens) {
		for (p = S_gens; p->next; p = p->next);
        	p->next = rel_pt;
	} else 
		S_gens = rel_pt;
        return;
}
tc_add_cc(n, tc_pvar)
int	n;
struct  tc_str_vars     *tc_pvar;
{
	Rel_stk_type *p;
	int length,i;
/* allocate COSREP for cosets representative. */
	free(COSREP);
	free(COSREP_A);
	COSREP = (int *) eemalloc(sizeof(int) * (MST_LEVEL + 1));
	COSREP_A = (char *) eemalloc(MST_LEVEL + 1);
/* get the coset representative and its length. i will return the order
*  of the coset representative, but we ignore it here.
*/
	length = tc_cosrep(n, &i, tc_pvar);
/* allocate space for the new subg generator (eg COSREP). */
	p = (Rel_stk_type *) eemalloc(sizeof(Rel_stk_type));
/* put COSREP into subg generator.  */
	if (Gen_st[1].gen == '0') {
		p->rel_1 = (int *) eemalloc(sizeof(int) * (length + 1));
		p->len_1 = length;
		for (i = 1; i <= length; i++)
			p->rel_1[i] = COSREP[i];
		p->rel = (char *) 0;
	} else {
		p->rel = (char *) eemalloc(length + 1); /* +1 for '\0' */
        	p->len = length;
		for (i = 1; i <= length; i++) 
			p->rel[i - 1] = COSREP_A[i];
		p->rel[i-1] = '\0';
		p->rel_1 = (int *) 0;
	}
	p->next = (Rel_stk_type *) 0;
	p->exp = 1;
	Sgen_no++;
	Newsg = 1;  
	rel_pt = p;
/* connect the new to the chain of subg generators.  */
	INDEX = FALSE;
	add_sg();
/* unrenumber of the coset table  */
	tc_unrenumber(tc_pvar);
        MSTFLG = FALSE;
/* initialize varibles for coinc processing.  */
	MSGLVE = 0;
	DED_PT = DED_END;
/* make n == 1  */
	tc_coinc(1,Num2Addr(n),tc_pvar);
	if (INDEX1) {
               tc_text(12, tc_pvar);
               INDEX = TRUE;
	}
	return;
}	
		
