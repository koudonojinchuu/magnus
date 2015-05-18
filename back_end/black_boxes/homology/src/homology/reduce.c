/* ****  reduce.c  ****

   Written by:  Jamie P. Curmi
   Date:		July 1993
   For:			Dr J. R. J. Groves and Prof. C. Miller
				Dept of Mathematics, Uni of Melb, Parkville, Australia.

   =========================================================================
   Given a rewriting system, rewrites a word, reducing it to Normal Form.
   =========================================================================

   Use the executable as follows:

		reduce [file]	

   Input is expected to be either from a file, or if no filename is given,
   from standard input.  It should conform to the Geometry Group Standard
   format, and in particular should be of the form outputted by kbeqn or
   cfsa - that is, we can just use the output from kbeqn as the input to
   our program.

*/

#include <stdio.h>
#include <grpinput.h>
#include <ndimbdry.h>
#include <rewrite.h>

#define VERSION_NUM 1.3

#define	MAX_SIZE	200

#define USAGE		"Usage:  reduce [file]"
#define CANT_OPEN	"ERROR: Problem opening file!"
#define NO_GEN		"ERROR: One of the generators is not defined for this group!"
#define FORMAT		"Error:  in file format!"
#define GEN_ERR		"Error:  Something that was not a generator was encountered!"

/***********************************************************************/
/* De-comment the line below if the system you are using does not make */
/* use of a command line (such as the Mac).                            */
/*#define NO_CMD_LINE */
/***********************************************************************/

#define ERROR(A)        {fprintf(stderr, "%s\n", A); exit(-1);}

/* OTHER FUNCTIONS */
#ifndef NO_CMD_LINE
void	command_line(FILE **fp, int argc, char *argv[]);
#else
void    command_input(FILE **fp);
#endif

#ifndef NO_CMD_LINE /* version that does not use command line - such as Mac */
main(int argc, char *argv[])
#else
main()
#endif
{
	FILE			*fp = stdin;	  /* File pointer, default stdin */
	char			*test, *string, aw[MAX_SIZE];
	int				i, n, count;

	printf("\nreduce - version %1.2f\n\n", VERSION_NUM);

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

	/* read in the word */
	test = NEW(char, 1000);
	string = NEW(char, 1000);
	printf("Enter word seperating generators by spaces (then return):\n");
	gets(string);
	string[strlen(string)] = ' ';
	string[strlen(string) + 1] = '\0';
	for(i = 0, count = 0; sscanf(string + count, "%s%n", aw, &n) == 1;
										count += n, i++)
		if ((test[i] = genstr_to_genint(aw)) == 0)
			ERROR(NO_GEN);
	test[i] = '\0';

	printf("\nThe initial word: ");
	printword(test);
	printf("\n");

	printf("\nThe reduced word: ");
	reduce(test);
	printword(test);
	printf("\n");

	free(test);
	free(string);
	return 0;
}



#ifndef NO_CMD_LINE
void command_line(FILE **fp, int argc, char *argv[])
/* Checks the command line, and opens up a file if requested. */
{
	if (argc > 2)		/* Altered later if more arguments */
		ERROR(USAGE);
	if (argc == 1)
		return;
	if ((*fp = fopen(argv[1],"r")) == NULL)
		ERROR(CANT_OPEN);
	return;
}
#endif


#ifdef NO_CMD_LINE
void command_input(FILE **fp)
/* For use on systems that don't have a command line, such as the Mac.  Rather
   than enter command line arguments, you are prompted for them. */
{
    char s[256];

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


