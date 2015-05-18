#include "tcyacc.h" 
#include "y.tab.h" 
#include "string.h"
#include "parser.h"
#include "stdio.h"
extern int lineno;
extern FILE *fin;
char match[] = {
00,
00  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,03  ,04  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
03  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
02  ,02  ,02  ,01  ,02  ,'0' ,01  ,01  ,
'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,
'0' ,'0' ,01  ,02  ,01  ,01  ,01  ,01  ,
01  ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,02  ,01  ,02  ,02  ,01  ,
01  ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,01  ,01  ,01  ,01  ,01  ,
0};
yylex(){
register	int     c;
register	int     s;
	for(;;) {
	c = getc(fin);
/* skip comments which are started with # */
        if (c == '#')
                do {
                while ((c=getc(fin)) != EOF && c != '\n');
                lineno++;
                if (c == EOF)
                        return 0;
                } while ((c=getc(fin)) == '#');

	switch (match[c + 1]) {
		case	0:	return (0);
		case	1:	fprintf(fout," lexical ERROR in line %d near %c\n",lineno, c);
	                        exit(1);
		case	2:	return c;
		case  'A':	yylval.gen[0] = c;
				return(TOKEN_GEN);
		case    3:	break;
		case    4:	lineno++;
				break;
		case  '0':      ungetc(c, fin);
                                fscanf(fin, "%d", &yylval.vali);
                                return (TOKEN_INT);
		default:        fprintf(fout," lexical ERROR in line %d near %c\n",lineno, c);
                        exit(1);
        }
	}
}

		

