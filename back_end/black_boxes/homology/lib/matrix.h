/* matrix.h     - header file for applications using matrices for homology
   of groups.
   ===========================================================================
   Written by:  Jamie P. Curmi
   Date:        September-October 1992
   Project:     Computational Tools for Homology of Groups
   For:         Dr J. R. J. Groves and Prof. C. Miller
                Department of Mathematics, University of Melbourne,
                Parkville, Victoria, Australia.
   ===========================================================================
	Users will need the following global variables:
        extern int  Num_gens, Num_rules;
        extern char **Word1, **Word2;
	These should be created through the use of functions in grpinput.h

	HOW TO USE THESE FUNCTIONS:
	===========================

	1)	The MATRIX type is only a pointer to a pointer to double.  In order
		to create a matrix, you must allocate space to where the matrix
		points... for example:
			matrix = NEW(double *, rows);
		and then for each row, specify the number of columns.  Example:
			for (r=0; r < rows; r++)
				matrix[r] = ZERO_NEW(double, cols);
		This will initialise all elts of the matrix to 0.
		You can now access elts as if matrix is an array via matrix[row][col].
		This closely follows normal mathematics conventions.

	2)	"show_matrix" will display a matrix (with n  decimal place precision).

		"gaussian_elim" will perform gaussian elimination on a matrix,
		reducing it to row-echelon form.

		"count_non_zero_rows" counts the number of non-zero rows in the
		matrix.  If "gaussian_elim" is called previously, this will calculate
		the rank of the matrix.

	3)	"make_matrix_from_pair_boundaries" forms a matrix from the boundary
		calculations of critical pairs of the group.  Boundary calculations
		are based on those discussed in J.R.J.Groves "Rewriting Systems and
		Homology of Groups" (Uni of Melbourne, Dept of Mathematics, Research
		report 1990).  All actions are regarded as trivial.

		"make_matrix_from_rules" forms a matrix from the boundary calculations
		of rewriting rules for the group.  Boundary calculations are 
		specified as:
		For the rule:
			a1*a2*a3... > b1*b2*b3
		we have the boundary
			[a1] + [a2] + [a3] + ... - [b1] - [b2] - [b3] - ...

	4)	When you have finished with a MATRIX structure, you should free up
		space with a call to "delete_matrix".
*/

#ifndef H_MATRIX
#define H_MATRIX		/* Make sure it is only included once! */

#include "homcommon.h"
#include "ndimbdry.h"
#include "rewrite.h"

/* TYPEDEF'S */

#ifndef H_2DIMBDRY		/* don't redefine if already included 2dimbdry.h */
typedef struct boundary_triv {
	int						count;
	int						rule;
	struct boundary_triv	*next;
} BOUNDARY_TRIV;		/* Boundary with all actions trivial type */
#endif /* H_2DIMBDRY */

typedef double** MATRIX;

extern MATRIX			make_matrix_from_rules(void);
extern MATRIX			make_matrix_from_pair_boundaries(BOUNDARY_TRIV **boundary, int *height);
extern void				show_matrix(FILE *fp, MATRIX matrix, int width, int height, int num_per_line);
extern void				gaussian_elim(MATRIX a, int width, int height);
extern int				count_non_zero_rows(MATRIX a, int width, int height);
extern void				delete_matrix(MATRIX matrix, int height);
extern MATRIX			make_matrix_n(BOUNDARY **boundary, CRITICALS *lower_dim_tuples, int *num);
extern MATRIX			transpose(MATRIX m, int cols, int rows);

#endif /* H_MATRIX */
