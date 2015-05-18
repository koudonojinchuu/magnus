/* ndimbdry.h     - header file for applications decomposing cubes for
                  homology of groups.
   ===========================================================================
   Written by:  Jamie P. Curmi
   Date:        July 1993 - 1994
   Project:     Computational Tools for Homology of Groups
   For:         Dr J. R. J. Groves and Prof. C. Miller
                Department of Mathematics, University of Melbourne,
                Parkville, Victoria, Australia.
   ===========================================================================
	Users will need the following global variables:
        extern int  Num_rules;
        extern char **Word1, **Word2;
	These should be created through the use of functions in grpinput.h

*/

#ifndef H_NDIMBDRY
#define H_NDIMBDRY			/* Make sure it is only included once! */

#include "homcommon.h"

	/* These are used to indicate the type of boundary required */
#define	TRIVIAL				0 /* indicates no action should be retained */
#define RETAIN_INV_TUPLES	0 /* tuples of the form aa^-1a... are retained */
#define	LEFTACTION			1 /* indicates left actions should be retained */
#define IGNORE_INV_TUPLES	2 /* tuples of the form aa^-1a... are trivial */

/* TYPEDEF'S */

typedef struct cube_info{
	int		pos;
	int 	len;
	struct cube_info *next;
} CUBE_INFO;				/* Cube info used in decomposing cubes */


typedef struct cube_decomp {
	char				*word;
	int					dim;
	CUBE_INFO			*vertices;		
	int					canonical;
	short				sign;
#ifdef _STATS
	int					level;		/* Statistics purposes Only */
#endif
} CUBE;				/* Used to store a cube in Groves' Form */


#ifndef H_2DIMBDRY			/* don't redefine if 2dimbdry.h was included! */
typedef struct part_info {
	int					count;
	char				*left_action;
	struct part_info	*next;
} PART_INFO;                /* Used to store a left action in a boundary */
#endif


typedef struct boundary {
	char			*word;
	int				sign;
	PART_INFO		*coeff;
	CUBE_INFO		*vertices;
	struct boundary	*left, *right;
} BOUNDARY;

extern void delete_cube(CUBE *pp);
extern void delete_boundary(BOUNDARY *boundary);
extern void display_cube(FILE *fp, CUBE *parent);
extern void display_boundary(FILE *fp, BOUNDARY *boundary);
extern BOUNDARY *calc_bound(CUBE *main_cube, int type);
extern int is_inv_tuple(char *word, int pos, int len);
extern CUBE *create_cube(char *word);
extern CUBE *create_n_cube(char *word, CUBE_INFO *critical_vert, int n);

#ifdef _STATS
extern void get_stats(int *top_nd_count, int *sub_nd_count, int *top_d_count,
									int *sub_d_count, int *max_level);
#endif

#endif /* H_NDIMBDRY */
