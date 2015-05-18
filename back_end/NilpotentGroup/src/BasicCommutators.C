
// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents:
//
// Principal Authors: Sergey Lyutikov, Eugene Paderin
//
// Status: Useable.
//
// Revision History:
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//

#include "BasicCommutators.h"
#include "PolyWordIterator.h"

//====================================================================
//====================== helper stuff ================================
//====================================================================
/*@db
inline int sign(int g) {
  if( g == 0 ) return 0;
  return g > 0 ? 1 : -1;
}
*/
//====================================================================
//====================== class BCEntry ===============================
//====================================================================

bool BCEntry::operator == ( const BCEntry& v ) const {
  if ( weight != v.weight ) return false;
  if ( weight != 1 )
    if ( lPart != v.lPart ) return false;
  return ( rPart == v.rPart );
}

bool BCEntry::operator < ( const BCEntry& v ) const {
  if ( weight < v.weight ) return true;
  if ( weight > v.weight ) return false;
  if ( weight == 1 ) return ( rPart < v.rPart );
  if ( lPart == v.lPart ) return ( rPart < v.rPart );
  return ( lPart < v.lPart );
}

ostream& operator << (ostream& s, const BCEntry& e) {
  s << "[" << e.lPart << "," << e.rPart << "]";
}


//====================================================================
//================ class BasicCommutators ============================
//====================================================================

//------------------------------------------------------------------
//  Constructors and initialization
//------------------------------------------------------------------


//=================== constructor ====================================

BasicCommutators::BasicCommutators(int numgen, int nilclass, bool init) 
  : nilClass(nilclass), numGens(numgen), 
    firstOfWeight(nilclass+2),
    initialized(false)
{

  // quick validity check

  if ( numgen < 1 || nilclass < 1 ) { 
    theNumberOfBC = 0;
    return;
  }

  // Find the number of basic commuatators of each particular
  // weight.
  // The number of basic commutators of weight m is:
  //
  // N(m) = (1/m) ( numgen^m - \sum_{d|m} d * N(d) )
  //
  // (see Witt E. Treue Darstellung Liescher Ringe. J. Reine
  // Angew. Math 117, 152-160, 1937)

  int nm = numgen;

  for ( int m = 1; m <= nilclass; m++ ) {
    int k = nm;
    for ( int d = 1; d < m; d++ )
      if ( ( m % d ) == 0 ) 
	k -= d * firstOfWeight.val(d+1);

    firstOfWeight.ref(m+1) = k / m;
    nm *= numgen;
  }

  firstOfWeight[1] = 1;

  for ( int i = 2; i <= nilclass+1; i++ )
    firstOfWeight[i] += firstOfWeight[i-1];

  theNumberOfBC = firstOfWeight[nilclass + 1] - 1;

  //cout << firstOfWeight << endl << theNumberOfBC << endl;

  if(init) initialize();
}

//=================== special constructor ===============================
//@ep now excluded, see comments in BasicCommutators.h.

/************************************************************************

// A kind of copy constructor, but nilclass can be less than bc.nilClass
BasicCommutators::BasicCommutators(const BasicCommutators& bc, int nilclass)
  : nilClass(nilclass), numGens(bc.numGens),
    firstOfWeight(bc.firstOfWeight),
    entries(bc.entries), 
    initialized(true)
{
  // This is a special constructor, so there are some constraints

  if(bc.nilClass < nilclass || ! bc.initialized)
    error("BasicCommutators: wrong use of special constructor");

  theNumberOfBC = firstOfWeight[nilclass + 1] - 1;
}
***************************************************************************/


//=================== initialize ====================================

void BasicCommutators::initialize() const {      // Logical const!
  
  if(initialized) return;

  BasicCommutators *This  = (BasicCommutators *)this;

  // Filling entries

  This->entries = VectorOf<BCEntry> ( theNumberOfBC + 1 );

  // first `numgen' basic commutators of weight 1 are the generators.
  for (int i = 1; i <= numGens; i++ ) 
    This->entries[i] = i;

  int curEntry = numGens + 1;

  // in this loop we generate commutators of given weight
  for ( int weight = 2; weight <= nilClass; weight++ ) {

    // in this loop we generate [g,h] where g is of weight weight_left 
    for ( int weight_left = (weight + 1) / 2; weight_left < weight; weight_left++ ) {

      // here we generate [left_gen, right_gen]
      for ( int left_gen = firstOfWeight[weight_left]; 
	    left_gen < firstOfWeight[weight_left + 1]; 
	    left_gen++ ) {
	int weight_right = weight - weight_left;
        for ( int right_gen = firstOfWeight[weight - weight_left];
	      right_gen < firstOfWeight[weight_right + 1]; 
	      right_gen++ ) {
	  
          if ( commutatorIsBasic(left_gen, right_gen) ) {
	    This->entries[curEntry++] = BCEntry(left_gen, right_gen, weight);
          }
	}
      }
    }
  }

  This->initialized = true;

  //cout << entries << endl;

}


//------------------------------------------------------------------
//  Methods dealing with basic commutators
//------------------------------------------------------------------


VectorOf<Chars> BasicCommutators::commutatorNames() const {
  VectorOf<Chars> res(theNumberOfBC);
  for(int i=0; i < theNumberOfBC; i++)
    res[i] = commutatorName(i+1);
  return res;
}


//====================== findBC ===================================

//@ep We are just seeking for the entry (g,h) in the table.
//    Could be implemented more effectively.

int BasicCommutators::findBC(int g, int h) const {

  if( generatorsCommute(g, h) ) return 0;

  if(! commutatorIsBasic(g,h) )
    error("Commutator is not basic in BasicCommutators::findBC");

  int res_weight = weightOf(g) + weightOf(h);
  
  for(int i = firstOfWeight[res_weight]; i < firstOfWeight[res_weight + 1]; i++)
    if(entries.val(i).lPart == g && entries.val(i).rPart == h) 
      return i;

  error("Cannot find basic commutator in BasicCommutators::findBC");
  return 0;
}

//--------------------------------------------------------------------
//  Methods for collecting process
//--------------------------------------------------------------------

//======================= commuteLetters ==============================

// If left.gen > right.gen, commutes the letters.
// If left.gen == right.gen, collects them.
// If left.gen < right.gen, returns the word ( left + right ).
PolyWord BasicCommutators::commuteLetters(const Letter& left, const Letter& right) const {

  if(left.gen < right.gen)
    return PolyWord(left) * right;

  if(left.gen == right.gen) {
    Letter res = left;
    if( (res.power += right.power) == 0 )
      return PolyWord();
    else
      return res;
  }

  // The most interesting part begins :)

  // if the generators commute, just swap them

  if( generatorsCommute( ord(left.gen), ord(right.gen) ) )
    return PolyWord(right) * left;

  // if the generators do not commute, we have to produce new letters :(

  PolyWord answer;

  if( commutatorIsBasic( ord(left.gen), ord(right.gen) ) ) {

    PolyWord newWord = findCommutationRelation(left, right);

    // Cj^p Ci^q = Ci NW^p Ci^{q-1}

    answer.append( Letter(right.gen, sign(right.power) ) );
      
    for(int i = 0; i < abs(left.power); i++)
      answer.append(newWord);

    answer.append( Letter(right.gen, right.power - sign(right.power) ) );
      
  }
  else {
  
    // this is a rare case when the left commutator is not basic.
    // so we have to decompose it

    answer.append( decomposeLetter(left) );
    answer.append(right);
  }
	
  return answer;
}

//==================== findCommutationRelation =======================

PolyWord BasicCommutators::findCommutationRelation( Letter Cj, Letter Ci ) const {

  //  cout << "Called findCommutationRelation(" << Cj << ", " << Ci << ")" << endl;

  int i = ord(Ci.gen);
  int j = ord(Cj.gen);
  
  if( ! commutatorIsBasic(j, i) )
     error(" Cannot find commutation relation for improper pair of basic commutators");

  if( Cj.power > 0 && Ci.power > 0 ) {

    // produce relation Ci^-1 Cj Ci = Cj Cji, where Cji is basic

    Cj.power = 1;
    return Cj * Letter( findBC(j, i) );
  }

  else if( Cj.power > 0 && Ci.power < 0 ) {

    // produce relation Ci Cj Ci^-1 = Cj Wji

    Cj.power = 1;

    PolyWord right1 = Cj;
    PolyWord right2;

    int u = j; 

    while(1) {

      u = findBC(u, i);
      if( u == 0 ) break;
      right2 = Letter(u, -1) * right2;

      u = findBC(u, i);
      if( u == 0 ) break;
      right1 = right1 * Letter(u, 1);
    }

    PolyWord right = right1 * right2;
    right.freelyReduce();
    return right;
  }

  else if( Cj.power < 0 && Ci.power > 0 ) {
  
    // produce relation Ci^-1 Cj^-1 Ci = Cj^-1 Wji
    // this is a very special case
    
    //    cout << "This is a case Cj.power < 0 && Ci.power > 0." << endl;

    Letter CjInv(j, -1);

    PolyWord right1 = CjInv;
    PolyWord right2;

    int v = findBC(j, i);

    while(1) {

      if( v == 0 ) break;
      right2 = Letter(v, -1) * right2;

      v = findBC(v, j);
      if( v == 0 ) break;
      right1 = right1 * Letter(v, 1);
 
      v = findBC(v, j);
    }

    PolyWord right = right1 * right2;
    right.freelyReduce();
    return right;
  }
  else {
  
    // produce relation Ci Cj^-1 Ci^-1 = Wji Cj^-1

    Letter CjInv(j, -1);

    PolyWord right1;
    PolyWord right2 = CjInv;

    int u = j; 

    while(1) {

      u = findBC(u, i);
      if( u == 0 ) break;
      right1 = right1 * Letter(u, 1);

      u = findBC(u, i);
      if( u == 0 ) break;
      right2 = Letter(u, -1) * right2;
    }

    PolyWord right = right1 * right2;
    right.freelyReduce();
    return right;
  }
}

//-================== decomposeLetter ===============================

PolyWord BasicCommutators::decomposeLetter( Letter C ) const {
  if( C.power == 0 ) return PolyWord();
  if( ord(C.gen) <= numGens ) return PolyWord( C );

  int left =  leftIndexOf( ord(C.gen) );
  int right = rightIndexOf( ord(C.gen) );

  PolyWord comm;

  if( C.power > 0 ) comm = commutator( PolyWord( Letter(left, 1) ), 
				       PolyWord( Letter(right) ) );
  else comm = commutator( PolyWord( Letter(right, 1) ), 
			  PolyWord( Letter(left) ) );
  
  int count = abs( C.power );

  PolyWord res;
  for(int i = 0; i < count; i++)
    res.append(comm);
  return res;
}


//------------------------------------------------------------------
//  I/O and debugging stuff
//------------------------------------------------------------------


ostream& operator < ( ostream& ostr, const BasicCommutators& BC )
{
  ostr < BC.nilClass < BC.numGens < BC.theNumberOfBC < BC.firstOfWeight
    < BC.initialized < BC.entries;
  return ostr;
}
  
istream& operator > ( istream& istr, const BasicCommutators& bc)
{
  BasicCommutators& BC = (BasicCommutators&)bc;
  istr > BC.nilClass > BC.numGens > BC.theNumberOfBC > BC.firstOfWeight
    > BC.initialized > BC.entries;
  return istr;
}

void BasicCommutators::print( ostream& BClist, const VectorOf<Chars>& genNames ) const {


  NGWordForms WF = *this;

  // form a file

  for(int w = 1; w <= nilClass; w++) {

    // print banner
    BClist << endl << "Commutators of weight " << w << " (total: "
	   << numberOfWeight(w) << ")" << endl << endl;

    // print commutators

    for(int i = firstOfWeight.val(w); i < firstOfWeight.val(w+1); i++) {

      BClist << commutatorName(i);

      if( w > 1) {
	BClist << " = " << WF.commutatorStructure(i);
      }

      BClist << " = " << WF.asBracketedWord(i, genNames) << endl;
    }
  }
}


//===========================================================================
//===================== class NGWordForms ===================================
//===========================================================================


//-----------------------------------------------------------------
//  Conversions to the Word
//-----------------------------------------------------------------


Word NGWordForms::toWord(int c) const {
  int cGen = ::abs(c);

  if( cGen <= BC.numGens ) return Word(Generator(c));

  Word left = toWord( BC.leftIndexOf(cGen) );
  Word right = toWord( BC.rightIndexOf(cGen) );

  Word res = left.inverse() * right.inverse() * left * right;
  res = res.freelyReduce();

  if(c < 0) res = res.inverse();
  return res;
}


// Transforms the given PolyWord to a common Word in terms of the group
Word NGWordForms::toWord(const PolyWord& pw) const {
  Word res;
  ConstPolyWordIterator iter(pw);

  for ( iter.startFromLeft(); !iter.done(); iter.stepRight() ) {
    Letter s = iter.thisLetter();
    Word w = toWord(ord(s.gen));

    if(s.power < 0) {
      s.power = -s.power;
      w = w.inverse();
    }

    for(int i = 0; i < s.power; i++)
      res *= w;

  }
  return res.freelyReduce();
}



// Transforms the word given in terms of basic generators
// to a common Word in terms of the group

Word NGWordForms::toGroupWord(const Word& w) const {
  Word res;

  for ( int i = 0; i < w.length(); i++) {
    int g = ord( w[i] );
    
    if( g > 0 )
      res *= toWord(g);
    else
      res *= toWord(-g).inverse();
  }

  return res.freelyReduce();
}


//-----------------------------------------------------------------
//   Conversions to the text string
//-----------------------------------------------------------------


//--- to string in terms of basic commutators (c1, c2, c3....) ----


Chars NGWordForms::commutatorStructure(int commIndex) const {
  if(commIndex <= BC.numGens) {
    return BC.commutatorName(commIndex);
  }

  Chars res = Chars("[") 
    + BC.commutatorName( BC.leftIndexOf(commIndex) )
    + Chars(", ")
    + BC.commutatorName( BC.rightIndexOf(commIndex) )
    + Chars("]");

  return res;
}


// Outputs the given word in terms of basic commutator names:

Chars NGWordForms::asCommutatorWord(const PolyWord& w) const {
  Chars res;

  if( w.isEmpty() ) return Chars("1");

  ConstPolyWordIterator iter(w);

  for ( iter.startFromLeft(); !iter.done(); iter.stepRight() ) {
    res += asCommutatorWord( iter.thisLetter() ) + " ";
  }
  return res;
}
  
// Outputs the given letter in terms of commutator names:

Chars NGWordForms::asCommutatorWord(Letter s) const {
  Chars res;
  res += BC.commutatorName(ord(s.gen));
  if(s.power != 1)
    res += Chars("^") + Chars(s.power);
  return res;
}
  

//---- to string in terms of group generators ----------------------
//  Names of the generators are provided


//@njz
//Chars NGWordForms::asBracketedWord(int commIndex,
//			   const VectorOf<Chars>& genNames,
//			   bool embeddingBrackets = true) const {
Chars NGWordForms::asBracketedWord(int commIndex,
			   const VectorOf<Chars>& genNames,
			   bool embeddingBrackets) const {
//
  if(commIndex <= BC.numGens)
    return genNames[commIndex-1];
  else {
    Chars res = asBracketedWord(BC.leftIndexOf(commIndex), genNames, false) +
      "," + asBracketedWord(BC.rightIndexOf(commIndex), genNames, true);
    if(embeddingBrackets)
      res = Chars("[") + res + Chars("]");
    return res;
  }
}


// Outputs word in the bracketed form (in terms of group generators)

Chars NGWordForms::asBracketedWord(const PolyWord& w, 
				   const VectorOf<Chars>& genNames) const {

  Chars res;
  ConstPolyWordIterator iter(w);

  if( w.isEmpty() ) return Chars("1");

  for ( iter.startFromLeft(); !iter.done(); iter.stepRight() ) {
    Letter s = iter.thisLetter();
    res += asBracketedWord(ord(s.gen), genNames);
    if(s.power != 1)
      res += Chars("^") + Chars(s.power);
    res += Chars(" ");
  }
  return res;
}

