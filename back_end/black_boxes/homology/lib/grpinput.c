/* grpinput.c     - library file for applications reading in output files from
   kbeqn.
   ===========================================================================
   Written by:  Jamie P. Curmi
   Date:        June - July 1992
   Project:     Computational Tools for Homology of Groups
   For:         Dr J. R. J. Groves and Prof. C. Miller
                Department of Mathematics, University of Melbourne,
                Parkville, Victoria, Australia.
   ===========================================================================
   The following global variables are defined in this file:
        int     Num_states,
                Num_gens,
                Num_rules,
                Start,
                **Fsa;

        char    **Word1,
                **Word2,
                **Alphabet;

   Input is expected to be either from a file, or if no filename is given,
   from standard input.  It should conform to the Geometry Group Standard
   format, and in particular should be of the form outputted by kbeqn - that
   is, we can just use the output from kbeqn as the input to our program.
*/

#include <stdio.h>
#include "homlib.h"
#include "grpinput.h"

/* INPUT ROUTINES not exported */
static char*	readstring(FILE *fp, char *s);
static int		skip_until(FILE *fp, char *s);
static char*	get_word(FILE *fp);

/**** GLOBALS ****/
int			Num_states,				/* Number states in Fsa */
			Num_gens,				/* Number of generators in group */
			Num_rules,				/* Number of Rewriting rules */
			Start,					/* Start state of the Fsa */
			**Fsa,					/* Finite State Automata */
			Next[_HOM_MAX_SIZE];			/* Used by 'search' algorithm */

char		**Word1,	/* LHS of rewriting rules */
			**Word2,	/* RHS of rewriting rules */
			**Alphabet;	/* Gives the string version of a generator's
				      		 numeric value */



int read_file_info(FILE *fp)
/* Reads in from file pointed to by fp, information of the form supplied by
   the output of kbeqn or cfsa.  The function initialises all the simple global
   variables (Num_states, Num_gens, Num-rules, Start), and reads in the
   Fsa, creates 'Alphabet' which stores the generator strings in the array
   position indexed by the generators numeric value, and creates 'Word1'
   and 'Word2' (the LHS and RHS respectively of each rewriting rule) by
   converting each side of the Rewriting rule from its generator string form
   to its numeric form.
   The Fsa, Word1, Word2 and Alphabet are created as memory efficiently as
   possible (ie malloc'd to exact size required).  We can do this because
   exact number of states, generators etc are first determined.  
   Returns FORMAT_ERROR if a format error occurs, or NOT_A_GEN if something
   is encountered that is not a generator when a generator was expected,
   otherwise 0.  */
{
	char	keyword[_HOM_MAX_SIZE];
	int	i, j;

	if (skip_until(fp, "fsa"))
		return FORMAT_ERROR;

	if (skip_until(fp, "states"))
		return FORMAT_ERROR;
	Num_states = atoi(readstring(fp, keyword));

	if(skip_until(fp, "symbols"))
		return FORMAT_ERROR;
	Num_gens = atoi(readstring(fp, keyword));

	if (skip_until(fp, "alphabet"))
		return FORMAT_ERROR;
	if(skip_until(fp, "{"))
		return FORMAT_ERROR;

	/* Create 'Alphabet' */
	Alphabet = NEW(char *, Num_gens + 1);
	for (i=1; i <= Num_gens; i++) {
		if (skip_until(fp,"="))
			return FORMAT_ERROR;
		readstring(fp, keyword);
		if (keyword[strlen(keyword) -1] == '}')
			/* generator terminated by a '}', so get rid of it */
			keyword[strlen(keyword) - 1] = '\0';
		Alphabet[i] = NEW(char, strlen(keyword) + 1);
		strcpy(Alphabet[i], keyword);
	}

	if (skip_until(fp, "start"))
		return FORMAT_ERROR;
	if(skip_until(fp, "{"))
		return FORMAT_ERROR;
	/* Infact, kbeqn's Fsa  always has Start = 1.  This is included
	   so that should a different Fsa that does not start at 1 be used,
	   the program will still work.  */
	Start = atoi(readstring(fp, keyword));

	/* Create the Fsa */
	/* We want to know the exact number of rewriting rules before we
	   read them so that we can create space for them efficiently.
	   We do this by looking through the fsa as we read it, and finding
	   the max of the -ve of each entry (ie the most negative indicates
	   the last rule that is used, and hence the number of rules).  */
	if (skip_until(fp, "atable"))
		return FORMAT_ERROR;
	Fsa = NEW(int *, Num_states + 1);
	for (i = 1; i <= Num_states; i++) {
		Fsa[i] = NEW(int, Num_gens + 1);
		if (skip_until(fp, "A"))
			return FORMAT_ERROR;
		for (j=1; j <= Num_gens; j++) {
			Fsa[i][j] = atoi(readstring(fp, keyword));
			Num_rules = MAX(Num_rules, -Fsa[i][j]);
		}
	}

	/* Read the rewriting rules */
	/* Word1 = LHS, Word2 = RHS */
	if (skip_until(fp, "rels"))
		return FORMAT_ERROR;
	if (skip_until(fp, "{"))
		return FORMAT_ERROR;
	Word1 = NEW(char *, Num_rules + 1);
	Word2 = NEW(char *, Num_rules + 1);
	for (i=1; i <= Num_rules; i++) {
		if ((Word1[i] = get_word(fp)) == NULL)	/* Get the LHS */
			return NOT_A_GEN;
		if (skip_until(fp, ">"))
			return FORMAT_ERROR;
		if ((Word2[i] = get_word(fp)) == NULL)	/* Get the RHS */
			return NOT_A_GEN;
	}
	return 0;
}



static char *readstring(FILE *fp, char *s)
/* Reads a string of non-white-space characters from 'fp' into 's', skipping
   over any line that begins with a '#' (indicating a comment). 
   Returns NULL if EOF, otherwise the string itself.  */
{
	int             c;

	while ((c = fscanf(fp,"%s",s)) != EOF && *s == '#')
		while((c=fgetc(fp)) != '\n' && c != EOF)
			;
	return (c == EOF) ? NULL : s;	/* Check if EOF */
}



static int skip_until(FILE *fp, char *s)
/* reads from fp, one string of non-white-space chars at a time, until the
   string matches 's'.  If the 's' is not found, returns 1, otherwise 0. */
{
	char	keyword[_HOM_MAX_SIZE], *n;

	while ((n = readstring(fp, keyword)) != NULL && strcmp(keyword,s))
		;
	if (n == NULL)
		return 1;
	return 0;
}



static char* get_word(FILE *fp)
/* Reads a word (with the word written in generator string form (eg a*b^-1)),
   converts it to a string of numeric values (eg 34) and returns this string.
   Note: the word "epsilon" is a keyword used to represent the Identity. 
   Return NULL if it encounters something that is not a generator.  */
{
	char	*key, gen[_HOM_MAX_SIZE], keyword[_HOM_MAX_SIZE], *word;
	int	i,n, count = 0;		/* n is the size of each generator
					   read by sscanf */

	readstring(fp, keyword);
	if (strcmp(keyword, "epsilon")) {	/* NOT the identity */
		/* Convert all '*' symbols to a space so as to separate
		generators from each other.  This makes processing easier. */
		for (i=0, count = 0; keyword[i] != '\0'; i++)
			if (keyword[i] == '*') {
				count++;
				keyword[i] = ' ';
			}

		word = NEW(char, count + 2);
		key = keyword;
		for (i=0; i< count + 1; i++) {
			/* get each generator... */
			sscanf(key, "%s%n", gen, &n);
			key+=n;	/* Move to next generator */
			/* Convert the generator to its numeric value */
			if ((word[i] = genstr_to_genint(gen)) == 0)
				return NULL;
				/*	ERROR(NOT_A_GEN);*/
		}
		word[i] = 0;	/* end string with a 0 */
	} else {	/* the keyword was "epsilon" */
		word = NEW(char, 1);
		word[0] = 0;	/* Empty string */
	}
	return word;
}



char genstr_to_genint(char *gen)
/* Converts a generator string 'gen' to the generator's numeric value.  
   Returns 0 if the string is not in the Alphabet.     */
{
	int	i;

	for (i=1; i <= Num_gens && strcmp(gen, Alphabet[i]); i++)
		;	/* Look for the string in the Alphabet */
	return (i == Num_gens + 1) ? 0 : i;
}
