/* rewrite.c     - library file for applications that use rewriting routines
   ===========================================================================
   Written by:  Jamie P. Curmi
   Date:        June - July 1992
   Project:     Computational Tools for Homology of Groups
   For:         Dr J. R. J. Groves and Prof. C. Miller
                Department of Mathematics, University of Melbourne,
                Parkville, Victoria, Australia.
   ===========================================================================
   Users will need the following global variables:
        int     Num_states,
                Num_gens,
                Num_rules,
                Start,
                **Fsa;

        char    **Word1,
                **Word2,
                **Alphabet;

   These can be initialised through the routines in grpinput.h
*/
#include <stdio.h>
#include "homlib.h"
#include "rewrite.h"

/* OTHER FUNCTIONS not exported */
static void			initnext(char *p);
static int			search(char *p, char *a);
static CRITICALS	*find_n_criticals_rec(char *word, int n);
static int 			cmp(void *a, void *b);

/**** GLOBALS ****/
extern int		Num_rules,		/* Number of Rewriting rules */
				Start,			/* Start state of the Fsa */
				**Fsa;			/* Finite State Automata */

extern char		**Word1,	/* LHS of rewriting rules */
				**Word2,	/* RHS of rewriting rules */
				**Alphabet;

/* not exported... */
static int		Next[_HOM_MAX_SIZE];	/* Used by 'search' algorithms */

extern void printword(FILE *fp, char *word);


char *reduce(char *word)
/* Reduce 'word', by replacing any occurrences of the LHS of the current
   relations by their RHS. The FSA is used for this.  'History' is used to
   remember the complete sequence of states it goes through on reading.
   'word' is altered, and is also returned by the function.  This allows
   the function to be used in two ways, eg:
	reduce(w);
   or
	if strcmp(reduce(w1),w2) then ....

	NOTE: word must have enough room to handle the ENTIRE word after rewriting!
*/
{
	int	len, st, longer, history[_HOM_MAX_LEN];
	char	*midwd, *ptr1, *ptr2, *ptr;

	len = 0;
	history[0] = st = Start;
	for (midwd = word; *midwd != '\0'; midwd++) {
		st = history[++len] = Fsa[st][*midwd];
		if (st < 0) { /* st = -n means that we have just read the LHS
				 of relation  n. Replace it by RHS, and go
				 back to the beginning of that subword. */
			st = -st;
			ptr1 = midwd;
			ptr2 = Word2[st] - 1;
			if ((longer = (strlen(Word2[st]) - strlen(Word1[st]))) > 0) {
			/* Some rewriting rules may replace a shorter word
			   by a longer one.  This part of the code pushes
			   everything over so that room exists for the
			   replacement. */
				for (ptr = word + strlen(word); ptr > midwd; ptr--)
					*(ptr + longer) = *ptr;
				ptr1 += longer;
			}

			/* Go back to beginning of subword */
			len -= strlen(Word1[st]);
			ptr = midwd = word + len - 1;

			/* Replace with RHS of rewriting rule */
			while (*(++ptr2) != '\0')
				*(++ptr) = *ptr2;
			if (ptr != ptr1) {
				while (*(++ptr1) != '\0')
					*(++ptr) = *ptr1;
				*(++ptr) = '\0';
			}
			st = history[len];/* go back to previous state in Fsa */
		}
	}
	return word;
}



char *critical(char *word, R_INFO *r_info)
/* Finds first occurence of a critical tuple (eg, n=2 => critical pair,
   n=3 => critical triple)
   Returns the beginning of the critical, and r_info, containing
   the following information:
	r_info.n			 =  number of tuples found.
	r_info.word[i].where = ith place where a rewriting rule should be applied.
	r_info.word[i].which = the rewriting rule that should be applied.
*/
{
	int	st, pos=0;
	char	*crit, *last;

	/* Find first application of a rule (ie a LHS) */
	for (st = Start; *word != '\0' && st >= 0; word++, pos++)
		st = Fsa[st][*word];
	if (st >= 0) {		/* Already reduced! */
		r_info->n = 0;
		return NULL;
	}
	pos = pos - strlen(Word1[-st]) + 1;
	r_info->n = 1;
	r_info->apply[0].where = pos - 1;
	r_info->apply[0].which = -st;
	last = word - 1;		/* Record end of 1st word to rewrite */
	word = word - strlen(Word1[-st]) + 1;/* Go back to beginning of word */
	crit = word - 1;		/* Record position of critical */

	for (pos++, st = Start; *word != '\0'; word++, pos++) {
		st = Fsa[st][*word];	/* Use Fsa to find where to rewrite */
		if (st < 0) {	/* A rewriting rule exists... */
			word = word - strlen(Word1[-st]) + 1;
			if (word > last)  /* No overlap => STOP!! */
				break;
			pos = pos - strlen(Word1[-st]) + 1;
			r_info->apply[r_info->n].where = pos - 1;
			r_info->apply[r_info->n].which = -st;
			++r_info->n;		  /* Overlap, so increase degree */
			st = Start;
		}
	}
	return crit;
}

int decisions(char *word, R_INFO *r_info)
/* Finds all places where rewriting rules can be used in a given word
   Returns the num of rewriting rules found, and r_info, containing
   the following information:
	r_info.n			 =  number of rewriting rules found
	r_info.word[i].where = ith place where a rewriting rule should be applied.
	r_info.word[i].which = the rewriting rule that should be applied.
*/
{
	int	st, pos=0;

	r_info->n = 0;

	for (st = Start; *word != '\0'; word++,pos++) {
		st = Fsa[st][*word];	/* Use Fsa to find where to rewrite */
		if (st < 0) {	/* A rewriting rule exists... */
			word = word - strlen(Word1[-st]) + 1;
			pos = pos - strlen(Word1[-st]) + 1;
			r_info->apply[r_info->n].where = pos;
			r_info->apply[r_info->n].which = -st;
			++r_info->n;		  /* Overlap, so increase degree */
			st = Start;
		}
	}
	return r_info->n;
}

static void initnext(char *p)
/* Initialises the Next array, for use with the Knuth-Morris-Pratt search
   algorithm.  This must be called, with the pattern to be matched as it's
   argument, before using the 'search' function.  The algorithm takes the
   character that caused the mismatch into account, so as to give a more
   efficient algorithm.  'Next' determines how far to back up the pattern to
   be matched when a mismatch occurs.  It is Global so as to avoid having
   to pass the variable over and over again for each pattern match.  */
{
	int	i, j, m = strlen(p);

	Next[0] = -1;	/* -1 is used so after loop is iterated in 'search',
					   j becomes 0 (ie begin matching again). */
	for (i = 0, j = -1; i < m; i++, j++,  /* match pattern against itself */
		Next[i] = (p[i] == p[j]) ? Next[j] : j) {/* Take mismatch into account */
		while ((j >= 0) && (p[i] != p[j])) {
			j = Next[j];
		}
	}
}

static int search(char *p, char *a)
/* Search for pattern string 'p' in string 'a' using the Knuth-Morris-Pratt
   pattern matching algorithm.  The function returns the index to the string
   'a' where a match occurs, or where there is a partial match at the end
   of the string 'a' (eg. if p = "abac" and a = "dcadcdab", the function
   would return 6 ).
   NOTE:  A CALL TO 'initnext' MUST BE MADE BEFORE MATCHING THE
          PATTERN 'p' TO ANY STRING.   */
{
	int	i, j, m = strlen(p), n = strlen(a);

	/* Stop if at end, or j == m (which implies a match) */
	for (i = 0, j = 0; j < m && i < n; i++, j++)
		while ((j >= 0) && (a[i] != p[j]))
			j = Next[j];
	return i-j;	/* position of match */
}

void find_criticals(CRITICALS *tuples)
/* Takes a rewriting system consisiting of LHS in 'Word1' array, of which
   there are 'Num_rules' rules (both Globals), and lists all critical 2-tuples
   that could possibly occur from these rules.  These are stored in the
   CRITICALS structure,  which must be big enough to hold the criticals found.
   This is performed by matching each LHS with other LHS to form an overlap.
   The two combined rules are then stored in the record 'tuples' along with
   the number of criticals.

   *** NOTE: THIS ONLY GIVES 2-TUPLES.  USE FIND_N_CRITICALS OTHERWISE ***

   Overlaps are handled so as to provide for the occurence of multiple
   overlaps.  Eg. the two words "dcaba" and "abac" can form criticals:
   "dcabac" and "dcababac".

   Since we are only looking for overlaps, we need not check an entire
   string for a given pattern.  Eg. if we are searching for the overlap
   of "dcaba" and "abac", there is no need to check for overlap with the
   first "d" and "c" in word 1, since we want the words to overlap at the end.
*/
{
	int		i,j, len, stri, strj, count;
	char	*combined;

	tuples->num = 0;

	for (i=1; i <= Num_rules; i++) {  /* For each rule's LHS... */
		stri = strlen(Word1[i]);

		for (j = 1; j <= Num_rules; j++) { /* Combine with other LHS's */
			strj = strlen(Word1[j]);
			initnext(Word1[j]);	/* only do once for each Word1[j] */

			for (count = MAX(stri - strj,0) + 1; j != i && count < stri;) {
			/* 'count' is used to track where in the first word we begin
			   looking for an overlap.  We start the matching process only
			   as far back as we need to go, but the furthest we can go back
			   is 0 ... hence the use of MAX.  */

				if ((len = search(Word1[j], Word1[i] + count)) < (stri - count)) {
					/* Found an overlap!  Copy both words into 'combined'
					   to form a critical.  */
					combined = NEW(char, len + count + strj + 1);
					strncpy(combined, Word1[i], len + count);
					strncpy(combined + len + count, Word1[j], strj);

					tuples->info[tuples->num++]->word = combined;

				}
				count = count + len + 1; /* move to next pos for a possible
										    multiple ovelap */
			}
		}
	}
}


char *apply(char *word, int pos, int rule)
/* Applies the rewriting rule number 'rule' to the word at position 'pos' in
   the word.  Returns the re-written word.  NOTE the original word is still
   in existance (the memory is not free'd!!)		*/
{
	char *newword;

	/* Make it the right size, even if replacing smaller by larger */
	newword = NEW(char, strlen(word) + strlen(Word2[rule]) 
					-strlen(Word1[rule] + 1));
	strncpy(newword, word, pos);	/* Copy first part */
	strcpy(newword+pos, Word2[rule]);	/* Replace LHS with RHS */
	strcpy(newword+pos+strlen(Word2[rule]), word+pos+strlen(Word1[rule]));	/* Copy rest */

	return newword;
}

int find_rule(char *word, int pos, int len)
/* Given a word, and a substring at position 'pos' and length 'len', returns
   the rewriting rule number or 0 if it does not exist. */
{
	int i;
	char *test;

	test = NEW(char, len + 1);
	strncpy(test, word+pos, len);
	test[len] = '\0';

	for (i=1; i <= Num_rules; i++)
		if (strcmp(test, Word1[i]) == 0) {
			free(test);
			return i;
		}
	free(test);
	return 0;
}


int any_rewriting_rules(char *word, int len)
/* Given a word of length 'len', returns _TRUE if any rewriting rules are
   in that word, _FALSE otherwise. */
{
	char	*temp;
	int		k;

	if (len == 0)
		return _FALSE;
	temp = NEW(char, len + 1);
	strncpy(temp, word, len);
	temp[len] = '\0';

	for (k=1; k <= Num_rules; k++) {
		initnext(Word1[k]);
		if (search(Word1[k], word) <= len - strlen(Word1[k])) {
			free(temp);
			return _TRUE;
		}
	}
	free(temp);
	return _FALSE;
}


CRITICALS *find_n_criticals(int n)
/* Takes a rewriting system consisiting of LHS in 'Word1' array, of which
   there are 'Num_rules' rules (both Globals), and lists all critical n-tuples
   that could possibly occur from these rules.  These are stored in the
   CRITICALS structure,  which must be big enough to hold the criticals found.
   This is performed by matching each LHS with other LHS to form an overlap.
   The two combined rules are then stored in the record 'tuples' along with
   the number of criticals.  The returned tuples are sorted to aid matching
   algorithms in other functions.

   Overlaps are handled so as to provide for the occurence of multiple
   overlaps.  Eg. the two words "dcaba" and "abac" can form criticals:
   "dcabac" and "dcababac".

   Since we are only looking for overlaps, we need not check an entire
   string for a given pattern.  Eg. if we are searching for the overlap
   of "dcaba" and "abac", there is no need to check for overlap with the
   first "d" and "c" in word 1, since we want the words to overlap at the end.
*/
{
	int			i, k;
	CRITICALS	*tuples, *next_tuples;

	tuples = NEW(CRITICALS, 1);
	tuples->num = 0;

	for (i=1; i <= Num_rules; i++) {
		next_tuples = find_n_criticals_rec(Word1[i], n);
		for (k=0; k < next_tuples->num; k++) {
			if (tuples->num > _HOM_MAX_CRITICALS - 1)
				ERROR("ERROR: Too many critical tuples!\n");
			/* Don't free up words, just make our structure point to them */
			tuples->info[tuples->num++] = next_tuples->info[k];
		}
		free(next_tuples);
	}

	/* Sort the tuples so they are in order */
	qsort(tuples->info, tuples->num, sizeof(CRITICAL_INFO **), cmp);
	return tuples;
}


static
int cmp(void *a, void *b)
/* Comparison function used in qsort */
{
    int scomp;
    CUBE_INFO *p, *pp;

	scomp = strcmp((*((CRITICAL_INFO **) a))->word,
							(*((CRITICAL_INFO **) b))->word);
	if (scomp == 0) {
		for (p = (*((CRITICAL_INFO **) b))->vert,
				pp = (*(CRITICAL_INFO **) a)->vert;
			p != NULL; p = p->next, pp = pp->next) {
			if (pp->pos < p->pos)
				return -1;
			if (pp->pos > p->pos)
				return 1;
		}
		return 0;
	}
	return scomp;
}


static
CRITICALS *find_n_criticals_rec(char *word, int n)
/* Recursively, given a word, finds all possible n-tuples beginning with that
   word.  This is called by find_n_criticals.  See that function for details.
*/
{
	int		j, k, len, stri, strj, strk, count;
	char	*combined;
	CRITICALS	*tuples, *next_tuples;
	CUBE_INFO	*p;

	tuples = NEW(CRITICALS, 1);
	tuples->num = 0;

	if (n==1) {
		tuples->num = 1;
		tuples->info[0] = NEW(CRITICAL_INFO, 1);
		tuples->info[0]->word = NEW(char, strlen(word) + 1);
		strcpy(tuples->info[0]->word, word);
		tuples->info[0]->vert = NEW(CUBE_INFO, 1);
		tuples->info[0]->vert->pos = 0;
		tuples->info[0]->vert->len = strlen(word);
		tuples->info[0]->vert->next = NULL;
		return tuples;
	}

	stri = strlen(word);
	for (j = 1; j <= Num_rules; j++) { /* Combine with other LHS's */
		strj = strlen(Word1[j]);
		initnext(Word1[j]);	/* only do once for each Word1[j] */

		for (count = MAX(stri - strj, 1); count < stri;) {
		/* 'count' is used to track where in the first word we begin
		   looking for an overlap.  We start the matching process only
		   as far back as we need to go, but the furthest we can go back
		   is 0 ... hence the use of MAX.  */

			if ((len = search(Word1[j], word + count)) < (stri - count)) {
				next_tuples = find_n_criticals_rec(Word1[j], n-1);
				for (k=0; k < next_tuples->num; k++) {
					/* Combine them, and clean up next_tuples */
					strk = strlen(next_tuples->info[k]->word);
					combined = NEW(char, len + count + strk + 1);
					strncpy(combined, word, len + count);
					strncpy(combined + len + count,
										next_tuples->info[k]->word, strk);
					*(combined+len+count+strk) = '\0';

					free(next_tuples->info[k]->word);
					tuples->info[tuples->num] = NEW(CRITICAL_INFO, 1);
					tuples->info[tuples->num]->word = combined;

					tuples->info[tuples->num]->vert = NEW(CUBE_INFO, 1);
					tuples->info[tuples->num]->vert->pos = 0;
					tuples->info[tuples->num]->vert->len = stri;
					tuples->info[tuples->num]->vert->next = next_tuples->info[k]->vert;
					free(next_tuples->info[k]);
					for (p = tuples->info[tuples->num]->vert->next; p!= NULL;
								p = p->next)
						p->pos += (count + len);
					tuples->num++;
				}
				free(next_tuples);
			}
			count = count + len + 1; /* move to next pos for a possible
									    multiple overlap */
		}
	}
	return tuples;
}


void display_critical(FILE *fp, CRITICAL_INFO *c)
/* Displays a given critical */
{
	CUBE_INFO *pp;

	printword(fp, c->word);
	
	for (pp = c->vert; pp != NULL; pp = pp->next)
		fprintf(fp, " [%d,%d]", pp->pos, pp->len);
	fprintf(fp, "\n");

}


void delete_criticals(CRITICALS *tuples)
/* Deletes a pointer to a CRITICAL structure, and it's associated words etc. */
{
	int			i;
	CUBE_INFO	*p, *temp;

	for (i=0; i < tuples->num; i++) {
		free(tuples->info[i]->word);
		for (p = tuples->info[i]->vert; p != NULL; p = temp) {
			temp = p->next;
			free(p);
		}
		free(tuples->info[i]);
	}
	free(tuples);
}


void remove_inverse_criticals(CRITICALS **tuples)
/* Removes all criticals of the form aa^-1aa^-1.... from tuples.
   Note that the original CRITICALS structure is free'd, so it must have
   been dynamically allocated. 
*/
{
	CUBE_INFO *p, *temp;
	int			i;
	CRITICALS	*newtuples = NEW(CRITICALS, 1);

	newtuples->num = 0;
	for (i = 0; i < (*tuples)->num ;i++)
		if (!is_inv_tuple((*tuples)->info[i]->word, 0,
									strlen((*tuples)->info[i]->word)))
			newtuples->info[newtuples->num++] = (*tuples)->info[i];
		else { /* Not an inverse tuples, so get rid of it */
			free((*tuples)->info[i]->word);
			for (p = (*tuples)->info[i]->vert; p != NULL; p = temp) {
				temp = p->next;
				free(p);
			}
			free((*tuples)->info[i]);
		}
	free(*tuples);
	
	*tuples = newtuples;
}
