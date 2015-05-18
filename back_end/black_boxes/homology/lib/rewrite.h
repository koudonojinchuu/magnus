/* rewrite.h     - header file for applications that use rewriting routines
   ===========================================================================
   Written by:  Jamie P. Curmi
   Date:        June - July 1992
   Project:     Computational Tools for Homology of Groups
   For:         Dr J. R. J. Groves and Prof. C. Miller
                Department of Mathematics, University of Melbourne,
                Parkville, Victoria, Australia.
   ===========================================================================
   Users will need to declare the following global variables:
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

#include "homcommon.h"
#include "ndimbdry.h"

#ifndef H_REWRITE
#define H_REWRITE		/* Make sure it is only included once! */

/* TYPEDEF'S */
typedef struct {
	char		*word;
	CUBE_INFO	*vert;
} CRITICAL_INFO;			/* Info part of a critical */

typedef struct {
	int			num;
	CRITICAL_INFO *info[_HOM_MAX_CRITICALS];
} CRITICALS;							/* Structure to store criticals */

#ifndef H_2DIMBDRY		/* Don't include if 2dimbdry.h included already */
typedef struct {
	int		where;
	int 	which;
} INFO;
#endif

typedef struct {
	int		n;
	INFO	apply[_HOM_MAX_N];
} R_INFO;							/* Rewriting info for a word */

/* EXPORTED FUNCTIONS */
extern char	*reduce(char *word);
extern char	*critical(char *word, R_INFO *r_info);
extern int	decisions(char *word, R_INFO *r_info);
extern void	find_criticals(CRITICALS *tuples);
extern char	*apply(char *word, int pos, int rule);
extern int find_rule(char *word, int pos, int len);
extern int any_rewriting_rules(char *word, int len);
extern CRITICALS *find_n_criticals(int n);
extern void delete_criticals(CRITICALS *tuples);
extern void remove_inverse_criticals(CRITICALS **tuples);
extern void display_critical(FILE *fp, CRITICAL_INFO *c);



#endif /* H_REWRITE */
