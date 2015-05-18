/* ndimbdry.c  - library code for decomposing cubes
   ===========================================================================
   Written by:	Jamie P. Curmi
   Date:		July 1993 - 1994
   Project:		Computational Tools for Homology of Groups
   For:			Dr J. R. J. Groves and Prof. C. Miller
				Department of Mathematics, University of Melbourne,
				Parkville, Victoria, Australia.
   ===========================================================================
   The library exports those functions and macros defined in decomp.h
   The library imports 3 global variables used by most parts of the project:
		extern int	Num_rules;
		extern char	**Word1, **Word2;
   Their use is as follows:
		Num_rules is the number of rules in the FSA.
		Word1 is the LHS of our rewriting rules.
		Word2 is the RHS of our rewriting rules.

   These functions are for calculating the boundary of an n-dimensional
   cube, given the groups rewriting system and the word for which the
   boundary is required.  Those functions exported to users can be found
   in decomp.h


 *****************************************************************************
 **  IGNORE all lines beginning with PR, DC.                                **
 **  This is for DEBUGGING only!                                            **
 **                                                                         **
 **  A quick guide to reading the code:                                     **
 **                                                                         **
 **  GLOBALS always have a single upper case letter, followed by lower case.**
 **  LOCALS always begin with a lower case letter.                          **
 **  MACROS & TYPEDEFS always consist of all upper case letters.            **
 *****************************************************************************
*/


#include <stdio.h>
#include <string.h>
#include "grpinput.h"
#include "homlib.h"
#include "rewrite.h"
#include "ndimbdry.h"


/************************************************************/
/*******************    Debugging stuff   *******************/
/************************************************************/

/* NOTE: _STATS is defined in the makefile for creating a version that
  includes Statistical Information. */

/*#define	_DEBUG*/

#ifdef  _DEBUG
#define PR(A)	{printf("%s\n", A); fflush(NULL);}
#define DC(C)	{display_cube(stdout, C); fflush(NULL);}
#else
#define	PR(A)	/* */
#define	DC(C)	/* */
#endif

#ifdef _STATS /* Statistics are being used */

/* Globals used for testing number of calls to routines */
int Subcube_nd_count, Topcube_nd_count, Topcube_d_count, Subcube_d_count;
int Max_level;

/* Functions used.... */
extern void printword(FILE *fp, char *word);

static void init_stats(void);

#endif


/************************************************************/
/********************    Other Macros    ********************/
/************************************************************/

#define POW2(X)     (1 << (X))			/* calculates 2^X */
#define	SIGN(A)		(((A) < 0) ? '-' : '+')

    /* These are macros for speed! */
#define	IS_CUBE(C)				((C)->vertices->len != 0)
#define IS_NOT_TRIVIAL_CUBE(C)	((!is_generically_doubled(C)) && (!is_product(C)))

    /* ....for working on sets */
#define	UNION(A,B)		((A) | (B))
#define	INTER(A,B)		((A) & (B))
#define	DIFF(A,B)		~((A) ^ (~(B)))
#define INCSET(A)		((A) << 1)
#define INCSETN(A, N)	((A) << (N))
#define ELEMENT_OF(A,B)	((A) & (B))

/* Rotates left K bits the N bits in A */
#define ROTATE(A,N,K)	((((A) << (K)) & (POW2(N) - 1)) + ((A) >> ((N)-(K))))


/************************************************************/
/**********************     Typedefs   **********************/
/************************************************************/

typedef struct sorted_vertices {
	int				pos;
	unsigned int	index;
} SORTED_VERTICES;		/* Used to store the correct positions of vertices
						   in sorted order of increasing position */


/************************************************************/
/**********    Functions not exported to users     **********/
/************************************************************/

static int find_all_rewriting_areas(char *word, CUBE_INFO *info);
static void find_default_rewriting_areas(CUBE *cube);
static int find_default_rewriting_area(char *word, CUBE_INFO *vert, int *pos, int *len);
static int find_first_rewriting_area(char *word, int *pos, int *len);
static int count_rewriting_areas(char *word);
static int compare_vertices(CUBE *cube, unsigned int a, unsigned int b);
static void calc_cover(CUBE *cube, BOUNDARY *boundary_head);
static void add_canonical_cube(CUBE *cube, BOUNDARY *boundary_head);
static int compare_vertex_to_boundary(CUBE_INFO *vert, BOUNDARY *b);
static void decompose_nondegen_cube(CUBE *cube, BOUNDARY *boundary_head);
static CUBE_INFO *copy_cube_rewrite_info(CUBE_INFO *pp);
static void copy_cube_vertex(CUBE_INFO *src, CUBE_INFO *dst);
static CUBE *get_nondegen_topcube(CUBE *parent);
static CUBE *get_nondegen_subcube(CUBE *parent, int rule_number);
static int is_trivial_canonical(CUBE *cube);
static int is_degen_cube(CUBE *cube, int *vertex);
static int is_canonical_cube(CUBE *cube);
static int is_generically_degen(CUBE *cube);
static int is_generically_doubled(CUBE *cube);
static int is_product(CUBE *cube);
static int check_one_split(CUBE *cube, int split);
static int find_split(CUBE *cube, int skip);
static int check_product(CUBE *cube, unsigned int a, unsigned int b);
static CUBE *extract_upper_face_cube(CUBE *cube, int i, int sign);
static CUBE *extract_lower_face_cube(CUBE *cube, int i, int sign);
static CUBE *get_degen_topcube(CUBE *parent, int i);
static void delete_boundary_tree (BOUNDARY *boundary);
static void delete_vertices(CUBE_INFO *vert);
static void delete_coeff(PART_INFO *coeff);
static void display_boundary_tree(FILE *fp, BOUNDARY *boundary);
static void decompose_degen_cube(CUBE *cube, BOUNDARY *boundary_head);
static int sort_vertex_positions(CUBE *cube, SORTED_VERTICES **sv, int *sign);
static void tidy_rewriting_areas(CUBE *cube);
static CUBE *get_degen_subcube(CUBE *parent, int i, int j);
static void reorder_vertices(CUBE *cube);
static void remove_zero_boundaries(BOUNDARY *boundary_head);
static void remove_zero_boundaries_rec(BOUNDARY *parent, BOUNDARY *child);
static char *rewrite_word(char *word, int pos, int len);
static char *rewrite_rule(char *word, int pos, int len, int rule, int *diff);
static char *rewrite_given_areas(char *w, CUBE_INFO *vert);
static void fix_rewrite_areas(char *orig_word, CUBE_INFO *old_rewrite, CUBE_INFO *top_rewrite);
static int check_inv(CUBE *cube, int i);


/** USED FOR TESTING **/
static void display_a_vertex(FILE *fp, CUBE_INFO *pp);


/************************************************************/
/************    Global Variable Declararions    ************/
/************************************************************/

    /* External Global variables used by the program */
extern int Num_rules;
extern char **Word1, **Word2;

    /* Local Globals */
static int Boundary_action;		/* Global storing boundary action required */
static int Inverse_tuple_action;	/* Global storing action on inv tuples */


/************************************************************/
/****************    Function Definitions    ****************/
/************************************************************/


static
CUBE *get_nondegen_topcube(CUBE *parent)
/* Returns the TOP cube from a cube being decomposed.  The top cube is
   canonical.  The 'parent' is not freed...this is left to the user. */
{
	CUBE	*child;

PR("In GET_NONDEGEN_TOPCUBE");

	child = NEW(CUBE,1);
	child->dim = parent->dim;
	child->sign = parent->sign;
	child->canonical = _TRUE;

#ifdef _STATS
	Topcube_nd_count++;
	child->level = parent->level + 1;
#endif

	child->word = NEW(char, strlen(parent->word) + 1);
	strcpy(child->word, parent->word);
	child->vertices = copy_cube_rewrite_info(parent->vertices);
PR("OUT OF GET_NONDEGEN_TOPCUBE");
	return child;
}


static
CUBE_INFO *copy_cube_rewrite_info(CUBE_INFO *pp)
/*  Copies the information from the first linked list in the vertices
    'pp', creating a new INFO structure with this information.  This 
	is basically used for obtaining a canonical cube. */
{
	CUBE_INFO *child;

	child = NEW(CUBE_INFO, 1);
	copy_cube_vertex(pp, child);

	return child;
}


static
void copy_cube_vertex(CUBE_INFO *src, CUBE_INFO *dst)
/* Copies a vertex in src (source) into dst (destination).  Note that
   dst is expected to exist already (it may be part of an array of
   CUBE_INFO structures, as used in CUBE). */
{
	dst->pos = src->pos;
	dst->len = src->len;
	for (src = src->next; src != NULL; src = src->next) {
		dst = dst->next = NEW(CUBE_INFO, 1);
		dst->pos = src->pos;
		dst->len = src->len;
	}
	dst->next = NULL;
}


static
CUBE *get_nondegen_subcube(CUBE *parent, int rule_number)
/* Returns the SUB cube resulting from rewriting the non-degen cube's
   (parent's) head word at the position indicated by rule_number (
   rule_number 0 corresponds to the first edge, and so on... The
   information used is stored in the first linked list of the 'vertices'
   array).
   THE VERTICES MUST BE IN ORDER TO USE THIS ALGORITHM.  If not, use
   get_nondegen_subcube_general.
   The parent is not freed...this is left to the user. */
{
	unsigned int	rule_index,		/* Index to rule number */
					s,				/* each index in loop */
					set_union;		/* Union of sets */
	int				num_vertices, i, j, k, n, rule, current_pos, current_len;
	int				diff, apply_pos, before_len, after_len, new_pos, new_len;
	CUBE			*child;
	CUBE_INFO		*rule_pntr, *parent_vert, *child_vert, *pp, *pc;

PR("IN GET_NONDEGEN_SUBCUBE");
DC(parent);

	rule_index = POW2(rule_number);
	num_vertices = POW2(parent->dim);

	/* Find the exact postion telling us pos and len of rewrite. */
	rule_pntr = parent_vert = parent->vertices;
	for (i= rule_number; i > 0 && rule_pntr != NULL; i--)
		rule_pntr = rule_pntr->next;

	apply_pos = rule_pntr->pos;
	before_len = rule_pntr->len;

	/* rule = actual rule number in our rewriting system */
	rule = find_rule(parent->word, apply_pos, before_len);
	after_len = strlen(Word2[rule]);
	diff = after_len - before_len;

	/* Set up the child info */
	child = NEW(CUBE, 1);
	n = child->dim = parent->dim;
	child_vert = child->vertices = NEW(CUBE_INFO, num_vertices);

	child->word = apply(parent->word, apply_pos, rule);
	child->canonical = _FALSE;			/* at least until we check */
	child->sign = parent->sign;

#ifdef _STATS
	Subcube_nd_count++;
	child->level = parent->level + 1;
#endif

	/* Now calculate all vertices.  Vertices must be written in terms
	   of postion and len of rewrite in the head word to arrive at that
	   vertex. */
	for (s=1; s < num_vertices; s++) {
		pc = child_vert + s;
		pc->pos = pc->len = 0;	/* initially */
		if (ELEMENT_OF(rule_index, s)) { /* rule_index in s */
			current_pos = current_len = 0;
			for (pp = parent_vert + s; pp!= NULL; pp = pp->next)
				if (pp->pos != apply_pos ||
						pp->len != before_len) {
					/* Then it is not the rewriting rule... */
					if (pc->len != 0)
						pc = pc->next = NEW(CUBE_INFO, 1);
					pc->pos = pp->pos;
					pc->len = pp->len;
				}
		} else {
			set_union = UNION(rule_index, s);
			current_pos = current_len = 0;
			/* current_pos and current_len store pos and len, increasing len
			   with overlap until we find one without overlap.  At this
			   point we then create the CUBE_INFO (if necessary) and put
			   it in.  This avoids having to delete a created structure if
			   we later find that it is only the rewriting rule. */
			for (j = 0; j < n; j++) {
				/* Check if each vertex j is part of the vertex 'set_union' */
				if (ELEMENT_OF(POW2(j), set_union)) {
					/* the index has POW2(j) as an element */
					for (k = j, pp = parent_vert; pp != NULL && k > 0 ; k--)
						pp=pp->next;
					/* Make adjustments for areas that don't fully overlap
					   with the rewrite area, only partially. */
					new_pos = pp->pos;
					new_len = pp->len;

					/* Check for overlaps, or ignore if just a rewriting rule */
					if (current_len == 0) { /* First time */
						current_pos = new_pos;
						current_len = new_len;
					} else if (new_pos < current_pos + current_len) {/*overlap*/
						current_len = new_pos - current_pos + new_len;
					} else {
						if (current_pos != apply_pos ||
											current_len != before_len) {
							/* Not just the rewriting rule */
							if (pc->len != 0)
								pc = pc->next = NEW(CUBE_INFO, 1);
							pc->pos = current_pos;
							pc->len = current_len;
						}
						current_len = new_len;
						current_pos = new_pos;
					}
				}
			}
			/* Check the last one out */
			if (current_pos != apply_pos || current_len != before_len) {
				/* Not just the rewriting rule */
				if (pc->len != 0)
					pc = pc->next = NEW(CUBE_INFO, 1);
				pc->pos = current_pos;
				pc->len = current_len;
			}
		}
		
		pc->next = NULL;

		if (diff != 0) {
			/* Now go through and add "diff" in case of length changes,
			   if the length after rewriting increases */
			for (pc = child_vert + s; pc != NULL; pc = pc->next) {
				if (pc->pos <= apply_pos && pc->pos + pc->len > apply_pos)
					pc->len += diff;
				if (pc->pos > apply_pos)
						pc->pos += diff;
			}
		}
	}
	tidy_rewriting_areas(child);
	/* Put in the child's new rewriting areas */
	find_default_rewriting_areas(child);

PR("OUT GET_NONDEGEN_SUBCUBE");
	return child;
}


static
int sort_vertex_positions(CUBE *cube, SORTED_VERTICES **sv, int *sign)
/* Given vertex 'vert' and the dimension 'dim' of a cube, sorts the
   rewriting areas of the vertex in to increasing position, with degenerate
   vertices coming first.  A special structure (SORTED_VERTICES) is returned
   in sv.  It should be free'd when no longer needed.
   The fuction also returns in 'sign' the sign of the cube if we were to
   physically sort the vertices, and returns _TRUE if sorting took place,
   or _FALSE if the vertices were already sorted.
   SELECTION SORT IS USED.  WE MAY WANT TO REPLACE THIS WITH Q-SORT. */
{
	SORTED_VERTICES	t;
	CUBE_INFO		*vert;
	int				i, j, min, dim = cube->dim;
	int				sorted = _FALSE;

PR("IN SORT VERTEX POSITIONS");
DC(cube);

	*sv = NEW(SORTED_VERTICES, dim);
	*sign = cube->sign;

	for (vert = cube->vertices, i=0; i < dim; i++) {
		if ((cube->vertices + POW2(i))->len != 0) {
			(*sv)[i].pos = vert->pos;
			vert = vert->next;
		} else		/* For degenerate's give them priority -1 (ie first) */
			(*sv)[i].pos = -1;
		(*sv)[i].index = i;
	}

	for (i=0; i < dim - 1; i++) {
		min = i;
		for (j = i+1; j < dim; j++)
			if ((*sv)[j].pos < (*sv)[min].pos)
				min = j;
		if (i != min) {
			sorted = _TRUE;
			*sign *= -1;	/* change sign for each posititon swap */
			t = (*sv)[min]; (*sv)[min] = (*sv)[i]; (*sv)[i] = t;
		}
	}

PR("OUT SORT VERTEX POSITIONS\n");
DC(cube);
	return sorted;
}


static
int find_all_rewriting_areas(char *word, CUBE_INFO *info)
/* Searches through the word for left-hand-sides (rewriting areas),
   returning the number of rewriting rules found, and (in the info
   structure) positions and lengths of these rewriting rules.
   ALGORITHM: Matches each left hand side of the rewriting rules to
   each position in our word until a match is achieved.  Records
   these matches.  */
{
    int i, j, k, found, num = 0, len = strlen(word);
	CUBE_INFO *p = info;

	p->pos = p->len = 0;	/* Defaults should no rewriting rules be found */

    for (i=0; i < len-1; i++)
        for (k=1; k <= Num_rules; k++) {
            for (found = _FALSE, j = 0; i + j <= len; j++) {
                if (Word1[k][j] == '\0') { /* A complete match */
                    found = _TRUE; /* No longer need to look for matches here */
					if (num > 0)  /* first is already used so create another */
						p = p->next = NEW(CUBE_INFO, 1);
					p->pos = i;
					p->len = j;
					num++;
                    break;
                }
                if (word[i+j] != Word1[k][j]) /* No match */
                    break; /* out of j loop */
            }
        	if (found)
				break; /* out of k loop */
        }

	p->next = NULL;
	return num;
}


static
int find_first_rewriting_area(char *w, int *pos, int *len)
/* Finds the first rewriting rule in a given word 'w', at position 'pos'
   of length 'len'. 
   Returns the pos and len.  Function returns rule number, or 0. */
{
    int i, j, k, length = *len;
	char *word = w + *pos;

PR("IN FIND_FIRST_REWRIT...");

    for (i=0; i < length; i++)
        for (k=1; k <= Num_rules; k++)
            for (j = 0; i + j <= length; j++) {
                if (Word1[k][j] == '\0') { /* A complete match */
					*pos += i;
					*len = j;
					return k;
                }
                if (word[i+j] != Word1[k][j]) /* No match */
                    break; /* out of j loop */
            }
PR("OUT FIND_FIRST_REWRIT...");
	return 0;
}


static
int find_default_rewriting_area(char *word, CUBE_INFO *vert, int *pos, int *len)
{
	int rule;

	for (; vert != NULL; vert = vert->next) {
		*pos = vert->pos;
		*len = vert->len;
		if (rule = find_first_rewriting_area(word, pos, len))
			return rule;
	}
	return 0;
}


static
void find_default_rewriting_areas(CUBE *cube)
{
	int i, pos, len;
	CUBE_INFO *p;

	cube->vertices->pos = 0;
	cube->vertices->len = 0;

	for (i = 0, p = cube->vertices; i < cube->dim; i++)
		if (find_default_rewriting_area(cube->word,
							cube->vertices + POW2(i), &pos, &len)) {
			if (p->len != 0)  /* first is used so create another */
				p = p->next = NEW(CUBE_INFO, 1);
			p->pos = pos;
			p->len = len;
		}
	p->next = NULL;
}


static
int count_rewriting_areas(char *word)
/* Searches through the word for left-hand-sides (rewriting areas),
   returning the number of rewriting rules found.  Actually the same as
   find_all_rewriting_areas, but used when we first need to know how many
   there are to create the correct size structure.  This seems a waste
   of time, but fortunately should only be used once.  */
{
    int i, j, k, found, num = 0, len = strlen(word);

    for (i=0; i < len-1; i++)
        for (k=1; k <= Num_rules; k++) {
            for (found = _FALSE, j = 0; i + j <= len; j++) {
                if (Word1[k][j] == '\0') { /* A complete match */
                    found = _TRUE; /* No longer need to look for matches here */
					num++;
                    break;
                }
                if (word[i+j] != Word1[k][j]) /* No match */
                    break; /* out of j loop */
            }
        	if (found)
				break; /* out of k loop */
        }

	return num;
}


int is_inv_tuple(char *word, int pos, int len)
/* Given a word, and pos & len of a subword in that word, checks if that
   subword is of the form a*a^-1*a*a^-1....  Return _TRUE if it is this form,
   _FALSE otherwise. */
{
	int	newpos, rule;

	if (len < 2 || word[pos] == word[pos + 1])
		return _FALSE;
	for (newpos = pos; newpos < pos + len - 2; newpos++)
		if (word[newpos] != word[newpos + 2]	/* every 2nd must be = */
				|| (rule = find_rule(word, newpos, 2)) == 0 /* must be a rule */
				|| strlen(Word2[rule]))			/* Must reduce to "" */
			return _FALSE;
	/* Check the last two generators in the subword */
	if ((rule = find_rule(word, newpos, 2)) == 0 || strlen(Word2[rule]))
		return _FALSE;
	return _TRUE;
}


static
int check_inv(CUBE *cube, int i)
/* Takes a cube, and the i'th vertex (used in rewriting to obtain the
   i'th lower face), checks if the rewriting rule used to rewrite along
   this edge is of the form a*a^-1 or a^-1*a.  If so, returns _TRUE, otherwise
   _FALSE.

   THIS IS NO LONGER USED AS ITS USE WAS FOUND TO BE INCORRECT!
*/
{
	int j, rule;
	CUBE_INFO *rule_pntr;

	rule_pntr = cube->vertices;
	/* Find the exact postion telling us pos and len of rewrite. */
	for (j= i; j > 0 && rule_pntr != NULL; j--)
		rule_pntr = rule_pntr->next;

	if (rule_pntr->len != 2) /* Can't be inverse word such as a*a^-1*/
		return _FALSE;

	/* rule = actual rule number in our rewriting system */
	rule = find_rule(cube->word, rule_pntr->pos, rule_pntr->len);

	/* If Word2[rule] = "", then this must have been an inverse pair! */
	return (strlen(Word2[rule])) ? _FALSE : _TRUE;
}


void delete_cube(CUBE *cube)
/* Frees malloced space used in a CUBE structure. */
{
	CUBE_INFO	*next, *ip, *t = cube->vertices;
	int			i;

	for (i=0; i < ((cube->canonical) ? 1 : POW2(cube->dim)); i++, t++)
		/* free each link (except header link) */
		/* Note a canonical cube has only 1 linked list */
		for (ip = t->next; ip != NULL; ip = next)	{
			next = ip->next;
			free (ip);
		}
	free (cube->vertices);	/* now free the header links */
	free (cube->word);		/* free the word */
	free (cube);			/* free the cube itself */
	return;
}


void delete_boundary(BOUNDARY *boundary)
/* Frees malloced space used in creating a boundary tree. */
{
	if (boundary->left != NULL)
		delete_boundary_tree(boundary->left);
	free(boundary);  /* free the sentinel */
}


static
void delete_boundary_tree (BOUNDARY *boundary)
/* Should be called by delete_boundary, as it only gets rid of the tree
   and not the sentinel! */
{
	if (boundary->left != NULL)
		delete_boundary_tree (boundary->left);
	if (boundary->right != NULL)
		delete_boundary_tree (boundary->right);
	free(boundary->word);
	delete_vertices(boundary->vertices);
	delete_coeff(boundary->coeff);
	free(boundary);
}


static
void delete_vertices(CUBE_INFO *vert)
/* Used to free the vertices in a boundary structure */
{
	CUBE_INFO *temp;

	for (; vert!= NULL; vert = temp) {
		temp = vert->next;
		free(vert);
	}
}


static
void delete_coeff(PART_INFO *coeff)
/* Used to free the coefficients in a boundary structure */
{
	PART_INFO *temp;

	for (; coeff!= NULL; coeff = temp) {
		temp = coeff->next;
		free(coeff->left_action);
		free(coeff);
	}
}


void display_boundary(FILE *fp, BOUNDARY *boundary)
/* Displays a boundary in the form:
   + sign (left_actions)[word] [pos,len] [pos,len].... + ....  */
{
	if (boundary->left == NULL) {
		fprintf(fp, "Zero boundary!\n");
		return;
	}
	display_boundary_tree(fp, boundary->left);
	fprintf(fp, "\n");
}


static
void display_boundary_tree(FILE *fp, BOUNDARY *boundary)
/* Recursively handles the displaying of the boundary tree */
{
	PART_INFO *p;
	CUBE_INFO *pp;

	if (boundary == NULL)
		return;

	display_boundary_tree(fp, boundary->left);

	fprintf(fp, "+ (");
	for (p = boundary->coeff; p!= NULL; p=p->next) {
		if (p->count > 0)
			fprintf(fp, "+");
		fprintf(fp, "%d", p->count);
		if (p->left_action != NULL)
			printword(fp, p->left_action);
		fprintf(fp, " ");
	}
	fprintf(fp, ") [");
	printword(fp, boundary->word);
	fprintf(fp, "] ");

	for (pp = boundary->vertices; pp != NULL; pp = pp->next)
		fprintf(fp, "[%d,%d] ", pp->pos, pp->len);
	fprintf(fp, "  ");

	display_boundary_tree(fp, boundary->right);
}


static
int is_trivial_canonical(CUBE *cube)
/* Checks a canonical cube and returns _TRUE if trivial, _FALSE otherwise.
   For speed, it ASSUMES the cube is canonical, and vertices are sorted.  */
{
	int			count;
	CUBE_INFO	*curr, *prev;

PR("In IS_TRIV");

	prev = cube->vertices;
	for (count=1, curr = prev->next; curr!= NULL; count++, curr = curr->next) {
		/* Check if we have any duplicated vertices, or rules don't overlap.
		   If so it is TRIVIAL! */
		if ((curr->pos == prev->pos && curr->len == prev->len) || curr->pos >= prev->pos + prev->len)
			return _TRUE;
		prev = curr;
	}

PR("OUT IS_TRIV...");
	return ((count != cube->dim) ? _TRUE : _FALSE);
}


static
int is_degen_cube(CUBE *cube, int *vertex)
/* Checks a general cube and returns _TRUE if degenerate, _FALSE otherwise.
   Returns in vertex the vertex which was degenerate, or dim otherwise. */
{
	CUBE_INFO	*info = cube->vertices;
	int			n = cube->dim;

PR("IN IS_DEGEN... (NO OUT)");

	for (*vertex = 0; *vertex < n; (*vertex)++)
		if ((info + POW2(*vertex))->len == 0)
			return _TRUE;
	return _FALSE;
}


static
int is_canonical_cube(CUBE *cube)
/* Checks a general cube and returns _TRUE if canonical, _FALSE otherwise.
   Assumes a general cube.  Does NOT check if the cube is KNOWN canonical.
   A known canonical will most likely crash the function, but is not checked
   for speed reasons. */
{
	unsigned int	s, i, test;
	CUBE_INFO		*pp, *pc, compare;

PR("IN IS_CAN...no out");
DC(cube);
	for (s=1; s < POW2(cube->dim); s++) { /* test each vertice */
		compare.pos = compare.len = 0;
		pp = cube->vertices;
		test = 0;
		for (i = 0, pc = pp + s; pc != NULL; pp = pp->next, i++) {
			/* Take each CUBE_INFO structure, and create an index in
			   test.  If this index (test) does not match the actual
			   index for the vertice (s), then this is not canonical! */
			if (pp == NULL) /* There is no matching rewrite area */
				return _FALSE;
			if (compare.len != 0) {
				if (compare.pos  + compare.len <= pp->pos) /* no overlap */
					return _FALSE;
				compare.len = pp->len + pp->pos - compare.pos;
			} else { /* new comparison */
				compare.len = pp->len;
				compare.pos = pp->pos;
			}
			if (pc->pos == compare.pos) {
				if (pc->len == compare.len) { /* exact match! */
					pc = pc->next;	/* Compare next! */
					compare.pos = compare.len = 0;
				} else if (pc->len < compare.len) /* Can't match! */
					return _FALSE;
				test = UNION(test, POW2(i)); /* i is in the index! */
			}
			else 
				compare.pos = compare.len = 0;
		}
		if (test != s)	/* test index does not match actual */
			return _FALSE;
	}
	return _TRUE;
}


static
int is_generically_degen(CUBE *cube)
/* Checks a general cube and returns _TRUE if generically degenerate, _FALSE
   otherwise.  Assumes a general cube, for speed.
   Basically we check if there exists i s.t. for all s,
   C(s union {i}) = C(s) */
{
	unsigned int	s, i;
	int				n, final;

PR("IN IS_GEN_DEGEN... (NO OUT)");

	n = cube->dim;
	final = POW2(n);
	for (i = 0; i < n; i++) {
		if ((cube->vertices + POW2(i))->len != 0) /* Test that C(0) = C({i}) */
			continue;
		for (s=1; s < final - 1; s++)
			/* make sure (sU{i}) != ({i}) (trivial), then compare */
			if (!ELEMENT_OF(s, POW2(i)) &&
					!compare_vertices(cube, UNION(s, POW2(i)), s))
						break;
		if (s == final - 1) /* then all s satisfy C(s U {i}) = C(s) */
			return _TRUE;
	}
	return _FALSE;
}


static
int compare_vertices(CUBE *cube, unsigned int a, unsigned int b)
/* Returns _TRUE if the vertices are identical, _FALSE otherwise, where 'a'
   is one vertex, 'b' is the other. */
{
	CUBE_INFO *pa, *pb, *vert = cube->vertices;

	for (pa = vert + a, pb = vert + b; pa != NULL && pb != NULL;
											pa=pa->next, pb=pb->next)
		if (pa->pos != pb->pos || pa->len != pb->len)
			return _FALSE;
	if (pa == NULL && pb == NULL)	/* incase one is 'longer' than the other */
		return _TRUE;
	return _FALSE;
}


static
int is_generically_doubled(CUBE *cube)
/* Checks a general cube and returns _TRUE if generically doubled, _FALSE
   otherwise.  Assumes a general cube, for speed.
   Basically we check if there exists i & j s.t. for all s,
   C(s union {i}) = C(s union {j}) */
{
	unsigned int	s, i, j, union_s_i, union_s_j;
	int				n, final;

PR("IN IS_GEN_DOUBL... (NO OUT)");

	n = cube->dim;
	final = POW2(n);

	for (i = 0; i < n; i++)
		for (j = i + 1; j < n; j++) {
			for (s = 0; s < final - 1; s++) {
				union_s_i = UNION(s, POW2(i));
				union_s_j = UNION(s, POW2(j));
				if (union_s_i != union_s_j &&
						!compare_vertices(cube, union_s_i, union_s_j))
					break;
			}
			if (s == final - 1) {
				return _TRUE;
			}
		}
	return _FALSE;
}


static
int is_product(CUBE *cube)
/* Checks a general cube and returns _TRUE if a product cube, _FALSE
   otherwise.  Assumes a general cube, for speed.
   The algorithm splits the cube into two lower dimensional parts, that
   add to the original dimension, then checks the split to see if it's
   product gives us the original cube.  This is done until a product is
   achieved, or we have exhausted all split possiblities. */
{
	int				split = 0;

PR("In Is_product\n");
DC(cube);
	while ((split = find_split(cube, split)) != -1)
		if (check_one_split(cube, split) == _TRUE) {
			return _TRUE;
		}
	return _FALSE;
}


static
int check_one_split(CUBE *cube, int split)
/* Given a cube, and the position to split the cube, the functions checks
   to see if the product gives us the original cube. */
{
	unsigned int	a, b;

	for (a = 1; a < POW2(split); a++)
		for (b = 1; b < POW2(cube->dim - split); b++)
			if (!check_product(cube, a, b << split))
				/* We use b << split as we want to convert our b's indexs
				   into the indexs of our original cube. */
				return _FALSE;
	return _TRUE;
}


static
CUBE *extract_upper_face_cube(CUBE *cube, int i, int sign)
/* Extracts the i'th upper face from the given 'cube'.  Note that i'th
   actually means the face consisting of rewriting along all edges *except*
   the i'th edge. 
   NOTE: 1) The cube must *not* be canonical or degenerate!
            This is not checked for speed!
         2) The first edge is i = 0.
         3) The cube returned is Canonical. */
{
	CUBE	*upper;
	CUBE_INFO	*pp, *pc;
	int			j;

PR("IN EXTRACT_UPPER");

	upper = NEW(CUBE, 1);
	upper->word = NEW(char, strlen(cube->word) + 1);
	strcpy(upper->word, cube->word);
	upper->dim = cube->dim - 1;		/* upper faces are 1 dim lower */
	upper->canonical = _TRUE;
	upper->sign = sign;
#ifdef _STATS
	upper->level = cube->level + 1;
#endif

	pp = cube->vertices;
	pc = upper->vertices = NEW(CUBE_INFO, 1);

	pc->len = pc->pos = 0;
	/* This handles just the first CUBE_INFO structure.  It is separated
	   for efficiency (to avoid testing for the first). */
	for (j=0; pp != NULL; j++, pp = pp->next)
		if (j != i) {
			pc->pos = pp->pos;
			pc->len = pp->len;
			pp=pp->next;
			j++;
			break;
		}
	for (; pp!= NULL; j++, pp = pp->next) /* copy the rest, except for j == i */
		if (j != i) {
			pc = pc->next = NEW(CUBE_INFO, 1);
			pc->pos = pp->pos;
			pc->len = pp->len;
		}
	pc->next = NULL;

PR("OUT EXTRACT_UPPER");
	return upper;
}


static
CUBE *extract_lower_face_cube(CUBE *cube, int i, int sign)
/* Extracts the i'th lower face from the given 'cube'.  Note that i'th
   actually means the face whose head is obtained by rewriting along the
   cubes i'th edge. 
   NOTE: 1) The cube must *not* be canonical or degenerate!
            This is not checked for speed!
         2) The first face is i = 0.
		 3) i'th lower face must exist or the function will crash
		    (no check made) */
{
	CUBE			*lower;
	CUBE_INFO		*pp, *pc, *rule_pntr, *child_vert, *parent_vert;
	int				j, rule, apply_pos, before_len, after_len, diff, final_hi;
	unsigned int	aindex, child_index, parent_index, hi, lo;

PR("IN EXTRACT_LOWER");
DC(cube);

	/* Find the exact postion telling us pos and len of rewrite. */
	rule_pntr = parent_vert = cube->vertices;
	for (j= i; j > 0 && rule_pntr != NULL; j--)
		rule_pntr = rule_pntr->next;

	apply_pos = rule_pntr->pos;
	before_len = rule_pntr->len;
	/* rule = actual rule number in our rewriting system */
	rule = find_rule(cube->word, apply_pos, before_len);
	after_len = strlen(Word2[rule]);
	diff = after_len - before_len;

	lower = NEW(CUBE, 1);
	lower->word = apply(cube->word, apply_pos, rule);
	lower->dim = cube->dim - 1;		/* lower faces are 1 dim lower */
	lower->canonical = _FALSE;
	lower->sign = sign;
#ifdef _STATS
	lower->level = cube->level + 1;
#endif

	child_vert = lower->vertices = NEW(CUBE_INFO, POW2(lower->dim));

	aindex = POW2(i);
	final_hi = POW2(cube->dim - i - 1);

	child_index = 1;
	for (hi = 0; hi < final_hi; hi++)
		for (lo = (hi == 0) ? 1 : 0; lo < aindex; lo++, child_index++) {
			parent_index = (hi << (i+1)) | lo | aindex;
			pc = child_vert + child_index;
			pc->pos = pc->len = 0;	/* initially */
			/* Do first CUBE_INFO only, as it already exists.
			   This is messy, but more efficient than having one loop */
			for (pp = parent_vert + parent_index; pp!= NULL; pp = pp->next)
				if (pp->pos != apply_pos || pp->len != before_len) {
					/* Then it is not the rewriting rule... */
					pc->pos = pp->pos;
					pc->len = pp->len;
					if (diff != 0) {
						if (pc->pos <= apply_pos && pc->pos + pc->len > apply_pos)
							pc->len += diff;
						if (pc->pos > apply_pos)
							pc->pos += diff;
					}
					pp = pp->next;
					break;
				}
			/* now do the others */
			for (; pp != NULL; pp = pp->next)
				if (pp->pos != apply_pos || pp->len != before_len) {
					pc = pc->next = NEW(CUBE_INFO, 1);
					pc->pos = pp->pos;
					pc->len = pp->len;
					if (diff != 0) {
						if (pc->pos <= apply_pos && pc->pos + pc->len > apply_pos)
							pc->len += diff;
						if (pc->pos > apply_pos)
							pc->pos += diff;
					}
				}
			pc->next = NULL;
		}

	tidy_rewriting_areas(lower);
	/* Put in the child's new rewriting areas */
	find_default_rewriting_areas(lower);

PR("OUT EXTRACT_LOWER");

	return lower;
}


static
void add_canonical_cube(CUBE *cube, BOUNDARY *boundary_head)
/* Adds a canonical cube to the boundary list... 
   Note the original cube is not deleted, but rewrite positions are altered! */
{
	BOUNDARY	*prev, *curr;
	PART_INFO	*p, *t;
	CUBE_INFO	*v;
	int			first, second, scomp, j;
	char		*left_action, *word;

PR("IN ADD_CAN");
DC(cube);

	if (!is_trivial_canonical(cube)) {
		/* Work out first and last postions */
		v = cube->vertices;
		first = v->pos;
		for (j = cube->dim - 1; v != NULL && j > 0; j--, v = v->next)
			v->pos -= first;	/* Adjust pos so as to ignore left actions */
		second = v->pos + v->len;
		v->pos -= first;

	if (Inverse_tuple_action == IGNORE_INV_TUPLES 
					&& is_inv_tuple(cube->word, first, second-first))
		return;	/* Ignore inverse tuples if instructed */

		/* Get the left action (if needed) and the word (without left action */
		if (Boundary_action == LEFTACTION) {
			left_action = NEW(char, first + 1);
			strncpy(left_action, cube->word, first);
			*(left_action + first) = '\0';
		} else
			left_action = NULL;

		/* Get word, removing left and right actions */
		word = NEW(char, second - first + 1);
		strncpy(word, cube->word + first, second - first);
		*(word + second - first) = '\0';

/*printf("%d ", cube->sign);
printword(stdout, word); fflush(NULL);
display_a_vertex(stdout, cube->vertices); fflush(NULL);*/

		prev = boundary_head;
		curr = prev->left;	/* curr is now the root (initially NULL) */

		scomp = -1; /* Make sure it goes left first! */
		/* Search the tree for it */
		if (curr != NULL && (scomp = strcmp(word, curr->word)) == 0)
			scomp = compare_vertex_to_boundary(cube->vertices, curr);
		while (curr != NULL && scomp != 0) {
			prev = curr;
			curr = (scomp < 0) ? curr->left : curr->right;
			if (curr != NULL && (scomp = strcmp(word, curr->word)) == 0)
				scomp = compare_vertex_to_boundary(cube->vertices, curr);
		}
		if (curr == NULL) {	/* First Time!  Create a new node */
			curr = NEW(BOUNDARY, 1);
			if (scomp < 0)
				prev->left = curr;
			else
				prev->right = curr;
			curr->word = word;
			curr->left = curr->right = NULL;
			curr->vertices = copy_cube_rewrite_info(cube->vertices);
			curr->coeff = NEW(PART_INFO, 1);
			curr->coeff->left_action = left_action;
			curr->coeff->count = cube->sign;
			curr->coeff->next = NULL;
		} else { /* Already exists... */
			free(word);
			if (Boundary_action == TRIVIAL) {
				curr->coeff->count += cube->sign;
			} else {
				for (p = curr->coeff; p!= NULL &&
						strcmp(p->left_action, left_action) != 0; p = p->next)
					;
				if (p != NULL) {
					p->count += cube->sign;
					free(left_action);
				} else {
					t = curr->coeff;
					curr->coeff = NEW(PART_INFO, 1);
					curr->coeff->left_action = left_action;
					curr->coeff->count = cube->sign;
					curr->coeff->next = t;
				}
			}
		}
	}
PR("out of ADD_CAN");
}


static
int compare_vertex_to_boundary(CUBE_INFO *vert, BOUNDARY *b)
{
	CUBE_INFO *p = b->vertices;

	for (; vert != NULL; vert = vert->next, p = p->next) {
		if (p->pos < vert->pos)
			return -1;
		if (p->pos > vert->pos)
			return 1;
	}
	return 0;
}


BOUNDARY *calc_bound(CUBE *main_cube, int type)
/* Calculates the boundary of a cube 'main_cube' in Groves' Form.
   Type is a bit wise 'or' of the following:
			TRIVIAL				(indicating no action should be retained)
			RETAIN_INV_TUPLES	(tuples of the form aa^-1a... are retained)
			LEFTACTION			(indicating left actions should be retained)
			IGNORE_INV_TUPLES	(tuples of the form aa^-1a... are trivial)
	The boundary is returned.
	main_cube is not free'd.  This is left to the user. */
{
	CUBE *upper_face_cube, *lower_face_cube;
	int			i, n = main_cube->dim;
	BOUNDARY	*boundary_head;

PR("IN CALC_BOUND");

#ifdef _STATS
	init_stats();
#endif

	Boundary_action = type & TRIVIAL;		/* Globals holding action types */
	Inverse_tuple_action = type & IGNORE_INV_TUPLES;

	/* The head of the boundary list is only a sentinel to the root of
	   the tree.  This reduces the need to always return the boundary head */
	boundary_head = NEW(BOUNDARY, 1);
	boundary_head->right = boundary_head->left = NULL;

	for (i = 0; i < n; i++) {
		add_canonical_cube(upper_face_cube =
				extract_upper_face_cube(main_cube, i, (i%2) ? -1 : 1),
															boundary_head);
		delete_cube(upper_face_cube);
	}

	for (i = 0; i < n; i++) {
		/*if (!check_inv(main_cube, i)) { *//* Don't extract if of form a*a^-1 */
			calc_cover(lower_face_cube =
				extract_lower_face_cube(main_cube, i, (i%2) ? 1 : -1),
															boundary_head);
			delete_cube(lower_face_cube);
	/*	}*/
	}

	remove_zero_boundaries(boundary_head);

PR("OUT CALC_BOUND");
	return boundary_head;
}


static
void calc_cover(CUBE *cube, BOUNDARY *boundary_head)
/* Calculates the cover of 'cube', adding to the boundary list headed by
   'boundary_head' */
{
	int		vertex; /* Dummy for this version of the program */

PR("In CALC_COVER");
DC(cube);

#ifdef _STATS
	if (cube->level > Max_level)
		Max_level = cube->level;
#endif

	if (!IS_CUBE(cube))	/* Trivial */
		return;

	reorder_vertices(cube);

	if (is_canonical_cube(cube))
		add_canonical_cube(cube, boundary_head);
	else if (!is_generically_doubled(cube)) {
		if (is_degen_cube(cube, &vertex)) {	/* vertex not needed as
											   vertices are always sorted */
			if (!is_generically_degen(cube)) {
				decompose_degen_cube(cube, boundary_head);
			}
		} else if (!is_product(cube))
			decompose_nondegen_cube(cube, boundary_head);
	}
}


static
void decompose_nondegen_cube(CUBE *cube, BOUNDARY *boundary_head)
/* Decomposes a non degenerate cube into top cube and bottom cubes */
{
	CUBE		*topcube, *subcube;
	int i, n = cube->dim;

PR("In DECOMPOSE_NONDEGEN_TOP");
	add_canonical_cube(topcube = get_nondegen_topcube(cube), boundary_head);
	delete_cube(topcube);

	for (i = 0; i < n; i++) {
		calc_cover(subcube = get_nondegen_subcube(cube, i), boundary_head);
		delete_cube(subcube);
	}
PR("Out DECOMPOSE_NONDEGEN_TOP");
}


static
void decompose_degen_cube(CUBE *cube, BOUNDARY *boundary_head)
/* Decomposes a degenerate cube into top cubes and bottom cubes */
{
	CUBE *subcube;
	int		i, j, n = cube->dim;

	for (i=1; i < n; i++) {
		calc_cover(subcube = get_degen_topcube(cube, i), boundary_head);
		delete_cube(subcube);
	}
	
	for (i = 1; i < n; i++)
		for (j = i+1; j < n; j++) {
			calc_cover(subcube = get_degen_subcube(cube, i, j), boundary_head);
			delete_cube(subcube);
		}
}


static
CUBE *get_degen_topcube(CUBE *parent, int i)
/* Returns the TOP cube from a DEGENERATE cube being decomposed. 
   The first vertex must be degenerate...if not use get_degen_topcube_general.
   The 'parent' is not freed...this is left to the user. */
{
	CUBE		*child;
	CUBE_INFO		*pp, *pc;
	unsigned int	s, s1, s2;
	int				n, num_vertices;

PR("IN GET_DEGEN_TOP\n");
DC(parent);

	n = parent->dim;
	num_vertices = POW2(n);

	/* set up child info (the top cube) */
	child = NEW(CUBE,1);
	child->dim = parent->dim;
	child->sign = parent->sign * ((i%2) ? -1 : 1);
	child->canonical = _FALSE;

#ifdef _STATS
	Topcube_d_count++;
	child->level = parent->level + 1;
#endif

	child->word = NEW(char, strlen(parent->word) + 1);
	strcpy(child->word, parent->word);

	pp = parent->vertices;
	pc = child->vertices = NEW(CUBE_INFO, num_vertices);

	/* Now create the top cube */
	for (s=1; s < num_vertices - 1; s++) {
		s1 = INTER(s, POW2(i) - 1);
		s2 = DIFF(s, s1);
		if (s2)
			copy_cube_vertex(pp + UNION(UNION(INCSET(s1), s2), 1), pc + s);
		else
			copy_cube_vertex(pp + INCSET(s1), pc + s);
	}
	copy_cube_vertex(pp + s, pc + s);
	find_default_rewriting_areas(child);

PR("OUT GET_DEGEN_TOP");
DC(child);
	return child;
}


static
CUBE *get_degen_subcube(CUBE *parent, int i, int j)
/* Returns a SUB cube from a DEGENERATE cube being decomposed. 
   The first vertex must be degenerate...if not use get_degen_subcube_general
   funcxtion.
   The 'parent' is not freed...this is left to the user. */
{
	CUBE		*child;
	CUBE_INFO		*pc, *parent_vert;
	unsigned int	s, s1, s2, s3, c;
	int				n, num_vertices, rewrite_head = _FALSE;

PR("BEFORE GETTING DEGEN SUB CUBE");
DC(parent);

	n = parent->dim;
	num_vertices = POW2(n);

	/* Set up child info (sub cube) */
	child = NEW(CUBE,1);
	child->dim = parent->dim;
	child->sign = parent->sign * (((i+j)%2) ? 1 : -1);
	child->canonical = _FALSE;
#ifdef _STATS
	Subcube_d_count++;
	child->level = parent->level + 1;
#endif

	child->vertices = NEW(CUBE_INFO, num_vertices);
	parent_vert = NEW(CUBE_INFO, 1);
	/* Make a copy, as we need the original for fixing up the rewrite areas */
	copy_cube_vertex(parent->vertices + POW2(i), parent_vert);

	rewrite_head = parent_vert->len;	/* If degen, no rewriting required */
	if (rewrite_head) {
		child->word = rewrite_given_areas(parent->word, parent_vert);
	} else {	/* No need to rewrite words, just move them */
		child->word = NEW(char, strlen(parent->word) + 1);
		strcpy(child->word, parent->word);
	}

	/* Now get the sub-cube */
	for (s=1; s < num_vertices; s++) {
		pc = child->vertices + s;
		pc->pos = pc->len = 0;
		s1 = INTER(s, (POW2(i) - 1));
		s2 = INTER(s, (POW2(j-i) - 1) << i);
		s3 = INTER(s, (POW2(n-j) - 1) << j);
		if (s3)
			c = UNION(UNION(UNION(UNION(s1, INCSET(s2)), POW2(i)), 1),s3);
		else	
			c = UNION(UNION(s1, INCSET(s2)), POW2(i));
		copy_cube_vertex(parent->vertices + c, pc);
		if (rewrite_head) {
			/* Take the rewrite area and rewrite so it is now in terms of
               the new head of this sub cube */
			fix_rewrite_areas(parent->word, pc, parent_vert);
		}
	}

	delete_vertices(parent_vert);
	tidy_rewriting_areas(child);
	find_default_rewriting_areas(child);

PR("AFTER GETTING DEGEN SUB CUBE");
DC(child);

	return child;
}


static
void fix_rewrite_areas(char *orig_word, CUBE_INFO *old_rewrite,
												CUBE_INFO *top_rewrite)
/* Given a word (orig_word), the original rewrite areas of that word
   (old_rewrite), and the areas where that word will be rewritten
   (top_rewrite), alters old_rewrite so that it is now written in terms
   of areas to rewrite the original word AFTER it has been rewritten
   according to 'top_rewrite'.  This is used to fix up the rewrite areas
   after creating a degenerate subcube (see get_degen_subcube). */
{
	CUBE_INFO	*p;
	char		tmpword[_HOM_MAX_SIZE];	/* Used for temp storage of word */
	int			diff, pos_adjust = 0, len_adjust = 0;

	for (p = old_rewrite; p != NULL && top_rewrite!= NULL;) {
		if (p->pos <= top_rewrite->pos
					&& p->pos + p->len >= top_rewrite->pos + top_rewrite->len) {
			/* Rewrite area affected by rewriting according to top_rewrite */
			strncpy (tmpword, orig_word + top_rewrite->pos, top_rewrite->len);
			tmpword[top_rewrite->len] = '\0';
			reduce(tmpword);

			/* We don't make adjustments on pos or len until we've gone
               through at least one more pass - in case we have two rewrite
               areas in top_rewrite contained in the one rewrite area p */
			diff = strlen(tmpword) - top_rewrite->len;
			len_adjust += diff;
			top_rewrite = top_rewrite->next;
		} else {
			p->pos += len_adjust;	/* Don't change pos until past it */
			pos_adjust += len_adjust;	/* Pos should be changed for next */
			p->len += len_adjust;
			len_adjust = 0;
			p = p->next;
		}
	}
	/* Make sure we move the current p (as top_rewrite may be NULL) */
	if (p != NULL) { /* Do first separately as we don't want to move it's
						pos more than required */
		p->pos += pos_adjust;
		pos_adjust += len_adjust;
		p->len += len_adjust;
		/* Do others as they must be moved as with the previous + len_adjust*/
		for (p = p->next; p != NULL; p = p->next)
			p->pos += pos_adjust;
	}
}


CUBE *create_cube(char *word)
/* Given the head of a cube, the function creates a complete cube structure
   with vertices given by the overlap of the rules which can be applied to
   this word.  */
{
	CUBE		*cube;
	CUBE_INFO		*cube_vert, *pp, *pc;
	unsigned int	i, j, s;
	int				n, num_vertices, current_len, current_pos;

	/* Set up the cube info */
	cube = NEW(CUBE, 1);
	n = cube->dim = count_rewriting_areas(word);
	num_vertices = POW2(n);
	cube_vert = cube->vertices = NEW(CUBE_INFO, num_vertices);

	cube->word = NEW(char, strlen(word) + 1);
	strcpy(cube->word, word);
	cube->canonical = _FALSE;
	cube->sign = 1;
#ifdef _STATS
	cube->level = 0;
#endif

	/* Put in the cube's new rewriting areas */
	find_all_rewriting_areas(cube->word, cube_vert);

	for (s = 1; s < num_vertices; s++) {
		current_pos = current_len = 0;
		pc = cube_vert + s;
		pc->pos = pc->len = 0;	/* initially */
		/* current_pos and current_len store pos and len, increasing len
		   with overlap until we find one without overlap.  At this
		   point we then create the CUBE_INFO (if necessary) and put it in */
		for (i = 0; i < n; i++)
			if (ELEMENT_OF(POW2(i), s)) {
				/* the index has POW2(i) as an element */
				for (j = i, pp = cube_vert; pp != NULL && j > 0 ; j--)
					pp=pp->next;
				if (current_len == 0) { /* First time */
					current_len = pp->len;
					current_pos = pp->pos;
				} else if (pp->pos < current_pos + current_len) {/*overlap*/
					current_len = pp->pos - current_pos + pp->len;
				} else {
					if (pc->len != 0)
						pc = pc->next = NEW(CUBE_INFO, 1);
					pc->pos = current_pos;
					pc->len = current_len;
					current_len = pp->len;
					current_pos = pp->pos;
				}
			}
		/* Check the last one out */
		if (pc->len != 0)
			pc = pc->next = NEW(CUBE_INFO, 1);
		pc->pos = current_pos;
		pc->len = current_len;
		pc->next = NULL;
	}
	return cube;
}


CUBE *create_n_cube(char *word, CUBE_INFO *critical_vert, int n)
/* Given the head of a cube, the function creates a complete cube structure
   with vertices given by the overlap of the rules whose position and
   length are stored in critical_vert. */
{
	CUBE		*cube;
	CUBE_INFO		*cube_vert, *pp, *pc;
	unsigned int	i, j, s;
	int				num_vertices, current_len, current_pos;

	/* Set up the cube info */
	cube = NEW(CUBE, 1);
	num_vertices = POW2(n);
	cube_vert = cube->vertices = NEW(CUBE_INFO, num_vertices);

	cube->word = NEW(char, strlen(word) + 1);
	strcpy(cube->word, word);
	cube->canonical = _FALSE;
	cube->sign = 1;
	cube->dim = n;
#ifdef _STATS
	cube->level = 0;
#endif

	/* Put in the cube's new rewriting areas */
	pp = cube_vert;
	pp->pos = critical_vert->pos;
	pp->len = critical_vert->len;
	for (critical_vert = critical_vert->next; critical_vert != NULL;
								critical_vert = critical_vert->next) {
		pp = pp->next = NEW(CUBE_INFO, 1);
		pp->pos = critical_vert->pos;
		pp->len = critical_vert->len;
	}
	pp->next = NULL;

	for (s = 1; s < num_vertices; s++) {
		current_pos = current_len = 0;
		pc = cube_vert + s;
		pc->pos = pc->len = 0;	/* initially */
		/* current_pos and current_len store pos and len, increasing len
		   with overlap until we find one without overlap.  At this
		   point we then create the CUBE_INFO (if necessary) and put it in */
		for (i = 0; i < n; i++)
			if (ELEMENT_OF(POW2(i), s)) {
				/* the index has POW2(i) as an element */
				for (j = i, pp = cube_vert; pp != NULL && j > 0 ; j--)
					pp=pp->next;
				if (current_len == 0) { /* First time */
					current_len = pp->len;
					current_pos = pp->pos;
				} else if (pp->pos < current_pos + current_len) {/*overlap*/
					current_len = pp->pos - current_pos + pp->len;
				} else {
					if (pc->len != 0)
						pc = pc->next = NEW(CUBE_INFO, 1);
					pc->pos = current_pos;
					pc->len = current_len;
					current_len = pp->len;
					current_pos = pp->pos;
				}
			}
		/* Check the last one out */
		if (pc->len != 0)
			pc = pc->next = NEW(CUBE_INFO, 1);
		pc->pos = current_pos;
		pc->len = current_len;
		pc->next = NULL;
	}
	return cube;
}


static
int find_split(CUBE *cube, int skip)
/* Used in is_product.  Finds the first split (after skipping 'skip' splits)
   in the rewriting areas of our cube.  Returns -1 if the cube can be split
   no more.
   For example, if the word (with rewriting areas indicated) is:
                      abcdefghijkl
					  --
					   ---___ __ 
	then the first split is 2, then 3, then -1. */
{
	CUBE_INFO	*p;
	int split, prev_pos, prev_len;

	for (split = 0, p = cube->vertices; split != skip && p != NULL;
													p = p->next, split++)
		;

	prev_pos = p->pos;
	prev_len = p->len;
	for (p = p->next, split++; p != NULL; p = p->next, split++) {
		if (p->pos >= prev_pos + prev_len)
			return split;
		prev_pos = p->pos;
		prev_len = p->len;
	}

	return -1;
}


static
int check_product(CUBE *cube, unsigned int a, unsigned int b)
/* Given a cube, and the index's a and b, checks to see if their product
   is the same as that of (a U b).  This is used by check_one_split to
   check if a given split of a cube gives us two smaller cubes whose
   product is the larger cube. */
{
	CUBE_INFO *vert1, *vert2, *vert3;

	vert1 = cube->vertices + a;
	vert2 = cube->vertices + b;
	vert3 = cube->vertices + UNION(a,b);

	for (;vert1 != NULL && vert3 != NULL; vert1=vert1->next, vert3=vert3->next)
		if (vert1->pos != vert3->pos || vert1->len != vert3->len)
			return _FALSE;
	if (vert1 != NULL)
		return _FALSE;
	for (;vert2 != NULL && vert3 != NULL; vert2=vert2->next, vert3=vert3->next)
		if (vert2->pos != vert3->pos || vert2->len != vert3->len)
			return _FALSE;
	if (vert2 != NULL || vert3 != NULL)
		return _FALSE;

	return _TRUE;
}


static
void tidy_rewriting_areas(CUBE *cube)
/* Tidies all rewriting areas for all vertices of cube, by finding degenerate
   vertices and replacing them with (0,0) - if not already in this form.
   This helps with degerate tests and sorting.  */
{
	int		i, n = POW2(cube->dim);
	int		pos, len;
	CUBE_INFO *p, *prev, *tmp;

PR("IN TIDY_AREAS");

	for (i = 1; i < n; i++)
		for (prev = p = cube->vertices + i; p != NULL;) {
			pos = p->pos;
			len = p->len;
			if (!find_first_rewriting_area(cube->word, &pos, &len)) {
				/* This rewriting area is rewritten as far as possible */
				tmp = p->next;
				if (prev == p) { /* At beginning of list.  Don't delete node! */
					if (tmp == NULL) { /* No more! */
						p->pos = p->len = 0;
						p->next = NULL;
						break;
					} else {
						p->pos = tmp->pos;
						p->len = tmp->len;
						p->next = tmp->next;
						free (tmp);
					}
				} else {	/* It's not the first, so we can delete it! */
					prev->next = tmp;
					free (p);
					p = tmp;
				}
			} else {	/* Move to the next */
				prev = p;
				p = p->next;
			}
		}

PR("OUT TIDY_AREAS");
}


static
void reorder_vertices(CUBE *cube)
/* Reorders the vertices of a cube, so as to have all rewrite areas in
   increasing order of position, and degenerate vertices first. */
{
	CUBE_INFO		*new_vert, *old_vert, *p, *temp;
	int				sign, n = cube->dim;
	unsigned int	i, s, news;
	SORTED_VERTICES	*sv;

	if (sort_vertex_positions(cube, &sv, &sign) == _FALSE) { /*Already sorted!*/
		free(sv);
		return;
	}
PR("IN REORDER");
DC(cube);

	cube->sign = sign;	/* The new sign after reordering */

	/* Now do the reorder according to the structure sv */
	old_vert = cube->vertices;
	new_vert = NEW(CUBE_INFO, POW2(n));

	for (s = 1; s < POW2(n) - 1; s++) {
		news = 0;
		for (i = 0; i < n; i++)
			if (ELEMENT_OF(POW2(i),s))
				news |= POW2(sv[i].index);
		copy_cube_vertex(old_vert + news, new_vert + s);
	}
	copy_cube_vertex(old_vert + s, new_vert + s);

	/* Free up memory used by the old copy */
	for (i = 0; i < POW2(n); i++)
		for (p = (old_vert + i)->next; p != NULL; p=temp) {
			temp = p->next;
			free(p);
		}
	free(old_vert);

	cube->vertices = new_vert;
	find_default_rewriting_areas(cube);
	free(sv);
PR("OUT REORDER");
DC(cube);
PR("\n\n");
}


static
void remove_zero_boundaries(BOUNDARY *boundary_head)
/* Removes zero boundaries */
{
	remove_zero_boundaries_rec(boundary_head, boundary_head->left);
}


static
void remove_zero_boundaries_rec(BOUNDARY *parent, BOUNDARY *child)
/* Removes zero boundaries recursively.  Called by remove_zero_boundaries. */
{
	BOUNDARY *c, *temp;

	if (child == NULL)
		return;

	while (child != NULL && child->coeff->count == 0) {
		temp = child;
		if (child->right == NULL)
				child = child->left;
		else if (child->left == NULL)
				child = child->right;
		else if (child->right->left == 0) {
			child = child->right;
			child->left = temp->left;
		} else {
			for (c = child->right; c->left->left != NULL; c=c->left)
				;
			child = c->left;
			c->left = child->right;
			child->left = temp->left;
			child->right = temp->right;
		}	
		if (parent->right == temp)
			parent->right = child;
		else
			parent->left = child;
		free(temp->word);
		delete_vertices(temp->vertices);
		delete_coeff(temp->coeff);
		free(temp);
	}

	if (child == NULL)
		return;

	remove_zero_boundaries_rec(child, child->left);
	remove_zero_boundaries_rec(child, child->right);
}


static
char *rewrite_given_areas(char *w, CUBE_INFO *vert)
/* Given a word, and a list of rewrite areas, rewrite's the word at those
   areas - returning the rewritten word.  NOTE: The original word is not
   free'd and remains the same. */
{
	char	*word, *newword;
	int		change_pos = 0;

	word = NEW(char, strlen(w) + 1);
	strcpy(word, w);

	for(;vert != NULL; vert = vert->next) {
		newword = rewrite_word(word, vert->pos + change_pos, vert->len);
		change_pos += (strlen(newword) - strlen(word));
		free(word);
		word = newword;
	}

	return word;
}


static
char *rewrite_word(char *word, int pos, int len)
/* Given a word, and the pos and len of the area within the word to rewrite,
   rewrites the word - returning the rewritten word.  NOTE: The original
   word is not free'd and remains the same. */
{
	char	*newword, subword[_HOM_MAX_SIZE];

	if (len == 0) {	/* Why waste time?! */
		newword = NEW(char, strlen(word) + 1);
		strcpy(newword, word);
		return newword;
	}

	/* Produce and reduce the subword */
	strncpy(subword, word + pos, len);
	subword[len] = '\0';
	reduce(subword);

	/* Create the newword, and join all sections together */
	newword = NEW(char, strlen(word) + strlen(subword) - len + 10);
	strncpy(newword, word, pos);
	newword[pos] ='\0';
	strcat(newword, subword);
	strcat(newword, word + pos + len);

	return newword;
}


static
char *rewrite_rule(char *word, int pos, int len, int rule, int *diff)
/* Rewrite's a rule in a word at pos 'pos'.  The rule is of length 'len',
   and is numbered 'rule' (in the system).  The program returns 'diff',
   being the difference in size before and after rewriting, and the
   rewritten word.  NOTE:  The original word is not free'd and is
   unchanged. */
{
	char	*newword;
	int		after_len = strlen(Word2[rule]);

	*diff = after_len - len;
	newword = NEW(char, strlen(word) + *diff + 1);
	strncpy(newword, word, pos);
	newword[pos] ='\0';
	strcat(newword, Word2[rule]);
	strcat(newword, word + pos + len);

	return newword;
}



/**********************************************************************
 *******************   Functions used for TESTING   *******************
 **********************************************************************/


void display_cube(FILE *fp, CUBE *parent)
/* Displays a cube decomposition */
{
	int	i;
	CUBE_INFO *pp;

	fprintf(fp, "\nword:\t\t\"");
	printword(fp, parent->word);
	fprintf(fp, "\"\ndim:\t\t%d\n", parent->dim);
	fprintf(fp, "sign:\t\t%d\n", parent->sign);
#ifdef _STATS
	fprintf(fp, "LEVEL: %d \n", parent->level);
#endif
	fprintf(fp, "canonical:\t%s\n", (parent->canonical) ? "True": "False");
	for (i=0; i < ((parent->canonical) ? 1 : POW2(parent->dim)); i++) {
		fprintf(fp, "%d: ",i);
		pp = parent->vertices + i;
		do {
			fprintf(fp, "[%d,%d]   ", pp->pos, pp->len);
		} while ((pp = pp->next) != NULL);
		fprintf(fp, "\n");
	}
	return;
}


CUBE *read_cube(char *file_name)
/* Reads a cube from a file format */
{
	FILE *fp;
	CUBE *cube;
	CUBE_INFO *v, *vv;
	char dummy, *w, str[2];
	int ret, c, dim, canonical, sign, size, pos, len;

	str[1] = '\0';

	if ((fp = fopen(file_name, "r")) == NULL)
		ERROR("read_cube: File does not exist!");


	cube = NEW(CUBE, 1);
	w = cube->word = NEW(char, 50);

	fscanf(fp, "%1s", &dummy);
	c = dummy;
	while (isalpha(c)) {
		str[0] = (char) c;
		*(w++) = genstr_to_genint(str);
		c = fgetc(fp);
	}
	*w = '\0';

	fscanf(fp, "%d", &dim);
	cube->dim = dim;

	fscanf(fp, "%d", &canonical);
	cube->canonical = canonical;

	fscanf(fp, "%d", &sign);
	cube->sign = sign;

	size = (canonical) ? 1 : POW2(dim);
	v = vv = cube->vertices = NEW(CUBE_INFO, size);

	vv->len = 0;
	fscanf(fp, "%1s", &dummy);
	ungetc(dummy, fp);
	while ((ret = fscanf(fp,"(%d,%d)", &pos, &len)) != EOF && ret != 0) {
		if (vv ->len != 0)
			vv = vv->next = NEW(CUBE_INFO, 1);
		vv->pos = pos;
		vv->len = len;
		while ((c=fgetc(fp)) != '\n' && isspace(c) && c != EOF)
			;
		dummy = c;
		if (c == EOF) {
			vv->next = NULL;
			break;
		}
		if (c == '\n') {
			vv->next = NULL;
			vv = ++v;
			vv->len = 0;		/* Last line must not contain a \n at end */
			fscanf(fp, "%1s", &dummy);
		}
		ungetc(dummy, fp);
	}

	fclose(fp);

	return cube;
}


#ifdef _STATS
static
void init_stats(void)
/* For Statistics.  Initialises the counters */
{
	Topcube_nd_count = Subcube_nd_count = 0;
	Topcube_d_count = Subcube_d_count = 0;
	Max_level = 0;
}


void get_stats(int *top_nd_count, int *sub_nd_count, int *top_d_count,
				int *sub_d_count, int *max_level)
/* External function that returns statistical info in the statistical version
   of this library (ie _STATS has been defined). */
{
	*top_nd_count = Topcube_nd_count;
	*sub_nd_count = Subcube_nd_count;
	*top_d_count = Topcube_d_count;
	*sub_d_count = Subcube_d_count;
	*max_level = Max_level;
}

#endif /* _STATS */


static
void display_a_vertex(FILE *fp, CUBE_INFO *pp)
{
	do {
		fprintf(fp, "[%d,%d]   ", pp->pos, pp->len);
	} while ((pp = pp->next) != NULL);
	fprintf(fp, "\n");
}



/**********************************************************************
 **********  May be required if we encounter problems later  **********  
 **********************************************************************/



