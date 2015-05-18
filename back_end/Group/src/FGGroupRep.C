// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of the FGGroupRep class.
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//
// 07/96 Alexey M. implemented IPC tools

#include "FGGroupRep.h"
#include "IPC.h"


// Methods dealing with group structure:

bool FGGroupRep::compare( const GroupRep* G ) const
{
  const FGGroupRep *G1 = (const FGGroupRep*) G;

  if( theNumberOfGenerators == G1->theNumberOfGenerators )
    return true;

  return false;
}

// Methods dealing with group elements:


Bool FGGroupRep::isSyntacticIdentity( const Elt& e) const
{
  return ((Word)e).length() == 0;
}


Elt FGGroupRep::nextElt( const Elt& e ) const
{
  return ((Word)e).nextInShortLex(theNumberOfGenerators);
}


Elt FGGroupRep::multiply( const Elt& e1, const Elt& e2 ) const
{
  return ( ((Word)e1) * ((Word)e2) ).freelyReduce();
}


Elt FGGroupRep::inverseOf( const Elt& e ) const
{
  return ((Word)e).inverse();
}


void FGGroupRep::printGenerator( ostream& ostr, int n ) const
{
  if ( theNamesOfGenerators.length() == 0 )
	 ostr << 'x' << abs(n);
  else
	 ostr << theNamesOfGenerators[abs(n) - 1];
  if ( n < 0 )
	 ostr << "^-1";
}


void FGGroupRep::printGenerators( ostream& ostr ) const
{
  if ( theNumberOfGenerators > 0 )
	 printGenerator(ostr, 1);
  for( int i = 1; i < theNumberOfGenerators; i++ ) {
	 ostr << ", ";
	 printGenerator(ostr, i + 1);
  }
}


void FGGroupRep::printWord( ostream& ostr, const Word& w ) const
{
  int len = w.length();
  if ( len == 0 ) ostr << '1';
  else {
	 int lastOrd = ord(w[0]);
	 int power = 0;
	 int curOrd;

	 for( int i = 0; i <= len; i++ ) {
		curOrd = ( i == len ? 0 : ord(w[i]) );
		if ( lastOrd == curOrd ) ++power;
		else {
		  if ( theNamesOfGenerators.length() == 0 )
			 ostr << 'x' << abs(lastOrd);
		  else
			 ostr << theNamesOfGenerators[abs(lastOrd) - 1];
		  if ( lastOrd < 0 )
			 ostr << "^-" << power;
		  else if ( power > 1 )
			 ostr << "^" << power;
		  if ( i < len ) ostr << ' ';
		  lastOrd = curOrd;
		  power = 1;
		}
	 }
  }

/*
  int len = w.length();
  if ( len == 0 ) ostr << '1';
  else {
	 printGenerator(ostr, ord(w[0]));
	 for( int i = 1; i < len; i++ ) {
		ostr << ' ';
		printGenerator(ostr, ord(w[i]));
	 }
  }
*/
}


void FGGroupRep::printSetOfWords( ostream& ostr, const SetOf<Word>& S ) const
{
  SetIterator<Word> I(S);
  ostr << "{ ";
  while( !I.done() ) {
	 printWord(ostr, I.value());
	 if ( I.next() ) { ostr << ", "; }
  }
  ostr << " }";  
}

//@am methods for IPC purposes
void FGGroupRep::write(ostream& ostr) const
{
   GroupRep::write(ostr);
   ostr < theNumberOfGenerators;
   ostr < theNamesOfGenerators;
   ostr < '\n'; //@dp temp
}

void FGGroupRep::read(istream& istr) 
{
   GroupRep::read(istr);
   istr > theNumberOfGenerators;
   istr > theNamesOfGenerators;
}

//@db temporary added left and right brackets
void FGGroupRep::printVectorOfWords( ostream& ostr, const VectorOf<Word>& V,
		 char* leftBracket, char* rightBracket ) const
{
  ostr << leftBracket << " ";
  int len = V.length();
  for( int i = 0; i < len; ++i ) {
	 printWord(ostr, V[i]);
	 if ( i < len - 1 ) { ostr << ", "; }
  }
  ostr << " " << rightBracket;  
}



