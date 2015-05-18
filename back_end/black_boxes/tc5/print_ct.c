/* File print_ct.c */
#include "print_ct.h"
#define CT_T(i,j)	ct[(i-1) * NCOL + j + 1]
Void
tc_print_ct(n1, n2, n3, tc_pvar)
Int	n1;
Int	n2;
Int	n3;
struct  tc_str_vars     *tc_pvar;
{
/* #include "tc_void.h" */
char    gen_l;
Int     gen_n;
Int     i,j;
Int     *ct;
Int	cosrepl;
Int	row1;
Int	row2;
Int	step;
Int 	o;

/* subroutine to write out the coset table. 
*  when n is positive, prints the first n rows of the coset table.
*  when n=0 prints all of the table.
*  when n is negative, prints the first n rows of the table with the
*  corresponding coset representation.
*/
/* print the head of the coset table */
        ct = COSET_TABLE;
        fprintf(fout,"      ");
        if (Gen_st[1].gen == '0')
                for(i = 1; i <= NCOL; i++) {
                        gen_n = COL_TO_GEN[i];
                        fprintf(fout,"    %d ",gen_n);
                }
        else
                for(i = 1; i <= NCOL; i++) {
                        gen_n = COL_TO_GEN[i];
                        if (gen_n < 0) {
                                gen_l = Gen_st[-gen_n].gen;
                                fprintf(fout,"    %c ",toupper(gen_l));
                        } else {
                                gen_l = Gen_st[gen_n].gen;
                                fprintf(fout,"     %c ",gen_l);
                        }
                }
        fprintf(fout,"\n");
/*  print coset table. if n < 0 allocate COSREP for cosets representative. */
	if (n1 < 0) {
		free(COSREP);
		free(COSREP_A);
		COSREP = (int *) eemalloc(sizeof(int) * (MST_LEVEL + 1));
		COSREP_A = (char *) eemalloc(MST_LEVEL + 1);
	}
	if (n1 == 0) {
		row1 = 1;
		row2 = NALIVE;
		step = 1;
	} else if ((iabs(n2)) > NALIVE) {
		row1 = iabs(n1);
		row2 = NALIVE;
		step = iabs(n3);
	} else {
		row1 = iabs(n1);
                row2 = iabs(n2);
		step = iabs(n3);
	}

        for(i = row1; i <= row2; i += step) {
                fprintf(fout,"%5d.",i);
/* prints out row i. */
                for(j = 1; j <= NCOL; j++)
			fprintf(fout,"%7d", CT_T(i,j));
		fprintf(fout,"\n");
		if (n1 < 0 && i != 1) {
/* prints out the representative of coset i. call tc_cosrep() to get coset
*  representative which is stored in COSREP[] and tc_cosrep() returns
*  the length of the coset representative. o will bring back the order of 
* the coset representative, but we ignore it here.
*/
			cosrepl = tc_cosrep(i, &o, tc_pvar);
			{  PrintCosetRep;  }
		}
	}
}
/* End File */
tc_sc(n, tc_pvar)
Int     n;
struct  tc_str_vars     *tc_pvar;
{
char    gen_l;
Int     gen_n;
Int     i,j;
Int     *ct;
Int     cosrepl;
Int	n1;
Int  	sc_flag;
Int	o;

	ct = COSET_TABLE;
	sc_flag = 0;
/* print the head of the coset table 
        fprintf(fout,"      ");
        if (Gen_st[1].gen == '0')
                for(i = 1; i <= NCOL; i++) {
                        gen_n = COL_TO_GEN[i];
                        fprintf(fout,"    %d ",gen_n);
                }
        else
                for(i = 1; i <= NCOL; i++) {
                        gen_n = COL_TO_GEN[i];
                        if (gen_n < 0) {
                                gen_l = Gen_st[-gen_n].gen;
                                fprintf(fout,"    %c ",toupper(gen_l));
                        } else {
                                gen_l = Gen_st[gen_n].gen;
                                fprintf(fout,"     %c ",gen_l);
                        }
                }
        fprintf(fout,"\n");
*/
/*  print coset table. if n <= 0 allocate COSREP for cosets representative. */
        if (n <= 0) {
                free(COSREP);
		free(COSREP_A);
                COSREP = (int *) eemalloc(sizeof(int) * (MST_LEVEL + 1));
		COSREP_A = (char *) eemalloc(MST_LEVEL + 1);
        }
        if (n == 0 || n > NALIVE)
                n1 = NALIVE;
        else
                n1 = iabs(n);
/* print stabilising cosets.  */
        for (i = 2; i <= n1; i++) {
                if (0 == tc_normal(i, tc_pvar))
                        continue;
/* i is stabilising.  */
		if (sc_flag == 0) {
			fprintf(fout," Stabilising cosets:\n  %d ", i);
			sc_flag = 1;
		} else 
			fprintf(fout,"  %d ",i);
/*
                for(j = 1; j <= NCOL; j++)
                        fprintf(fout,"%7d", CT_T(i,j));
                fprintf(fout,"\n");
*/
                if (n <= 0){
/* prints out the representative of coset i. call tc_cosrep() to get coset
*  representative which is stored in COSREP[] and tc_cosrep() returns
*  the length of the coset representative. o will bring back the order of
* the coset representative, but we ignore it here.
*/
                        cosrepl = tc_cosrep(i, &o, tc_pvar);
			{  PrintCosetRep;  }
                }
        }
	fprintf(fout,"\n");
	if (sc_flag == 0)
		fprintf(fout," No stabilising cosets.\n");
}
tc_cycles(tc_pvar)
struct  tc_str_vars     *tc_pvar;
{
#define  LINEWIDTH	76
register Int i;
register Int j;
register Int t;
register Int *y;
register Int *p;
register Int n;
register Int length;
register Int k;
register Int kn;
register Int *ct;
char	tem[30];
/* print out the coset table in cycle (permutation representation). */

	ct = COSET_TABLE;
	for (j = 1; j <= NDGEN; j++) {
/* find the column corresponding to generators.  */
		k = GEN_TO_COL[j];
		if (Gen_st[1].gen == '0') 
			fprintf(fout,"%d=",j);
		else
			fprintf(fout,"%c=",Gen_st[j].gen);
		length = 2;
		for (i = 1; i <= NALIVE; i++) {
/* is i an one_cycle?  */
			if (CT_T(i, k) == i) {
				CT_T(i, k) = -i;
				continue;
			}
/* have we used coset i in previous cycle?  */
			if (CT_T((kn = i),k) <= 0)
				continue;
/* no, trace out this cycle.  */
			sprintf(tem,"%d",kn);
			n = strlen(tem);
			length += n;
			if (length + 1 >= LINEWIDTH) {
                                fprintf(fout,"\n  ");
                                length = 2 + n;
                        }
/* SER, printf( replaced by fprintf(fout, */
			fprintf (fout,"(%d",kn);
			length++;
			t = CT_T(kn, k);
/* mark this coset number as be used previously.  */
                        CT_T(kn, k) = -t;
                        kn = t;
			while (CT_T(kn,k) > 0) {
				sprintf(tem,"%d",kn);
				n = strlen(tem);
				length += n + 1;
				if (length > LINEWIDTH) {
					fprintf(fout,",\n  %d",kn);
					length = n + 3;
				} else 
					fprintf(fout,",%d",kn);
				t = CT_T(kn, k);
/* mark this coset number as be used previously.  */
				CT_T(kn, k) = -t;
				kn = t;
			}
/* we reached the end of the cycle. */
			fprintf(fout,")");
			length++;
		}
		if (length == 2) 
			fprintf(fout,"(1)\n");
		else
			fprintf(fout,"\n");
	}
/* change all negative values in the coset table back to positive values; */
	/*  y = &CT(Num2Addr(COSET1), 1);
	p = &CT(Num2Addr(NEXTDF), 1);   BUGGY CODE SER ?? */
	y = &CT(COSET1, 1);
	p = &CT(NEXTDF, 1);
	for (; y < p; y++)
		*y = (*y < 0) ? -*y : *y;
}	
tc_o(o, tc_pvar)
Int	o;
struct  tc_str_vars     *tc_pvar;
{
register Int    j;
register Int    i;
register Int    cosrepl;
Int    order;
/* find a coset with order iabs(o) mod the subgroup and print out its
*  coset representative.
*  if o < 0 all cosets of order iabs(o) are printed.
*  if o == 0, print an ordered list of all orders found in the 
*  	 coset table, mod the subgroup.
*/
	if (INDEX1) {
		fprintf(fout,"INDEX 1\n");
		return;
	}
	if (CTFLG == 0) {
		fprintf(fout,"NO COSET TABLE\n");
		return;
	}
	if (STDCT == 0) {
/* get the standard coset table.  */
		tc_compact(tc_pvar);
                tc_ctrenumber(tc_pvar);
        }
	if (MSTFLG == 0) 
		tc_mst(tc_pvar); /* construct ms tree. */
/* allocate COSREP for cosets representative. */
        free(COSREP);
	free(COSREP_A);
        COSREP = (int *) eemalloc(sizeof(int) * (MST_LEVEL + 1));
	COSREP_A = (char *) eemalloc(MST_LEVEL + 1);
	for (i = 2; i <= NALIVE; i++) {
/* get the coset representative and its length. order will return the order
*  of the coset representative.
*/
		cosrepl = tc_cosrep(i, &order, tc_pvar);	
		if (o == 0) {
/* print an ordered list of all orders found in the
*        coset table, mod the subgroup.
*/ 
			fprintf(fout,"%d. Order=%d\n", i, order);
			continue;
		} 
		if (iabs(o) == order) {
/* print the satisfied cosets. */
			fprintf(fout,"%d. Order=%d\n", i, order);
			{ PrintCosetRep; }
			if (o > 0)
				break;
		}
	}
}	
