/* ****  chom_stats.c  ****

   Written by:  Jamie P. Curmi
   Date:		1994
   For:			Dr J. R. J. Groves and Prof. C. Miller
				Dept of Mathematics, Uni of Melb, Parkville, Australia.

   =========================================================================
   Creates a script for use in "magma" to calculate Homology of a group
   for different dimensions. The algorithm creates the matrix for calculating
   Homology dimension n, given a group and its rewriting system.  The boundary
   calculations used are based on those given by J. R. J. Groves
   "Rewriting Systems and Homolgy of Groups" in his research report from
   The University of Melbourne, Department of Mathematics, 1990 and original
   algorithms by J.R.J.Groves 1993-1994.

   This version includes STATISTICAL INFORMATION.  The statistical
   information is placed in the file inputfile.stats in the current
   directory.
   =========================================================================

   We use the executable as follows:

				chom_stats [-v] start end inputfile [outputfile]

   '-v' is optional.  This will include extra information, such as
			the number of criticals in each dimension, and the invariants
			of the matrices used.
   'start' is the starting dimension whose Matrix we will produce (> 1).
   'end' is the end dimension whose Matrix we will produce.
   'inputfile' is a file that conforms to the Geometry Group Standard
			format, and in particular should be of the form outputted by
			"kbeqn" - that is, we can just use the output from "kbeqn" as
			the input to our program - or "cfsa".
   'outputfile' is optional.  If no filename is given, stdout is used.

   As an example:
			chom_stats 2 4 groupA.kbeqn grpA.output
   would produce homology calculations for dimensions 2, 3 and 4, with the
   output being placed in the file "grpA.output", and statistical info
   being placed in the file "groupA.kbeqn.stats".
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <grpinput.h>
#include <ndimbdry.h>
#include <rewrite.h>
#include <matrix.h>

/***********************************************************************/
/* De-comment the line below if the system you are using does not make */
/* use of a command line (such as the Mac).                            */
/*#define NO_CMD_LINE*/
/***********************************************************************/

#define USAGE		"Usage:  chom_stats [-v] start end inputfile [outputfile]\n        where end >= start > 1.  Statistics are outputted to 'inputfile.stats'"
#define CANT_OPEN	"Error:  Problem opening file!"
#define STATS_PROB	"Error:  Problem creating file for statistics!"

#define ERROR(X)	{fprintf(stderr, "%s\n", X); exit(-1);}

/* OTHER FUNCTIONS */
#ifndef NO_CMD_LINE
void command_line(FILE **fin, FILE **fout, int argc, char *argv[],
			int *start, int *end, int *verbose, char *statsfilename);
#else
void command_input(FILE **fin, FILE **fout, int *start, int *end,
										int *verbose, char *statsfilename);
#endif

#ifndef NO_CMD_LINE /* version that does not use command line - such as Mac */
main(int argc, char *argv[])
#else
main()
#endif
{
	FILE			*fin, *fout  = stdout;	  /* File pointer, default stdout */
	FILE			*fstats;
	CRITICALS		*tuples, *lower_dim_tuples;
	int				i, j;
	BOUNDARY		**boundary;
	CUBE			*cube;
	int				n, num_rows, num_cols, start, end, verbose;
	MATRIX			matrix, tmatrix;
	int				sizelist[100]; /* Max length of size list */
	CUBE_INFO		*p;
	char			*c, statsfilename[256];
	time_t			time1, time2;


#ifndef NO_CMD_LINE
	/* Check the command line and open file if required */
	command_line(&fin, &fout, argc, argv, &start, &end, &verbose,
												statsfilename);
#else
    /* Read command info from standard input */
    command_input(&fin, &fout, &start, &end, &verbose, statsfilename);
#endif

	/* Read in info from the file, setting up Fsa and so on... */
	read_file_info(fin);
	fclose(fin);	/* No more reading required */

	fprintf(fout, "print \"Homology Calculation Results\";\n");
	fprintf(fout, "print \"\";\n");
	fprintf(fout, "print \"The group '%s' has the following generators:\";\n",
							statsfilename);
	fprintf(fout, "print \"        ");

	strcat(statsfilename, ".stats");
	if ((fstats = fopen(statsfilename,"w")) == NULL)
		ERROR(STATS_PROB);
	fprintf(fstats, "- Statistics Report -\n");

	for (i = 1; i <= Num_gens; i++)
		fprintf(fout, "%s  ", Alphabet[i]);
	fprintf(fout, "\";\n");

	fprintf(fout, "print \"and the following rewriting rules:\";\n");
	for (i = 1; i <= Num_rules; i++) {
		fprintf(fout, "print \"        ");
		for(c = Word1[i]; *c != '\0'; c++)
			fprintf(fout, "%s", Alphabet[(int) *c]);
		fprintf(fout, " > ");
		for(c = Word2[i]; *c != '\0'; c++)
			fprintf(fout, "%s", Alphabet[(int) *c]);
		if (c==Word2[i])
			fprintf(fout, "1");
		fprintf(fout, "\";\n");
	}
	fprintf(fout, "print \"\";\n");

	fprintf(fout, "n:=%d;\n", start);
	fprintf(fout, "ranklist:=[];\nZ:=Integers();\n");

	if (start != 1) {
		/* tuples is always used again in the next calculation as tuples
		   become lower_dim_tuples.  For this reason, the first tuples
		   calculation is performed outside the main loop.  */
		tuples = find_n_criticals(start - 1);
		/*  remove_inverse_criticals(&tuples);  We are ignoring inverse tuples */
		sizelist[0] = tuples->num;
	} else
		sizelist[0] = Num_gens;


	for (j = 1, n = start; n <= end; j++, n++) {

		if (n == 1) {
			matrix = make_matrix_from_rules();
			num_cols = Num_gens;
			num_rows = Num_rules;
			tuples = find_n_criticals(1);
		} else {
			lower_dim_tuples = tuples;

			tuples = find_n_criticals(n);
			/* remove_inverse_criticals(&tuples);  ignoring inverse tuples */

			/* Calc boundaries and store in an array */
			boundary = NEW(BOUNDARY *, tuples->num);

			fprintf(fstats, "\n** Dimension: %d **\n", n);
			for (i = 0; i < tuples->num; i++) {
				fprintf(fstats, "\nCritical %d: ", i);
				display_critical(fstats, tuples->info[i]);
				time1 = time(NULL);
				boundary[i] = calc_bound(cube =
						create_n_cube(tuples->info[i]->word,
						tuples->info[i]->vert, n), TRIVIAL | RETAIN_INV_TUPLES);
						/* We are ignoring inverse tuples */
				time2 = time(NULL);
				{
				int top_nd, sub_nd, top_d, sub_d, max_level;
				get_stats(&top_nd, &sub_nd, &top_d, &sub_d, &max_level);
				fprintf(fstats, "\tNumber of non-degenerate top-cubes = %d\n", top_nd);
				fprintf(fstats, "\tNumber of non-degenerate sub-cubes = %d\n", sub_nd);
				fprintf(fstats, "\tNumber of degenerate top-cubes = %d\n", top_d);
				fprintf(fstats, "\tNumber of degenerate sub-cubes = %d\n", sub_d);
				fprintf(fstats, "\tMaximum level of recursion achieved = %d\n", max_level);
/*				fprintf(fstats, "\tTime Taken (nearest second) = %.0f\n", difftime(time2, time1));*/
				}
				fprintf(fstats, "\tCalculated Boundary:\n");
				display_boundary(fstats, boundary[i]);
				delete_cube(cube);
			}

			num_rows = tuples->num;
			/* Create the matrix */
			matrix = make_matrix_n(boundary, lower_dim_tuples, &num_rows);
			num_cols = lower_dim_tuples->num;
		}
		tmatrix = transpose(matrix, num_cols, num_rows);

		/* Display....for now.  Note num_cols and Num_rows are reversed. */
		fprintf(fout, "R:=RMatrixSpace(Z,%d,%d);\n", num_cols, num_rows);
		fprintf(fout, "t:=R![\n");
		show_matrix(fout, tmatrix, num_rows, num_cols, 20);
		fprintf(fout, "];\n");
		fprintf(fout, "ranklist[%d]:=Insert(ElementaryDivisors(t),1,Rank(t));\n", j);
		sizelist[j] = num_rows;

		/* Clean up */
		if (n != 1) {
			for (i=0; i < tuples->num; i++)
				delete_boundary(boundary[i]);
			free(boundary);
				delete_criticals(lower_dim_tuples);
		}
	
		delete_matrix(matrix, num_rows);
		delete_matrix(tmatrix, num_cols);

	}
	delete_criticals(tuples);
	sizelist[j] = 0;
	fprintf(fout, "sizelist:=[%d", sizelist[0]);
	for (i=1; sizelist[i] != 0; i++)
		fprintf(fout, ",%d", sizelist[i]);
	fprintf(fout, "];\n");
	fprintf(fout, "for i in [1..#sizelist-2] do\n");
	fprintf(fout, "\tprint \"Calculating Homology for dimension\", i+n,\":\";\n");
	fprintf(fout, "\tprint \"        The torsion-free rank is\",\n");
	fprintf(fout, "\t\tsizelist[i+1]-ranklist[i,1]-ranklist[i+1,1];\n");
	fprintf(fout, "\tif (IsEmpty(Remove(ranklist[i+1],1))) then\n");
	fprintf(fout, "\t\tprint \"        The homology group is torsion-free. \";\n");
	fprintf(fout, "\telse\n");
	fprintf(fout, "\t\tprint \"        The torsion invariants are\",\n");
	fprintf(fout, "\t\tRemove(ranklist[i+1],1);\n");
	fprintf(fout, "\tend if;\n");
	fprintf(fout, "\tprint \" \";\n");
	fprintf(fout, "end for;\n");

	if (verbose) {
		fprintf(fout, "print \"---------------------------------------------------------------------------\";\n");

		fprintf(fout, "print \"The number of critical tuples in each dimension:\";\n");
		fprintf(fout, "for i in [0..#sizelist-2] do\n");
		fprintf(fout, "\tprint \"        Dimension\", i+n,\":\",\n");
		fprintf(fout, "\t\tsizelist[i+2];\n");
		fprintf(fout, "end for;\n");

		fprintf(fout, "print \" \";\n");
		fprintf(fout, "print \"The invariants of the matrices in each dimension:\";\n");
		fprintf(fout, "for i in [0..#sizelist-2] do\n");
		fprintf(fout, "\tprint \"        Dimension\", i+n,\":\",\n");
		fprintf(fout, "\t\tRemove(ranklist[i+1],1);\n");
		fprintf(fout, "end for;\n");
		fprintf(fout, "print \"---------------------------------------------------------------------------\";\n");
	}

	fclose(fout);
	fclose(fstats);
	return 0;
}


#ifndef NO_CMD_LINE
void command_line(FILE **fin, FILE **fout, int argc, char *argv[],
				int *start, int *end, int *verbose, char *statsfilename)
/* Checks the command line, and opens up a file if requested.
   Further work on the program will undoubtably lead to more command line
   arguments, which can easily be added in here.  */
{
	int		count, count_arg;

	*verbose = 0;

	if (argc > 6 || argc < 4)		/* Altered later if more arguments */
		ERROR(USAGE);
	for (count_arg = count = 1; count_arg < argc; count++, count_arg++)
		switch (count) {
			case 1:
				if (*argv[count_arg] == '-' && *(argv[count_arg]+1) == 'v')
					*verbose = 1;
				else
					count_arg--;
				break;
			case 2:
				if ((*start = atoi(argv[count_arg])) < 2)
					ERROR(USAGE);
				(*start)--;
				break;
			case 3:
				if ((*end = atoi(argv[count_arg])) < 2 || *end < *start)
					ERROR(USAGE);
				break;
			case 4:
				if ((*fin = fopen(argv[count_arg],"r")) == NULL)
					ERROR(CANT_OPEN);
				strcpy(statsfilename, argv[count_arg]);
				break;
			case 5:
				if ((*fout = fopen(argv[count_arg],"w")) == NULL)
					ERROR(CANT_OPEN);
				break;
		}
}
#endif


#ifdef NO_CMD_LINE
/* For use on systems that don't have a command line, such as the Mac.  Rather
   than enter command line arguments, you are prompted for them. */
void command_input(FILE **fin, FILE **fout, int *start, int *end,
										int *verbose, char *statsfilename)
{
    char s[256];

	do {
        printf("Do you want to use verbose mode?\n");
                printf("(yes or no - press enter for default (no)): ");
        *s = '\0';
                *verbose = 0;
        if (*gets(s) == 0)
                        break;
        if (*s == 'y' || *s == 'Y') {
                        *verbose = 1;
                        break;
                }
        else if (*s == 'n' || *s == 'N')
                        break;
                else
                        printf("** Please enter yes or no\n\n");
    } while (1);

    do {
        printf("Enter the starting dimension (> 1): ");
        *s = '\0';
        gets(s);
        if (*s == '\0' || (*start = atoi(s)) <= 1) {
            printf("** Please enter an integer > 1\n\n");
            *start = 0;
        }
    } while (*start == 0);

    do {
        printf("Enter the ending dimension (>= start): ");
        *s = '\0';
        gets(s);
        if (*s == '\0' || (*end = atoi(s)) < *start) {
            printf("** Please enter an integer >= %d\n\n", *start);
            *end = 0;
        }
    } while (*end == 0);

    do {
		printf("Enter the name of the input file containing the group's complete\nrewriting system: ");
		*s = '\0';
		if (*gets(s) == 0)
			continue;
		if ((*fin = fopen(s, "r")) == NULL) {
			printf("** An error occured opening that file (does it exist?)\n\n");
			*s = '\0';
			continue;
			}
		strcpy(statsfilename, s);
	} while (*s == '\0');

    do {
		printf("Enter the name of the output file containing the group's complete\nrewriting system ((Press 'Enter' for default (standard output)): ");
		*fout = stdout;
		*s = '\0';
		if (*gets(s) == 0)
			break;
		if ((*fout = fopen(s, "w")) == NULL) {
			printf("** An error occured opening that file!\n\n");
			*s = '\0';
			}
	} while (*s == '\0');
}
#endif

