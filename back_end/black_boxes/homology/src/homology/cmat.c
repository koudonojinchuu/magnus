/* ****  cmat.c  ****

   Written by:  Jamie P. Curmi
   Date:		November 1993
   For:			Dr J. R. J. Groves and Prof. C. Miller
				Dept of Mathematics, Uni of Melb, Parkville, Australia.

   =========================================================================
   Creates the matrix for calculating Homology dimension n, given a group and
   its rewriting system.  The boundary calculations used are based on those
   given by J.R.J.Groves "Rewriting Systems and Homolgy of Groups" in his
   research report from The University of Melbourne, Department of
   Mathematics, 1990 and original algorithms by J. R. J. Groves 1993-1994.
   =========================================================================

   We use the executable as follows:

						cmat n [inputfile]

						where n > 1

   Input is expected to be either from a file, or if no filename is given,
   from standard input.  It should conform to the Geometry Group Standard
   format, and in particular should be of the form outputted by "kbeqn" - that
   is, we can just use the output from "kbeqn" as the input to our program -
   or "cfsa".

   The program does the following:
   + Finds all critical n-tuples for the system.
   + Calculates the boundary for each critical n-tuple, regarding all actions
     as trivial.
   + Creates a matrix from these boundaries.
   + Displays this matrix.
*/

#include <stdio.h>
#include <stdlib.h>
#include <grpinput.h>
#include <ndimbdry.h>
#include <rewrite.h>
#include <matrix.h>

/***********************************************************************/
/* De-comment the line below if the system you are using does not make */
/* use of a command line (such as the Mac).                            */
/*#define NO_CMD_LINE */
/***********************************************************************/

#define VERSION_NUM 1.1
#define USAGE		"Usage:  cmat n [file]\n        where n > 1\n"
#define CANT_OPEN	"Error:  Problem opening file!"
#define FORMAT		"Error:  in file format!"
#define GEN_ERR		"Error:  Something that was not a generator was encountered!"

#define ERROR(A)        {fprintf(stderr, "%s\n", A); exit(-1);}

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
	CRITICALS		*tuples, *lower_dim_tuples;
	int				i, temp;
	BOUNDARY		**boundary;
	CUBE			*cube;
	int				n, num_rows, num_cols;
	MATRIX			matrix, tmatrix;

	printf("\ncmat - version %1.2f\n\n", VERSION_NUM);

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
	/* remove_inverse_criticals(&tuples);  We are ignoring inverse tuples */

	/* Calc boundaries and store in an array */
	boundary = NEW(BOUNDARY *, tuples->num);

	for (i = 0; i < tuples->num; i++) {
		boundary[i] = calc_bound(cube =
					create_n_cube(tuples->info[i]->word,
					tuples->info[i]->vert, n), TRIVIAL | RETAIN_INV_TUPLES);
					/* We are ignoring inverse tuples */
		delete_cube(cube);
	}

	lower_dim_tuples = find_n_criticals(n-1);
	/* remove_inverse_criticals(&lower_dim_tuples);  Ignoring inverse tuples */

	num_rows = tuples->num;
	/* Create the matrix */
	matrix = make_matrix_n(boundary, lower_dim_tuples, &num_rows);
	num_cols = lower_dim_tuples->num;

	/* It is often useful to have rows and columns the other way */
	/* Get transpose, and swap rows and cols */
	tmatrix = transpose(matrix, num_cols, num_rows);
	temp = num_rows;
	num_rows = num_cols;
	num_cols = temp;

	/* Display....for now */
	printf("\nMatrix: rows = %d, cols = %d\n\n", num_rows, num_cols);
	show_matrix(stdout, tmatrix, num_cols, num_rows, 20);

	/*printf("\nMatrix after Gaussian Elimination...\n");
	gaussian_elim(matrix, num_cols, num_rows);
	show_matrix(stdout, matrix, num_cols, num_rows);

	rank_criticals_n = count_non_zero_rows(matrix, num_cols, num_rows);*/

	/* Clean up */
	for (i=0; i < tuples->num - Num_gens; i++)
	delete_boundary(boundary[i]);
	free(boundary);
	delete_criticals(tuples);
	delete_criticals(lower_dim_tuples);
	
	delete_matrix(matrix, num_cols);
	delete_matrix(tmatrix, num_rows);

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

