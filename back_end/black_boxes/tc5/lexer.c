#include "tcyacc.h" 
#include "y.tab.h" 
#include "string.h"
#include "parser.h"
#include "stdio.h"
extern int lineno;
extern char *eemalloc(); 
extern	Keyword keywords[];
tclex()  /* tc lexical analyzer  */
{
/* recognize comments started with #.  3/2/93 */
extern FILE *fin;
int 	c;
int	s;
	while ((c=getc(fin)) == ' ' || c == '\t');
/* skip comments which are started with # */
	if (c == '#')
		do {
		while ((c=getc(fin)) != EOF && c != '\n');
		lineno++;
		if (c == EOF)
                	return 0;
		} while ((c=getc(fin)) == '#'); 
	if (c == EOF)
		return 0;
	if (isdigit(c)) {
		ungetc(c, fin);
		fscanf(fin, "%d", &yylval.vali);
		return (TOKEN_INT);
	}
	if (isalpha(c)) { /* keywords */
		char sbuf[BUF_LEN], *p = sbuf;
		do {
			if (p >= sbuf + sizeof(sbuf) - 1) {
				*p = '\0';
				execerror("name too long", sbuf);
			}
			*p++ = c;
		} while ((c=getc(fin)) != EOF && c != ':' && c != '\n');
		if (c == '\n')
			ungetc(c, fin);	
		*p  = '\0';
		if ((s = lookup(sbuf)) == 0) 
			execerror("invalid key word", sbuf);
		if (s == ENUM || s == SUBG || s == RES || s == DI){
			p = sbuf;
			while ((c=getc(fin)) != EOF && c != '\n') {
				if (p >= sbuf + sizeof(sbuf) - 1) {
                                	*p = '\0';
                                	execerror("name too long", sbuf);
                        	}
                        	*p++ = c; 
			}
			*p  = '\0';
			lineno++;
			yylval.words = (char *) eemalloc(100);
			strcpy(yylval.words, sbuf);
		}
		return (s);
	}
	switch (c) {
		case ',':	return c;
		case ';':	return c;
		case '\n': 	lineno++; return c;
	
		default:	fprintf(fout," lexical error in line %d near %c\n",lineno, c);
				exit(1);
	}
}
lookup(s)
char *s;
{
	int	i;
	s[4] = '\0';
	for (i = 0; i < KEY_LEN; i++)
		if (strcmp(keywords[i].name, s) == 0)
                        return (keywords[i].kval);
	s[3] = '\0';
	for (i = 0; i < KEY_LEN; i++)
                if (strcmp(keywords[i].name, s) == 0)
                        return (keywords[i].kval);
	s[2] = '\0';
	for (i = 0; i < KEY_LEN; i++)
                if (strcmp(keywords[i].name, s) == 0)
                        return (keywords[i].kval);

	return 0; /* 0 ==> not found  */
}
