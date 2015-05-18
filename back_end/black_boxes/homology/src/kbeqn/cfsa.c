/* cfsa.c - calculating FSA from a rewriting system.

 ===========================================================================
   Written by:  Jamie P. Curmi
   Date:		July 1993 (changes made January 1994)
   Project:		Computational Tools for Homology of Groups
   For:			Dr J. R. J. Groves and Prof. C. Miller
				Department of Mathematics, University of Melbourne,
				Parkville, Victoria, Australia.
   ===========================================================================

   cfsa converts a rewriting system to it's FSA for use with the Homology
   programs.  Input should conform to the Geometry Group standard format,
   and should contain a list of generators and relators.
   It will NOT automatically put inverses in unless instructed by using the
   inv option.

   This is basically a hack of Derek's code (from kbeqn.c) with Jamie's
   modifications! ;-)  The code is pretty awful, but I can't be blamed
   for this one as I cut and pasted other people's.

NOTE
	You must ensure you have an ACTUAL rewriting system.

SYNOPSIS
     cfsa [-inv] [ gpname ]

OPTIONS
	-inv       Automatically insert inverses for the group.

*/

/***********************************************************************/
/* De-comment the line below if the system you are using does not make */
/* use of a command line (such as the Mac).                            */
/*#define NO_CMD_LINE */
/***********************************************************************/


#define tmalloc(D,T,N) {D = (T *) malloc(sizeof(T)*(N)); \
  if (D==0) { fprintf(stderr,"Out of space.\n"); exit(2);}}
#define tfree(D) {if (D) free( (char *) D); D=0;}

#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/times.h>

#include "defs.h"
#include "list.h"
#include "word.h"
#include "input.h"

#define PROGNAME "fsa"
#define FSASPACE 400000
#define MRELS 10000
#define MLEN  5000
#define MSTLLEN 40001

#define	USAGE	"Usage:  cfsa [-inv] [groupname]\n"

extern word    *user_gen_name;
extern gen     *inv_of;
extern int      gen_array_size;
extern int      num_gens;

char           *word1[MRELS],
               *word2[MRELS],
                test1[MLEN],
                test2[MLEN],
                tidyon,
               *wspe,
                onintr = 0,
                autohalt,
                inf[100],
                outf[100],
                tidset,
				s[255],
                fileio;
int             nrels,
                ninvrels,
                nnewrels,
                mrels = MRELS,
                fsasp[FSASPACE],
               *fsa[128],
                mstates,
                nstates,
                history[MLEN],
                tidint,
                lasttid,
                stl1[MSTLLEN],
                stl2[MSTLLEN],
                maxeqns,
                maxstates,
                maxtime;
int             format = 0;	/* set to be one if there's a format number */

short          *cf1,
               *cf2;
FILE           *op = stdout,
               *ip = stdin,
               *fopen();
struct tms      buffer;
boolean         ordgens = FALSE;

boolean read_next_rel2();

/* word1[i] and word2[i] point to the left and right hand sides of the ith
 * relation. There can be at most MRELS relations, and the individual strings
 * can have length at most MLEN. The FSA will be stored within the array
 * fsasp, as a sequence of vectors, one for each generator pointed to by
 * *fsa, where the length of each vector is equal to the current number of
 * states. mstates is the maximum number of states that we have room for. */

#ifndef NO_CMD_LINE /* version that does not use command line - such as Mac */
main(argc, argv)
	int             argc;
	char           *argv[];
#else
main()
#endif
{
	int             len,
	                i,
	                j,
	                k,
	                l,
	                m,
	                n,
	                p,
	                onrels,
					split_pos,
					relno;
	char           *ptr,
					*ptr1,
					*ptr2,
	                c,
	                d,
	                keyword[30];
	word            uw1,
	                uw2,
	                pw2,
	                rel;
	boolean			insert_inverse = FALSE;

	setbuf(stdout, (char *) 0);
	setbuf(stderr, (char *) 0);

	fileio = 0;
	/* autohalt means stop after maxeqns equations, maxstates states or
	 * maxtime seconds fileio means use input and output from files and
	 * not stdio */

#ifndef NO_CMD_LINE
	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i],"-inv"))
			insert_inverse = TRUE;
		else {
			if (i < argc - 1)
				badusage();
			strcpy(inf, argv[i]);
			strcpy(outf, inf);
			strcat(outf, ".");
			strcat(outf, PROGNAME);
			if ((ip = fopen(inf, "r")) == 0) {
				fprintf(stderr, "Cannot open %s.\n", inf);
				exit(2);
			}
			op = fopen(outf, "w");
		}
	}
#else
    do {
        printf("Enter the name of the file containing the group\n(Press 'Enter' for default (standard input)): ");
		ip = stdin;
        *inf = '\0';
        if (*gets(inf) == 0)
            break;
        if ((ip = fopen(inf, "r")) == NULL) {
            printf("** An error occured opening that file (does it exist?)\n\n");
            *inf = '\0';
        }
		strcpy(outf, inf);
		strcat(outf, ".");
		strcat(outf, PROGNAME);
		op = fopen(outf, "w");
    } while (*inf == '\0');

	do {
        printf("Do you want inverses inserted for this group?\n");
		printf("(yes or no - press enter for default (no)): ");
        *s = '\0';
		insert_inverse = FALSE;
        if (*gets(s) == 0)
			break;
        if (*s == 'y' || *s == 'Y') {
			insert_inverse = TRUE;
			break;
		}
        else if (*s == 'n' || *s == 'N')
			break;
		else
			printf("** Please enter yes or no\n\n");
    } while (1);
#endif
	/* Input generators */
	user_gen_name = vzalloc2(word, gen_array_size);
	for (i = 0; i < gen_array_size; ++i)
		word_init(user_gen_name + i);

	n = readstring(keyword);
	if (n == EOF) {
		fprintf(stderr, "Error: Could not find keyword 'words' or 'gens'.\n");
		exit(2);
	}
	if (strcmp(keyword, "Format") == 0) {
		format = 1;
		n = readstring(keyword);
	}
	while (strcmp(keyword, "words") && strcmp(keyword, "gens")) {
		if (n == EOF) {
			fprintf(stderr, "Error: Could not find keyword 'words' or 'gens'.\n");
			exit(2);
		}
		n = readstring(keyword);
	}
	read_gen_name_array(ip);
	if (num_gens > 127) {
		fprintf(stderr, "cfsa has a maximum of 127 generators.\n");
		exit(2);
	}
	n = readstring(keyword);
	if (strcmp(keyword, "inverses") == 0) {
		read_inverse_array(ip);
		n = readstring(keyword);
		if (strcmp(keyword, "rels")) {
			fprintf(stderr, "Error: Could not find keyword 'rels'.\n");
			exit(2);
		}
	} else {
		default_inverse_array(); ;
		if (strcmp(keyword, "rels")) {
			fprintf(stderr, "Error: Could not find keyword 'rels'.\n");
			exit(2);
		}
	}

	/* Any generators without inverses are assumed to have order 2, so we
	 * insert any such inverse relations. However, we will check later
	 * that for such generators the relevant relator a^2 is present. */
	/* This has been removed for the present */
	/* Now insert inverse relations (unless -inv option used) */

	nrels = 0;
	if (!insert_inverse)	/* inverses option not used */
		ninvrels = 0;
	else {	/* add the inverses */
		for (i = 1; i <= num_gens; i++) {
			nrels++;
			tmalloc(word1[nrels], char, 3) tmalloc(word2[nrels], char, 1)
			word1[nrels][0] = i;
			word1[nrels][1] = inv_of[i];
			word1[nrels][2] = 0;
			word2[nrels][0] = 0;
		}
		ninvrels = nrels;
	}

	/* Now define the pointers for the FSA and initialize it. Initially,
	 * each generator defines its own state. */
	mstates = num_gens == 0 ? FSASPACE : FSASPACE / num_gens;
	for (i = 1; i <= num_gens; i++)
		fsa[i] = fsasp + (i - 1) * mstates - 1;
	tmalloc(cf1, short, mstates + 1) tmalloc(cf2, short, mstates + 1)
		nstates = num_gens + 1;
	for (i = 1; i <= num_gens; i++)
		for (j = 1; j <= nstates; j++)
			fsa[i][j] = i + 1;
	for (i = 1; i <= nrels; i++)
		fsa[*(word1[i] + 1)][*word1[i] + 1] = -i;
	/* looks crazy, but its right! */

	/* Now input the defining relators.  */
	find_char('\{', ip);
	nrels = ninvrels;

	word_init(&rel);
	relno = nrels+1;

	while (read_next_rel2(&rel, ip, &split_pos)) {
		
		/* Now transfer to Derek's mode of storage! */
		tmalloc(word1[relno], char, 200)
		tmalloc(word2[relno], char, 200)
		ptr1 = word1[relno];
		ptr2 = word2[relno];
		/* split_pos is used to split 'rel.g' into left and right parts,
		   as Derek's code stores "a > b" as "ab^-1 > epsilon", which is
		   not what we want. */
		for (i = rel.first; i < split_pos; i++)
				*(ptr1++) = (char) (rel.g)[i];
		for (j = rel.last; j >= i; j--)
				*(ptr2++) = (char) inv_of[(rel.g)[j]];
		*ptr1 = '\0';
		*ptr2 = '\0';
		modifyfsa(relno);
		word_reset(&rel);

		relno++;
	}

	nrels = relno - 1;
	word_clear(&rel);
	find_char('}', ip);

	output();

    return 0;

}

modifyfsa(relno)
	int             relno;

/* Modify the FSA so that it returns the failure state -relno  whenever it
 * encounters the LHS of the new relation with number relno as a substring.
 * This is the trickiest part of the program. */
{
	int             genno,
	               *cg,
	               *cgp,
	               *cge,
	               *list1,
	               *list2,
	               *swlist,
	                nnstates,
	                x,
	                y,
	                z,
	                i,
	               *ip,
	               *lp,
	               *lpe;
	char           *ptr;
	short          *cvp,
	               *cve,
	               *swcf;

	ptr = word1[relno];
	genno = *(ptr++);
	cg = cgp = fsa[genno];
	cge = cgp + nstates;
	if (*ptr == '\0') {
		while (++cgp <= cge) {
			*cgp = -relno;
		}
	} else {
		list1 = stl1;
		list2 = stl2;
		*list1 = 0;
		cvp = cf1;
		cve = cf1 + nstates;
		while (++cvp <= cve)
			*cvp = 0;
		while (++cgp <= cge)
			if (*cgp > 0 && cf1[*cgp] == 0) {
				cf1[*cgp] = 1;
				(*list1)++;
				if (*list1 >= MSTLLEN) {
					fprintf(stderr, "Too many states in list.\n");
					exit(2);
				}
				list1[*list1] = *cgp;
			}
		genno = *(ptr++);
		while (*ptr) {
			cg = cgp = fsa[genno];
			cge = cgp + nstates;
			*list2 = 0;
			nnstates = 0;
			cvp = cf2;
			cve = cf2 + nstates;
			while (++cvp <= cve)
				*cvp = 0;
			while (++cgp <= cge)
				if ((y = *cgp) > 0) {
					x = cgp - cg;
					if (cf2[y] == 0) {
						if (cf1[x] == 1) {
							cf2[y] = 1;
							(*list2)++;
							if (*list2 >= MSTLLEN) {
								fprintf(stderr, "Too many states in list.\n");
								exit(2);
							}
							list2[*list2] = y;
						} else
							cf2[y] = -1;
					} else if (cf2[y] == -1 && cf1[x] == 1) {
						nnstates++;
						z = nstates + nnstates;
						if (z > mstates) {
							fprintf(stderr, "Too many states.\n");
							exit(2);
						}
						for (i = 1; i <= num_gens; i++)
							fsa[i][z] = fsa[i][y];
						cf2[z] = 1;
						cf2[y] = nnstates + 1;
						(*list2)++;
						if (*list2 >= MSTLLEN) {
							fprintf(stderr, "Too many states in list.\n");
							exit(2);
						}
						list2[*list2] = z;
					} else if (cf2[y] == 1 && cf1[x] != 1) {
						nnstates++;
						if (nnstates >= 127) {
							fprintf(stderr, "Too many new states.\n");
							exit(2);
						}
						z = nstates + nnstates;
						if (z > mstates) {
							fprintf(stderr, "Too many states.\n");
							exit(2);
						}
						for (i = 1; i <= num_gens; i++)
							fsa[i][z] = fsa[i][y];
						cf2[z] = 1;
						cf2[y] = nnstates + 1;
						for (i = 1; i <= *list2; i++)
							if (list2[i] == y) {
								list2[i] = z;
								break;
							}
					}
				}
			lp = list1;
			lpe = lp + *lp;
			while (++lp <= lpe) {
				ip = cg + *lp;
				if (*ip > 0 && (x = cf2[*ip] - 1) > 0) {
					*ip = nstates + x;
					if ((y = cf2[*lp] - 1) > 0)
						cg[nstates + y] = nstates + x;
				}
			}
			nstates += nnstates;
			swlist = list1;
			list1 = list2;
			list2 = swlist;
			swcf = cf1;
			cf1 = cf2;
			cf2 = swcf;
			genno = *(ptr++);
		}
		cg = fsa[genno];
		for (i = 1; i <= *list1; i++)
			cg[list1[i]] = -relno;
	}
	return (1);
}

output()
{
	int             j,
	                k;

	fprintf(op, "fsa {\n");
	fprintf(op, "\tstates %d\n\tsymbols %d\n", nstates, num_gens);
	fprintf(op, "\tvariables 1\n\tno_eos\n");
	fprintf(op, "\talphabet {");
	for (j = 1; j <= num_gens; j++) {
		fprintf(op, " %d = ", j);
		gen_print(op, j);
	}
	fprintf(op, "}\n");
	fprintf(op, "\tstart { 1 }\n\n%%\natable\n");
	for (j = 1; j <= nstates; j++) {
		fprintf(op, "%4d ", j);
		fprintf(op, "A  ");
		for (k = 1; k <= num_gens; k++)
			fprintf(op, " %2d", fsa[k][j]);
		fprintf(op, ";\n");
	}
	fprintf(op, "}\n\n");
	fprintf(op, "rels {\n");
	for (j = 1; j <= nrels; j++)
		if (strlen(word2[j]) == 0) {
			wordprint(word1[j]);
			fprintf(op, " > epsilon\n");
		} else {
			wordprint(word1[j]);
			if (strlen(word1[j]) > 20)
				putc('\n', op);
			putc(' ', op);
			putc('>', op);
			putc(' ', op);
			wordprint(word2[j]);
			putc('\n', op);
		}
	fprintf(op, "}\n");
}

wordprint(w)
	char           *w;
{
	int             i,
	                l;

	l = strlen(w);
	for (i = 0; i <= l - 2; i++) {
		gen_print(op, w[i]);
		putc('*', op);
	}
	gen_print(op, w[l - 1]);
}

stringtoint(w)
	char           *w;
{
	char           *p;
	int             n;

	p = w;
	n = 0;
	while (*p != '\0') {
		if (isdigit(*p) == 0)
			badusage();
		n = 10 * n + (*p - '0');
		p++;
	}
	return (n);
}

badusage()
{
	fprintf(stderr, USAGE);
	exit(2);
}

readstring(s)
	char           *s;
{
	int             n;

	while ((n = read_char(ip)) == ' ');
	while (n != ' ') {
		if (n == -1) {
			*s = '\0';
			return (-1);
		}
		*(s++) = n;
		n = read_char(ip);
	}
	*s = '\0';
	return (0);
}

/*
printwordy(w)
	char	*w;
{
	while (*w != '\0')
		printf("%d ", (int) *(w++));
	printf("\\0");
}
*/


/* This is read_next_rel (Derek's) with a slight modification */
boolean
read_next_rel2(relp,rfile,split_pos)
  word * relp;
  FILE * rfile;
  int  * split_pos;
{
  boolean ans=FALSE;
  word uw1;
  word_traverser wt;
  gen g;
  word_init(&uw1);
  ans = read_next_word(&uw1,rfile);
  if (ans){
    int c;
    word2prog_word(&uw1,relp);
	*split_pos = relp->last + 1 + relp->first; /* The only modification */
    while ((c=read_char(rfile))==' ');
    if (c=='=' || c=='>'){
      word uw2, pw2;
      word_init(&uw2);
      word_init(&pw2);
      read_next_word(&uw2,rfile);
      word2prog_word(&uw2,&pw2);
      word_inv(&pw2,&pw2);
      word_append(relp,&pw2);
      word_clear(&uw2);
      word_clear(&pw2);
    }
    else
      ungetc(c,rfile);
  }
  word_clear(&uw1);
  word_traverser_init(&wt,relp);
  while (word_next(&wt,&g)){
    if (g==INVALID_GEN){
      fprintf(stderr,"Undeclared generator or syntax error in relator.\n");
      bad_data();
    }
  }
  word_traverser_clear(&wt);
  return ans;
}
