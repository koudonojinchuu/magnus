// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of class File.
//
// Principal Author: Dmitry Bormotov
//
// Status: Under development.
//
// Revision History:
// * 11/18/97  @am  implemented a method to keep
//   information about colors. Colors are kept in 
//   <filename>.color file. This file consists of records of
//   type: "#<file_pos>%<colorName>@", where file_pos - numeric
//   position in the file in which colors were changed, colorName -
//   string, representing color in rgb format(see below).
//   
//
// Special Notes:
//  
// * This is a simple deriviation from fstream, specially designed 
//   for reporting large amount of data ( f.e. details of algorithms
//   work ) to Session Manager and Front End to make it available for
//   the end user.
//


#ifndef _File_h_
#define _File_h_


#include <stdio.h>
#include <fstream.h>
#include "MagnusHome.h"

// Colors have to be in form RGB,
// where R,G or B represents one hexademical digit of red,
// green or blue intensity, respectively.

const Chars mainColor = "000";
const Chars titleColor = "aaa";

// -------------------------------- File ----------------------------------- //


struct File : public fstream
{
  
  File( )  
  {
    cerr << "debug point 6-1" << endl;
    char s[L_tmpnam];
    char* tmpFileName = tempnam(MagnusTmp::magnusTmp(),NULL);
    theFileName = tmpFileName;
    free(tmpFileName);
    open(theFileName, ios::app|ios::out);
    cerr << "debug point 6-4" << endl;
    colorFile.open(theFileName+Chars(".color"),ios::app|ios::out);
    cerr << "debug point 6-3" << endl;
#if SAFETY > 0
    if( !(*this) )
      error( "File::File( ) : "
	     "Cannot open a file." );
#endif
    setColor(mainColor);
    cerr << "debug point 6-2" << endl;
  }
  // Creates temporary file 
  
  
  File( const Chars& fileName ) : 
    fstream( fileName, ios::app|ios::out ), 
    theFileName(fileName)
  {
    colorFile.open(theFileName+Chars(".color"),ios::app|ios::out);
#if SAFETY > 0
    if( !(*this) )
      error( "File::File( Chars fileName ) : "
	     "Cannot open a file." );
#endif
    setColor(mainColor);
  }
  // Creates file with a specific name
  
  
  Chars getFileName() const { return theFileName; }
  // Returns name of the file.
  
  void setColor( const Chars& colorName ) 
  {
#if SAFETY > 0
    if( colorName.length() !=3 )
      error( "void File::setColor( const Chars& colorName ) : "
	     "Wrong color argument." );
#endif
    
    char s[10];
    sprintf(s,"%.7d",(long)tellp());
    colorFile << "#" << s << "%" <<  colorName << "@";
    colorFile.flush();
  }
  
  
private:
  
  Chars theFileName; // holds name of the file
  fstream colorFile; // holds color mapping
};


inline ostream& ready(ostream& ostr) 
{
  ostr <<  flush;
  return ostr;
}

inline ostream& end(ostream& ostr) 
{
  ostr <<  flush;
  return ostr;
}


#endif  // _File_h_

