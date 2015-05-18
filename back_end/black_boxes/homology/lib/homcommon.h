/* homcommon.h     - header file with common macros used in the homology
   libraries
   ===========================================================================
   Written by:  Jamie P. Curmi
   Date:        July 1992
   Project:     Computational Tools for Homology of Groups
   For:         Dr J. R. J. Groves and Prof. C. Miller
                Department of Mathematics, University of Melbourne,
                Parkville, Victoria, Australia.
   ===========================================================================
*/

#ifndef H_HOMCOMMON
#define H_HOMCOMMON		/* Make sure it is only included once! */

/* Allocate memory of size N, type T.  No initial values. */
#define NEW(T,N)		(T *) malloc (sizeof(T) * (N))
/* Allocate memory of size N, type T... initialising all elts to 0 */
#define ZERO_NEW(T,N)	(T *) calloc ((N), sizeof(T))

#define _HOM_MAX_N			20		/* Max Num of rewriting rules in a word */
#define _HOM_MAX_LEN		5000	/* Max num of states in History */
#define _HOM_MAX_SIZE		300		/* Max size of a keyword */
#define _HOM_MAX_CRITICALS	8000	/* Max number of criticals */
#define _HOM_MAX_PATH_LEN	1000	/* Max length of a single path through the
									   graph (used in 2D calc. boundary) */
#define _FALSE			0
#define	_TRUE			1

extern int  Num_states,     /* Number states in Fsa */
			Num_gens,       /* Number of generators in group */
			Num_rules,      /* Number of Rewriting rules */
			Start,          /* Start state of the Fsa */
			**Fsa;          /* Finite State Automata */

extern char **Word1,        /* LHS of rewriting rules */
			**Word2,        /* RHS of rewriting rules */
			**Alphabet;     /* Gives the string version of a generator's
									numeric value */

#endif /* H_HOMCOMMON */
