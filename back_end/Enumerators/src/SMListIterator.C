// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of class
//
// Principal Author: Alexei Myasnikov
//
// Status: in progress
//
// Revision History:
//

#include "SMEnumerator.h"
#include "SMList.h"
#include "SMListIterator.h"
#include "Subgroup.h"
#include "Timer.h"
#include "MagnusHome.h"
#include "Map.h"
#include "conversions.h"

// --------------------- SMListIterator --------------------------------//

template <class T>
SMListIterator<T>::SMListIterator(const SMList<T>& l)
: theData( l.getData() ) ,
  theGroup( l.getGroup().getFPGroup() ),
  currentData( NULL ),
  eol_reached( false ),
  numberOfCurrent( 0 )
{
  theData.setReadMode();
  nextCell();  
}



template <class T>
const T& SMListIterator<T>::getData() 
{ 
  if ( !currentData )
    parseData();
  
  return *currentData; 
}

template <class T>
void SMListIterator<T>::deleteCurrentData()
{
  if ( currentData ) {
    delete currentData;  
    currentData = NULL;
  }
  currentLine = "";
}


template <class T>
void SMListIterator<T>::reset(){ 
  theData.setReadMode();
  numberOfCurrent = 0;
  eol_reached = false;
  nextCell();  
}

template <class T>
bool SMListIterator<T>::nextCell()
{
  if ( eol_reached )
    return false;
  if ( numberOfCurrent >= getNumberOfElements() ){
    eol_reached = true;
    return false;
  }
  numberOfCurrent++;
  deleteCurrentData();
  return true;   
}

template <> void SMListIterator<Word>::parseData()
{
  if (currentLine.length() == 0 ){
    currentLine = theData.getElementOf( numberOfCurrent-1 );
  }
  if (currentLine.length() > 0 ) {
    //@njz
    //    istrstream is( currentLine );
    std::istrstream is( currentLine );
    //
    
    Chars errM;
    
    Word w = theGroup.readWord( is,errM );
    
    if (errM.length() > 0) 
      error("void SMListFileIterator<Word>::parseData() :"
	    "couldn't read a word from the list.");
    
    
    deleteCurrentData();
    
    currentData = new Word(w.freelyReduce());
    
  } else 
    error("void SMListFileIterator<Word>::parseData() :"
	  "can't parse a word from the empty string.");   
}

template <> void SMListIterator<Map>::parseData()
{
  if (currentLine.length() == 0 ){
    currentLine = theData.getElementOf( numberOfCurrent-1 );
  }

  if (currentLine.length() > 0 ) {

    Map m( theGroup,theGroup );

    //@njz
    //    istrstream is( currentLine );
    std::istrstream is( currentLine );
    //
    Chars errM = is >> m;  
    
    if (errM.length() > 0) 
      error(Chars("void SMListFileIterator<Map>::parseData() :"
		  "couldn't read a map from the list.")+errM ); 
    
    deleteCurrentData();
    
    currentData = new Map( m );
    
  } else 
    error("void SMListFileIterator<Map>::parseData() :" 
	  "can't parse a word from the empty string.");     
}
 
template <> void SMListIterator<Subgroup>::parseData()
{
  if (currentLine.length() == 0 ){
    currentLine = theData.getElementOf( numberOfCurrent-1 );
  }

  if (currentLine.length() > 0 ) {
    // @am Chars' operators were written by some ...
    //    Chars tmpChars = Chars('{') + currentLine + Chars('}');

    //@njz
    //    istrstream is( currentLine );
    std::istrstream is( currentLine );
    //
    Chars errM;  
    
    SetOf<Word> s = theGroup.readSetOfWords(is, errM);
    
    if (errM.length() > 0) 
      error(Chars("void SMListFileIterator<Subgroup>::parseData() :"
		  "couldn't read a set of words from the list.")+errM ); 
    
    deleteCurrentData();
    
    currentData = new Subgroup( theGroup, s );
    
  } else 
    error("void SMListFileIterator<Subgroup>::parseData() :" 
	  "can't parse a word from the empty string.");     
}

template class SMListIterator<Word>;
template class SMListIterator<Subgroup>;
template class SMListIterator<Map>;



void EnumWriteWord::print( ostream& o ) const
{
  theG.printWord( o,theW );
  //  o << endl << endl;
}

void EnumWriteMap::print( ostream& o ) const
{
  o <<"{" << endl;
  theM.printOn(o);
  o << endl << "}";

}

void EnumWriteVectorOfWords::print( ostream& o ) const
{
  int numberOfGens = theV.length();
  int i;

  o << '{' << endl;
  for (i = 0; i< numberOfGens; i++){
    if ( theV[i].length() > 0 ){
      theG.printWord( o, theV[i] );
      if ( i <  numberOfGens -1 )
	o <<  ',' << endl << endl;
    }
  }
  o << endl << '}';
}

void EnumWriteSetOfWords::print( ostream& o ) const
{
  SetIterator<Word> I( theS );
  int numberOfGens = theS.cardinality();
  int i;

  if ( numberOfGens == 0 )
    error("EnumWriteSetOfWords::print( ostream& o ) const : "
	  " cannot write empty set.");

  o << '{' << endl;
  for (i = 0,I.reset();!I.done(); I.next(),i++){
    if ( I.value().length() > 0 ){
      theG.printWord(o, I.value());
      if ( i <  numberOfGens -1 )
	o <<  ',' << endl << endl;
    }
  }
  o << endl << '}' << endl;
}

void EnumWriteChars::print( ostream& o ) const
{
  o << theC;
}
