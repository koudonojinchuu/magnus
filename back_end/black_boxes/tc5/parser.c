#include "stdio.h"
#include "parser.h"
#include "y.tab.h"
#include "tcyacc.h"
extern	FILE    *fin, *fout;
extern  Word_pt *w_pt, word_;
extern	Gen_stk_type gen_stk[];
#define	ExecErrorInt	execerror("syntax error, expect an integer","")
#define ExecErrorDtm	execerror("syntax error, expect ';'","")
int
tc_parse()
{
	int c;
	for(; c = tclex();) {
		switch (c) {
		case AI:	 /* switch to another input file. */ 
				follow_ai();
				break;
		case AO:	/* switch to another output file */
				follow_ao();
				break;
		case BYE:	fprintf(fout,"Good Bye;\n");
                                /* exit() */
				return 0;
		case CC:	c = yylex();
				if (c != TOKEN_INT) 
					ExecErrorInt;
				Cc = yylval.vali;
				Cc_l = 1;
				if((c = yylex()) != ';')
					ExecErrorDtm;
				return 1;
		case COM:	c = tclex();
                                if (c != TOKEN_INT)
					ExecErrorInt;
				Compact = yylval.vali;
				if((c = tclex()) != ';')
					ExecErrorDtm;
				break;
		case CONT:	Restart = 1;
				break;
		case CT:	follow(Ct);	
				break;
		case CY:	if((c = tclex()) != ';' && c != '\n')
					ExecErrorDtm;
				Cy_l = 1;
				return 1;
		case DI:	diagnostic(yylval.words);
				break;
		case DR:	follow_dr_ds(&Relators);	
				break;
		case DS:	follow_dr_ds(&S_gens);
				break;
		case ENUM:	strcpy(Enum_name, yylval.words);
				break;
		case END:	return 1;
		case FI:	follow(Fi);	
				break;
		case GEN:	yyparse();
				Sgen_no = NumOfWord;
                                S_gens = Pt2Word;
				break;
		case GR:	follow_gen();
				break;
		case MAX:	follow(Max);	
				break;
		case ME:	c = yylex();
                                if (c != TOKEN_INT)
					ExecErrorInt;
				Message_interval = yylval.vali;
				if((c = yylex()) != ';' && c != '\n')
					ExecErrorDtm;
				break;
		case NC:	Nc_l = 1;
				if ((c = yylex()) == ';' || c == '\n') {
					Ncn = 0;
					return 1;
				} else if (c == TOKEN_INT) {
					Ncn = yylval.vali;
					if ((c=yylex()) == ';')
                                                return 1;
					else 
						ExecErrorDtm;
				} else
					execerror("syntax error, expect ';' or an integer","");
		case NO:	follow(RelInSg);
				break;
		case OO:	if ((c = yylex()) != TOKEN_INT)
					ExecErrorInt;
				Oo_l = 1;
				Oon = yylval.vali;
				if((c = yylex()) != ';')
					ExecErrorDtm;
				return 1;
		case PAR:	break;
		case PR:	follow(Pr);
				Pr_l = 1;
				return 1;
		case RC:	follow_rc();
				Rc_l = 1;
				return 1;
		case REL:	yyparse();
				Rel_no = NumOfWord;
                                Relators = Pt2Word;
				break;
		case RES:	Res_l = 1;
				strcpy(In_file, yylval.words);
				return 1;
		case RL:	yyparse();
				add_new(&Relators, Pt2Word);
				Rel_no += NumOfWord;
				Newrel += NumOfWord;
				Unsort = 'u'; 
				/* don't sort the new subg generators */
                                break;
		case RT:	follow(Rt);
				break;
		case SA:	Sa_l = 1;
				if((c = tclex()) != '\n' && c != ';')
					ExecErrorDtm;
				return 1;
		case SC: 	c = yylex();
                                if (c != TOKEN_INT)
					ExecErrorInt;
				Sc_l = 1;
				Scn = yylval.vali;
				if((c = yylex()) != ';')
					ExecErrorDtm;
				return 1;
		case SG:	yyparse();
				add_new(&S_gens, Pt2Word);
				Sgen_no += NumOfWord;
				Newsg += NumOfWord;
				Unsort = 'u'; 
				/* don't sort the new subg generators */
                                break;
		case SORT:	if((c = yylex()) == ';')
					Unsort = 's';
				else {
					Unsort = yylval.gen[0];
					if((c = yylex()) != ';' && c != '\n')
						ExecErrorDtm;
				}
				break;
		case SR:	Sr_l = 1;
				if((c = tclex()) != '\n' && c != ';')
					ExecErrorDtm;
				return 1;
		case SUBG:	strcpy(Subg_name, yylval.words);	
				break;
		case TI:	c = yylex();
                                if (c != TOKEN_INT)
					execerror("syntax error","");
				Time = yylval.vali;
				if((c = yylex()) != ';')
					ExecErrorDtm;
				break;
		case TW:	follow_tw();
				return 1;
		case WO:	follow_wo();
				break;	
		case '\n':	break;
		default:	execerror("syntax error","");
		}
	}
	return 0;
}
				
follow(ch)
int *ch;
{
int 	c;
	c = yylex();
	if(c == TOKEN_INT) 
		ch[0] = yylval.vali;
	if((c=yylex()) == ',')
		if((c=yylex()) == TOKEN_INT) {
			ch[1] = yylval.vali;
			if ((c=yylex()) == ',') 
				if ((c=yylex()) == TOKEN_INT) {
					ch[2] = yylval.vali;
					if ((c=yylex()) == ';' || c == '\n')
						return;
					else
						ExecErrorDtm;
				} else 
					ExecErrorInt;
			else if (c == ';' || c == '\n') {
				ch[2] = 1;
				return;
			} else
				ExecErrorDtm;
		} else
			ExecErrorInt;
	else if (c == ';' || c == '\n') {
		ch[1] = ch[0];
		ch[2] = 1;
		return;
	} else
		execerror("syntax error", "");
	
}	

follow_rc()
{
int	c;
	c = yylex();
	if(c == ';' || c == '\n') {
		Rcn = 0;
                RcStop = 0;
                RcDesire = 0;
                return; 
	} else if (c == TOKEN_INT) {
		Rcn = yylval.vali;
		if((c=yylex()) == ',')
			if((c=yylex()) == TOKEN_INT) {
				RcStop = yylval.vali;
				if ((c=yylex()) == ',') 
					if ((c=yylex()) == TOKEN_INT) {
						RcDesire = yylval.vali;
						if ((c=yylex()) == ';' || c == '\n')
							return;
						else
							ExecErrorDtm;
					} else 
						ExecErrorInt;
				else if (c == ';' || c == '\n') {
					RcDesire = 0;
					return;
				} else
					ExecErrorDtm;
			} else
				ExecErrorInt;
		else if (c == ';' || c == '\n') {
			RcStop = 0;
			RcDesire = 0;
			return;
		} else
			execerror("syntax error", c);
	}	
}	
diagnostic(s)
char *s;
{
        int i,length;
        for(i=0; i<=9; i++)
                Diag[i] = 0;
        length = strlen(s);
        for(;length > 0; length--) {
                switch (*s) {
                case '1':
                        Diag[1] = 1;
                        break;
                case '2':
                        Diag[2] = 2;
                        break;
                case '3':
                        Diag[3] = 3;
                        break;
                case '4':
                        Diag[4] = 4;
                        break;
                case '5':
                        Diag[5] = 5;
                        break;
                case '6':
                        Diag[6] = 6;
                        break;
                case '7':
                        if (Gen_st[1].gen == '0') {
                          fprintf(fout,"CANN'T DO DI 7 WITH NUMBER GENERATOR\n");
                                break;
                        }
                        Diag[7] = 7;
                        break;
                case '8':
                        Diag[8] = 8;
                        break;
                case '9':
                        Diag[9] = 9;
                        break;
                case '0':
                        Diag[0] = 0;
                        break;
		case ';':
			break;
                default:
                        fprintf(fout," Illegal diagnostic code\n");
                        exit(0);
                }
                s++;
        }
}

follow_dr_ds(ch)
Rel_stk_type **ch;
{
int 	c;
int	n;
int	l;
	
	c = yylex();
	if (c == ';' || c == '\n')
		return; 
	l = 0;
	while (c == TOKEN_INT) { 
		n = yylval.vali;
		if (ch == &Relators)
			if (n > Rel_no || n <= 0)
                        	yyerror(5);
			else {
				del_rel_sg(n, ch);
				l++;
                        	Del_l = 1;
			}
		else
			if (n > Sgen_no || n <= 0)
				yyerror(5);
			else {
				del_rel_sg(n, ch);
				l++;
				Del_l = 1;
			}
			
		if ((c=yylex()) == ',')
			c = yylex();
		else if (c == ';' || c == '\n') {
			del_r_s(ch);
			init_gen_stk(); /* initialize generator stack */
			if (ch == &Relators)
				Rel_no -= l;
			else
				Sgen_no -= l;
                        Del_l = 0;
			return;
		} else
			execerror("syntax error", "");
	}
	execerror("syntax error", "");
}	
del_rel_sg(n,ptt)
int     n;
Rel_stk_type **ptt;
{
        Rel_stk_type *ft;
        int     i;
        ft = *ptt;
                for (i = 1; i < n && ft; i++)
                        ft = ft->next;
        if (ft) ft->len = 0;
}
del_r_s(ptt)
Rel_stk_type **ptt;
{
        Rel_stk_type *st,*ft;
/* ft points to the prior of st */
        ft = *ptt;
        st = ft->next;
        for (; st!=(Rel_stk_type *)0; )
                if (st->len == 0) {
/* delete node st  */
                        ft->next = st->next;
                        if (st->rel != (char *) 0)
                                free(st->rel);
                        else
                                free(st->rel_1);
                        free(st->rel_n);
                        free(st);
                        st = ft->next;
                } else  {
/* get next node. */
                        ft = st;
                        st = st->next;
                }
/* test if the first node needs to be delete.   */
        ft = *ptt;
        if (ft->len == 0) {
                *ptt = ft->next;
                if (ft->rel != (char *) 0)
                        free(ft->rel);
                else
                        free(ft->rel_1);
                free(ft->rel_n);
                free(ft);
        }
}
follow_gen()
{
int	c;
	c = yylex();
	if (c == TOKEN_INT) {
		if (yylval.vali <= 0)
			yyerror(2);
		else {
			Gen_no = yylval.vali;
			 gen_stk[1].gen = '0'; 
				/* label for number generators */
			put_gen();
		}
		if((c=yylex()) == ';')
			return;
		else
			ExecErrorDtm;
	}		
/* for letter generators. */	
	if(c == TOKEN_GEN) { 
		Gen_no = 1;
		gen_stk[Gen_no].gen = yylval.gen[0];
		gen_stk[Gen_no].gen_inv = 0;
	} else 
		execerror("syntax error, expect letters","");
	while ((c=yylex()) == ',') {
		if((c=yylex()) == TOKEN_GEN) {
			Gen_no++;
			gen_stk[Gen_no].gen = yylval.gen[0];
			gen_stk[Gen_no].gen_inv = 0;
		} else
			execerror("syntax error, expect letters","");
	}
	if (c == ';') {
		put_gen();
		return;
	} else
		ExecErrorDtm;
}
add_new(pt_ori, pt_new)
Rel_stk_type **pt_ori;
Rel_stk_type *pt_new;
{
	Rel_stk_type *p;
	p = *pt_ori;
	if (p) {
		for (;  p->next; p = p->next);
                p->next = pt_new;
        } else
		*pt_ori = pt_new;
	return;
}

follow_tw()
{
	int 	c;
	int	n;
	
	c = yylex();
	if (c == ';')
		return; 
	if (c == TOKEN_INT) { 
		Twn = yylval.vali;
		if ((c=yylex()) == ',') {
			yyparse();
			Tw_l = 1;
			return;
		} 
	} else
		execerror("syntax error", "");
}	
follow_ai()
{
char sbuf[BUF_LEN], *p = sbuf;
FILE *ftmp;
int  c;
	c = getc(fin);
	do {
                if (p >= sbuf + sizeof(sbuf) - 1) {
                	*p = '\0';
                        execerror("name too long", sbuf);
                }
                *p++ = c;
        } while ((c=getc(fin)) != EOF && c != ';');
	*p = '\0';
	if ((strcmp(sbuf, "stdin")) == 0) {
		if (fin == stdin)
			return;
		fclose(fin);
		fin = stdin;
	} else if ((ftmp = fopen(sbuf,"r")) == NULL) 
		fprintf(fout,"AI: can't open %s\n", sbuf);
	else {
		if (fin == stdin)
			fin = ftmp;
		else {
			fclose(fin);
			fin = ftmp;
		}
	}
	return;
}

follow_ao()
{
char sbuf[BUF_LEN], *p = sbuf;
int	c;
FILE	*ftmp;
	c = getc(fin);
	do {
                if (p >= sbuf + sizeof(sbuf) - 1) {
                	*p = '\0';
                        execerror("name too long", sbuf);
                }
                *p++ = c;
        } while ((c=getc(fin)) != EOF && c != ';');
	*p = '\0';
	if ((strcmp(sbuf, "stdout")) == 0) {
		if (fout == stdout)
			return; 
		fclose(fout);
		fout = stdout;
	} else if ((ftmp = fopen(sbuf,"w")) == NULL) 
		fprintf(fout,"AO: can't open %s\n", sbuf);
	else {
		if (fout == stdout)
			fout = ftmp;
		else {
			fclose(fout);
			fout = ftmp;
		}
	}
	return;
}
follow_wo()
{
int 	c;
	if ((c = yylex()) == TOKEN_INT) {
		if (c < 0) { 
			yyerror(2);
			Workspace = 200000;
		} else
			Workspace = yylval.vali;
		free(Space);
		Space = (int *)eemalloc(Workspace * sizeof(int));
		if ((c = yylex()) == ',') {
			if ((c = yylex()) == TOKEN_GEN) {
				Page_strategy = yylval.gen[0];
				if ((c = yylex()) == ';')
					return;
				else
					ExecErrorDtm;
			} else
				execerror("syntax error, expect a letter","");
		} else if (c == ';')
			return;
		else
			ExecErrorDtm;
	} else
		ExecErrorInt;
}
				
			
