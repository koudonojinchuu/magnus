/* 2dimbdry.c  - library code for calculating boundaries in homology
   ===========================================================================
   Written by:	Jamie P. Curmi
   Date:		July 1992
   Project:		Computational Tools for Homology of Groups
   For:			Dr J. R. J. Groves and Prof. C. Miller
				Department of Mathematics, University of Melbourne,
				Parkville, Victoria, Australia.
   ===========================================================================
   The library exports those functions and macros defined in 2dimbdry.h
   The library imports 5 global variables used by most parts of the project:
		extern int	Start;
		extern int	**Fsa;
		extern char	**Word1, **Word2;
		extern char	**Alphabet;
   Their use is as follows (see rewriting.h for more details):
		Fsa stores the Finite-State-Automata for a given rewriting system.
		Start is the initial state of the Fsa.
		Word1 is the LHS of our rewriting rules.
		Word2 is the RHS of our rewriting rules.
		Alphabet stores the string version of a generators numeric value.
*/

#include <stdio.h>
#include "homlib.h"
#include "rewrite.h"
#include "2dimbdry.h"

enum PATH_DIRECTION			{LEFT, RIGHT};

#define	SIGN(A)		(((A) < 0) ? '-' : '+')

/* Init boundary header for calculating boundaries with left actions */
#define	INIT_BOUNDARY_LA(A)	{A = NEW(BOUNDARY_LA, 1);	\
				 A->part = NULL;	\
				 A->rule = -1;		\
				 A->next = NULL;}

/* Init boundary header for calculating boundaries with all actions trivial*/
#define	INIT_BOUNDARY_TRIV(A)	{A = NEW(BOUNDARY_TRIV, 1);	\
				 A->count = 0;	\
				 A->rule = -1;		\
				 A->next = NULL;}

/* Functions not exported to programs that include this library */
static int	store_left_branch (char *node, R_INFO r_info, PATH_INFO *left_branch);
static void	quick_sort(char a[], int l, int r);
static void	add_to_boundary_la (BOUNDARY_LA *head, int count, char *action, int rule);
static void	remove_zero_counts_la(BOUNDARY_LA *head);

static void	add_to_boundary_triv (BOUNDARY_TRIV *head, int count, int rule);
static void	remove_zero_counts_triv(BOUNDARY_TRIV *head);

/* External Global variables used by the program */
extern int Start, Num_rules, Num_gens;
extern int **Fsa;
extern char **Word1, **Word2;
extern char **Alphabet;


void printword(FILE *fp, char *word)
/* Display a given word, by printing each of its numeric value (stored as
   a char) as it's string stored in Alphabet.  It is more efficient for
   generators to be stored as chars, but for display purposes we can not
   print them as such.  */
{
	char *p;

	for(p = word; *p != '\0'; p++)
		fprintf(fp, "%s", Alphabet[(int) *p]);
}


BOUNDARY_LA *calc_2_cube_boundary_la(char *word)
/* Given a word, creates, calculates and returns the boundary of it's
   corresponding 2-cube.  Returns NULL if no boundary.
   Uses should remember to use clean_up_boundary_la when they are finished
   with the returned structure.
*/
{
	BOUNDARY_LA		*boundary_head;
	R_INFO			r_info;
	PATH_INFO		left_branch[_HOM_MAX_PATH_LEN];
	char			*node, *temp, *left_action;
	int				i, num_in_left, apply_pos, apply_rule, found;

	/* Initialise boundary head.  Must be done before calling add_to_boundary */
	INIT_BOUNDARY_LA(boundary_head);

	/* Copy the word into node so we can manipulate it, but don't lose it */
	node = NEW(char, strlen(word) + 1);
	strcpy(node,word);

	/* skip until we get to a node that splits */
	while (decisions(node, &r_info) == 1) {
		temp = apply(node, r_info.apply[LEFT].where, r_info.apply[LEFT].which);
		free(node);
		node = temp;
	}

	if (r_info.n == 0) 		/* No boundary!!!! */
		return NULL;

	/* Store the left branch (taking right paths) for comparison of the
	   right branch.  This is done so as to determine when to stop traversing
	   the right branch because we don't need any further information. */
	num_in_left = store_left_branch (node, r_info, left_branch);

	/* Make a new copy of the first node to use */
	node = NEW(char, strlen(left_branch[0].word) + 1);
	strcpy(node, left_branch[0].word);

	/* Do the right branch.  Only store each node as it is encountered, and
	   free nodes that are no longer of use.  Stop when we reach a node that is
	   in the left branch.  */
	decisions(node, &r_info);	
	
	apply_pos =  r_info.apply[RIGHT].where;
	apply_rule =  r_info.apply[RIGHT].which;

	/* Keep left action */
	left_action = NEW(char, apply_pos + 1);
	strncpy(left_action, node, apply_pos);
	*(left_action + apply_pos) = '\0';

	/* keep left action sorted to aid in algebraic groupings */
	quick_sort(left_action, 0, apply_pos - 1);

	add_to_boundary_la(boundary_head, 1, left_action, apply_rule);
			/* +1 for right boundary */

	temp = apply(node, apply_pos, apply_rule);
	free (node);
	node = temp;

	/* Check to see if we have intersected the left branch.  This must be
	   done for rare cases where the intersection is immediate. */
	found = _FALSE;
	for (i = 0; !found && i <= num_in_left; i++)
		if (strcmp(left_branch[i].word, node) == 0)	/* in left branch! */
			found = _TRUE;

	while( !found) {	/* not intersecting left branch yet... */
		decisions(node, &r_info);	
		apply_pos =  r_info.apply[LEFT].where;
		apply_rule =  r_info.apply[LEFT].which;

		/* Keep left action */
		left_action = NEW(char, apply_pos + 1);
		strncpy(left_action, node, apply_pos);
		*(left_action + apply_pos) = '\0';

		/* keep left action sorted to aid in algebraic groupings */
		quick_sort(left_action, 0, apply_pos - 1);

		add_to_boundary_la(boundary_head, 1, left_action, apply_rule);
			/* +1 for right boundary */

		/* clean up the node */
		temp = apply(node, r_info.apply[LEFT].where, r_info.apply[LEFT].which);
		free (node);
		node = temp;

		/* Check to see if we have intersected the left branch */
		for (i = 0; !found && i <= num_in_left; i++)
			if (strcmp(left_branch[i].word, node) == 0)	/* in left branch! */
				found = _TRUE;
	}

	/* Do the left branch now!  Clean up nodes as we go...*/
	for (i=0; strcmp(left_branch[i].word, node); i++) {
		apply_pos =  left_branch[i].where;
		apply_rule =  left_branch[i].which;

		/* Keep left action */
		left_action = NEW(char, apply_pos + 1);
		strncpy(left_action, left_branch[i].word, apply_pos);
		*(left_action + apply_pos) = '\0';

		/* keep left action sorted to aid in algebraic groupings */
		quick_sort(left_action, 0, apply_pos - 1);

		add_to_boundary_la(boundary_head, -1, left_action, apply_rule);
			/* -1 for left boundary */

		free(left_branch[i].word);
	}

	/* Clean up those nodes that we stored but didn't need. */
	for (; i<= num_in_left; i++)
		free(left_branch[i].word);

	free (node);		/* free storage of the final reduced form */

	/* remove all zero counts */
	remove_zero_counts_la(boundary_head);

	return boundary_head;
}


static int store_left_branch (char *node, R_INFO r_info, PATH_INFO *left_branch)
/* Stores the left branch (taking right paths) from 'node' for comparison of
   the right branch.  This is done so as to determine when to stop traversing
   the right branch because we don't need any further information.  r_info
   should already be as applied to that node through function decisions (as
   in ....decisions(node, &r_info);... ).  Returns the number of nodes stored
   in the left branch.  */
{
	int		i;
	PATH_INFO	current_info;

	/* store initial node */
	current_info.word = node;
	current_info.where = r_info.apply[LEFT].where;
	current_info.which = r_info.apply[LEFT].which;
	left_branch[0] = current_info;
	node = apply(node, r_info.apply[LEFT].where, r_info.apply[LEFT].which);
	decisions(node, &r_info);	

	/* do the rest until we have the node in normal (reduced) form */
	/* NOTE: we use "r_info.n - 1" so that we always choose the rightmost
	   branch....that is 0 (if only 1 branch), and 1 (if two).   */
	for (i = 1; i < _HOM_MAX_PATH_LEN && r_info.n != 0; i++) {
		current_info.word = node;
		current_info.where = r_info.apply[r_info.n - 1].where;
		current_info.which = r_info.apply[r_info.n - 1].which;
		left_branch[i] = current_info;
		node = apply(node, r_info.apply[r_info.n - 1].where, r_info.apply[r_info.n - 1].which);
		decisions(node, &r_info);	
	}
	current_info.word = node;
	left_branch[i] = current_info;			/* Record the normal (reduced) form */

	return i;		/* count of number of words stored in left branch */
}


BOUNDARY_TRIV *calc_2_cube_boundary_triv(char *word)
/* Given a word, creates, calculates and returns the boundary of it's
   corresponding 2-cube.  Returns NULL if no boundary.
   Uses should remember to use clean_up_boundary_triv when they are finished
   with the returned structure.
*/
{
	BOUNDARY_TRIV	*boundary_head;
	R_INFO			r_info;
	PATH_INFO		left_branch[_HOM_MAX_PATH_LEN];
	char			*node, *temp;
	int				i, num_in_left, apply_pos, apply_rule, found;

	/* Initialise boundary head.  Must be done before calling add_to_boundary */
	INIT_BOUNDARY_TRIV(boundary_head);

	/* Copy the word into node so we can manipulate it, but don't lose it */
	node = NEW(char, strlen(word) + 1);
	strcpy(node,word);

	/* skip until we get to a node that splits */
	while (decisions(node, &r_info) == 1) {
		temp = apply(node, r_info.apply[LEFT].where, r_info.apply[LEFT].which);
		free(node);
		node = temp;
	}

	if (r_info.n == 0) 		/* No boundary!!!! */
		return NULL;

	/* Store the left branch (taking right paths) for comparison of the
	   right branch.  This is done so as to determine when to stop traversing
	   the right branch because we don't need any further information. */
	num_in_left = store_left_branch (node, r_info, left_branch);

	/* Make a new copy of the first node to use */
	node = NEW(char, strlen(left_branch[0].word) + 1);
	strcpy(node, left_branch[0].word);

	/* Do the right branch.  Only store each node as it is encountered, and
	   free nodes that are no longer of use.  Stop when we reach a node that is
	   in the left branch.  */
	decisions(node, &r_info);	
	
	apply_pos =  r_info.apply[RIGHT].where;
	apply_rule =  r_info.apply[RIGHT].which;

	add_to_boundary_triv(boundary_head, 1, apply_rule);
			/* +1 for right boundary */

	temp = apply(node, apply_pos, apply_rule);
	free (node);
	node = temp;

	/* Check to see if we have intersected the left branch.  This must be
	   done for rare cases where the intersection is immediate. */
	found = _FALSE;
	for (i = 0; !found && i <= num_in_left; i++)
		if (strcmp(left_branch[i].word, node) == 0)	/* in left branch! */
			found = _TRUE;

	while( !found) {	/* not intersecting left branch yet... */
		decisions(node, &r_info);	
		apply_pos =  r_info.apply[LEFT].where;
		apply_rule =  r_info.apply[LEFT].which;

		add_to_boundary_triv(boundary_head, 1, apply_rule);
			/* +1 for right boundary */

		/* clean up the node */
		temp = apply(node, r_info.apply[LEFT].where, r_info.apply[LEFT].which);
		free (node);
		node = temp;

		/* Check to see if we have intersected the left branch */
		for (i = 0; !found && i <= num_in_left; i++)
			if (strcmp(left_branch[i].word, node) == 0)	/* in left branch! */
				found = _TRUE;
	}

	/* Do the left branch now!  Clean up nodes as we go...*/
	for (i=0; strcmp(left_branch[i].word, node); i++) {
		apply_pos =  left_branch[i].where;
		apply_rule =  left_branch[i].which;

		add_to_boundary_triv(boundary_head, -1, apply_rule);
			/* -1 for left boundary */

		free(left_branch[i].word);
	}

	/* Clean up those nodes that we stored but didn't need. */
	for (; i<= num_in_left; i++)
		free(left_branch[i].word);

	free (node);		/* free storage of the final reduced form */

	/* remove all zero counts */
	remove_zero_counts_triv(boundary_head);

	return boundary_head;
}


BOUNDARY_LA *calc_boundary_la (char *word)
/* Given a word, calculates the total boundary of the words graph using
   the left action but regarding the right action as trivial.
   Returns NULL if no boundary.
   Currently, the boundary is just displayed in terms of:
		LeftAction[Rule Used], where 'Rule Used' is the number of the
					rule used as listed in the original input file
   		eg.  ..... + bc[12] + .....
*/
{
	R_INFO		r_info;
	char		*node, *temp, *left_action, *first;
	int			apply_pos, apply_rule;
	BOUNDARY_LA	*boundary_head;

	/* Initialise boundary head.  Must be done before calling add_to_boundary */
	INIT_BOUNDARY_LA(boundary_head);

	/* Copy the word into node so we can manipulate it, but don't lose it */
	node = NEW(char, strlen(word) + 1);
	strcpy(node, word);

	/* skip until we get to a node that splits */
	while (decisions(node, &r_info) == 1) {
		temp = apply(node, r_info.apply[LEFT].where, r_info.apply[LEFT].which);
		free(node);
		node = temp;
	}

	if (r_info.n == 0)		/* No boundary!!!! */
		return NULL;

	/* ...where to begin when calculating the left boundary */
	first = NEW(char, strlen(node) + 1);
	strcpy(first, node);

	/* Calculate right boundary first.... */
	while (decisions(node, &r_info)) {	/* do until final reduced form */
		apply_pos =  r_info.apply[r_info.n - 1].where;
		apply_rule =  r_info.apply[r_info.n - 1].which;

		/* Keep left action */
		left_action = NEW(char, apply_pos + 1);
		strncpy(left_action, node, apply_pos);
		*(left_action + apply_pos) = '\0';

		/* keep left action sorted to aid in algebraic groupings */
		quick_sort(left_action, 0, apply_pos - 1);

		add_to_boundary_la(boundary_head, 1, left_action, apply_rule);
				/* +1 for right boundary */

		/* clean up the area */
		temp = apply(node, apply_pos, apply_rule);
		free (node);
		node = temp;
	}
	free (node);		/* free storage of the final reduced form */

	/* Starting at first again.... */
	node = first;

	/* Calculate left boundary now.... */
	while (decisions(node, &r_info)) {	/* do until final reduced form */
		apply_pos =  r_info.apply[LEFT].where;
		apply_rule =  r_info.apply[LEFT].which;

		/* keep left action */
		left_action = NEW(char, apply_pos + 1);
		strncpy(left_action, node, apply_pos);
		*(left_action + apply_pos) = '\0';

		/* keep left action sorted to aid in algebraic groupings */
		quick_sort(left_action, 0, apply_pos - 1);

		add_to_boundary_la(boundary_head, -1, left_action, apply_rule);
				/* -1 for left boundary */

		/* clean up the area */
		temp = apply(node, apply_pos, apply_rule);
		free (node);
		node = temp;
	}
	free (node);		/* free storage of the final reduced form */
	free (first);		/* free storage of the first (root) node */

	/* remove all zero counts */
	remove_zero_counts_la(boundary_head);

	return boundary_head;
}


static void	remove_zero_counts_la(BOUNDARY_LA *head)
/* Get rid of those parts of the boundary that have 0 counts
   due to cancellations (eg (ab - ab)[7]....).  This is usually performed
   AFTER calculating the boundary, as it is more efficient than,
   for example, removing a 0 boundary part, and then having to add that
   part back in at a later date.  */
{
	BOUNDARY_LA		*prev_head;
	PART_INFO		*p, *prev;

	for (prev_head = head, head = head->next; head != NULL; prev_head = head, head = head->next) {
		for (prev = p = head->part; p != NULL; prev = p, p = p->next)
			if (p->count == 0) {	/* clean up that part */
				free(p->left_action);
				/* skip over that node and free up mem */
				if (p != head->part)
					prev->next = p->next;
				else
					head->part = p->next;
				free(p);
			} 
		if (head->part == NULL) {	/* we have a rule with 0 left action */ 
			prev_head->next = head->next;	/* clean up */
			free (head);
			head = prev_head;
		}
	}
}


static void quick_sort(char a[], int l, int r)
/* sort the array of chars from a[l] to a[r] using the quick sort algorithm.
   Uses a recursive algorithm which should be optimised by the compiler. */
{
	char	v, i, j, t;
	if (r > l) {
		v = a[r];
		i = l - 1;
		j = r;

		for (;;) {
			while (a[++i] < v)
				;
			while (a[--j] > v)
				;
			if (i >= j)
				break;
			t = a[i]; a[i] = a[j]; a[j] = t;
		}
		t = a[i]; a[i] = a[r]; a[r] = t;
		quick_sort(a, l, i-1);
		quick_sort(a, i+1, r);
	}
}


static void add_to_boundary_la (BOUNDARY_LA *head, int count, char *action, int rule)
/* Adds a left action and rule to a boundary, with header 'head'.  Count
   should normally be +1 or -1 (depending on right or left boundary path).
   NOTE: INIT_BOUNDARY_LA(head) must be used before calling this routine.
   WARNING:'action' may be deallocated if it is already part of the boundary. */
{
	BOUNDARY_LA	*temp;
	PART_INFO	*p;

	/* find position to add boundary part, keeping rules in sorted order */
	for (; head->next != NULL && rule >= head->next->rule; head = head->next)
		;
	if (head->rule == rule) {	/* have already encountered this rule before */
		/* Find position to add our left action */
		for (p = head->part; p->next != NULL && strcmp(action, p->left_action); p = p->next)
			;
		if (p->next == NULL && strcmp(action, p->left_action)) {
			/* A new left action for this rule */
			p->next = NEW(PART_INFO,1);
			p->next->count = count;
			p->next->left_action = action;
			p->next->next = NULL;
		} else { /* an old one, so just add on count and free up action */
			p->count += count;
			free (action);
		}
	} else {	/* This is a new rule to add.... */
		temp = head->next;
		head->next = NEW(BOUNDARY_LA, 1);
		head->next->rule = rule;
		head->next->part = NEW(PART_INFO, 1);
		head->next->next = temp;

		head->next->part->count = count;
		head->next->part->left_action = action;
		head->next->part->next = NULL;
	}

}


void display_boundary_la(BOUNDARY_LA *head)
/* Display a given boundary with left actions from its boundary list.
   The boundary is displayed in the form:
		 + (all left actions for rule)[rule] +....
   For example:
		+ (cd - 1 )[9] + (1 - b )[10] + ...   */
{
	PART_INFO	*p;

	if (head == NULL) {	/* Nothing to display... */
		printf("\n");
		return;
	}

	for(head = head->next; head != NULL; head = head->next) {
		printf("+ (");
		for (p = head->part; p != NULL; p = p->next) {
			if (p != head->part || SIGN(p->count) == '-')
				/* Don't put + for the first entry if positive */
				printf("%c ", SIGN(p->count));
			if (ABS(p->count) != 1 || strlen(p->left_action) == 0)
				/* don't display a 1 unless their is no action */
				printf("%d", ABS(p->count));
			printword(stdout, p->left_action);
			printf(" ");
		}
		printf(")[%d] ", head->rule);	/* display rule */
	}
	printf("\n");
}


void clean_up_boundary_la (BOUNDARY_LA *head)
/* Free all allocated memory for a given boundary list with left actions.
   This should only be used when a boundary is completely finished with. */
{
	PART_INFO	*p, *ptemp;
	BOUNDARY_LA	*temp;

	while(head != NULL) {		/* start at head... */
		for (p = head->part; p != NULL;) {	/* do each part... */
			ptemp = p->next;
			free (p->left_action);
			free (p);
			p = ptemp;
		}
		temp = head->next;
		free (head);
		head = temp;
	}	
}


BOUNDARY_TRIV *calc_boundary_triv (char *word)
/* Given a word, calculates the total boundary of the words graph regarding
   left and right actions as trivial.
   Currently, the boundary is just displayed in terms of:
		count[Rule Used], where 'Rule Used' is the number of the
					rule used as listed in the original input file
   		eg.  ..... + [12] + 2[10] - [5] + .....
*/
{
	R_INFO		r_info;
	char		*node, *temp, *first;
	int			apply_pos, apply_rule;
	BOUNDARY_TRIV	*boundary_head;

	/* Initialise boundary head.  Must be done before calling add_to_boundary */
	INIT_BOUNDARY_TRIV(boundary_head);

	/* Copy the word into node so we can manipulate it, but don't lose it */
	node = NEW(char, strlen(word) + 1);
	strcpy(node, word);

	/* skip until we get to a node that splits */
	while (decisions(node, &r_info) == 1) {
		temp = apply(node, r_info.apply[LEFT].where, r_info.apply[LEFT].which);
		free(node);
		node = temp;
	}

	if (r_info.n == 0) 	/* No boundary!!!! */
		return NULL;

	/* ...where to begin when calculating the left boundary */
	first = NEW(char, strlen(node) + 1);
	strcpy(first, node);


	/* Calculate right boundary first.... */
	while (decisions(node, &r_info)) {	/* do until final reduced form */
		apply_pos =  r_info.apply[r_info.n - 1].where;
		apply_rule =  r_info.apply[r_info.n - 1].which;

		add_to_boundary_triv(boundary_head, 1, apply_rule);
				/* +1 for right boundary */

		/* clean up the area */
		temp = apply(node, apply_pos, apply_rule);
		free (node);
		node = temp;
	}
	free (node);		/* free storage of the final reduced form */

	/* Starting at first again.... */
	node = first;

	/* Calculate left boundary now.... */
	while (decisions(node, &r_info)) {	/* do until final reduced form */
		apply_pos =  r_info.apply[LEFT].where;
		apply_rule =  r_info.apply[LEFT].which;

		add_to_boundary_triv(boundary_head, -1, apply_rule);
				/* -1 for left boundary */

		/* clean up the area */
		temp = apply(node, apply_pos, apply_rule);
		free (node);
		node = temp;
	}
	free (node);		/* free storage of the final reduced form */
	free (first);		/* free storage of the first (root) node */

	/* remove all zero counts */
	remove_zero_counts_triv(boundary_head);

	return boundary_head;
}


static void	remove_zero_counts_triv(BOUNDARY_TRIV *head)
/* Get rid of those parts of the boundary that have 0 counts
   due to cancellations (eg 0[7]....).  This is usually performed AFTER
   calculating the boundary, as it is more efficient than removing than,
   for example, removing a 0 boundary part, and then having to add that
   part back in at a later date.  */
{
	BOUNDARY_TRIV	*prev_head;

	for (prev_head = head, head = head->next; head != NULL; prev_head = head, head = head->next)
		if (head->count == 0) {	/* remove that rule (clean up) */
			prev_head->next = head->next;
			free (head);
			head = prev_head;
		}
}


static void add_to_boundary_triv (BOUNDARY_TRIV *head, int count, int rule)
/* Adds a rule (regarding left and right actions as trivial) to a boundary,
   with header 'head'.  Count should normally be +1 or -1 (depending on right
   or left boundary path).
   NOTE: INIT_BOUNDARY_TRIV(head) must be used before calling this routine. */
{
	BOUNDARY_TRIV	*temp;

	/* find position to add rule, keeping rules in sorted order */
	for (; head->next != NULL && rule >= head->next->rule; head = head->next)
		;
	if (head->rule == rule) 	/* have already encountered this rule before */
			head->count += count;
	else {	/* This is a new rule to add.... */
		temp = head->next;
		head->next = NEW(BOUNDARY_TRIV, 1);
		head->next->rule = rule;
		head->next->count= count;
		head->next->next = temp;
	}

}


void display_boundary_triv(BOUNDARY_TRIV *head)
/* Display a given boundary, with left and right actions regarded as trivial,
   from its boundary list.
   The boundary is displayed in the form:
		 + count[rule] +....
   For example:
		+ [9] + [10] - 2[12] + ...   */
{
	for(head = head->next; head != NULL; head = head->next) {
		printf("%c ", SIGN(head->count));
		if (ABS(head->count) != 1) /* don't display a 1 */
			printf("%d", ABS(head->count));
		printf("[%d] ", head->rule);	/* display rule */
	}
	printf("\n");
}


void clean_up_boundary_triv (BOUNDARY_TRIV *head)
/* Free all allocated memory for a given boundary list with left actions.
   This should only be used when a boundary is completely finished with. */
{
	BOUNDARY_TRIV	*temp;

	while(head != NULL) {		/* start at head... */
		temp = head->next;
		free (head);
		head = temp;
	}	
}
