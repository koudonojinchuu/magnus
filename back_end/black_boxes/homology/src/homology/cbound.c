/* ****  cbound.c  ****

   Written by:  Jamie P. Curmi
   Date:		June-july 1992
   For:			Dr J. R. J. Groves and Prof. C. Miller
				Dept of Mathematics, Uni of Melb, Parkville, Australia.

   =========================================================================
   Calculates the boundary of a word's 2-cube and total graph formed by
   the directed graph of the supplied rewriting system starting at that word.
   For examples of this, see J.R.J.Groves "Rewriting Systems and Homolgy of
   Groups" (research report, the Uni of Melbourne, Dept of Maths, 1990)
   =========================================================================

   As the program stands, we use the executable as follows:

			cbound [-leftaction | -trivial] [file.kbeqn]

   If option -leftaction is given, the boundary is calculating by
   regarding right actions as trivial, but keeping the left action.

   The default option is -trivial, which regards both right and left
   actions as trivial.

   Input is expected to be either from a file, or if no filename is given,
   from standard input.  It should conform to the Geometry Group Standard
   format, and in particular should be of the form outputted by kbeqn - that
   is, we can just use the output from kbeqn as the input to our program.

   The program does the following:
   Asks for a word (of the form 'c d b a <return> ^D' for the word 'dcba'),
   converts this to a numeric string (eg 5731), and then calculates the
   boundary of it's 2-cube, and the total boundary of it's graph.

   Eg.

	<unix prompt> cbound -leftaction testgrp.kbeqn
  	Enter word (separate each generator by a space):
	c d b a
	^D

	2-cube boundary...
	+ (cd - c )[9] + (c - bc )[11] + (ac - c )[13]
	
	...etc...
*/

#include <stdio.h>
#include <homcommon.h>
#include <grpinput.h>
#include <2dimbdry.h>

/***********************************************************************/
/* De-comment the line below if the system you are using does not make */
/* use of a command line (such as the Mac).                            */
/*#define NO_CMD_LINE */
/***********************************************************************/

#define USAGE	"Usage:  cbound [-leftaction | -trivial] [file.kbeqn]\n"
#define CANT_OPEN	"*ERROR* Problem opening file!\n"

#define MAX_SIZE      200
#define ERROR(X)    {fprintf(stderr, "%s\n", X); exit(-1);}

enum types	{LEFTACTION, TRIVIAL};

/* OTHER FUNCTIONS */
#ifndef NO_CMD_LINE
void	command_line(FILE **fp, int argc, char *argv[]);
#else
void    command_input(FILE **fp);
#endif


/**** GLOBALS USED BY THE LIBRARY FUNCTIONS ****/
int			Num_states,		/* Number states in Fsa */
			Num_gens,		/* Number of generators in group */
			Num_rules,		/* Number of Rewriting rules */
			Start,			/* Start state of the Fsa */
			**Fsa;			/* Finite State Automata */

char		**Word1,		/* LHS of rewriting rules */
			**Word2,		/* RHS of rewriting rules */
			**Alphabet;		/* Gives the string version of a generator's
				       								numeric value */

/**** GLOBALS in this source ****/
int			Boundary_type = TRIVIAL;			/* default */


#ifndef NO_CMD_LINE /* version that does not use command line - such as Mac */
main(int argc, char *argv[])
#else
main()
#endif
{
	FILE			*fp = stdin;	  /* File pointer, default stdin */
	char			*test, aw[MAX_SIZE];
	int				i;
	BOUNDARY_LA		*boundary_la;
	BOUNDARY_TRIV	*boundary_triv;

#ifndef NO_CMD_LINE
	/* Check the command line and open file if required */
	command_line(&fp,argc, argv);
#else
    /* Read command info from standard input */
    command_input(&fp);
#endif

	/* Read in info from the file, setting up Fsa and so on... */
	read_file_info(fp);
	fclose(fp);	/* No more reading required */

	/* read in the test word */
	test = NEW(char, 1000);
	printf("Enter test word (separate each gen by a space):\n");
	for (i=0; scanf("%s", aw) != EOF; i++)
		test[i] = genstr_to_genint(aw);
	test[i] = '\0';

	printf("\nThe test word: ");
	printword(stdout, test);
	printf("\n");

	if (Boundary_type == LEFTACTION) {
		printf("\n2-cube boundary...\n");
		boundary_la = calc_2_cube_boundary_la(test);
		display_boundary_la(boundary_la);
		clean_up_boundary_la(boundary_la);

		printf("\nTotal boundary...\n");
		boundary_la = calc_boundary_la(test);
		display_boundary_la(boundary_la);
		clean_up_boundary_la(boundary_la);
	} else {
		printf("2-cube boundary...\n");
		boundary_triv = calc_2_cube_boundary_triv(test);
		display_boundary_triv(boundary_triv);
		clean_up_boundary_triv(boundary_triv);

		printf("\nTotal boundary...\n");
		boundary_triv = calc_boundary_triv(test);
		display_boundary_triv(boundary_triv);
		clean_up_boundary_triv(boundary_triv);
	}
	exit (0);
}



#ifndef NO_CMD_LINE
void command_line(FILE **fp, int argc, char *argv[])
/* Checks the command line, and opens up a file if requested.  If the 
   -leftaction option is used, sets the global Boundary_type to indicate
   which function group to use.
   Further work on the program will undoubtably lead to more command line
   arguments, which can easily be added in here.  */
{
	int		count;

	if (argc > 3)		/* Altered later if more arguments */
		ERROR(USAGE);
	for (count = 1; count < argc; count++)
			if (strcmp(argv[count], "-leftaction") == 0)
				Boundary_type = LEFTACTION;
			else if (strcmp(argv[count], "-trivial") == 0)
				;
			else if (count == argc - 1) {
				if ((*fp = fopen(argv[count],"r")) == NULL)
					ERROR(CANT_OPEN);
			} else 
					ERROR(USAGE);
}
#endif


#ifdef NO_CMD_LINE
void command_input(FILE **fp)
/* For use on systems that don't have a command line, such as the Mac.  Rather
   than enter command line arguments, you are prompted for them. */
{
    char s[256];

    do {
        printf("Do you want to retain left actions in boundary calculations?\n");
        printf("(yes or no - press enter for default (no)): ");
        *s = '\0';
        Boundary_type = TRIVIAL;
        if (*gets(s) == 0)
                        break;
        if (*s == 'y' || *s == 'Y') {
                        Boundary_type = LEFTACTION;
                        break;
                }
        else if (*s == 'n' || *s == 'N')
                        break;
                else
                        printf("** Please enter yes or no\n\n");
    } while (1);

    do {
        printf("Enter the name of the file containing the group's complete rewriting system.\n(Press 'Enter' for default (standard input)): ");
        *fp = stdin;
        *s = '\0';
        if (*gets(s) == 0)
            break;
        if ((*fp = fopen(s, "r")) == NULL) {
            printf("** An error occured opening that file (does it exist?)\n\n");
            *s = '\0';
        }
    } while (*s == '\0');
}
#endif

