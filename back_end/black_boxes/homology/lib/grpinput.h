/* grpinput.h     - header file for applications reading in output files from
   kbeqn.
   ===========================================================================
   Written by:  Jamie P. Curmi
   Date:        June - July 1992
   Project:     Computational Tools for Homology of Groups
   For:         Dr J. R. J. Groves and Prof. C. Miller
                Department of Mathematics, University of Melbourne,
                Parkville, Victoria, Australia.
   ===========================================================================
	The following global variables are defined.
	int		Num_states,
			Num_gens,
			Num_rules,
			Start,
			**Fsa;

	char	**Word1,
			**Word2,
			**Alphabet;

   Input is expected to be either from a file, or if no filename is given,
   from standard input.  It should conform to the Geometry Group Standard
   format, and in particular should be of the form outputted by kbeqn - that
   is, we can just use the output from kbeqn as the input to our program.
*/

#include "homcommon.h"

#ifndef H_GRPINPUT
#define H_GRPINPUT		/* Make sure it is only included once! */

#define	FORMAT_ERROR	1	/* An error in the file format was encountered */
#define	NOT_A_GEN		2	/* Something that was not a generator was
							   encountered where a generator was expected */

/* PUBLIC FUNCTIONS */
extern int	read_file_info(FILE *fp);
extern char	genstr_to_genint(char *gen);

#endif /* H_GRPINPUT */
