// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of the FreeGroupRep class
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//
// * 01/96 Dmitry B. added new function:
//
//    Word FreeGroupRep::getN_thElement( int n ) const;
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//

#include "FreeGroupRep.h"
#include "PresentationParser.h"
#include "AbelianWord.h"
#include "File.h"

// Methods dealing with the free group rep:


int FreeGroupRep::order( ) const
{
  FreeGroupRep* This = (FreeGroupRep*)this;
  // break physical constness under maintenence of logical constness

  This->theOrder = ( theNumberOfGenerators == 0 ? 1 : 0 );
  return theOrder;
}


Trichotomy FreeGroupRep::isTrivial( ) const
{
  if ( theNumberOfGenerators == 0 ) return YES;
  else return NO;
}


Trichotomy FreeGroupRep::isFinite( ) const
{
  if ( theNumberOfGenerators == 0 ) return YES;
  else return NO;
}


Trichotomy FreeGroupRep::isInfinite( ) const
{
  if ( theNumberOfGenerators > 0 ) return YES;
  else return NO;
}


Trichotomy FreeGroupRep::isAbelian( ) const
{
  if ( theNumberOfGenerators < 2 ) return YES;
  else return NO;
}

bool FreeGroupRep::compare( const GroupRep* G ) const
{
  return FGGroupRep::compare(G);
}

// This function is used by FreeGroupRep::nielsenBasis and
// FreeGroupRep::inverseAutomorphism.
// I don't see any reason to make this function a friend of FreeGroup.
Bool firstIsLess( const Word& v, const Word& w ) {
  
  if ( v.length() < w.length() ) return TRUE;
  if ( v.length() > w.length() ) return FALSE;
  
  Word v_initial, v_terminal, w_initial, w_terminal;
  
  v_initial = v.initialSegment( ( v.length() + 1 ) / 2 );
  v_terminal = v.terminalSegment( ( v.length() + 1 ) / 2 ).inverse();
  w_initial = w.initialSegment( ( w.length() + 1 ) / 2 );
  w_terminal = w.terminalSegment( ( w.length() + 1 ) / 2 ).inverse();
  
  if ( v_initial < v_terminal ) {
    if ( w_initial < w_terminal ) {
      if ( v_initial < w_initial ) return TRUE;
      if ( v_initial == w_initial && v_terminal < w_terminal )
        return TRUE;
    }
    else {
      if ( v_initial < w_terminal ) return TRUE;
      if ( v_initial == w_terminal && v_terminal < w_initial )
        return TRUE;
    }
  }
  else {
    if ( w_initial < w_terminal ) {
      if ( v_terminal < w_initial ) return TRUE;
      if ( v_terminal == w_initial && v_initial < w_terminal )
        return TRUE;
    }
    else {
      if ( v_terminal < w_terminal ) return TRUE;
      if ( v_terminal == w_terminal && v_initial < w_initial )
        return TRUE;
    }
  }
  return FALSE; 
}


FreeGroupRep::NielsenBasis::NielsenBasis( const VectorOf<Word>& vec,
					  const VectorOf<Chars>& namesOfGenerators,
					  bool writeToFile )
  : theNamesOfGenerators( namesOfGenerators ),
    theOldGenerators( vec ), basisComputed( false ),
    keepDetails( writeToFile ),
    theFileName( writeToFile ? File().getFileName() : Chars("") )
{ 
}

Chars FreeGroupRep::NielsenBasis::getFileName( ) const 
{
  return  theFileName;
}

VectorOf<Word> FreeGroupRep::NielsenBasis::expressNewGensInOldGens( )
{
  if( !basisComputed )
    computeNielsenBasis();
  return newGeneratorsExprssedInOldGerators;
}

VectorOf<Word> FreeGroupRep::NielsenBasis::newGenerators( ) 
{
  if( !basisComputed )
    computeNielsenBasis();
  return theNewGenerators;
}

void FreeGroupRep::NielsenBasis::printParentGroup( ostream& ostr ) const 
{
  ostr << "< ";
  for( int i = 0; i < theNamesOfGenerators.length(); ++i ) {
    if( i ) ostr << ", ";
    ostr << theNamesOfGenerators[i];
  }
  ostr << " >";
}

void FreeGroupRep::NielsenBasis::printWord( ostream& ostr, const Word& w ) const
{
  for( int i = 0; i < w.length(); ++i ) {
    if( i > 0 ) ostr << ' ';
    int gen = ord(w[i]);
    ostr << theNamesOfGenerators[ abs(gen)-1 ];
    if( gen < 0 ) ostr << "^-1";
  }
  if( w.length() == 0 ) 
    ostr << '1';
}

void FreeGroupRep::NielsenBasis::printBasis( ostream& ostr, 
  const VectorOf<Word>& basis, bool detailedView ) const
{
  int len = basis.length(); 

  for( int i = 0; i < len; ++i ) {
    if( detailedView ) 
      ostr << " h" << (i+1) << " = ";
    else if( i > 0 )
      ostr << ", ";
    printWord( ostr, basis[i] );
    if( detailedView )
      ostr << endl;
  }

  if( detailedView )
    ostr << endl;
  else
    ostr << " )";
}
  
void FreeGroupRep::NielsenBasis::printTransformation( ostream& ostr, 
  const VectorOf<Word>& basis, int i, int g1, int g2 ) const 
{
  ostr << "Replace h" << i 
       << " by h" << abs(g1) << ( g1 < 0 ? "^-1" : "" )
       << " * h" << abs(g2) << ( g2 < 0 ? "^-1" : "" ) 
       << "." << endl << endl;
}


// This procedure computes Nielsen basis for the given subgroup and 
// keep track expression new generators of the subgroup in old ones.

void FreeGroupRep::NielsenBasis::computeNielsenBasis( )
{
  File *filePtr = ( !keepDetails ? 0 : new File(theFileName) );
  File &file = *filePtr;
  bool compactView = false;
  int stepCount = 0;

  int len = theOldGenerators.length();
  VectorOf<Word> newBasis( len );
  VectorOf<Word> newBasisInOld( len );

  // Keep track of expression of new subgroup generators in old ones.
  for( int i = 0; i < len; ++i ) {
    newBasis[i] = theOldGenerators[i];
    newBasisInOld[i] = Generator(i+1);
  }
  
  // Print general info (group,initial generators of the subgroup) 
  // info into logfile.
  if( keepDetails ) {
    file.setColor( titleColor );
    file << "Computing Nielsen basis of a subgroup of a free group:" << endl;
    file << "The free group F = ";
    file.setColor( mainColor );
    printParentGroup( file );
    file.setColor( titleColor );
    file << endl << "The given generators of the subgroup: " << endl;
    file.setColor( mainColor );
    printBasis( file, newBasis, !compactView );
    file << endl;
  }

  if ( len > 0 ) {
    
    // Compute Nielsen basis only for subgroup with 1 generators at least.

    // This procedure will work with freely reduced basis only. 
    // It checks this property now.
    bool freelyReduced = false;
    for ( int i = 0; i < len; i++ ) {
      Word tmp =  newBasis[i].freelyReduce();
      if( tmp.length() != newBasis[i].length() )
	freelyReduced = true;
      newBasis[i] = tmp;
    }

    // Also we can remove all identities from the Nielsen basis right now.
    int identities = 0;
    int ii = 0;
    while ( ii < len ) {
      if ( newBasis[ii].length() != 0 ) 
	++ii;
      else {
	--len;
	++identities;
	newBasis[ ii ] = newBasis[ len ];
	newBasisInOld[ ii ] = newBasisInOld[ len ];
      }
    }

    if( keepDetails && ( freelyReduced || identities > 0 ) ) {
      // Print the info about freely reducing and removing identities 
      // from the basis.
      file.setColor( titleColor );
      if( freelyReduced ) {
	file << "0) Make generators of the subgroup freely reduced";
      }
      if( identities > 0 ) {
	if( freelyReduced )
	  file << " and remove";
	else 
	  file << "0) Remove";
	file << "identities from the generators set";
      }
      file.setColor( mainColor );
      file << '.' << endl;
      file << "The new generators of the subgroup are" << endl << endl;
      printBasis( file, newBasis, !compactView );
      file << endl;

    }

    newBasis.shrink( len );
    newBasisInOld.shrink( len );

    if( keepDetails ) {
      file.setColor( titleColor );
      file << "A step of computation is removing an identity(s) from the current basis \n"
	"or replacing a generator x_i with new generator g(x_i,x_j) such that \n"
	"g is less than x_i in lexical order. \n"
	"g(x_i,x_j) has one of these forms: x_i*x_j, X_i*x_j, x_j*X_i, x_j*x_i. \n\n";
      file.setColor( mainColor );
    }
    
    if ( len > 1  ) {
    
      for (int i = 1; i < len; i++ ) {

	// If the current element is an identity then find non-identity one
	// and replace the current one.
	bool shrink = false;
	while ( newBasis[i].length() == 0 ) {
	  ++stepCount;
	  --len;
	  newBasis[ i ] = newBasis[ len ];
	  newBasisInOld[ i ] = newBasisInOld[ len ];
	  newBasis.shrink( len );
	  newBasisInOld.shrink( len );
	  shrink = true;
	  if ( i >= len ) break; 
	}
	if( keepDetails && shrink ) {
	  file << stepCount << ") Remove identity h" << i+1 << "." << endl << endl;
	  file << "The new generators of the subgroup are" << endl << endl;
	  printBasis( file, newBasis, !compactView );
	}

	if( i >= len ) break;

	// Can we use x_i with some combination w(x_i,x_j), 0 < j < i
	// to get new basis which is less than old one in lex order?
	// Each w(x_i,x_j) is x_i*x_j or X_i*x_j or x_j*X_i or x_j*x_i.

	// Can we use x_i to reduce the basis by one of Nielsen transformation:
	// x_i*x_j or X_i*x_j or x_j*X_i or x_j*x_i, where 0 <= j < i

	// It checks this property of x_i.
	
	for ( int j = 0; j < i; j++ ) {

	  int keepGoing = 0;
	  int longerOne = firstIsLess( newBasis[i], newBasis[j] ) ? j : i;
	  // Select the greatest word x_k between x_i and x_j in lex order.
	  
	  // 1. Check the transformation x_k -> x_i * x_j,
	  int g1 = i+1; 
	  int g2 = j+1;
	  Word tmpWord = ( newBasis[i] * newBasis[j] ).freelyReduce();
	  Word tmpWordInOld = ( newBasisInOld[i] * newBasisInOld[j] ).freelyReduce();
	  if ( !firstIsLess( tmpWord, newBasis[longerOne] ) ) {
	    // 2. Check the transformation x_k -> X_i * x_j.
	    g1 = -(i+1); 
	    g2 = j+1;
	    tmpWord = ( newBasis[i].inverse() * newBasis[j] ).freelyReduce();
	    tmpWordInOld = ( newBasisInOld[i].inverse() * newBasisInOld[j] ).freelyReduce();
	    if ( !firstIsLess( tmpWord, newBasis[longerOne] ) ) {
	      // 3. Check the transformation x_k -> x_j * X_i. 
	      g1 = j+1;
	      g2 = -(i+1);
	      tmpWord = ( newBasis[j] * newBasis[i].inverse() ).freelyReduce();
	      tmpWordInOld = ( newBasisInOld[j] * newBasisInOld[i].inverse() ).freelyReduce();
	      if ( !firstIsLess( tmpWord, newBasis[longerOne] ) ) {
		// 4. Check the transformation x_k -> x_j * x_i.
		g1 = j+1;
		g2 = i+1;
		tmpWord = ( newBasis[j] * newBasis[i] ).freelyReduce();
		tmpWordInOld = ( newBasisInOld[j] * newBasisInOld[i] ).freelyReduce();
		if ( !firstIsLess( tmpWord, newBasis[longerOne] ) )
		  //$sl g++ 2.6.0 is getting confused with operator "continue",
		  //    so I was forced to avoid using it.
		  keepGoing = 1;
	      }
	    }
	  }
	  if ( !keepGoing ) {
	    // A Nielsen transformation is found.
	    ++stepCount;
	    if( keepDetails ) {
	      file << stepCount << ") ";
	    }

	    if ( longerOne == i ) {
	      // The transormation is x_i -> w(x_i,x_j);

	      // Print the transformation into logfile.
	      if( keepDetails ) {
		printTransformation( file, newBasis, i+1, g1, g2 );
	      }

	      // Replace old generator x_i with new one.
	      // Keep track expression of new x_i in old generators.
	      newBasis[i] = tmpWord;
	      newBasisInOld[i] = tmpWordInOld;
	      --i; 

	      if( keepDetails ) {
		// Print current basis into logfile.
		file << "The new generators of the subgroup are" << endl << endl;
		printBasis( file, newBasis, !compactView );
	      }
	      break;
	    }
	    else {
	      // The transformation is x_j -> w(x_i,x_j);

	      // Print the transformation into logfile.
	      if( keepDetails ) {
		printTransformation( file, newBasis, j+1, g1, g2 );
	      }
	      
	      // Rotate generators: x_i-1 -> x_j, x_i -> x_i-1, new x_j -> x_i.
	      // Keep track expression of x_i, x_i-1, new x_j in old generators.
	      newBasis[j] = newBasis[i-1];
	      newBasisInOld[j] = newBasisInOld[i-1];
	      
	      newBasis[i-1] = newBasis[i];
	      newBasisInOld[i-1] = newBasisInOld[i];

	      newBasis[i] = tmpWord;
	      newBasisInOld[i] = tmpWordInOld;
	      
	      // Step back and continue check x_i (on the x_i-1 place) 
	      i--;
	      j--;

	      if( keepDetails ) {
		// Print current basis into logfile.
		file << "The new generators of the subgroup are" << endl << endl;
		printBasis( file, newBasis, !compactView );
	      }
	    }
	  }
	  else 
	    keepGoing = 0;
	}
      }
    }
    if ( len > 0 &&  newBasis[len-1].length() == 0 ) {
      if( keepDetails ) {
	file << ++stepCount << ") Remove identity h" << len << "." << endl;
      }
      --len;
      newBasis.shrink( len );
      newBasisInOld.shrink( len );
    }
  }
  
  if( keepDetails ) {
    // Finally prints found Nielsen basis into logfile.
    file.setColor( titleColor );
    file << endl << "The Nielsen basis of the subgroup is " << endl;
    file.setColor( mainColor );
    printBasis( file, newBasis, !compactView );
    file << endl << end;
  }

  theNewGenerators = newBasis;
  newGeneratorsExprssedInOldGerators = newBasisInOld;
  basisComputed = true;
}

VectorOf<Word> FreeGroupRep::nielsenBasis(const VectorOf<Word>& V) const 
{
  NielsenBasis nielsenBasis( V, theNamesOfGenerators, false );
  return nielsenBasis.newGenerators();
}

Bool FreeGroupRep::isAutomorphism(const VectorOf<Word>& V) const
{
  #if SAFETY > 0
    if ( V.length() != theNumberOfGenerators ) 
      error("Wrong argument in FreeGroupRep::isAutomorphism");
  #endif
  VectorOf<Word> nielsen = nielsenBasis(V);

  if ( nielsen.length() != V.length() ) return FALSE;
  for ( int i = nielsen.length()-1; i >= 0; i-- )
    if ( ((Word)nielsen[i]).length() != 1 ) return FALSE;

  return TRUE;
}

Bool FreeGroupRep::isInnerAutomorphism(const VectorOf<Word>& V) const
{

  #if SAFETY > 0
    if ( V.length() != theNumberOfGenerators )
      error("Wrong argument in FreeGroupRep::isAutomorphism");
  #endif

  if ( theNumberOfGenerators == 0 ) return TRUE;
  if ( theNumberOfGenerators == 1 ) 
    return ((Word)V[0]).freelyReduce().length() == 1;

  Word temp;
  if ( ((Word)V[0]).length() > ((Word)V[1]).length() ) temp = (Word)V[0];
  else temp = (Word)V[1];
  temp = temp.freelyReduce();
  Word conjugator = temp.initialSegment(temp.length()/2);

  for ( int i = V.length()-1; i >= 0; i-- ) {
    temp = (Word)V[i];
    temp = temp.conjugateBy(conjugator);
    temp = temp.freelyReduce();
    if ( temp != Word(Generator(i+1)) ) return FALSE;
  }

  return TRUE;
}

bool FreeGroupRep::isIAAutomorphism(const VectorOf<Word>& V) const
{

#if SAFETY > 0
  if ( V.length() != theNumberOfGenerators )
    error("Wrong argument in FreeGroupRep::isIAAutomorphism");
#endif

  if( !isAutomorphism(V) )
    return false;

  for( int i = theNumberOfGenerators-1; i >= 0; --i ) {
    AbelianWord tmp( theNumberOfGenerators, inv(Generator(i+1)) * V[i] );
    if( tmp.fullLength() != 0 )
      return false;
  }

  return true;
}

VectorOf<Word> FreeGroupRep::inverseAutomorphism(const VectorOf<Word>& V) 
const 
// If `V' doesn't define an automorphism it returns garbige.
{
  int len = V.length();

//@ep patched 9-Apr-96
//  if ( len == 0 ) return V;
  if ( len <= 1 ) return V;
  
  VectorOf<Word> temp = V;
  VectorOf<Word> res(len);
  Word tmpWord, xWord;
  
  int keepGoing = 0, longerOne;
  
  for ( int i = 0; i < len; i++ ) {
    res[i] = Generator(i+1);
    temp[i] = ((Word)temp[i]).freelyReduce();
  }
  
  if ( ((Word)temp[0]).length() == 0 ) return res;

//@ep patched 9-Apr-96
//  if ( len <= 1  ) return res;

  for( int i = 1; i < len; i++ ) {
    // Check if temp[i] is identity.
    if ( ((Word)temp[i]).length() == 0 ) return res;
    
    for ( int j = 0; j < i; j++ ) {
      longerOne = ( firstIsLess((Word)temp[i],(Word)temp[j]) ) ? j : i;
      tmpWord = ( (Word)temp[i] * (Word)temp[j] ).freelyReduce();
      xWord = ((Word)res[i]*(Word)res[j]).freelyReduce();
      if ( !firstIsLess(tmpWord,(Word)temp[longerOne]) ) {
        tmpWord = ( ((Word)temp[i]).inverse()*(Word)temp[j] ).freelyReduce();
	xWord = ( ((Word)res[i]).inverse()*(Word)res[j] ).freelyReduce();
        if ( !firstIsLess( tmpWord, (Word)temp[longerOne] ) ) {
          tmpWord = ( (Word)temp[j]*( (Word)temp[i]).inverse() ).freelyReduce();
	  xWord = ( (Word)res[j]*( (Word)res[i]).inverse() ).freelyReduce();
          if ( !firstIsLess( tmpWord, (Word)temp[longerOne] ) ) {
            tmpWord = ( (Word)temp[j] * (Word)temp[i] ).freelyReduce();
	    xWord = ( (Word)res[j] * (Word)res[i] ).freelyReduce();
            if ( !firstIsLess( tmpWord, (Word)temp[longerOne] ) )
              
//$sl g++ 2.6.0 is getting confused with operator "continue", so
//    I was forced to avoid using it.
              
              keepGoing = 1;
          }
        }
      }
      if ( !keepGoing ) {
        if ( longerOne == i ) {
          temp[i] = tmpWord;
	  res[i] = xWord;
          j = -1;
        }
        else {
          temp[j] = ( Word )temp[i-1];
	  res[j] = (Word)res[i-1];
          temp[i-1] = ( Word )temp[i];
	  res[i-1] = (Word)res[i];
          temp[i] = tmpWord;
	  res[i] = xWord;
          i--;
          j--;
        }
      }
      else keepGoing = 0;
    }
  }
  VectorOf<Word> result(len);
  for ( int i = 0; i < len; i++ ) {
    int gen = ord(((Word)temp[i])[0]);
    if ( gen > 0 ) result[gen-1] = (Word)res[i];
    else result[-(gen+1)] = ((Word)res[i]).inverse();
  }
  return result;
}


// finds exponents for all generators. A word of a free group is in
// the commutator subgroup iff all exponents are zero ( take factor
// of the free group by the commutator subgroup. It's abelian. )
// This algorithm is linear on the length of `w'.
Bool FreeGroupRep::inCommutatorSG(const Word& w) const
{
  VectorOf<int> V(theNumberOfGenerators+1);
  int check=0;

  //$sl I don't remember is `V' filled with zeros?
  for ( int i = 0; i < theNumberOfGenerators; i++ ) V[i] = 0;

  for ( int i = w.length()-1; i >= 0; i-- ) {
        int gen = ord(w[i]);
        int abs = gen > 0 ? gen : -gen;
        if ( V[abs] == 0 ) {
                if ( gen > 0 ) V[abs] = 1;
                else V[abs] = -1;
                check++;
	      }
        else {
                if ( gen > 0 ) V[abs] = (int)V[abs]+1;
                else V[abs] = (int)V[abs]-1;
                if ( V[abs] == 0 ) check--;
	      }
      }
  if ( check == 0 ) return TRUE;
  return FALSE;
}

Bool FreeGroupRep::isCommutator(const Word& u, Word& u1, Word& u2) const
{
  // `u' need not be freely reduced
  Word w = u.freelyReduce();
  int w_len = w.length();

  // Cheap check: if the length of `w' is odd
  if ( w_len % 2 ) return FALSE;

  // trivial word
  if ( w_len == 0 ) {
    u1 = u2 = Word();
    return TRUE;
  }

  // Cheap check: `w' is in commutator subgroup
  if ( !inCommutatorSG(w) ) return FALSE;

  // find cyclically reduced copy of `w':
  // If w = a^-1 [w1,w2] a, then w = [a^-1 w1 a, a^-1 w2 a].
  int i = 0, j = w_len-1;
  while ( ord(w[i]) == -ord(w[j]) ) {
    // since w is freely reduced we don't have to check if i < j;
    i++;
    j--;
  }

  // We assume that elementary operations on word are fast enough.
  Word a = w.terminalSegment(i);
  w = w.subword(i,j+1);
  w_len = w.length();           // faster than w_len -= ( i-1 )<<1;

  // trying to find irreducible decomposition of cyclic permutation
  // w' of w :
  // w' = x^-1 y^-1 z^-1 x y z
  // this is possible iff `w' is a commutator.
  // as soon as we find such a decomposition w = [yx,yz].
  // See Wicks.

  int half_len = w_len/2;
  int x_len, y_len;

  for ( int t = 0; t < w_len; t++ ) {
    for ( x_len = 1; x_len < half_len; x_len++ ) {
      i = 0;
      j = half_len+x_len-1;
      while ( ord(w[i]) == -ord(w[j]) ) { i++; j--; }
      
      // check if segments [0,x_len) and [half_len,half_len+x_len) are
      // inverse to each other.
      if ( i >= x_len ) {
	int yz_len = half_len - x_len;
	for ( y_len = 1; y_len <= yz_len; y_len++ ) {
	  i = x_len;
	  j = half_len+x_len+y_len-1;
	  while ( ord(w[i]) == -ord(w[j]) ) { i++; j--; }

	  // check if segment [x_len,x_len+y_len) and
	  // [half_len+x_len,half_len+x_len+y_len) are
	  // inverse to each other.
	  if ( i >= x_len+y_len ) {
	    // last check if z also has inverse.
	    i = x_len+y_len;
	    j = w_len-1;
	    while ( ord(w[i]) == -ord(w[j]) ) { i++; j--; }
	    if ( i >= half_len ) {
	      if ( x_len + y_len >= half_len ) {
		u1 = w.initialSegment(x_len).inverse();
		u2 = w.terminalSegment(half_len-x_len);
	      }
	      else {
		u1 = w.initialSegment(x_len+y_len).inverse();
		u2 = w.terminalSegment(half_len-x_len);
	      }
	      u1 = u1.conjugateBy(a);
	      u2 = u2.conjugateBy(a);
	      u1 = u1.conjugateBy(w.terminalSegment(t).inverse());
			u2 = u2.conjugateBy(w.terminalSegment(t).inverse());
	      u1 = u1.freelyReduce();
	      u2 = u2.freelyReduce();
	      return TRUE;
	    }
	  }
	}
      }
    }
    w = w.conjugateBy(w.initialSegment(1));
    w = w.freelyReduce();
  }    // t cycle.

  return FALSE;
}


// this function is used by 'productOfCommutators'
Chars oneStep(const VectorOf<Chars>& v,const Word& a,const Word& b,int flag) 
{
  Chars res;
  
  if( flag )
    res += Chars("\\\[ ");
  else
    res += Chars("\[ ");
  
  int count = 1 , k;
  for( int i = 0 ; i < a.length() ; i++ )
    {
      if( i < a.length() - 1 && a[i] == a[i+1] )
	{
	  count++;
	  continue;
	}
      
      k = Generator(a[i]).hash();
      k = ( k > 0 ) ? k : - k;
      res += v[k-1];
      
      if( Generator(a[i]).hash() > 0 && count > 1 )
	res += Chars("^") + Chars(count);
      
      if( Generator(a[i]).hash() < 0)
	    res += Chars("^-") + Chars(count);
      
      res += Chars(" ");
      count = 1;
    }
  
  res += Chars(", ");
  
  count = 1;
  for( int i = 0 ; i < b.length() ; i++ )
    {
      if( i < b.length() - 1 && b[i] == b[i+1] )
	{
	  count++;
	  continue;
	}
      
      k = Generator(b[i]).hash();
      k = ( k > 0 ) ? k : - k;
      res += v[k-1];
      
      if( Generator(b[i]).hash() > 0 && count > 1 )
	res += Chars("^") + Chars(count);
      
      if( Generator(b[i]).hash() < 0 )
	res += Chars("^-") + Chars(count);
      
      res += Chars(" ");
      count = 1;
    }
  
  if( flag )
    res += Chars("\\] ");
  else
    res += Chars("] ");

  return res;
}

Chars FreeGroupRep::productOfCommutators(const Word& w,File& file)
{
  if(!w.freelyReduce().length()) 
    return Chars("trivial");
  
  if( !w.allExponentSumsZero() )
    return Chars("not in derived");
  
  Word u1,u2;
  if(isCommutator(w,u1,u2))
    {
      FreeGroup K(theNamesOfGenerators);
      
      K.printWord(file,w);
      file << "  is a commutator of  "; 
      K.printWord(file,u1);
      file << "  and  ";
      K.printWord(file,u2);
      file << endl;
      
      return oneStep(theNamesOfGenerators,u1,u2,1);
    }
  
  Word tmp = w.freelyReduce();
  Chars res,res1;
  FreeGroup K(theNamesOfGenerators);
  
  file << "The steps of the rewriting process: " << endl << endl;

  while(tmp.length())
    {
      K.printWord(file,tmp);
      file << " = " << endl << endl << " = "; 
      
      int i;
      for( i = 1 ; i < tmp.length() ; i++ )
	if(Generator(tmp[0]).hash() == - Generator(tmp[i]).hash()) 
	  break;
      
      int j,k;
      for( j = 1 , k = i - 1 ; k > j ; j++ , k-- )
	if(Generator(tmp[j]).hash() != - Generator(tmp[k]).hash())
	  break;
      
      Word a = tmp.subword(k+1,i+1);
      
      Word invb = tmp.subword(j,k+1) , b = invb.inverse();
      
      tmp = (invb * tmp.terminalSegment(tmp.length() - i - 1)).freelyReduce();
          
      res += oneStep(theNamesOfGenerators,a,b,1);
      res1 += oneStep(theNamesOfGenerators,a,b,0);
      
      file << res1 << "  "; 
    }
  
  return res;
}

// this function is used by 'productOfSquares'
Chars square(const VectorOf<Chars>& v,const Word& a) 
{
  Chars res;
  if( a.length() > 1 )
    {
      res += Chars("( ");
      
      int count = 1 , k;
      for( int i = 0 ; i < a.length() ; i++ )
	{
	  if( i < a.length() - 1 && a[i] == a[i+1] )
	    {
	      count++;
	      continue;
	    }
	  
	  k = Generator(a[i]).hash();
	  k = ( k > 0 ) ? k : - k;
	  res += v[k-1];
	  
	  if( Generator(a[i]).hash() > 0 && count > 1 )
	    res += Chars("^") + Chars(count);
	  
	  if( Generator(a[i]).hash() < 0)
	    res += Chars("^-") + Chars(count);
	  
	  res += Chars(" ");
	  count = 1;
	}
      
      res += Chars(")^2 ");
    }
  else
    {
      Generator g = a[0];
      int p = g.hash();
      p = ( p > 0 ) ? p : - p;
      res += v[p-1];
      
      if( Generator(a[0]).hash() > 0 )
	res += Chars("^2 ") ;
      
      if( Generator(a[0]).hash() < 0 )
	res += Chars("^-2 ");
    }
  return res;
}

Chars FreeGroupRep::productOfSquares(const Word& w,File& file)
{
  if(!w.freelyReduce().length()) 
    return Chars("trivial");
  
  for( int i = 0 ; i < w.length() ; i++ )
    {
      Generator g = w[i];
      if( w.numberOfOccurrences(g) % 2 )
	return Chars("not in square");
    }

  Chars res;
  Word u1,u2;
  if(isCommutator(w,u1,u2))
    {
      FreeGroup K(theNamesOfGenerators);
      
      K.printWord(file,w);
      file << "  is a commutator, so it can be presented as a product of squares of three elements -  ";
      K.printWord(file, u1.inverse() * u2.inverse() );
      file << "  ,  ";
      K.printWord(file, u2.freelyReduce() );
      file << "  and  ";
      K.printWord(file, (u2.inverse() * u1 * u2).freelyReduce() );
            
      res += square(theNamesOfGenerators,
		    (u1.inverse() * u2.inverse()).freelyReduce());
      res += square(theNamesOfGenerators,u2.freelyReduce());
      res += square(theNamesOfGenerators,
		    (u2.inverse() * u1 * u2).freelyReduce());
      return res;
    }
      
  Word tmp = w.freelyReduce();
  FreeGroup K(theNamesOfGenerators);

  file << "The steps of the rewriting process: " << endl << endl;

  while(tmp.length())
    {
      K.printWord(file,tmp);
      file << " = " << endl << endl << " = ";
      
      int i;
      for( i = 1 ; i < tmp.length() ; i++ )
	{
	  if( Generator(tmp[0]).hash() == Generator(tmp[i]).hash() ||
	      Generator(tmp[0]).hash() == - Generator(tmp[i]).hash() ) 
	    break;
	}
      Word a,b,invb;
      
      if( Generator(tmp[0]).hash() == - Generator(tmp[i]).hash() )
	{ 
	  int j,k;
	  for( j = 1 , k = i - 1 ; k > j ; j++ , k-- )
	    if(Generator(tmp[j]).hash() != - Generator(tmp[k]).hash())
	      break;
	  
	  a = tmp.subword(k+1,i+1);
	  
	  invb = tmp.subword(j,k+1);
	  b = invb.inverse();
	  
	  tmp = (invb * tmp.terminalSegment(tmp.length() - i - 1)).freelyReduce();
	  
	  file << res << oneStep(theNamesOfGenerators,a,b,0) << "  ";
	  K.printWord(file,tmp);
	  
	  if( tmp.length() )
	    file << " = " << endl << endl << " = "; 
     	  
	  res += square(theNamesOfGenerators,
			(a.inverse() * invb).freelyReduce());
	  res += square(theNamesOfGenerators,b);
	  res += square(theNamesOfGenerators,(invb * a * b).freelyReduce());
	  
	  file << res << "  ";
	}
      else
	{
	  a = tmp.initialSegment(i);
	  
	  tmp = (a.inverse() * tmp.terminalSegment(tmp.length() - i)).freelyReduce();
	  
	  res += square(theNamesOfGenerators,a);
	  
	  file << res << "  ";
	}
    }
  return res;
}



void FreeGroupRep::printOn(ostream& ostr) const
{
  ostr << "< ";
  printGenerators( ostr );
  ostr << " >";
}


GroupRep* FreeGroupRep::readFrom(istream& istr, Chars& errMesg) const
{
  PresentationParser P(istr);
  return P.parseFreeGroup( errMesg );
}


// Methods dealing with group elements:


Elt FreeGroupRep::eval( const Word& w ) const
{
  #if SAFETY > 0
    if ( ord(w.maxOccurringGenerator()) > theNumberOfGenerators )
	   error("tried to evaluate Word with no interpretation in FreeGroup");
  #endif
  return w.freelyReduce();
}


Trichotomy FreeGroupRep::areEqual(const Elt& e1, const Elt& e2) const
{
  if ( ((Word&)e1).freelyReduce() == ((Word&)e2).freelyReduce() )
	 return YES;
  return NO;
}


Trichotomy FreeGroupRep::wordProblem( const Word& w ) const
{
  if ( (w.freelyReduce()).length() == 0 )
	 return YES;
  return NO;
}


Trichotomy FreeGroupRep::conjugacyProblem( const Word& u, const Word& v ,Word& c) const
{
  Word con_part_1;
  Word con_part_2;
  Word tmp_w1 = u.freelyReduce();
  Word tmp_w2 = v.freelyReduce();
  Word w1 = tmp_w1.cyclicallyReduce();
  Word w2 = tmp_w2.cyclicallyReduce();
  int frLength;
  if ((frLength = tmp_w1.length()- w1.length()) > 0){
    con_part_1 = tmp_w1.initialSegment(frLength/2);
  }
  if ((frLength = tmp_w2.length() - w2.length()) > 0){
    con_part_2  = tmp_w2.initialSegment(frLength/2);
  }  
  int i = w1.length();  
  if ( i == w2.length() ) {
    do {
      if ( w1 == w2 ) {
	c = con_part_1*con_part_2.inverse();
	return YES;
      }
      if ( --i ){
	con_part_2 *= w2[0];
	w2 = w2.cyclicallyPermute(1);
      }
    } while ( i );
  }
  return NO;
}


Trichotomy FreeGroupRep::conjugacyProblem( const Word& u, const Word& v ) const
{
  Word conjugator;
  return conjugacyProblem(u,v,conjugator);
}


int FreeGroupRep::numberOfElement(const Word& w ) const
{
  int twiceNumGens = 2 * theNumberOfGenerators;

  int result = 0;

  for( int i = 0; i < w.length(); ++i) {
    int gen = 2 * ord(w[i]);
    result = result * twiceNumGens + ( gen > 0 ? gen-1 : -gen );
  }

  return result;
}


/////////////////////////////////////////////////////////////////////////
//
// getN_thElement() and it's accessories
//
// 


Word FreeGroupRep::getN_thWord( int n, int numberOfGenerators ) const
{
  Word result;
  if( n <= 0 ) return result;
  int numOfGens = numberOfGenerators;
  
  do {
    --n;
    int remainder = n % numOfGens + 1;
    n = n/numOfGens;
    Generator g;
    if( odd(remainder) )
      g = Generator( (remainder+1)/2 );
    else
      g = Generator( -remainder/2 );
    result = Word(g) * result;
  } while( n > 0 );
  
  return result;
}


Generator FreeGroupRep::numToGen( int n ) const
{
  if( odd(n) )
    return Generator( (n+1)/2 );
  else
    return Generator( -n/2 );
}


int FreeGroupRep::genToNum( Generator g ) const
{
  int n = ord(g);
  if( n > 0 )
    return n * 2 - 1;
  else
    return -n * 2;
}


Word FreeGroupRep::getN_thElement( int n ) const
{
  Word result;
  if( n <= 0 ) return result;
  int numOfGens = 2 * theNumberOfGenerators;
  int numOfGens2 = numOfGens - 1;
  
  // compute the length of the element

  int length = 0;
  int rem = n;
  int bound = numOfGens;

  while( rem > bound ) {
    rem -= bound; 
    ++length;
    bound *= numOfGens2;
  }
  ++length;
  
  //compute the first letter of the element

  int first = (rem - 1) / (bound / numOfGens) + 1;
  result *= Word( numToGen(first) );
  if( length == 1) return result;

  //compute all others letters

  rem = (rem - 1) % (bound / numOfGens) + 1;
  int numOfCurLen = 1;
  
  for( int i = 0; i < length - 2; ++i ) {
    numOfCurLen *= numOfGens2;
    rem += numOfCurLen;
  }

  Word theRest = getN_thWord(rem, numOfGens2);
  int len = theRest.length();
  int prev = first;

  for( int i = 0; i < len; ++i ) { 
    
    int inv;
    if( odd(prev) )
      inv = prev + 1;
    else
      inv = prev - 1;
    
    int next = genToNum(theRest[i]);
    if( next >= inv ) ++next;
    
    result *= Word( numToGen(next) );
    prev = next;
  }
  
  return result;
}

