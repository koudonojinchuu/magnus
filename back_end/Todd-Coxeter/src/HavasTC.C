// Copyright (C) 1998 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contentsent: Implementations of classes CosetEnumerator. 
//
// Principal Author:  Alexei Myasnikov
//
// Status: In development
//
// Usage:
//
// Discussion:
//
// Revision History:
//
// Next Implementation Steps:
//

#include "HavasTC.h"


HavasTC::HavasTC(const FPGroup& group): 
  tc(MagnusHome::magnusHome()+ "/back_end/black_boxes/ace/ace"),
  theGroup( group ),
  theWorkspace( 10000000 ),
  theState( NSTARTED )
{

}

HavasTC::HavasTC(const FPGroup& group,const VectorOf<Word>& subgroup):
  tc(MagnusHome::magnusHome()+ "/back_end/black_boxes/ace/ace"),
  theGroup( group ),
  theWorkspace( 10000000 ),
  theState( NSTARTED )
{
  int l = subgroup.length();
  int numOfnotTrivial = 0;
  for (int i=0;i<l;i++){  
    if (subgroup[i].length() > 0 )
      numOfnotTrivial++;
  }

  theSubgroup = VectorOf<Word>(l);

  for (int i=0,n=0;i<numOfnotTrivial;i++){  
    if (subgroup[i].length() > 0 ){
      theSubgroup[n] = subgroup[i];
      n++;
    }
  }  
}

HavasTC::~HavasTC()
{
}
 
void HavasTC::start()
{
  if (!tc.start())
    error("HavasTC::start() : couldn't start the black box.");

  ostream& out = tc.toCommand();

  out// << "AO:" << theOutFileName << ";"
      << "ENUMeration: F \n"
      << "GRoup generators: " << theGroup.numberOfGenerators() << ";\n"
      << "RELators:";

  // print group relators
  int numOfRelators = theGroup.getRelators().cardinality();
  int i = 0;
  SetIterator<Word > I(theGroup.getRelators());
  for (I.reset(); !I.done(); I.next() ){
    printWord( out, I.value() );
    if (i < numOfRelators-1 ){
      out << ", ";
      i++;
    }
  }

  out << ";\nSUBGroup: H\n" << "GENerators:";

  // print subgroup generators (if any)
  for (int ii=0;ii<theSubgroup.length();ii++){
    printWord( out, theSubgroup[ii] );
    if (ii < theSubgroup.length()-1 )
      out << ", ";
  }

  /* @db ace3000
  out << ";\nPARameters:\n"
      << "        COMpact:0;\n"
      << "        MAX Cosets (min,max,step):0;\n"
      << "        CT factor:0;\n"
      << "        RT factor:0;\n"
      << "        TIme:0;\n"
      << "        FIll factor (min,max,step):0;\n"
      << "        MEssages:0;\n"
      << "        WOrkspace:" << theWorkspace << ";\n"
      << "        DIagnostics:0;\n"
      << "END\n" << flush;
  */
  out << ";\nEND\n" << flush;
  
  theState = RUNNING;
}


bool HavasTC::finished() 
{ 
  
  char theLine[1000];
  istream& in = tc.fromCommand();

  if (theState == SUCCESS || theState == NOMEM )
    return true;

  int nLines = 4;
 
  //@db tmp
  //ofstream out(MagnusTmp::magnusTmp()+"/tc_out", ios::out | ios::trunc );
 
  for (int l=0;l< nLines;l++){
    if (in.eof())
      return true;
    
    in.getline(theLine,1000);
 
    Chars tmpLine( theLine);
    
    //@db tmp
    //out << tmpLine << endl << endl;
    
    if ( isInitSegment( tmpLine,"End TC") ){
      theState = SUCCESS;
      return true;
    }
    
    if ( isInitSegment( tmpLine,"Workspace=") ){
      theState = NOMEM;
      return true; 
    }
    
    if ( isInitSegment( tmpLine," INSUFFICIENT SPACE" ) ){
      theState = NOMEM;
      return true;
    }
    
    if ( isInitSegment( tmpLine,"INDEX" ) ){
      theState = SUCCESS;
      Chars cIndex;
      for (int i=Chars("INDEX = ").length();
	   tmpLine[i] >= '0' &&  tmpLine[i] <= '9';
	   i++)
	cIndex += tmpLine[i];
      theIndex = atoi(cIndex);
      return true;
    }
  }
  return false;
}

bool HavasTC::isInitSegment(Chars str, Chars seg ) const
{
  if (str.length() >= seg.length() ){
    for (int i=0;i<seg.length();i++)
      if ( str[i] != seg[i])
	return false;
    return true;
  } else
    return false;
}



void HavasTC::printWord( ostream& ostr, const Word& w ) const
{
  int len = w.length();
  if ( len == 0 ) ostr << ' ';
  else {
    int lastOrd = ord(w[0]);
    int power = 0;
    int curOrd;
    
    for( int i = 0; i <= len; i++ ) {
      curOrd = ( i == len ? 0 : ord(w[i]) );
      if ( lastOrd == curOrd ) ++power;
      else {

	ostr << abs(lastOrd);

	if ( lastOrd < 0 )
	  ostr << "^-" << power;
	else if ( power > 1 )
	  ostr << "^" << power;
	if ( i < len ) ostr << '*';
	lastOrd = curOrd;
	power = 1;
      }
    }
  }
}
