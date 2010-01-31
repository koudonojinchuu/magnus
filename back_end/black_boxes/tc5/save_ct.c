/* file save_ct.c  */
#include "sys/time.h"
#include "tc.h"
#include "tcyacc.h"
#include <stdio.h>
#include <string.h>
char *
tc_save(tc_pvar)
struct tc_str_vars	*tc_pvar;
{
	FILE    *tcout, *fopen();
        Int     i,j;
        Rel_stk_type *ptt;
	struct timeval *tp,tpp; /* time of day */
        struct timezone *tzp, tzpp;
	long	betime;
        char    *tcdate,out_file[50];
	Int *y;
	Int end, c_tab,info_space;
	Int *p;
/* subroutine to save a minimum amount of information about 
*  an enumeration into a file so that
*  user will be able to recover it and resume this enumeration.
*  the enumeration definition in a structure pointed by pt, the 
*  enumeration processing information in a structure pointed by
*  tc_pvar and the coset table plus information in the back of y
*  array  are saved.
*  return the output file name.
*/
/* get hour:minute for suffix of output file.  */
	y = Y;
	tp = &tpp;
        tzp = &tzpp;
	while (gettimeofday(tp, tzp) < 0);/* get time of day */
	betime = tp->tv_sec;
	tcdate = ctime(&betime);
	tcdate += 11;
	if (*tcdate == ' ')
		tcdate++;
	strcpy(out_file,Enum_name);
	strcat(out_file,".");
	strncat(out_file,tcdate,5);
/* now out_file is in format: Enum_name.hour:minute  */
	tcout = fopen(out_file, "w");
	if (tcout == (FILE *) 0) {
		yyerror(7);
		return out_file;
	}
	fprintf(fout, "This enumeration information is saved in %s\n",out_file);
/* compute the minimum workspace needed for this enumeration and save
*  it in the first 2 places of the output file.
*/
	c_tab = NALIVE * NCOL + 2;
	info_space = (&y[BACKY] - DED_END);
	fprintf(tcout,"%d %d\n",c_tab,info_space);
	fprintf(tcout,"%d %s \n",strlen(Enum_name),Enum_name);
/* write out generators  */
	fprintf(tcout,"%d ",Gen_no);
	if (Gen_st[1].gen == '0') {
		fprintf(tcout,"0\n");
/* write out the relator number and total length. */
		fprintf(tcout,"%d %d\n",Rel_no,Rel_len);
/* write out relators  */
		for (ptt = Relators; ptt; ptt = ptt->next) {
/* write out length of ptt->rel_1 */
			fprintf(tcout,"%d ",ptt->len_1);
/* write out the number representation of relators. */
			for (j = 1; j <= ptt->len_1; j++)
				fprintf(tcout,"%d ",ptt->rel_1[j]);
			fprintf(tcout,"\n");
/* write out the length of ptt->rel_n.  */
			fprintf(tcout,"%d ",ptt->len);
/* write out the internal representation of relators. */
			for (j = 0; j < ptt->len; j++)
                                fprintf(tcout,"%d ",ptt->rel_n[j]);
			fprintf(tcout,"\n");
		}
/* write out subgroup generators (for making a nonempty string put 'A') */
		fprintf(tcout,"A%s\n",Subg_name);
		fprintf(tcout,"%d %d\n",Sgen_no, Sgn_len);
		for (ptt = S_gens; ptt; ptt = ptt->next) {
                        fprintf(tcout,"%d ",ptt->len_1);
                        for (j = 1; j <= ptt->len_1; j++)
                                fprintf(tcout,"%d ",ptt->rel_1[j]);
                        fprintf(tcout,"\n");
                        fprintf(tcout,"%d ",ptt->len);
                        for (j = 0; j < ptt->len; j++)
                                fprintf(tcout,"%d ",ptt->rel_n[j]);
                        fprintf(tcout,"\n");
		}
	} /* end of number generators */
	  else {
/* put "1 " out to tell it's letter generators.  */
		fprintf(tcout,"1 ");
		for (i = 1; i <= Gen_no; i++)
			fprintf(tcout,"%c",Gen_st[i].gen);
		fprintf(tcout,"\n");
/* write out the relator number and the total length.  */
		fprintf(tcout,"%d %d\n",Rel_no,Rel_len);
/* write out relators one by one.  */
		for (ptt = Relators; ptt; ptt = ptt->next) {
			fprintf(tcout,"%d ",ptt->len); /* length of ptt->rel */
/* write out the letter representation of relators. */
			for (j = 0; j < ptt->len; j++)
				fprintf(tcout,"%c",ptt->rel[j]);	
			fprintf(tcout,"\n");
			fprintf(tcout,"%d ",ptt->len); /* length of ptt->rel_n*/
/* write out the internal representation of relators. */
			for (j = 0; j < ptt->len; j++)
                                fprintf(tcout,"%d ",ptt->rel_n[j]);
			fprintf(tcout,"\n");
		}
/* write out subgroup name.(for making a nonempty string put 'A') */
                fprintf(tcout,"A%s\n",Subg_name);
/* write out the subgroup generator number, and total length.  */
                fprintf(tcout,"%d %d\n",Sgen_no, Sgn_len);
/* write out subgroup generators, one by one.  */
                for (ptt = S_gens; ptt; ptt = ptt->next) {
                        fprintf(tcout,"%d ",ptt->len);
                        for (j = 0; j < ptt->len; j++)
                                fprintf(tcout,"%c",ptt->rel[j]);
                        fprintf(tcout,"\n");
                        fprintf(tcout,"%d ",ptt->len);
                        for (j = 0; j < ptt->len; j++)
                                fprintf(tcout,"%d ",ptt->rel_n[j]);
                        fprintf(tcout,"\n");
                }
	}
/* write out other parameters  */
	fprintf(tcout,"%d %d %d %d %d %d %f\n",Compact,Max_coset,Rel_in_sgp, Fill_factor, F_type, A_factor,Time);
	fprintf(tcout,"%d %d %d\n",Rel_len,Sgn_len,Message_interval);
/* write tc_pvar  */
	fprintf(tcout,"%d %d %d\n",VeryFirst,NCOL,KN_Fel);
        fprintf(tcout,"%d %d %d %d\n",KN_Hlt,NEXTDF,MSGLVE,LVE);
        fprintf(tcout,"%d %d %d %d %d %d\n",NDGEN,NSGPG,NDREL,NALIVE,MAXCOS,TOTCOS);
        fprintf(tcout,"%d %d\n",STDCT,MAXROW);
        fprintf(tcout,"%d %d %d\n",INDEX,RELATORS_IN_SUBGROUP,FILL_FACTOR);
        fprintf(tcout,"%d\n",TABROW);
        fprintf(tcout,"%d\n",CTFLG);

/* write out y array  */
	y = Y;
	end = c_tab + FRONTY;
	for (i = FRONTY; i <= end; i++)
		fprintf(tcout,"%d ",y[i]);
	fprintf(tcout,"\n");
	for (p = RELATORS; p < PDL_ROW; p++)
		fprintf(tcout,"%d ",*p);
	fprintf(tcout,"\n");
	fclose(tcout);
	return out_file;
}
tc_restore(tc_pvar)
struct tc_str_vars       *tc_pvar;
{
/* subroutine to restore all information about a enumeration so that
*  user will be able to recover it and resume this enumeration.
*  the enumeration definition in a structure pointed by pt, the
*  enumeration processing information in a structure pointed by
*  tc_pvar and the coset table plus information in the back of y
*  array  are saved.
*  if the current workspace is inadequate to accommordate the restored
*  enumeration, a message will be printed out and the current enumeration 
*  information is kept as it was. 
*/
	FILE    *tcout, *fopen();
	Int	i,j;
	Rel_stk_type *ft, *ptt;
	Int *y, oworkspace;
	Int end, c_tab, info, info_space;
	Int *p;

	if ((tcout = fopen(FILE_NAME, "r")) == (FILE *) 0) {
                yyerror(7);
                return;
        }
	fscanf(tcout,"%d %d", &c_tab, &info_space);
	if (c_tab + info_space > Workspace) {
		printf("workspace is inadequate for restoring, restoring failed.\n");
		fclose(tcout);
		return;
	}
	init_pointer(tc_pvar);
	free_space_r();
	free_space_s();
/* free the space used by prior enumeration  */
	Relators = (Rel_stk_type *) 0;
	S_gens = (Rel_stk_type *) 0;	
	fscanf(tcout,"%d", &i);
	if (i)
		fscanf(tcout,"%s",Enum_name);
	else
		strcpy(Enum_name,"");
/* read i generators  */
	free(Gen_st);
        fscanf(tcout,"%d %d",&Gen_no,&i);
        Gen_st = (Gen_stk_type *) eemalloc((Gen_no + 1)*sizeof(Gen_stk_type));
        if (i == 0) {
/* number generators.  */
/* assume it is not involutory */
		for (i = 1; i <= Gen_no; i++)
                        Gen_st[i].gen_inv = -i;
		Gen_st[1].gen = '0';
/* read relators represented by numbers  */
                fscanf(tcout,"%d %d",&Rel_no,&Rel_len);
		for (i = 1; i <= Rel_no; i++) {
			ptt = (Rel_stk_type *) eemalloc(sizeof(Rel_stk_type));
			if (i == 1) {
				ft = ptt;
				ptt->next = Relators;
				Relators = ptt;
			} else { 
				ptt->next = ft->next;
				ft->next = ptt;	
			}
                        fscanf(tcout,"%d ",&ptt->len_1);
			ptt->rel_1 = (int *) eemalloc(sizeof(int) * (ptt->len_1 + 1));
                        for (j = 1; j <= ptt->len_1; j++)
                                fscanf(tcout,"%d ",&ptt->rel_1[j]);
                        fscanf(tcout,"%d ",&ptt->len);
			ptt->rel_n = (int *) eemalloc(sizeof(int) * (ptt->len));
                        for (j = 0; j < ptt->len; j++)
                                fscanf(tcout,"%d ",&ptt->rel_n[j]);
			ptt->exp = 1;
                }
/* read in subg name. */
                fscanf(tcout,"%s",Subg_name);
/* delete the 'A' ;  */
		strcpy(Subg_name,(Subg_name+1));
/* read in subg number and total length */
                fscanf(tcout,"%d %d",&Sgen_no, &Sgn_len);
/* read in subgroup generators  */
		for (i = 1; i <= Sgen_no; i++) {
/* allocate space for a subg gen.  */
                        ptt = (Rel_stk_type *) eemalloc(sizeof(Rel_stk_type));
			if (i == 1) {
/* connect first one to the subg gen pointer. */
                                ft = ptt;
                                ptt->next = S_gens;
				S_gens = ptt;
                        } else {
                                ptt->next = ft->next;
                                ft->next = ptt;
                        }
                        fscanf(tcout,"%d ",&ptt->len_1);
/* allocate space for this original subg gen. */
                        ptt->rel_1 = (int *) eemalloc(sizeof(int) * (ptt->len_1
+ 1));
/* read in this original subg gen. */
                        for (j = 1; j <= ptt->len_1; j++)
                                fscanf(tcout,"%d ",&ptt->rel_1[j]);
                        fscanf(tcout,"%d ",&ptt->len);
/* allocate space for the internal representation of this subg gen.*/
                        ptt->rel_n = (int *) eemalloc(sizeof(int) * (ptt->len));
/* read in the internal representation of this subg gen.*/
                        for (j = 0; j < ptt->len; j++)
                                fscanf(tcout,"%d ",&ptt->rel_n[j]);
			ptt->exp = 1;
                }
        } /* end of number generators */
          else {
                for (i = 0; i <= Gen_no; i++) {
                        fscanf(tcout,"%c",&Gen_st[i].gen);
			Gen_st[i].gen_inv = -i;
		}
/* read in relators represented by letters  */
/* read in relators number and total length */
                fscanf(tcout,"%d %d",&Rel_no,&Rel_len);
/* read in relators */
                for (i = 1; i <= Rel_no; i++) {
                        ptt = (Rel_stk_type *) eemalloc(sizeof(Rel_stk_type));
                        if (i == 1) {
/* connect first one to the relator pointer Relators.  */
                                ft = ptt;
                                ptt->next = Relators;
                                Relators = ptt;
                        } else {
                                ptt->next = ft->next;
                                ft->next = ptt;
				ft = ptt;
                        }
                        fscanf(tcout,"%d ",&ptt->len);
/* allocate space for the letter representation of relators. */
                        ptt->rel = (char *) eemalloc(sizeof(int) * (ptt->len + 1));
                        fscanf(tcout,"%s ",ptt->rel);
                        fscanf(tcout,"%d ",&ptt->len);
/* allocate space for the internal representation of relators. */
                        ptt->rel_n = (int *) eemalloc(sizeof(int) * (ptt->len));
                        for (j = 0; j < ptt->len; j++)
                                fscanf(tcout,"%d ",&ptt->rel_n[j]);
			ptt->exp = 1;
                }
/* read in subg name. */
/* delete the 'A'   */
                fscanf(tcout,"%s",Subg_name);
                strcpy(Subg_name,(Subg_name+1));
/* read in subg number and total length */
                fscanf(tcout,"%d %d",&Sgen_no, &Sgn_len);
/* read in subgroup generators  */
                for (i = 1; i <= Sgen_no; i++) {
/* allocate space for a subg gen.  */
                        ptt = (Rel_stk_type *) eemalloc(sizeof(Rel_stk_type));
                        if (i == 1) {
/* connect first one to the subg gen pointer. */
                                ft = ptt;
                                ptt->next = S_gens;
                                S_gens = ptt;
                        } else {
                                ptt->next = ft->next;
                                ft->next = ptt;
                        }
                        fscanf(tcout,"%d ",&ptt->len);
/* allocate space for this original subg gen. */
                        ptt->rel = (char *) eemalloc(sizeof(int) * (ptt->len + 1));
/* read in this original subg gen. */
                        fscanf(tcout,"%s ",ptt->rel);
                        fscanf(tcout,"%d ",&ptt->len);
/* allocate space for the internal representation of this subg gen.*/
                        ptt->rel_n = (int *) eemalloc(sizeof(int) * (ptt->len));
/* read in the internal representation of this subg gen.*/
                        for (j = 0; j < ptt->len; j++)
                                fscanf(tcout,"%d ",&ptt->rel_n[j]);
			ptt->exp = 1;
                }
        } /* end of letter generators */

/* read in other parameters  */
	fscanf(tcout,"%d %d %d %d %d %d %f",&Compact,&Max_coset,&Rel_in_sgp, &Fill_factor, &F_type, &A_factor,&Time);
        fscanf(tcout,"%d %d %d",&Rel_len, &Sgn_len, &Message_interval);
/* read in  tc_pvar  */
	fscanf(tcout,"%d %d %d",&VeryFirst,&NCOL,&KN_Fel);
        fscanf(tcout,"%d %d %d %d",&KN_Hlt,&NEXTDF,&MSGLVE,&LVE);
        fscanf(tcout,"%d %d %d %d %d %d",&NDGEN,&NSGPG,&NDREL,&NALIVE,&MAXCOS,&TOTCOS);
        fscanf(tcout,"%d %d",&STDCT,&MAXROW);
        fscanf(tcout,"%d %d %d",&INDEX,&RELATORS_IN_SUBGROUP,&FILL_FACTOR);
        fscanf(tcout,"%d",&TABROW);
        fscanf(tcout,"%d",&CTFLG);
/* read in y array  */
	y = Space;
	FRONTY = 1;
	BACKY = Workspace - 1;
	end = FRONTY + c_tab;
        for (i = FRONTY; i <= end; i++)
                fscanf(tcout,"%d ",&y[i]);
/* put parameters into tc_pvar partly */
	COMP_PCT = Compact;
	F_TYPE_DEF = F_type;
	A_FACTOR = A_factor;
	REL_LEN = Rel_len;
	SGEN_LEN = Sgn_len;
/* pointer configuration  */
	PDL_COL = &y[BACKY - 256]; 
        PDL_ROW = PDL_COL - 256; 
        GEN_TO_COL = PDL_ROW - 1 - NDGEN; 
        INV_TO_COL = GEN_TO_COL - 3 * NDGEN - 1; 
        COL_TO_GEN = INV_TO_COL - 2 * NDGEN - 1; 
        SUBG_LENGTH = COL_TO_GEN - NSGPG - 3; 
        SUBG_INDEX = SUBG_LENGTH - NSGPG - 1; 
        SUBGROUP_GEN = SUBG_INDEX - Sgn_len - 1; 
        RELATOR_EXPONENT = SUBGROUP_GEN - NDREL - 1; 
        RELATOR_LENGTH = RELATOR_EXPONENT - NDREL - 1; 
        RELATOR_INDEX = RELATOR_LENGTH - NDREL - 1; 
        RELATORS = RELATOR_INDEX - 2 * Rel_len - 1; 
        EDP = RELATORS - 2 * Rel_len - 2; 
        EDP_END = EDP - 2 * NDGEN - 2; 
        EDP_BEG = EDP_END - 2 * NDGEN - 1; 
        DED_END = EDP_BEG - 2; 
	DED_PT = DED_END;
	
        COSET_TABLE = &y[FRONTY];
	for (p = RELATORS; p < PDL_ROW; p++)
		fscanf(tcout,"%d",p); 
	fclose(tcout);
	return;
}
init_pointer(tc_pvar)
struct tc_str_vars       *tc_pvar;
{
int	i;
#define Init_(n)        for(i=0;i<2;i++) n[i]=0; n[2] = 1
	Sgen_no = 0;
        Rel_no = 0;
	Compact = 0;
        Time = 0.000000;
	Init_(Max);
        Init_(Ct);
        Init_(Rt);
	Init_(Fi);
        Init_(RelInSg);
        Message_interval = 0;
	Restart = 0;
	Newrel = 0;
        Newsg = 0;
	Page_strategy = 's';
	Unsort = 's';
/* initializt tc_pvar  */
	INDEX1 = FALSE;
	DEDWNG = FALSE;
	OVERFL = FALSE;
	COMPCT = TRUE;
	NEWREL = 0;
	NEWSG = 0;
	MST_LEVEL = 0;
	MSTFLG = 0;	
}
			
