// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the AbelianGroupRep class
//
// Principal Author: Dmitry Bormotov, Alexei Myasnikov
//
// Status: in progress
//
// Usage:
//
// Special Notes:
//
// Revision History:
//


#ifndef _ABELIAN_GROUP_REP_H_
#define _ABELIAN_GROUP_REP_H_


#include "AbelianWord.h"
#include "FPGroup.h"
#include "Matrix.h"
#include "PrimeNumbers.h"
#include "File.h"

// ---------------------------- AbelianGroupRep ---------------------------- //


class AbelianSGPresentationRep;

class AbelianGroupRep : public PureRep
{

public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Constructors:                                                      //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  AbelianGroupRep( const FPGroup& G, bool makeFile = false );
  // Constructs an abelian group with presentation G.
  // If bMakeFile is true, expression of new generators in terms
  // of old ones will be written in a file after the computation
  // is finished.
  
  // No default constructor, but it can be easily arranged by
  // combining default FPGroup constructor and previsious one.

  // Copy constructor and operator = provided by compiler


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void computeCyclicDecomposition( );
  // *** Time consuming algorithm ! ***
  // Computes cyclic decomposition of the abelian group.

  void findPrimaryBasis();
  // *** Time consuming algorithm ! ***
  // Computes primary decomposition of the abelian group.

  bool haveCyclicDecomposition( ) const { return bHaveCyclicDecomposition; }
  // True if the cyclic decomposition has been computed.

  bool havePrimaryDecomposition( ) const { return primeBasisFound; }
  // True if the primary decomposition has been computed.

  Chars AbelianGroupRep::getFileName( ) const;
  // Returns name of the file containing new generators.
  // Calls error() if no file has been created.
  
  Chars getFileNameOfPDGens( ) const;
  // Returns name of the file containing generators for prime decomposition.
  // Calls error() if no file has been created.

  const FPGroup getFPGroup( ) const { return theGroup; }
  // Returns the original group presentation.


  SetOf<Word> getAllRelators( ) const;  
  // Returns all relators of the original group plus all commutators.


  AbelianWord oldInAbelianForm( const Word& w ) const {
    return AbelianWord( theGroup.numberOfGenerators(), w );
  }
  // Computes the abelian form of a word written in terms of 
  // the old generators.

  /////////////////////////////////////////////////////////////////////////
  //  
  //  The rest of the public functions in the class can be used only if
  //  the cyclic decomposition is computed.
  //
  //

  int rankOfFreeAbelianFactor( ) const;
  // Returns rank of the free part.

  VectorOf<Integer> invariants( ) const;
  // Return the invariants m_i such that m_i | m_{i+1}. All m_i > 1.

  VectorOf<AbelianWord> oldToNewGens() const;
  // Returns images of the old generators in terms of the
  // new set of generators.
    
  VectorOf<AbelianWord> newToOldGens() const;
  // Returns images of the new generators in terms of the
  // new set of generators.

  AbelianGroupRep getCanonicalSmithPresentation( ) const;
  // Returns the cyclic decomposition as an object of the class 
  // AbelianGroupRep. In that object the new generators and the old 
  // generators are the same; the cyclic decomposition, invariants, 
  // the rank of free part and transformation vectors are already computed.
    
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Methods and operators which deal with the group:                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Integer order() const;
  // Returns order of the group, 0 means infinity.

  bool isTrivial() const;
  // Returns true if the group is trivial.

  bool isFree() const;
  // Returns true if the group is free.
 
  bool isomorphicTo( const AbelianGroupRep& G) const;
  // Returns true if the group is isomorphic to G.

  AbelianGroupRep computeIntegralHomology( int n ) const;
  // Returns the H_n group of this one.
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Methods and operators which deal with subgroups:                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Integer orderOfTheTorsionSubgroup() const;
  // Returns order of the torsion subgroup.
  
  AbelianSGPresentationRep makeSubgroupPresentation(const VectorOf<Word>& vG) const;
  // *** Time consuming algorithm ! ***
 // Makes presentation of subgroup as a group 

  VectorOf<Word> findSubgroupIsolator(const VectorOf<Word>& vG) const;
  // *** Time consuming algorithm ! ***
  // Returns the generators of Isolator of given subgroup

  VectorOf<Word> findVirtFreeComplementOfSG(const VectorOf<Word>& vG) const;
  // *** Time consuming algorithm ! ***
  // returns generators of virtual free complement, of a given subgroup

  VectorOf<Word> joinSubgroups(const VectorOf<Word>& vG1,const VectorOf<Word>& vG2) const;
  // Returns the generators of join subgroup of given groups
  
  VectorOf<Word> findSubgIntersection( const VectorOf<Word>& vG1 ,
				       const VectorOf<Word>& vG2 , 
				       File& file ) const;
  // Returns the generators for intersection of given subgroups   
  
  bool isPureCyclSubgroup(const Word& w) const;
  // True if given cyclic subgroup is pure
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Methods which deal with group elements:                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool areEqual( const Word&, const Word& ) const;
  // Returns true if u and v are equal.

  bool isTrivial( const Word& ) const;
  // Word problem.

  Integer orderOfElt( const Word& ) const;
  // Returns the order of an element, 0 means infinity.

  AbelianWord newToOldGens( const AbelianWord& ) const;
  // Converts an element from the new generators to the old ones.

  AbelianWord oldToNewGens( const AbelianWord& ) const;
  // Converts an element from the old generators to the new ones.

  AbelianWord findEltPrimeForm(const Word& w) const;
  // Converts an element from the old generators to the prime form ones.
  
  AbelianWord pBlockOfElt( const AbelianWord& w,Integer p )const;
  // Returns a p-block of an element. 

  Integer pHeightOfElt(const Word& w, const Integer& p) const;
  // Returns a p-height of a given alement.

  Integer powerOfEltInSubgroup(const Word& w,const VectorOf<Word>& sGroup) const;
  // Returns the power of element in subgroup

  bool isEltProperPower(const Word& w) const;
  // *** Time consuming algorithm ! ***
  // True if element is proper power

  void abelianMaximalRoot(const Word& w, Word& maxRoot, Integer& maxExp) const;
  // *** Time consuming algorithm ! ***
  // Compute the maximal root of an element. Put maximal root in  maxRoot and
  // maximal exponent in maxExp. If maxExp is 0, than there is no maximal root fpr
  // this element

  AbelianWord primeFormInOldGens(const AbelianWord& w) const;
  // Converts an element in prime form  to the old generators.

  int isPowerOfSecond(const Word& word1, const Word& word2) const;
  // *** Time consuming algorithm ! ***
  // If word1 is a power of word2, returns the power, if not returns 0

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Methods which deal with morpgisms:                                 //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Bool isEpimorphism(const VectorOf<Word>& V) const;
  // *** Time consuming algorithm ! ***
  // Returns TRUE iff generating vector `V' defines an 
  // epimorphism of this group. The length of `V' should be
  // equal to the number of generators of this group. 

  int orderOfAuto(const VectorOf<Word>& V) const;
  // *** Time consuming algorithm ! ***
  // Returns the order of Automorphism defined by V. 
  // If check == true, checks is it automorphism.

  VectorOf<Word> inverseAuto(const VectorOf<Word>& V) const;

  // *** Time consuming algorithm ! ***
  // Returns the inverse  of automorphism, defined by V. 
  // Does not make check is it automorphism or not!!!

  VectorOf<Word> fixedPointsOfAuto( const VectorOf<Word>& v ) const;
  // Returns the generating set for the subgroup of fixed points of the 
  // automorphism defined by v.
  // Does not make check is it automorphism or not!!!

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // I/O:                                                                //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  void printOn( ostream& ) const;
  // Print the cyclic decomposition in the form, like Z^5 x Z_2 x Z_4^2 ...
  
  void printWordInNewGens( ostream&, const AbelianWord& ) const;
  // Print a word in the new generators in additive notation.

  void printInPrimaryForm(ostream& ostr, const AbelianWord& aw) const;
  // Print a word in primary form: f1 + 2 p2 ... (aw must be in prime form)

  void printPrimaryDec( ostream& ostr) const;
  // Print the prymary decomposition in the form, like Z^5 x Z_2 x Z_4 x Z_3 ...

 
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  virtual void write( ostream& ostr ) const;

  virtual void read( istream& istr );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Representation methods:                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  AbelianGroupRep* clone( ) const { return new AbelianGroupRep(*this); }
  // overrides PureRep::clone()

protected:
  Chars theFileName;       
  // Name of that file.

  Chars theFileNameOfPD;
  // Name of file, with generators for Primary decomposition.

  int numOfGensInTorsionPartOfPD() const{
     return primeBasicMatrix.height();
  }
  // Returns the number of generators in torsion part of primary decomposition
private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const FPGroup theGroup;  
  // The original group.

  bool bMakeFile;          
  // If true the file with the new generators will be created.  
  
  bool bHaveCyclicDecomposition; 
  // True if the cyclic decomposition has been computed.

  int numOfNewGens;              
  // Number of the new generators, i.e rank of free part plus
  // number of invariants in the group.
  
  int rankOfFreePart;
  // The number of free generators in the cyclic decompostion.

  VectorOf<Integer> theInvariants;
  // Contains invariants m_i such that m_i | m_{i+1}. All m_i > 1.
  
  VectorOf<AbelianWord> theNewToOldGens;
  // Images of the new generators in terms of the new set of generators.

  VectorOf<AbelianWord> theOldToNewGens;
  // Images of the old generators in terms of the new set of generators.

  bool primeBasisFound;
  // True if the primary decomposition has been computed.

  int invariantToNewGens( int inv, Integer orderOfCyclic,
			   int stPos,Integer power = 1);
  // Computes the image of generators of canonical decomposition
  // in generators of primary decomposition.
  // Warning! Don't call this procedure unless you know what are you
  // doing.

  DArray<Integer> primeBasicMatrix;
  // Matrix with prime decompozition information. It consists of
  // primes, their powers (which are the orders of cyclic subgroups),
  // index of invariants, from wich primes were found, and images
  // of the primary generators in terms of the set of generators of
  // canonical decomposition and vise versa.

  // The following data members are suppose to be local variables 
  // in the function computeCyclicDecomposition(), but they are here
  // to avoid sending them all the time to that function`s auxiliary 
  // methods. 

  Integer** matrix; // the main matrix containing all the original relators
  int height;       // height of the main matrix
  int width;        // width of the main matrix
  Integer** newToOldGensMatrix; // transformation matrix
  Integer** oldToNewGensMatrix; // transformation matrix


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Private methods:                                                    //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////
  //
  // The following list of functions are for computeCyclicDecomposition()
  // only.
  //
  //

  void fillTransformationVectors( );
  // Commit the necessary information from the transformation matrices
  // to the transformation vectors. 

  void addColumn( int i, int j, Integer k );
  // Add k j-th columns to the i-th column in matrix, 
  // the transformation matrices changes accordingly.
  
  void swapColumns( int i, int j );
  // Swaps the i-th and j-th columns in matrix,
  // the transformation matrices changes accordingly.

  void swapGenColumns( int i, int j );
  // To be used in swapColumns(),
  // the transformation matrices changes accordingly.
 
  void canoniseInvariants( int i, int j );
  // Writes invariants in the proper order

  void swapInvariants( int i, int j );
  // To be used in canoniseInvariants()
  
  void makeTransformationMatrices( );
  // Initilaizes the transformation matrices.
  
  void makeMainMatrix( );
  // Initilaizes the main matrix.

  void destroyMatrices( );
  // Destroys all three matrices.

  virtual void makeFile( );
  // Creates the file containing the expression of the new generators
  // in terms of the old ones.

  virtual void makeFileOfPDGens( );
  // Creates the file containing the expression of the primary generators
  // in terms of the old ones.

  void sortVector(DArray<Integer>& vc,int colSort,int start, int finish);
  void sortPrimeDecom(DArray<Integer>& m);
  // This procedures makes sort of primeBasicMatrix.  
  
  void minimizeWordInNewGens( AbelianWord& w ) const;
  // Garanties that all powers of generators of the torsion part will
  // be less than corresponding orders of the generators.

  bool isAllZero(int from, int to,MatrixRow<Integer>& vc) const;
  // Checks the subvector elements to be zero, returns true if all zeros

  Bool matrixMult(const Matrix<Integer>& m,bool haveTorsion) const;

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Debugging stuff:                                                    //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  
#ifdef debug_AbelianGroup
  void printMatrix(char *name, Integer **A, int height, int width);
  friend int main( );
#endif

};

#endif



