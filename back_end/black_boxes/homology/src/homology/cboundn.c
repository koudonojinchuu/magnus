/* ****  cboundn.c  ****

   Written by:  Jamie P. Curmi
   Date:		November 1993
   For:			Dr J. R. J. Groves and Prof. C. Miller
				Dept of Mathematics, Uni of Melb, Parkville, Australia.

   =========================================================================
   Calculates the boundary of a word's n-cube.
   =========================================================================

   As the program stands, we use the executable as follows:

			cboundn [-leftaction | -trivial] [file]

   If option -leftaction is given, the boundary is calculating by
   regarding right actions as trivial, but keeping the left action.

   The default option is -trivial, which regards both right and left
   actions as trivial.

   Input is expected to be either from a file, or if no filename is given,
   from standard input.  It should conform to the Geometry Group Standard
   format, and in particular should be of the form outputted by kbeqn or
   cfsa - that is, we can just use the output from kbeqn as the input to our
   program.

   The program does the following:
   Asks for a word (of the form 'e d c b a <return>' for the word 'edcba'),
   converts this to a numeric string (eg 5731), and then calculates the
   boundary of it's n-cube.

   NOTE:  The program finds the first overlapping rewriting rules for
   the given word only.  It does not allow you to indicate where rewriting
   rules are.  This should be fixed in a future version.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "homcommon.h"
#include "grpinput.h"
#include "ndimbdry.h"

/***********************************************************************/
/* De-comment the line below if the system you are using does not make */
/* use of a command line (such as the Mac).                            */
/*#define NO_CMD_LINE */
/***********************************************************************/


#define VERSION_NUM 1.1

#define USAGE		"Usage:  cboundn [-leftaction | -trivial] [file]"
#define CANT_OPEN	"ERROR: Problem opening file!"
#define NO_GEN		"ERROR: One of the generators is not defined for this group!"
#define FORMAT		"Error:  in file format!"
#define GEN_ERR		"Error:  Something that was not a generator was encountered!"

#define ERROR(A)        {fprintf(stderr, "%s\n", A); exit(-1);}

#define MAX_SIZE	200


/* OTHER FUNCTIONS */
#ifndef NO_CMD_LINE
void	command_line(FILE **fp, int argc, char *argv[]);
#else
void    command_input(FILE **fp);
#endif


/**** GLOBALS in this source ****/
int			Boundary_type = TRIVIAL;			/* default */


#ifndef NO_CMD_LINE /* version that does not use command line - such as Mac */
main(int argc, char *argv[])
#else
main()
#endif
{
	FILE			*fp = stdin;	  /* File pointer, default stdin */
	char			*test, *string, aw[MAX_SIZE];
	int				i, n, count, truth;

	BOUNDARY		*boundary;
	
	CUBE			*uhead, *new, *top;
	CUBE_INFO		*p;

	printf("\ncboundn - version %1.2f\n\n", VERSION_NUM);

#ifndef NO_CMD_LINE
	/* Check the command line and open file if required */
	command_line(&fp,argc, argv);
#else
    /* Read command info from standard input */
    command_input(&fp);
#endif

	/* Read in info from the file, setting up Fsa and so on... */
	switch (read_file_info(fp)) {
		case FORMAT_ERROR:
			ERROR(FORMAT);
		case NOT_A_GEN:
			ERROR(GEN_ERR);
	}
	fclose(fp);  /* No more reading required */

	/* read in the head word 'test' */

	test = NEW(char, 1000);
	string = NEW(char, 1000);
	printf("Enter word seperating generators by spaces (then return):\n");
	gets(string);
	string[strlen(string)] = ' ';
	string[strlen(string) + 1] = '\0';
	for(i = 0, count = 0; sscanf(string + count, "%s%n", aw, &n) == 1;
										count += n, i++)
		if((test[i] = genstr_to_genint(aw)) == 0)
			ERROR(NO_GEN);
	test[i] = '\0';

	/* Creates the cube headed by the word 'test' */
	uhead = create_cube(test);
	display_cube(stdout, uhead);

	boundary = calc_bound(uhead, Boundary_type);	/* Calc Boundary */

	printf("\nDisplaying Boundary....\n\n");
	display_boundary(stdout, boundary);

	/* Clean up */
	delete_boundary(boundary);
	delete_cube(uhead);
	free(test);
	free(string);

	return 0;
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

	if (argc > 3){		/* Altered later if more arguments */
		ERROR(USAGE);
	}
	for (count = 1; count < argc; count++)
			if (strcmp(argv[count], "-leftaction") == 0)
				Boundary_type = LEFTACTION;
			else if (strcmp(argv[count], "-trivial") == 0)
				;
			else if (*(argv[count]) == '-') {
				ERROR("*ERROR* Not a valid option!\n");
			}
			else if (count == argc - 1) {
				if ((*fp = fopen(argv[count],"r")) == NULL) {
					ERROR(CANT_OPEN);
				}
			} else {
					ERROR(USAGE);
			}
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

