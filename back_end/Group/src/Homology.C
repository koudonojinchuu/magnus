// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of class Homology
//
// Principal Author: Roger Needham
//
// Status: Under development
//
// Revision History:
//


#include "Homology.h"
#include "MagnusHome.h"


// #define debug_Homology


extern "C" {
  char* tempnam(const char*, const char *); 
  // Lib functions SunOS doesn't declare.
}


Homology::Homology(const KBMachine M, int start_dim, int end_dim)
{
  #if SAFETY > 0
  if ( start_dim < 2 || end_dim < start_dim )
	 error("bad dimension parameter in Homology::Homology");
  #endif
  
  // Put the KBMachine in a file for chom
  char* tmpStr = tempnam(MagnusTmp::magnusTmp(), "homology");
  strcpy(tempFileName,tmpStr);
  free(tmpStr);

  ofstream ofstr(tempFileName);
  M.oldFormatPrintOn(ofstr);

#ifdef debug_Homology
  cerr << "\n\nThe KBMachine:" << endl;
  M.oldFormatPrintOn( cerr );
  cerr << endl << endl;
#endif

  // Make the BlackBox

  //@njz:
  //  ostrstream o;
  std::ostrstream o;
  //

  o << MagnusHome::magnusHome()
	 << "/back_end/black_boxes/homology/bin/chom "
	 << start_dim << ' ' << end_dim << ' ' << tempFileName
	 << ends;
  chom = new BlackBox( o.str() );

  o.freeze(0);
  chom->start();  //@rn Check status

  myState = READING;
  dimensionToDo = start_dim;
  lastDimension = end_dim;

  SNF = NULL;
  booted = false;
}


Homology::~Homology( )
{
  unlink(tempFileName);
  //  delete tempFileName; //@rn Is this ok? tempnam uses malloc.
  delete chom;
  delete SNF;
}


bool Homology::workOnNextGroup( )
{
  if ( myState == READING ) {

	 int rows;
	 Integer** matrix = readMatrix(rows, columns);
	 // Synchronous read.
	 if ( SNF != NULL ) delete SNF;
	 SNF = new SmithNormalForm(matrix, rows, columns);
	 // This gives up ownership of matrix.
	 myState = REDUCING;

  } else if ( myState == REDUCING ) {

	 // In the case of the first `bootstrap' matrix, we just need
	 // its torsion free rank, but we do the whole thing.

	 if ( SNF->continueComputation() ) {
		// Reduction is done.

		if ( booted ) {

		  myState = GOT_ONE;
		  return true;

		} else {  //@rn not updated properly in continuous run!

		  previousTorsionFreeRank = SNF->getTorsionFreeRank();
		  //@rn Should call this previousMatrixRank
		  booted = true;
		  myState = READING;

		}
	 }
  }

  return false;
  // Default value, overridden only in case REDUCING.
}


int Homology::getTorsionFreeRank( ) const
{
#if SAFETY > 0
  if ( myState != GOT_ONE )
	 error("called Homology::getTorsionFreeRank( ) prematurely");
#endif

#ifdef debug_Homology
  cerr << "\n\nprev rank = " << previousTorsionFreeRank
		 << "\ncurr rank = " << SNF->getTorsionFreeRank()
		 << "\ncols = " << columns << endl << endl << flush;
#endif

  return ( previousTorsionFreeRank + SNF->getTorsionFreeRank() - columns );
}


VectorOf<Integer> Homology::getTorsionInvariants( ) const
{
#if SAFETY > 0
  if ( myState != GOT_ONE )
	 error("called Homology::getTorsionInvariants( ) prematurely");
#endif

  return SNF->getTorsionInvariants();
}


Integer** Homology::readMatrix(int& rows, int& cols) const
{
  const int bufSize = 1024;
  char buffer[bufSize];
  // chom is polite about writing short lines, so this buffer is big enough.

  istream& istr = chom->fromCommand();
  
  while ( 1 ) { // We return out of this

	 // Find the part that gives the matrix dimensions.
	 // It should look like: "R:=RMatrixSpace(Z,12,27);"
	 
	 istr.getline(buffer, bufSize);

#ifdef debug_Homology
	 cerr << buffer << endl;
#endif

	 if ( sscanf(buffer, "R:=RMatrixSpace(Z,%d,%d);", &rows, &cols) == 2 ) {

		// Now look for the line "t:=R!["; the matrix follows it.

		do {
		  istr.getline(buffer, bufSize);

		} while ( strcmp( buffer, "t:=R![" ) );

		// Finally, get the actual matrix.

		char comma;
		//@njz
		//		Integer** matrix = new (Integer*)[rows];
		Integer** matrix = new Integer*[rows];
		//
		for( int i = 0; i < rows; ++i ) {
		  matrix[i] = new Integer[cols];
		  for( int j = 0; j < cols; ++j ) {
			 istr >> ws >> matrix[i][j] >> ws >> comma;
		  }
		}


#ifdef debug_Homology
		cerr << "\n\nGot matrix with " << rows << " rows, "
			  << cols << " cols:\n";
		for( int idx_i = 0; idx_i < rows; ++idx_i ) {
		  cerr << endl;
		  for( int idx_j = 0; idx_j < cols; ++idx_j )
			 cerr << matrix[idx_i][idx_j] << ' ';
		  cerr << endl << endl << flush;
		}
#endif

		return matrix;
	 }
  }
}


