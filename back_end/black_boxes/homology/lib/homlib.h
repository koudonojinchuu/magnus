/* homlib.h     - header file with macros that lib source files will use
   ===========================================================================
   Written by:  Jamie P. Curmi
   Date:        1992 - 1994
   Project:     Computational Tools for Homology of Groups
   For:         Dr J. R. J. Groves and Prof. C. Miller
                Department of Mathematics, University of Melbourne,
                Parkville, Victoria, Australia.
   ===========================================================================
*/

#ifndef H_HOMLIB
#define H_HOMLIB		/* Make sure it is only included once! */

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define ERROR(A)        {fprintf(stderr, A); exit(-1);}

#define ABS(A)          (((A) > 0) ? (A) : -(A))
#define MAX(A,B)        (((A) > (B)) ? (A) : (B))
#define MIN(A,B)        (((A) < (B)) ? (A) : (B))

/* Used to compare two real numbers to 0 */
#define	EQUAL(A,B)		(ABS((A)-(B)) < EPS)
#define	EPS				1e-5	/* Small number used in real comparison to 0 */

#endif /* H_HOMLIB */
