/* kbeqn2.c (modification of kbeqn.c) 

   This is now an even more horrible mixture of Derek's code and method of
   data storage, Sarah's input routines, and Jamie's modifications! ;-)

   Changes have been made to this code by Jamie P. Curmi during February/March
   and September/October 1992.  These changes are outlined below:

NEW SYNOPSIS
     kbeqn2 [ -t tidyint ] [ -h maxeqns maxstates maxtime ] [ -lex ]
     [ -rec ] [ -o ][ gpname ]

OLD OPTIONS
     -t		tidyint
	  	Set the value of KBtidyint equal to tidyint.

     -h   	maxeqns maxstates maxtime
	  	After a tidying-up  operation,  the  program  halts  if
	  	either  the  number of equations exceeds maxeqns, or if the
	  	number of states of the fsa exceeds  maxstates,  or the
	  	process-time exceeds maxtime seconds.

     -o   	Take input from gpname.ordgens rather than from gpname.
	  	This  option  should always be used if the output is to be
	  	used as input to findisoms(1).
NEW OPTIONS

     -lex	Lexicographical ordering
		** THE DEFAULT SETTING **
	  	This is the original ordering used in kbeqn.c.
		The ordering is lexicographic, with longer words having
		higher order than shorter words.

     -rec	Recursive Path ordering
		Ordering based on that described in the book "Confluent String
		Rewriting" by Matthias Jantzen, Defn 1.2.14, page 24.
		The algorithm is described at the header of 'rec_compare'

CHANGES TO KBEQN
      The New Options above.  I have tried to make the code as easy as
      possible to modify incase new ordering options are to be included
      (such as weighted lexicographical ordering) in the future.

      The shuffling algorithm (for transferring inverses to the other side),
      in the function 'insert', has been removed (commented out), as it
	  interfered with the recursive path ordering.  This should not affect
	  the algorithms.

	  If the program is interrupted (^C), it will output and stop after the
	  next tidying-up operation.  For some systems, this may take a long time
	  to occur (or may never occur).  If another interrupt is made any time
	  after the first, the program will stop and output the rewritng system
	  so far devised.

      The code has been 'beautified' using a C-Beautifier on our system.
      This helped with my understanding of the algorithms, and
      modifications.		- J.P.Curmi (1992)

*/

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

#define PROGNAME "kbeqn"
#define FSASPACE 400000
#define MRELS 10000
#define MLEN  5000
#define MSTLLEN 40001

#define	USAGE	"Usage:  kbeqn2 [-t tidyint] [-h maxeqns maxstates maxtime] [-lex]\n        [-rec] [-o] [groupname]\n"

/* Default Ordering is LEXICOgraphic, the other is RECURSIVE path ordering */
enum Orderings	{LEXICO, RECURSIVE};	/* If more orderings are required,
					   add more constants here and alter
					   the compare routine.  */

extern word    *user_gen_name;
extern gen     *inv_of;
extern int      gen_array_size;
extern int      num_gens;

void printword();

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
int		Ordering = LEXICO;	/* Set to ordering requested */
					/* LEXICO is default */
int             format = 0;	/* set to be one if there's a format number */

short          *cf1,
               *cf2;
FILE           *op = stdout,
               *ip = stdin,
               *fopen();
struct tms      buffer;
boolean         ordgens = FALSE;

/* word1[i] and word2[i] point to the left and right hand sides of the ith
 * relation. There can be at most MRELS relations, and the individual strings
 * can have length at most MLEN. The FSA will be stored within the array
 * fsasp, as a sequence of vectors, one for each generator pointed to by
 * *fsa, where the length of each vector is equal to the current number of
 * states. mstates is the maximum number of states that we have room for. */

interrupt()
{
	if (onintr == 1) {
		output();
		signal(SIGINT, SIG_DFL);
		exit (1);
	}
	fprintf(stderr, "\n ** kbeqn2 has been interrupted!\n");
	fprintf(stderr," ** The program will output and stop on next tidying-up operation.\n");
	fprintf(stderr," ** Interrupt once more to output rewriting system so far.\n");
	onintr = 1;
	signal(SIGKILL, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}

main(argc, argv)
	int             argc;
	char           *argv[];
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
	                arg;
	char           *ptr,
	                c,
	                d,
	                keyword[30];
	word            uw1,
	                uw2,
	                pw2,
	                rel;

	setbuf(stdout, (char *) 0);
	setbuf(stderr, (char *) 0);

	arg = 1;
	tidset = autohalt = fileio = 0;
	/* autohalt means stop after maxeqns equations, maxstates states or
	 * maxtime seconds fileio means use input and output from files and
	 * not stdio */
	while (argc > arg) {
		if (strcmp(argv[arg], "-h") == 0) {
			autohalt = 1;
			arg++;
			if (arg >= argc)
				badusage();
			maxeqns = stringtoint(argv[arg]);
			arg++;
			if (arg >= argc)
				badusage();
			maxstates = stringtoint(argv[arg]);
			arg++;
			if (arg >= argc)
				badusage();
			maxtime = stringtoint(argv[arg]);
		} else if (strcmp(argv[arg], "-t") == 0) {
			tidset = 1;
			arg++;
			if (arg >= argc)
				badusage();
			tidint = stringtoint(argv[arg]);
		} else if (strcmp(argv[arg], "-o") == 0) {
			ordgens = TRUE;
		} else if (strcmp(argv[arg], "-rec") == 0) {
			Ordering = RECURSIVE;
		} else if (strcmp(argv[arg], "-lex") == 0) {
			Ordering = LEXICO;	/* DEFAULT! */
		} else {
			if (fileio)
				badusage();
			if (argv[arg][0] == '-')
				badusage();
			fileio = 1;
			strcpy(inf, argv[arg]);
			strcpy(outf, inf);
			if (ordgens)
				strcat(inf, ".ordgens");
			strcat(outf, ".");
			strcat(outf, PROGNAME);
			if ((ip = fopen(inf, "r")) == 0) {
				fprintf(stderr, "Cannot open %s.\n", inf);
				fprintf(stderr, "Run ordrels -g %s.\n", argv[arg]);
				exit(2);
			}
			op = fopen(outf, "w");
		}
		arg++;
	}
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
		fprintf(stderr, "kbeqn has a maximum of 127 generators.\n");
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
		default_inverse_array();
		if (strcmp(keyword, "rels")) {
			fprintf(stderr, "Error: Could not find keyword 'rels'.\n");
			exit(2);
		}
	}

	/* Any generators without inverses are assumed to have order 2, so we
	 * insert any such inverse relations. However, we will check later
	 * that for such generators the relevant relator a^2 is present. */

	/* Now insert inverse relations */
	nrels = 0;
	for (i = 1; i <= num_gens; i++) {
		nrels++;
		tmalloc(word1[nrels], char, 3) tmalloc(word2[nrels], char, 1)
			word1[nrels][0] = i;
		word1[nrels][1] = inv_of[i];
		word1[nrels][2] = 0;
		word2[nrels][0] = 0;
	}
	ninvrels = nrels;

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
	while (read_next_rel(&rel, ip)) {
		/* Now transfer to Derek's mode of storage! */
		ptr = test1;
		for (i = rel.first; i <= rel.last; i++)
			*(ptr++) = (rel.g)[i];
		*ptr = '\0';
		*test2 = '\0';
		word_reset(&rel);

		if (strlen(test1) != 2 || test1[0] != inv_of[test1[1]]) {
			nrels++;
			if (insert(nrels) == 0)
				nrels--;
		}
	}
	word_clear(&rel);
	find_char('}', ip);

	tidyon = 0;
	if (tidset == 0) {
		n = readstring(keyword);
		if (n == EOF) {
			tidint = 30;
			goto Continue;
		}
		while (strcmp(keyword, "parameters")) {
			n = readstring(keyword);
			if (n == EOF) {
				tidint = 30;
				goto Continue;
			}
		}
		find_char('\{', ip);
again:
		while ((n = read_char(ip)) != 'K' && n != '}' && n != EOF);
		if (n == EOF || n == '}') {
			tidint = 30;
			goto Continue;
		}
		readstring(keyword);
		if (strcmp(keyword, "Btidyint"))
			goto again;
		if (fscanf(ip, "%d", &tidint) <= 0) {
			fprintf(stderr, "Error: Could not read parameter.\n");
			exit(2);
		}
	}
Continue:

	/* Now we are ready to start the algorithm. The policy is that
	 * whenever a new relation is found, it is immediately compared with
	 * all of the inverse relations. This seems to be most efficient. It
	 * might also be a good idea to immediately compare new relations
	 * with any other very short relations. Otherwise, a new relation has
	 * to wait in the queue before being compared with the others. */

	if (format)
		fprintf(op, "Format 2.2\n");

	signal(SIGINT, interrupt);
	signal(SIGKILL, interrupt);
	signal(SIGQUIT, interrupt);
	for (k = ninvrels + 1; k <= nrels; k++) {
		for (l = 1; l <= ninvrels; l++) {
			consider(k, l);
			consider(l, k);
			if (tidyon)
				break;
		}
		if (tidyon)
			break;
	}

	nnewrels = 0;
	for (i = ninvrels + 1; i <= nrels; i++) {
		for (j = 1; j <= i; j++) {
			onrels = nrels;
			if (tidyon)
				break;
			consider(i, j);
			if (i != j)
				consider(j, i);
			if (nrels > onrels)
				/* Compare new relations with inverse
				 * relations */
				for (k = onrels + 1; k <= nrels; k++) {
					for (l = 1; l <= ninvrels; l++) {
						if (tidyon)
							break;
						consider(k, l);
						consider(l, k);
					}
					if (tidyon)
						break;
				}
			if (tidyon)
				break;
		}
		/* If we have more than tidint new relations since last
		 * tidying up, then tidy; that is, go through all relations,
		 * remove the redundant ones, and then redefine the FSA. This
		 * will usually reduce its number of states. */
		if (tidyon) {
			i = tidyup(i) - 1;
			if (onintr) {
				output();
				exit(1);
			}
			tidyon = nnewrels = 0;
		}
	}

	/* Algorithm has now completed. */
	fprintf(op, "  #System is confluent.\n");
	/* Perform final tidying. */
	nrels = tidyup(nrels);
	output();
	exit(0);
}

insert(relno)
	int             relno;

/* Look at the words in test1 and test2, and remove any common prefixes and
 * suffixes. If they are not both empty at that stage, then use procedure
 * "compare" to see which word comes first in the ordering. Then install them
 * and point word1 and word2 at them. */
{
	int             bigger;
	char           *ptr1,
	               *ptr2,
	               *t1,
	               *t2,
	               *ts,
	               *ptre1,
	               *ptre2;

	ptr1 = test1;
	ptr2 = test2;
	ptre1 = ptr1 + strlen(test1);
	ptre2 = ptr2 + strlen(test2);
	while (ptr1 < ptre1)
		if (*ptr1 == *ptr2)
			*(ptr1++) = *(ptr2++) = 0;
		else
			break;
	if (ptr1 == ptre1 && ptr2 == ptre2)
		return (0);
	if (strlen(test1) && strlen(test2))
		while (*(ptre1 - 1) == *(ptre2 - 1) && ptre1 > ptr1 && ptre2 > ptr2) {
			ptre1--;
			ptre2--;
		}
	t1 = ptr1;
	t2 = ptr2;
	*ptre1 = *ptre2 = '\0';

	/* Common prefixes and suffixes have now been stripped
	   This portion of the code has been COMMENTED OUT because
	   it caused problems with the Recursive Path Ordering. */
	/* Transfer inverses to other side if possible.
	   Changed from old version so as to be more general for cases
	   where we replace a smaler segment with a larger one */
/*	if (strlen(t2) > strlen(t1)) {	*/ /* make t1 the longest */
/*		ts = t1;
		t1 = t2;
		t2 = ts;
		ts = ptre1;
		ptre1 = ptre2;
		ptre2 = ts;
	}
	while (strlen(t1) - strlen(t2) > 2) {*/	/* do all the transfers */
/*		ptre1--;
		*ptre2 = inv_of[*ptre1];
		*ptre1 = '\0';
		*(++ptre2) = '\0';
	}

	bigger = compare(t1, t2);*/	/* Now compare the two... */
/*	if (bigger == 2) {
		ts = t1;
		t1 = t2;
		t2 = ts;
		ts = ptre1;
		ptre1 = ptre2;
		ptre2 = ts;
	} */

	bigger = compare(t1, t2);	/* Compare the two... */
	if (bigger == 2) {
		ts = t1;
		t1 = t2;
		t2 = ts;
		ts = ptre1;
		ptre1 = ptre2;
		ptre2 = ts;
	}
	/* t1 is biggest according to the ordering, so it becomes the LHS of
	 * the new relation. */
	tmalloc(word1[relno], char, strlen(t1) + 1)
		ptr1 = t1;
	ptr2 = word1[relno];
	while (ptr1 < ptre1)
		*(ptr2++) = *(ptr1++);
	*(ptr2++) = '\0';
	tmalloc(word2[relno], char, strlen(t2) + 1)
		ptr1 = t2;
	ptr2 = word2[relno];
	while (ptr1 < ptre2)
		*(ptr2++) = *(ptr1++);
	*(ptr2++) = '\0';

	modifyfsa(relno);
	return (1);
}



consider(k, l)
	int             k,
	                l;

/* The left hand sides of the relations k and l are considered for common
 * parts, according to the Knuth-Bendix procedure, and any new relations
 * produced are processed. */
{
	char            ok,
	               *midwd,
	               *ptr1,
	               *ptr2,
	               *ptr,
	               *testwd1,
	               *testwd2;

	midwd = testwd1 = word1[k];
	testwd2 = word1[l];
	while (*midwd != '\0') {
		ptr1 = midwd;
		ptr2 = testwd2;
		ok = 1;
		while (*ptr1 != '\0' && *ptr2 != '\0') {
			if (*ptr1 != *ptr2) {
				ok = 0;
				break;
			} ptr1++;
			ptr2++;
		}
		if (ok)
			/* A common segment has been found. The new equation
			 * for consideration is inserted into words test1 and
			 * test2. */
		{
			if (*ptr1 == '\0') {
				ptr1 = testwd1 - 1;
				ptr = test1 - 1;
				while (++ptr1 < midwd)
					*(++ptr) = *ptr1;
				ptr1 = word2[l] - 1;
				while (*(++ptr1) != '\0')
					*(++ptr) = *ptr1;
				*(++ptr) = '\0';
				ptr1 = word2[k] - 1;
				ptr = test2 - 1;
				while (*(++ptr1) != '\0')
					*(++ptr) = *ptr1;
				ptr1 = ptr2 - 1;
				while (*(++ptr1) != '\0')
					*(++ptr) = *ptr1;
				*(++ptr) = '\0';
			} else {
				ptr2 = testwd1 - 1;
				ptr = test1 - 1;
				while (++ptr2 < midwd)
					*(++ptr) = *ptr2;
				ptr2 = word2[l] - 1;
				while (*(++ptr2) != '\0')
					*(++ptr) = *ptr2;
				ptr2 = ptr1 - 1;
				while (*(++ptr2) != '\0')
					*(++ptr) = *ptr2;
				*(++ptr) = '\0';
				ptr2 = word2[k] - 1;
				ptr = test2 - 1;
				while (*(++ptr2) != '\0')
					*(++ptr) = *ptr2;
				*(++ptr) = '\0';
			}
			/* Now reduce test1 and test2, using the current
			 * relations, and then install them if they are
			 * different. */
			reduce(test1, 0);
			reduce(test2, 0);
			if (insert(nrels + 1)) {
				nrels++;
				if (nrels >= mrels)
					exit(2);
				/* Decide if it is time to tidy up. */
				if (++nnewrels == tidint)
					tidyon = 1;
			}
		}
		midwd++;
	}
}

reduce(w, i)
	char           *w;
	int             i;

/* Reduce "w", by replacing any occurrences of the LHS of the current
 * relations by their LHS. The FSA is used for this. The complete sequence of
 * states that it goes through on reading "w" is remembered in the array
 * "history". If i!=0, do not use equation i for reduction. */
{
	int             l,
	                len,
	                st,
	                longer;
	char           *midwd,
	               *ptr1,
	               *ptr2,
	               *ptr;

	midwd = w;
	len = 0;
	history[0] = 1;
	st = 1;
	while (*midwd != '\0') {
		st = history[++len] = fsa[*midwd][st];
		if (st < 0 && st != -i)
			/* st= -n means that we have just read the LHS or
			 * relation  n. Replace it by RHS, and go back to the
			 * beginning of that subword. */
		{
			st = -st;
			ptr1 = midwd;
			ptr2 = word2[st] - 1;
			if ((longer = (strlen(word2[st]) - strlen(word1[st]))) > 0)
				/* With the current version of "compare" this
				 * cannot happen, but it has been put in in
				 * case it is ever desired to replace a
				 * segment by a longer one. */
			{
				ptr = w + strlen(w);
				while (ptr > midwd) {
					*(ptr + longer) = *ptr;
					ptr--;
				}
				ptr1 += longer;
			}
			len -= strlen(word1[st]);
			midwd = w + len - 1;
			ptr = midwd;
			while (*(++ptr2) != '\0')
				*(++ptr) = *ptr2;
			if (ptr != ptr1) {
				while (*(++ptr1) != '\0')
					*(++ptr) = *ptr1;
				*(++ptr) = '\0';
			}
			st = history[len];
			/*	midwd--;*/
		}
		midwd++;
	}
}

tidyup(done)
	int             done;

/* Remove redundant relations. "done" is the current relation being
 * processed. */
{
	int             i,
	                j,
	                k,
	                inew,
	                nnrels,
	                ret,
	                ndiff,
	                l1,
	                l2;
	char            c,
	                moving,
	                retain,
	                changed,
	               *w1,
	               *w2,
	               *ptr1,
	               *ptr2,
	               *ptre1,
	               *ptre2,
	               *elim,
	              **word1n,
	              **word2n,
	               *ch,
	               *t1,
	               *t2;

	lasttid = nrels;
	tmalloc(elim, char, nrels + 1)
		tmalloc(word1n, char *, nrels + 1)
		tmalloc(word2n, char *, nrels + 1)
		tmalloc(ch, char, nrels + 1)
		for (i = 1; i <= nrels; i++)
		elim[i] = 0;
	for (i = 1; i <= nrels; i++)
		ch[i] = 0;
	nnrels = 0;
	moving = 0;
	ret = done;
	changed = 0;
	for (i = 1; i <= nrels; i++) {
		fflush(stdout);
		strcpy(test1, word1[i]);
		strcpy(test2, word2[i]);
		k = checkreduce(test1, i);
		if (k == 1) {
			retain = 1;
			reduce(test2, i);
			if (strcmp(test2, word2[i])) {
				t1 = test1;
				t2 = test2;
				changed = ch[i] = 1;
			}
		} else {
			reduce(test1, i);
			reduce(test2, i);
			retain = strcmp(test1, test2);
			if (retain) {
				if (compare(test1, test2) == 1) {
					t1 = test1;
					t2 = test2;
				} else {
					t1 = test2;
					t2 = test1;
				}
				changed = ch[i] = 1;
			}
		}
		if (ch[i]) {
			while ((l1 = strlen(t1)) - (l2 = strlen(t2)) > 2) {
				t2[l2] = inv_of[t1[l1 - 1]];
				t1[l1 - 1] = t2[l2 + 1] = '\0';
			}
			tmalloc(word1n[i], char, strlen(t1) + 1)
				tmalloc(word2n[i], char, strlen(t2) + 1)
				strcpy(word1n[i], t1);
			strcpy(word2n[i], t2);
		}
		if (retain == 0) {
			moving = 1;
			elim[i] = 1;
		}
	}
	if (moving || changed) {
		moving = 0;
		for (i = 1; i <= nrels; i++)
			if (elim[i] == 0) {
				nnrels++;
				if (ch[i]) {
					fflush(stdout);
					tfree(word1[i]) tfree(word2[i])
						word1[i] = word1n[i];
					word2[i] = word2n[i];
				}
				if (moving)
					/* "moving" means that some previous
					 * relations have not been retained,
					 * and so we have to renumber words. */
				{
					fflush(stdout);
					word1[nnrels] = word1[i];
					word2[nnrels] = word2[i];
					word1[i] = word2[i] = 0;
				}
				if (i <= done)
					ret = nnrels;
			} else {
				moving = 1;
				if (i <= ninvrels)
					ninvrels--;
				fflush(stdout);
				tfree(word1[i]) tfree(word2[i])
			}
	} else
		nnrels = nrels;
	/* If any relations have been changed or eliminated, then we
	 * redisplay the remainder, and completely reconstruct the FSA from
	 * scratch. This is not too time consuming provided that we do not
	 * tidy up too often. */
	if (moving || changed) {
		nrels = nnrels;
		nstates = num_gens + 1;
		for (i = 1; i <= num_gens; i++)
			for (j = 1; j <= nstates; j++)
				fsa[i][j] = i + 1;
		for (i = 1; i <= nrels; i++)
			modifyfsa(i);
	}
	times(&buffer);
	i = buffer.tms_utime / 60;
	fprintf(op,
		"  #There are now %d equations and the fsa has %d states. Time=%d secs\n",
		nrels, nstates, i);
	fflush(op);
	if (autohalt && (nrels > maxeqns || nstates > maxstates || i > maxtime))
		onintr = 1;

	return (ret);
}

checkreduce(w, i)
	char           *w;
	int             i;

/* This returns 0 if "w" involves the LHS of any relation other than the ith.
 * (It is applied to the LHS of the ith relation when tidying.) */
{
	int             st;

	st = 1;
	while (*w != '\0') {
		st = fsa[*(w++)][st];
		if (st < 0 && st != -i)
			return (0);
	}
	return (1);
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
	if (*ptr == '\0')
		while (++cgp <= cge)
			*cgp = -relno;
	else {
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


lex_compare(w1,w2)
	char		*w1,
			*w2;

/* Compare words w1 and w2 to see which is bigger according to the ordering.
 * The ordering used here is longer words are bigger, and amongst equal
 * length words, lexicographical ordering according to the order of the
 * generators as input is used. (I use 'bigger' to mean greater in the
 * ordering.)  Returns 1 if w1 bigger than w2, otherwise 2.  */
{
	int len1,len2,bigger;
	char *we1,*we2;

	len1=strlen(w1);
	len2=strlen(w2);
	we1=w1+len1;
	we2=w2+len2;
	if (len1>len2)
		bigger=1;
	else if (len2>len1)
		bigger=2;
	else
		while (w1<we1) {
			if (*w1>*w2) {
				bigger=1;
				break;
			} else if (*w2>*w1) {
				bigger=2;
				break;
			}
			w1++;
			w2++;
		}
	return(bigger);
} 


rec_compare(w1, w2)
	char           *w1,
	               *w2;

/* Compare words w1 and w2 to see which is 'bigger' according to the
   ordering. The ordering used here is recursive path ordering (based on
   that described in the book "Confluent String Rewriting" by Matthias Jantzen,
   Defn 1.2.14, page 24).
   Returns 1 if w1 is bigger than w2, otherwise 2.
   ----------------------------------------------------------------------
   The ordering is as follows:

   let u, v be elements of X*
   u > v iff one of the following conditions is fulfilled;

   1) u = v			OR
   u = au', v = bv' for some a,b elements of X, u',v' elements of X*
   and then:
   2) a = b and u' > v'		OR
   3) a > b and u  > v'		OR
   4) u'> v
   ----------------------------------------------------------------------
   Written by : Jamie P. Curmi (February 1992) */
{
	if (strlen(w1) == 0 || strlen(w2) == 0)
		return (strlen(w1) == 0) ? 2 : 1;
	if (strcmp(w1, w2) == 0)
		return 1;
	if (*w1 == *w2)
		return (rec_compare(++w1, ++w2));
	if (*w1 > *w2)
		return (rec_compare(w1, ++w2));
	return (rec_compare(++w1, w2));
}

/* COMPARE: Compares two words 'w1' and 'w2' to see which is 'bigger'
   according to the ordering to be used (Set by the global 'Ordering').
   If more ordering options are to be provided, the new function should
   be given a different name and called from a 'case' statement in this
   function.  Returns 1 if w1 is bigger than w2, otherwise 2. */
compare(w1,w2)
	char *w1, *w2;
{
	switch (Ordering) {

		case RECURSIVE:		/* Recursive path ordering */
			return (rec_compare(w1,w2));

		default: /* LEXICOgraphical ordering */
			return (lex_compare(w1, w2));
	}
}

void printword(w)
char *w;
/* Display a given word**********************************value (stored as
   a char) as it's string st**************et.  It is more efficient for
   generators to be stored as chars, but ******************** we can not
   print ***********h.  */
{
        char *p;

        for(p = w; *p != '\0'; p++)
                fprintf(stderr, "%d", (int) *p);
}
