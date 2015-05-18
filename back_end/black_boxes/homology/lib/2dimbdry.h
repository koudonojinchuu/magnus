/* 2dimbdry.h     - header file for applications using the boundary library
   for homology of groups.
   ===========================================================================
   Written by:  Jamie P. Curmi
   Date:        July 1992
   Project:     Computational Tools for Homology of Groups
   For:         Dr J. R. J. Groves and Prof. C. Miller
                Department of Mathematics, University of Melbourne,
                Parkville, Victoria, Australia.
   ===========================================================================
	Users will need the following global variables:
        extern int  Start;
        extern int  **Fsa;
        extern char **Word1, **Word2;
        extern char **Alphabet;
	These should be created through the use of functions in grpinput.h

	HOW TO USE THESE FUNCTIONS:
	===========================

	1)	Set up the initial Fsa etc for your group (use functions in grpinput.h)

	2)	Decide on which type of boundary you would like to calculate.  If you
		want to regard left and right actions as trivial, use those functions
		and type declarations that end with _triv.  If you want to only regard
		right actions as trivial (and keep left actions), use those functions
		that end with _la.

	3)	You can calculate the 2-cube boundary that begins with your word
		( eg. calc_2_cube_boundary_la(...) ), or the entire boundary for
		a rewriting graph starting with your word
		( eg. calc_boundary_triv(...) ).

	4)  You can display the boundary, or use it by manipulating the returned
		structure (a linked list, as defined in the types, BOUNDARY_LA and
		BOUNDARY_TRIV).

	5)	When you have finished with the BOUNDARY structure, you should
		free up space with a call to clean_up_boundary_la or
		clean_up_boundary_triv.

	An example
	==========

	#include <stdio.h>
	#include "homcommon.h"
	#include "2dimbdry.h"

	.....

	main()
	{

		BOUNDARY_LA		*boundary;
		char			*word;
		.....
	
		(code to set up the Fsa etc.)
		.....
		(code to initialise the word)
		.....

		boundary = calc_boundary_la (word);
		display_boundary_la (word);
		clean_up_boundary_la (word);
	}

*/

#include "homcommon.h"

#ifndef H_2DIMBDRY
#define H_2DIMBDRY		/* Make sure it is only included once! */

/* TYPEDEF'S */

typedef struct {
	char	*word;
	int		where;
	int 	which;
} PATH_INFO;				/* Path info in calculating 2-cube boundaries */

#ifndef H_NDIMBDRY
typedef struct part_info {
	int					count;
	char				*left_action;
	struct part_info	*next;		
} PART_INFO;				/* Used to store a left action in a boundary */
#endif


typedef struct boundary_la {
	PART_INFO			*part;
	int					rule;
	struct boundary_la	*next;
} BOUNDARY_LA;				/* Boundary with left action type */


#ifndef H_REWRITE	/* don't redefine if rewrite.h was included! */
typedef struct {
    int     where;
    int     which;
} INFO;
#endif


#ifndef H_MATRIX	/* don't redefine if matrix.h was included! */
typedef struct boundary_triv {
	int						count;
	int						rule;
	struct boundary_triv	*next;
} BOUNDARY_TRIV;			/* Boundary with all actions trivial type */
#endif

extern void				printword(FILE *fp, char *word);

/* These are 2-dim versions */
extern BOUNDARY_LA		*calc_2_cube_boundary_la (char *word);
extern BOUNDARY_LA		*calc_boundary_la (char *word);
extern void				display_boundary_la (BOUNDARY_LA *head);
extern void				clean_up_boundary_la (BOUNDARY_LA *head);

extern BOUNDARY_TRIV	*calc_2_cube_boundary_triv (char *word);
extern BOUNDARY_TRIV	*calc_boundary_triv (char *word);
extern void				display_boundary_triv (BOUNDARY_TRIV *head);
extern void				clean_up_boundary_triv (BOUNDARY_TRIV *head);

/* n-dim versions are now included as part of ndimbdry  */

#endif /* H_2DIMBDRY */
