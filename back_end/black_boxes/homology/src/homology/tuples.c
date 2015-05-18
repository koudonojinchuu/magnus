/* ****  tuples.c  ****

   Written by:  Jamie P. Curmi
   Date:		Novemeber 1993
   For:			Dr J. R. J. Groves and Prof. C. Miller
				Dept of Mathematics, Uni of Melb, Parkville, Australia.

   =========================================================================
   Lists all critical n-tuples for a given group.
   =========================================================================

   As the program stands, we use the executable as follows:

						tuples n [file.kbeqn]

   Input is expected to be either from a file, or if no filename is given,
   from standard input.  It should conform to the Geometry Group Standard
   format, and in particular should be of the form outputted by kbeqn - that
   is, we can just use the output from kbeqn as the input to our program.

*/

#include <stdio.h>
#include <stdlib.h>
#include <grpinput.h>
#include <rewrite.h>

/***********************************************************************/
/* De-comment the line below if the system you are using does not make */
/* use of a command line (such as the Mac).                            */
/*#define NO_CMD_LINE*/
/***********************************************************************/

#define VERSION_NUM 1.1
#define USAGE		"Usage:  tuples n [file]\n        where n > 1"
#define CANT_OPEN	"ERROR:  Problem opening file!"
#define FORMAT      "Error:  in file format!"
#define GEN_ERR		"Error:  Something that was not a generator was encountered!"

#define ERROR(A)	{fprintf(stderr, "%s\n", A); exit(-1);}

/* OTHER FUNCTIONS */
#ifndef NO_CMD_LINE
void	command_line(FILE **fp, int argc, char *argv[], int *n);
#else
void    command_input(FILE **fp, int *n);
#endif

#ifndef NO_CMD_LINE /* version that does not use command line - such as Mac */
main(int argc, char *argv[])
#else
main()
#endif
{
	FILE			*fp = stdin;	  /* File pointer, default stdin */
	CRITICALS		*tuples;
	int				i;
	int				n;
	CUBE_INFO		*p;

	printf("\ntuples - version %1.2f\n\n", VERSION_NUM);

#ifndef NO_CMD_LINE
	/* Check the command line and open file if required */
	command_line(&fp,argc, argv, &n);
#else
	/* Read command info from standard input */
	command_input(&fp, &n);
#endif

	/* Read in info from the file, setting up Fsa and so on... */
	switch (read_file_info(fp)) {
		case FORMAT_ERROR:
			ERROR(FORMAT);
		case NOT_A_GEN:
			ERROR(GEN_ERR);
	}
	fclose(fp);	/* No more reading required */

	tuples = find_n_criticals(n);
	printf("Number of Critical %d-tuples = %d\n\n", n, tuples->num);

	for (i=0; i < tuples->num; i++) {
		printword(stdout, tuples->info[i]->word);
		printf("\t");
		for (p = tuples->info[i]->vert; p != NULL; p = p->next)
			printf("[%d,%d] ", p->pos, p->len);
		printf("\n");
	}
	/* Clean up */
	delete_criticals(tuples);
	return 0;
}


#ifndef NO_CMD_LINE
void command_line(FILE **fp, int argc, char *argv[], int *n)
/* Checks the command line, and opens up a file if requested.
   Further work on the program will undoubtably lead to more command line
   arguments, which can easily be added in here.  */
{
	int		count;

	if (argc > 3)		/* Altered later if more arguments */
		ERROR(USAGE);
	for (count = 1; count < argc; count++)
		switch (count) {
			case 1:
				if ((*n = atoi(argv[1])) < 2)
					ERROR(USAGE);
				break;
			case (2):
				if ((*fp = fopen(argv[2],"r")) == NULL)
					ERROR(CANT_OPEN);
				break;
		}
}
#endif


#ifdef NO_CMD_LINE
void command_input(FILE **fp, int *n)
/* For use on systems that don't have a command line, such as the Mac.  Rather
   than enter command line arguments, you are prompted for them. */
{
    char s[256];

    do {
        printf("Enter the Dimension (> 1): ");
        *s = '\0';
        gets(s);
        if (*s == '\0' || (*n = atoi(s)) <= 1) {
            printf("** Please enter an integer > 1\n\n");
            *n = 0;
        }
    } while (*n == 0);

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
