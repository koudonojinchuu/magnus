/* matrix.c  - library code for matrix calculations in homology
   ===========================================================================
   Written by:	Jamie P. Curmi
   Date:		September-October 1992
   Project:		Computational Tools for Homology of Groups
   For:			Dr J. R. J. Groves and Prof. C. Miller
				Department of Mathematics, University of Melbourne,
				Parkville, Victoria, Australia.
   ===========================================================================
   The library exports those functions and macros defined in matrix.h
   The library imports 4 global variables used by most parts of the project:
		extern int	Num_rules, Num_gens;
		extern char	**Word1, **Word2;
   Their use is as specified in matrix.h
*/


#include <stdio.h>
#include "homlib.h"
#include "matrix.h"

extern int Num_rules, Num_gens;
extern char **Word1, **Word2;

static int cmp(void *a, void *b);
static int make_row(BOUNDARY *boundary, CRITICALS *lower_dim_tuples, double **row);
static void make_row_rec(BOUNDARY *boundary, CRITICALS *lower_dim_tuples, double *row);

MATRIX make_matrix_from_pair_boundaries(BOUNDARY_TRIV **boundary, int *height)
/* Forms a matrix from the boundaries of critical pairs (supplied by 
   "boundary".  "height" points to an integer representing the number of
   boundaries in "boundary".  Each boundary represents one row of the matrix,
   with all "zero" boundaries not included for efficiency.
   As an example, if a boundary calculation for a particular row was:
		[2] + [3] - [7]
   and there were 8 rewriting rules (=> 8 columns), the matrix row would be:
		0 1 1 0 0 0 -1 0
   The function returns the matrix formed, and via "height", the height of
   the matrix formed.
   The number of columns in the matrix is Num_rules;
   The number of rows in the matrix is *height (at completion);
*/
{
	MATRIX			matrix; 
	int				i, row, newheight = *height;
	BOUNDARY_TRIV	*p;

	for (i=0; i < *height; i++)
		if (boundary[i]->next == NULL)	/* "zero" boundary, so ignore */
			--newheight;
	/* Create matrix rows of appropriate height */
	matrix = NEW(double *, newheight);

	for (row = 0, i = 0; i < *height; i++)
		if (boundary[i]->next != NULL) {
			/* Create columns, and initiliase matrix elts to zero */
			matrix[row] = ZERO_NEW(double, Num_rules);
			for (p=boundary[i]->next; p != NULL; p = p->next)
				/* matrix elts go from 0 to (height-1), so we use (rule-1) */
				matrix[row][(p->rule) - 1] = p->count;
			row++;
		}

	*height = newheight;
	return matrix;
}

MATRIX make_matrix_from_rules(void)
/* Forms a matrix from the boundaries of rewriting rules.  Each boundary
   represents one row of the matrix.  The boundaries of a rule are specified
   as thus:
	a1*a2*a3... > b1*b2*b3...  has boundary
		[a1] + [a2] + [a3] + ... - [b1] - [b2] - [b3] - ...
   The rows of the matrix are taken from this boundary calculation...for
   example:  If the boundary of one rule was (after simplification)
      [2] + [3] - [7]
   and there were 8 generators (=> 8 columns), the matrix row would be:
        0 1 1 0 0 0 -1 0
   The function returns the matrix formed.
   The number of columns in the matrix is Num_gens;
   The number of rows in the matrix is Num_rules;
*/
{
	MATRIX	matrix; 
	int		row;
	char	*p;

	/* Create matrix rows of appropriate height */
	matrix = NEW(double *, Num_rules);
	for (row = 0; row < Num_rules; row++) {
			/* Create columns, and initiliase matrix elts to zero */
			matrix[row] = (double *) calloc(Num_gens, sizeof(double));
			for (p = Word1[row+1]; *p != '\0'; p++)
				/* additions for the left hand side of a rule */
				matrix[row][(int)*p - 1] += 1;
			for (p = Word2[row+1]; *p != '\0'; p++)
				/* subtractions for the right hand side of a rule */
				matrix[row][(int)*p - 1] -= 1;
	}
	return matrix;
}

void show_matrix(FILE *fp, MATRIX matrix, int width, int height, int num_per_line)
/* Displays a matrix of specified width and height, with 0 decimal place
   precision).  Starts a new line at the end of each row, and after every
   'num_per_line' elements in each row.
*/
{
	int		i, j;

	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i++) {
			fprintf(fp, "%.0f", matrix[j][i]);
			if (i != width - 1 || j != height - 1)
				fprintf(fp, ", ", matrix[j][i]);
			if (i%num_per_line == num_per_line - 1)
				fprintf(fp, "\n");
		}
		if (i%num_per_line != 0)	/* In case we just did a newline. */
			fprintf(fp, "\n");
	}
}

void gaussian_elim(MATRIX a, int width, int height)
/* Performs gaussian elimination on matrix 'a' of specified 'width' and
   'height' - reducing to row-echelon form.
   NOTE:  The matrix contents is altered!
*/
{
	int		i, j, k, max, row;
	double	t;

	if (height <= 0 || width <= 0)	/* No need to do anything here!! */
		return;

	for (row = i = 0; i < width; i++) {	/* For each column...start with row 0 */
		/* Find max pivot elt (a[max][i]) in all rows */
		max = row;
		for (j = row + 1; j < height; j++)
			if (ABS(a[j][i]) > ABS(a[max][i]))
				max = j;
		if (EQUAL(a[max][i], 0))	/* If ~0, then move to next column */
			continue;
		/* Swap elts so pivot row is row below 'row' */
		for (k = i; k < width; k++) {
			t = a[row][k];
			a[row][k] = a[max][k];
			a[max][k] = t;
		}
		/* ...now do the row operations */
		for (j = row + 1; j < height; j++)
			for (k = width-1; k >= i; k--)
				a[j][k] -= a[row][k]*a[j][i]/a[row][i];
		row++;	/* look at rectanglular matrix starting at next row down */
		if (row >= height)
			break;
	}
}


int count_non_zero_rows(MATRIX a, int width, int height)
/* Returns a count of the number of non zero rows in a matrix 'a' of specified
   'width' and 'height'.
*/
{
	int	row, column;

	if (height <= 0 || width <= 0) /* Return 0 if no rows or columns */
		return 0;

	for (row = 0, column = 0; column < width && row < height; column++)
		if (!EQUAL(a[row][column], 0)) /* ~0 as we are using floating points! */
			row++;
		
	return row;
}

void delete_matrix(MATRIX matrix, int height)
/* Free up memory for a 'matrix' of specified 'height'.
*/
{
  int i;
//	for (; height > 0; height--)
//
//@rk bad code		free(*matrix);
        for (i = 0; i < height; i++)
	  free(matrix[i]);
	free(matrix);
}

MATRIX make_matrix_n(BOUNDARY **boundary, CRITICALS *lower_dim_tuples, int *num)
/* Calculates a matrix from an array of *num boundaries.  lower_dim_tuples
   contains the tuples of which the boundary elts are multiples of.  The
   coefficients form the matrix elts.  At completion, *num contains the
   number of actual rows in the matrix.  Returns the formed matrix.  */

{
	MATRIX		matrix;
	int			i, height = *num;

	matrix = NEW(double *, *num);

	for (i=0; i < height; i++) {
		make_row(boundary[i], lower_dim_tuples, &matrix[i]);
	}
	return matrix;
}
		

static
int make_row(BOUNDARY *boundary, CRITICALS *lower_dim_tuples, double **row)
/* Calcualtes a row from the boundary.  lower_dim_tuples contains the tuples
   of which the boundary elts are multiples of.  The coefficients form the
   matrix elts.  Returns _TRUE if the boundary was non zero, _FALSE otherwise. */
{

	boundary = boundary->left;
	*row = (double *) calloc(lower_dim_tuples->num, sizeof(double));

	make_row_rec(boundary, lower_dim_tuples, *row);

	return _TRUE;
}

static
void make_row_rec(BOUNDARY *boundary, CRITICALS *lower_dim_tuples, double *row)
/* Recursively calulates a row from a boundary tree */
{
	CRITICAL_INFO	**match;
	int		pos;

	if (boundary == NULL)
		return;
	make_row_rec(boundary->left, lower_dim_tuples, row);
	make_row_rec(boundary->right, lower_dim_tuples, row);

	/* Find position in tuples of boundary word */
	/*match = (CRITICAL_INFO **) bsearch(boundary->word,
						(char *) lower_dim_tuples->info,
						lower_dim_tuples->num, sizeof(CRITICAL_INFO **), cmp);*/
	match = (CRITICAL_INFO **) bsearch(boundary,
						(char *) lower_dim_tuples->info,
						lower_dim_tuples->num, sizeof(CRITICAL_INFO **), cmp);
	if (match == NULL)
		ERROR("make_row_rec: I could not find that word");
	pos = (int) (match - lower_dim_tuples->info);
	row[pos] = boundary->coeff->count;
}

static
int cmp(void *a, void *b)
/* Comparison function used in bsearch */
{
			int scomp;
			CUBE_INFO	*p, *pp;

			/*return strcmp((char *) a, (*((CRITICAL_INFO **) b))->word);*/

			scomp = strcmp(((BOUNDARY *) a)->word,
						(*((CRITICAL_INFO **) b))->word);
			if (scomp == 0) {
				for (p = (*((CRITICAL_INFO **) b))->vert,
						pp = ((BOUNDARY *) a)->vertices;
						p != NULL; p = p->next, pp = pp->next) {
					if (pp->pos < p->pos)
						return -1;
					if (pp->pos > p->pos)
						return 1;
				}
				return 0;
			}
			return scomp;
}


MATRIX transpose(MATRIX m, int cols, int rows)
/* Creates a matrix being the transpose of m.  m is not destroyed, and is
   left to the user.
*/
{
	int			i, j;
	MATRIX		t = NEW(double *, cols);

	for (i=0; i < cols; i++)
		t[i] = NEW(double, rows);
	for (i=0; i < rows; i++)
		for (j=0; j < cols; j++)
			t[j][i] = m[i][j];
	return t;
}

