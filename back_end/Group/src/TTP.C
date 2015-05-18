// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of class TTP (Tietze Transformation Package)
//
// Principal Authors: Dmitry Bormotov
//
// Status: provisional implementation
//
// Usage: 
//
// Revision History:
//


#include "TTP.h"
#include <unistd.h>


// -------------------------------- TTP ------------------------------------ //


bool TTP::run( ) const {
  
  File in;
  in << "n" << endl;

  SetOf<Word> S( theGroup.getRelators() );
  SetIterator<Word> I(S);

  for( int i = 0; !I.done(); I.next(), ++i ) {

    theGroup.printWord(in, I.value());
    if( i < S.cardinality() - 1 )
      in << ',';
    else
      in << '!' << endl << flush;
  }
  
  FILE* f = fopen( in.getFileName(), "r" );
  fseek(f,0,SEEK_END);
  long sz = ftell(f);
  fclose(f);
  if( sz > 254 ) return false; // the package doesn't accept long input
  
  Chars cmdline = Chars("cd ") + MagnusHome::magnusHome() 
    + "/back_end/black_boxes/TietzeTrek; ./TietzeTrek < " + in.getFileName() 
    + " > " + file.getFileName();

  //cout << "cmdline: " << cmdline << endl;

  system(cmdline);
  
  return false; // The program is not supposed to stop.
}


