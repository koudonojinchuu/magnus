%{ 
#include <stdlib.h>
#include "ctype.h"
#include "tctime.h"
#include "tcstdlib.h"
#include "tcyacc.h" 
#include "parser.h"
#include "string.h" 
/* #include "tcstdlib.h"*/  
extern	char *progname;
Rel_stk_type *rel_pt, *rel_end; 
extern	Word_pt *w_pt, word_;
extern Enum_def  enumd, *pt;
int numb;

char* eemalloc(unsigned n); /* check return from malloc */ 

Rel_stk_type* get_relator(char* s); /* put a relator to the relator stack */

char* word_power(char* w, char* p); /* work out (word)^(power)=(power)^-1(word)(power) */ 
%} 
%start words  
%token <vali> TOKEN_INT 
%token <gen> TOKEN_GEN
%type  <words>  ordwords word term bterm
%type <nums> numwords num_word num_term num_bterm num_aterm
%left	','
%left	'*' 
%right 	'^' 
%%
words:	  ';'  { /* no words */
                NumOfWord = 0;
                Pt2Word = (Rel_stk_type *) 0;
                init_gen_stk(); /* initialize generator stack */
                return;
                          }
	| ordwords ';' {
                NumOfWord = numb;
                Pt2Word = rel_pt;
                init_gen_stk(); /* initialize generator stack */
                return;
                }
	| numwords ';' {
                NumOfWord = numb;
                Pt2Word = rel_pt;
                init_gen_stk(); /* initialize generator stack */
                return;
                }
        ;
ordwords: word {  /* get word one by one.  */
		numb = 1;
		rel_pt = (Rel_stk_type *) 0; /* word stack pointer */  
		rel_pt = get_relator($1); /* put a word into word stack */  
		rel_end = rel_pt;
		free($1);
                } 
        | ordwords ',' word  {numb++; rel_end->next = get_relator($3);
		rel_end = rel_end->next; 
		free($3);
	  }  
	;
word:     term { $$ = $1;  }
        | word '*' term {  $$ = (char *)eemalloc(strlen($1) + strlen($3) + 1);
				strcpy($$, $1);
				strcat($$, $3);
				free($1); free($3);
		}
        | word term {  $$ = (char *)eemalloc(strlen($1) + strlen($2) + 1);
                                strcpy($$, $1);
                                strcat($$, $2);
                                free($1); free($2);
		}
        ;
term:	  bterm {
		$$  = $1;
	  }
        | bterm '^' TOKEN_INT { /* integer power */
                int power;
                power = ($3 < 0) ? -$3 : $3;
                $$ = (char *) eemalloc(power * strlen($1) + 1);
                                int_power($$,$1,$3);
                free($1);
                              }
        | bterm TOKEN_INT { /* integer power */
                int power;
                power = ($2 < 0) ? -$2 : $2;
                $$ = (char *) eemalloc(power * strlen($1) + 1);
                                int_power($$,$1,$2);
                 free($1);
                              }
        | bterm '^' bterm { /* word power */
                            $$ = word_power($1,$3);
                          }
        ;
bterm:    TOKEN_GEN { /* generator */
		$$ = (char *) eemalloc(2);
		*$$ = $1[0];
                *($$ + 1) = '\0';
                }
        | '(' word ')' { $$ = $2; 
				/*strcpy($$,$2);*/
			 } /* ( word ) */  
        | '[' word ',' word ']' { /* [w1,w2] */
                int  len1, len2, i;
                len1 = strlen($2);
                len2 = strlen($4);
		$$ = (char *) eemalloc(len1 + len1 +len2 + len2 + 1);
                int_power($$,$2,-1);
                /* put $4 into $$ */
                for (i = 0; i < len2; i++)
             /* reverse s and convert every generator in s to its inverse */
                        $$[len1+i] = (islower($4[len2-i-1]))? toupper($4[len2-i-1]) : tolower($4[len2-i-1]);
                $$[len1+i] = '\0';
                strcat($$, $2);
                strcat($$, $4); 
		free($2); free($4);
                }
	| '(' word ',' word ')' { /* (w1,w2) same as [w1,w2] */
		int  len1, len2, i;
                len1 = strlen($2);
                len2 = strlen($4);
                $$ = (char *) eemalloc(len1 + len1 +len2 + len2 + 1);
                int_power($$,$2,-1);
                /* put $4 into $$ */
                for (i = 0; i < len2; i++)
             /* reverse s and convert every generator in s to its inverse */
                        $$[len1+i] = (islower($4[len2-i-1]))? toupper($4[len2-i-1]) : tolower($4[len2-i-1]);
                $$[len1+i] = '\0';
                strcat($$, $2);
                strcat($$, $4);
                free($2); free($4);
                }	
        ;
numwords: num_word {
                int i;
                numb = 1; /* numb recoads the number of relators */
                rel_pt = (Rel_stk_type *) eemalloc(sizeof(Rel_stk_type));
                                        /* relator stack pointer */
                rel_pt->rel_1 = (int *) eemalloc(sizeof(int) * ($1->lengh + 1));
                for(i = 1; i <= $1->lengh; i++)
                        rel_pt->rel_1[i] = $1->numbs[i];
                rel_pt->len_1 = $1->lengh;
                rel_pt->next = (Rel_stk_type *) 0;
                rel_pt->rel = (char *) 0;
		rel_pt->exp = 1;
		rel_end = rel_pt;
	}
	| numwords ',' num_word {
                Rel_stk_type *rel_ptt,*p;
                int i;
                numb++;
		rel_ptt = (Rel_stk_type *) eemalloc(sizeof(Rel_stk_type));
                rel_ptt->rel_1 = (int *)eemalloc(sizeof(int)*($3->lengh+1));
                for(i = 1; i <= $3->lengh; i++)
                        rel_ptt->rel_1[i] = $3->numbs[i];
                rel_ptt->len_1 = $3->lengh;
                rel_ptt->rel = (char *) 0;
		rel_pt->exp = 1;
                rel_ptt->next = (Rel_stk_type *) 0;
		rel_end->next = rel_ptt; /* connect the new to the chain */
		rel_end = rel_ptt;
	 }
	;
num_word: num_term {
		$$  = $1;
	 }
	| num_word '*' num_term {
                int i,j;
		$$ = (Num_gen_word *) eemalloc(sizeof(Num_gen_word));
		$$->numbs = (int *) eemalloc(sizeof(int)*($1->lengh+$3->lengh+1));
		for(i = 1; i <= $1->lengh; i++)
                        $$->numbs[i] = $1->numbs[i];
                for(j = 1; j <= $3->lengh; j++,i++)
                        $$->numbs[i] = $3->numbs[j];
                $$->lengh = i - 1;
		free($1->numbs); free($3->numbs);
		free($1); free($3);
	 }
	;
num_term:  num_bterm {
		$$  = $1;
         }      
	| num_aterm {
		$$  = $1;      
         }             
        | num_term '^' num_aterm { /* (word)^(power)=(power)^-1(word)(power) */
                int i,j,leng;
                leng = $3->lengh;
		$$ = (Num_gen_word *) eemalloc(sizeof(Num_gen_word));
                $$->numbs = (int *)eemalloc(sizeof(int)*($1->lengh+leng+leng+1));	
		for(i = 0; i < leng; i++)
                        $$->numbs[i + 1] = -$3->numbs[leng - i];
                i++;
                for(j = 1; j <= $1->lengh; j++,i++)
                        $$->numbs[i] = $1->numbs[j];
                for(j = 1; j <= $3->lengh; j++,i++)
                        $$->numbs[i] = $3->numbs[j];
                $$->lengh = i - 1;
		free($1->numbs); free($3->numbs);
                free($1); free($3);
	  }
	| num_term '^' TOKEN_INT { /* (word)^n  */
                int i,j,t,power,leng,power1;
                power = $3;
		power1 = (power < 0) ? -power : power;
                leng = $1->lengh;
		$$ = (Num_gen_word *) eemalloc(sizeof(Num_gen_word));  
                $$->numbs = (int *)eemalloc(sizeof(int)*(leng*power1+1));
		if (power < 0) { /* (word)^-n  */
                        power = -power;
                        for(i = 0; i < leng / 2; i++) {
                                t = $1->numbs[i + 1];
                                $1->numbs[i + 1] = -$1->numbs[leng - i];
                                $1->numbs[leng - i] = -t;
                        }
                        if (leng % 2)
                           $1->numbs[leng / 2 + 1] = -$1->numbs[leng / 2 + 1];
                }
                i = 1;
                for ( ; power > 0; power--)
                        for(j = 1; j <= leng; j++,i++)
                                $$->numbs[i] = $1->numbs[j];
                $$->lengh = i - 1;
		free($1->numbs); 
                free($1); 
          }
	;
num_bterm: TOKEN_INT {
                if ($1 < 0 && $1 < -Gen_no || $1 > 0 && $1 > Gen_no) {
                        fprintf(fout,"%d is not a generator\n",$1);
                        exit(1);
                } else {
			$$ = (Num_gen_word *) eemalloc(sizeof(Num_gen_word));  
                	$$->numbs = (int *)eemalloc(sizeof(int)*2);
			$$->numbs[1] = $1; $$->lengh =1;
		}
	  }
	;
num_aterm:  '(' num_word ')' {
		$$ = $2;
	  }
	| '[' num_word ',' num_word ']' {
                /* [word1,word2] = (word)^-1(word2)^-1(word1)(word2)  */
                int i,j,len1,len2;
		len1 = $2->lengh;
		len2 = $4->lengh;
		$$ = (Num_gen_word *) eemalloc(sizeof(Num_gen_word));
		$$->numbs = (int *)eemalloc(sizeof(int)*(len1+len1+len2+len2+1));
		for(i = 1; i <= len1; i++)
                        $$->numbs[i] = -$2->numbs[i];
                for(j = 1; j <= len2; j++,i++)
                        $$->numbs[i] = -$4->numbs[j];
                for(j = 1; j <= len1;  j++,i++)
                        $$->numbs[i] = $2->numbs[j];
                for(j = 1; j <= len2; j++,i++)
                        $$->numbs[i] = $4->numbs[j];
                $$->lengh = i - 1;
		free($2->numbs); free($4->numbs);
		free($2);
		free($4);
         }
        | '(' num_word ',' num_word ')' {
                /* [word1,word2] = (word)^-1(word2)^-1(word1)(word2)  */
                int i,j,len1,len2;
                len1 = $2->lengh;
                len2 = $4->lengh;
                $$ = (Num_gen_word *) eemalloc(sizeof(Num_gen_word));
                $$->numbs = (int *)eemalloc(sizeof(int)*(len1+len1+len2+len2+1));
                for(i = 1; i <= len1; i++)     
                        $$->numbs[i] = -$2->numbs[i];    
                for(j = 1; j <= len2; j++,i++) 
                        $$->numbs[i] = -$4->numbs[j];    
                for(j = 1; j <= len1;  j++,i++)
                        $$->numbs[i] = $2->numbs[j];     
                for(j = 1; j <= len2; j++,i++) 
                        $$->numbs[i] = $4->numbs[j];     
                $$->lengh = i - 1;      
                free($2->numbs); free($4->numbs);
                free($2);
                free($4);
         }     
	;	
/* end of grammar */ 
%% 
#include "ctype.h"
#include "tcstdlib.h"
#include "stdio.h" 
/* #include "tcyacc.h"  
#include "tc.h"
#include "sys/types.h"
#include "miscellaneous.h"
*/
#include "tcerror.c" 
extern Gen_stk_type gen_stk[]; 
extern Word_pt *w_pt, word_;
extern Enum_def  enumd, *pt;
char 	*infile;	/* input file name */ 
int numb,lineno; 
execerror(s,t) /* print error message then exit */
char *s, *t;
{       warning(s,t);
        exit(1); /* cause normal program termination. */
}
warning(s, t)
/* print warning massege */
char *s, *t;
{       fprintf(stderr, "%s: %s", progname, s);
        if (t)
                fprintf(stderr, " %s", t);
        if (infile)
                fprintf(stderr, " in %s", infile);
        fprintf(stderr, " near line %d\n", lineno);
        exit(1);
}

char *eemalloc(n) /* check return from malloc */ 
unsigned n; 
{       char *p/*, *malloc()*/; 

        p = (char *)malloc(n); 
        if (p == 0){
                execerror("out of memory", (char *) 0); 
	}
        return p;
}  
init_gen_stk() /* initialize group generator stack */
{ 
	int i;

	for (i = 1; i <= Gen_no; i++)
                        Gen_st[i].gen_inv = -i;
}
Rel_stk_type *get_relator(s) /* put a relator to the relator stack */
char *s; 
{ 	Rel_stk_type *p,*st; 
	int length; 
	
	length = strlen(s); 
	p = (Rel_stk_type *) eemalloc(sizeof(Rel_stk_type)); 
/*
	p->rel = s;
*/
	p->rel = (char *) eemalloc(length + 1);
	p->len = length; 
	p->rel_1 = (int *) 0;
	p->exp = 1;
	strcpy(p->rel,s); 
	p->next = (Rel_stk_type *) 0;
 	return p; 
} 
int_power(p, cs, n) /* deal with integer power for a word */ 
char *p;
char *cs; 
int  n; 
{ 
char s_pool[1000];
register char* s = s_pool;
register char tem, tbm; 
register	int i,pi,length,length_1; 
register int A2a;
	length = strlen(cs); 
	length_1 = length - 1; 
	for(i = 0; i < length; i++)
		s[i] = cs[i];
	s[i] = '\0';
	if (n < 0) { /* negative power */   
		pi = abs(n); /* get absolute n */ 
		A2a = 'a' - 'A';
		for (i = 0; i <= (length_1) / 2; i++) 
/* reverse s and convert every generator in s to its inverse */ 
		{	tem = s[i];
			tbm = s[length_1 - i];
			if (tem >= 'a' && tem <= 'z')
				s[length_1 - i] = tem - A2a;
			else
				s[length_1 - i] = tem + A2a;
			if (tbm >= 'a' && tbm <= 'z')
                                s[i] = tbm - A2a;
                        else
                                s[i] = tbm + A2a;
		} 
	} 
	else   pi = n;    
/* allocate space for making power */
	if (pi) { /* pi != 0 */ 
		strcpy(p, s); 
		for (i = 2; i <= pi; i++) /* making power */ 
		 	p = strcat(p, s); 
	} else *p = '\0'; /* when int_power == 0 */ 
} 
char *word_power(w,p)  /* work out (word)^(power)=(power)^-1(word)(power) */ 
char *w;  /* word */ 
char *p;  /* power */ 
{	char *st, *ct; 

	ct = (char *) eemalloc(strlen(p) * 2 + strlen(w) + 1); /* +1 for '\0' */
	int_power(ct, p, -1); /* make (power)^-1 */ 
	strcat(ct,w); /* make (power)^-1(word) */ 
	strcat(ct,p); /* make (power)^-1(word)(power) */ 
	return ct; 
} 
		
