// Copyright (C) 1998 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
// 
// Contents: Implementation of DGESolver
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//
//


#include "DGESolver.h"

//#define debug_DGESolver


// ----------------------------- DGESolver --------------------------------- //


DGESolver::DGESolver( FreeGroup F,VectorOf<Chars> vNames, Word eq )
  : theGroup( F ), 
    varNames( vNames ),
    numOfVars( vNames.length() ),
    numOfConsts( F.numberOfGenerators() ),
    equation( eq ),
    eqLen( eq.length() )
{
  invVarPos = VectorOf<int>(numOfVars);
  for( int i = numOfVars; i < eqLen; ++i )
    invVarPos[abs(ord(equation[i]))-1] = i;
  varInfo = new VarInfo[numOfVars];
  invVarInfo = new VarInfo[numOfVars];
}

DGESolver::~DGESolver( )
{
  delete varInfo;
  delete invVarInfo;
}

void DGESolver::defineRange( int curVar )
{
  // find the first non-trivial value to the left, begin after it

  int var = curVar;
  while( true ) {
    if( var == 0 ) {
      varInfo[curVar].firstP = 0;
      break;
    }
    if( !varInfo[var-1].isTrivial ) {
      varInfo[curVar].firstP = varInfo[var-1].last + 1;
      break;
    }
    --var;
  }
  
  // find lastP by looking into the buffer

  int pos;
  for( pos = varInfo[curVar].firstP; pos < wLen; ++pos )
    if( buffer[pos] != 0 ) break;
  --pos;
  varInfo[curVar].lastP = pos;

  // find range for the inverse of the variable

  int n;
  for( pos = invVarPos[curVar] + 1; pos < eqLen; ++pos ) {
    n = abs(ord(equation[pos]));
    if( curVar > n && !varInfo[n].isTrivial )
      break;
  }
  if( pos >= eqLen )
    invVarInfo[curVar].lastP = wLen - 1;
  else
    invVarInfo[curVar].lastP = invVarInfo[n].firstP - 1;

  for( pos = invVarInfo[curVar].lastP; pos >= 0; --pos )
    if( buffer[pos] != 0 ) break;
  ++pos;
  invVarInfo[curVar].firstP = pos;
}


bool DGESolver::findInverseOf( Word u, int p, int q, int& first, int& last )
{
  int uLen = u.length();
  for( int i = q; i >=p; --i )

    if( -ord(u[0]) == ord(w[i]) ) {
      
      bool match = true;
      for( int j = 1; j < uLen; ++j )
	if( (i-j) < 0 || -ord(u[j]) != ord(w[i-j]) ) {
	  match = false;
	  break;
	}
      if( match ) {
	last = i;
	first = i+1-uLen;
	return true;
      }
    }
  
  return false;
}


bool DGESolver::pickNextValue( int curVar ) 
{
  // try to find another substitution for the inverse of curVar 

  int first, last;
  
  if( !varInfo[curVar].isTrivial ) {

    if( findInverseOf
	( w.subword(varInfo[curVar].first, varInfo[curVar].last + 1),
	  invVarInfo[curVar].firstP, invVarInfo[curVar].last - 1,
	  first, last )	)
      {
	// change the buffer
	
	for(int i = invVarInfo[curVar].first;i <= invVarInfo[curVar].last;++i)
	  buffer[i] = 0;
	for(int i = first; i <= last; ++i)
	  buffer[i] = -(curVar+1);
	  
	// assign new values and return

	invVarInfo[curVar].first = first;
	invVarInfo[curVar].last = last;
	return true;
      }
    else
      {
	// clean the buffer

	for(int i = invVarInfo[curVar].first;i <= invVarInfo[curVar].last;++i)
	  buffer[i] = 0;
      }
  }
  
  // choose new value for curVar and its inverse

  bool isTrivial = varInfo[curVar].isTrivial;
  int pos;

  if( isTrivial )
    pos = varInfo[curVar].firstP;
  else
    pos = varInfo[curVar].last + 1;

//  while( true ) {
  
  if( pos >= wLen || buffer[pos] != 0 )
    return false;
  
  if( isTrivial ) {
    first = pos;
    last = pos;
  } else {
    first = varInfo[curVar].first;
    last = pos;
  }
  
  isTrivial = false;
  
  // * now check if there a suitable inverse
  
  int iFirst, iLast;
  
  if( findInverseOf
      ( w.subword(first, last + 1),
	invVarInfo[curVar].firstP, invVarInfo[curVar].lastP,
	iFirst, iLast )	)
    {
      // change the buffer
      
      for(int i = iFirst; i <= iLast; ++i)
	buffer[i] = -(curVar+1);
      for(int i = first; i <= last; ++i)
	buffer[i] = curVar+1;
      
      // assign new values and return
      
      invVarInfo[curVar].first = iFirst;
      invVarInfo[curVar].last = iLast;
      varInfo[curVar].first = first;
      varInfo[curVar].last = last;
      varInfo[curVar].isTrivial = false;
      return true;
    }
  
  return false;
//    ++pos;
//  }
  
}


bool DGESolver::getSolution( const Word& u, Map& solution )
{
  w = u.cyclicallyReduce();
  wLen = w.length();
  
  // initialize
  {
    for( int i = 0; i < numOfVars; ++i )
      varInfo[i].isTrivial = true;

    buffer = VectorOf<int>(wLen);
    for( int i = 0; i < wLen; ++i )
      buffer[i] = 0;
  }
  

  // main loop: enumerate all substitutions, compare results with u,
  // finish when a solution is found or when all substitutions have
  // been enumerated
  while( true ) { 

    // build the next substitution
    // if cannot build return false
    {
      // start with the last variable
      int curVar = numOfVars - 1; // current variable of the substitution

      bool substitutionReady = false;
      while( !substitutionReady ) {

	// pick the next value for the current variable
	// if cannot, initialize it to 1, pick the variable to the left,
	// repeat the process
	{
	  // if the variable is trivial define it's range

	  if( varInfo[curVar].isTrivial )
	    defineRange(curVar);
	  
	  // try to pick the next possible value

	  if( pickNextValue(curVar) ) {
	    substitutionReady = true;

	    #ifdef debug_DGESolver
	    cout << "buffer: ";
	    for( int i = 0; i < wLen; ++i )
	      cout << " " << buffer[i];
	    cout << endl;
	    #endif
	  }
	  else {

	    if( !varInfo[curVar].isTrivial ) {
	      
	      varInfo[curVar].isTrivial = true;
	      
	      // clean the buffer
	      
	      for( int i = varInfo[curVar].first;
		   i <= varInfo[curVar].last; ++i)
		buffer[i] = 0;
	      
	      for( int i = invVarInfo[curVar].first;
		   i <= invVarInfo[curVar].last; ++i)
		buffer[i] = 0;
	    }
	    
	    --curVar;
	    
	    // if all substitutions have been enumerated return false
	    
	    if( curVar < 0 ) return false;
	  }
	}
      }
    }
    
    // check if it is equal to u, return true if successful

    int i;
    for( i = 0; i < wLen; ++i )
      if( buffer[i] == 0 ) break;
    if( i >= wLen ) {

      FreeGroup domain(varNames);
      VectorOf<Word> im(numOfVars);
      for( int j = 0; j < numOfVars; ++j ) {
	Word t = w.subword(varInfo[j].first, varInfo[j].last+1);
	if( ord(equation[j]) < 0 ) t = t.inverse();
	im[j] = t;
      }
      solution = Map(domain,theGroup,im);

      return true;
    }
  }
}
