// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the AbelianGroup class
//
// Principal Author: Dmitry Bormotov, Alexei Myasnikov
//
// Status: in progress
//
// Usage:
//
//   The class AbelianGroup incapsulates all abelian algoritithms in Magnus.
//   The function computeCyclicDecomposition() is suppose to be called before
//   you can start computing anything else. We denote the free generators
//   of the cyclic decomposition by f1,f2,...,  and the generators of the
//   torsion part by t1,t2,... . Those generators are often called
//   "the new generators" to distinct them from "the old generators", i.e.
//   generators of the original group.
//
//
// Special Notes:
//
// Revision History:
//


#ifndef _ABELIAN_GROUP_H_
#define _ABELIAN_GROUP_H_

#include "FPGroup.h"
#include "AbelianGroupRep.h"

// ----------------------------- AbelianGroup ------------------------------ //

class AbelianSGPresentation;

class AbelianGroup : public ObjectOf<AbelianGroupRep>
{

public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Constructors:                                                      //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  AbelianGroup( const FPGroup& G, bool makeFile = false )
    : ObjectOf<AbelianGroupRep>( new AbelianGroupRep(G, makeFile) )
  { }
  // Constructs an abelian group with presentation G.
  // If bMakeFile is true, expression of new generators in terms
  // of old ones will be written in a file after the computation
  // is finished.
  
  // No default constructor, but it can be easily arranged by
  // combining default FPGroup constructor and previsious one.

  // Copy constructor and operator = provided by compiler
  
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Accessors:                                                         //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
 
  void computeCyclicDecomposition( ) {
    change()->computeCyclicDecomposition();
  }
  // *** Time consuming algorithm ! ***
  // Computes cyclic decomposition of the abelian group.

  void findPrimaryBasis() {
    change()->findPrimaryBasis();
  }
  // *** Time consuming algorithm ! ***
  // Computes primary decomposition of the abelian group.  

  bool haveCyclicDecomposition( ) const {
    return look()->haveCyclicDecomposition();
  }
  // True if the cyclic decomposition has been computed.

  bool havePrimaryDecomposition( ) const {
    return look()->havePrimaryDecomposition();
  }
  // True if the primary decomposition has been computed.
  
  Chars getFileName( ) const {
    return look()->getFileName();
  }
  // Returns name of the file containing new generators.
  // Calls error() if no file has been created.

  Chars getFileNameOfPDGens( ) const{
    return look()->getFileNameOfPDGens();
  }
  // Returns name of the file containing generators for prime decomposition.
  // Calls error() if no file has been created.

  const FPGroup getFPGroup( ) const {
    return look()->getFPGroup();
  }
  // Returns the original group presentation.


  SetOf<Word> getAllRelators( ) const {
    return look()->getAllRelators();
  }
  // Returns all relators of the original group plus all commutators.


  AbelianWord oldInAbelianForm( const Word& w ) const {
    return look()->oldInAbelianForm( w );
  }
  // Computes the abelian form of a word written in terms of 
  // the old generators.
  

  /////////////////////////////////////////////////////////////////////////
  //  
  //  The rest of the public functions in the class can be used only if
  //  the cyclic decomposition is computed.
  //
  //
  
  int rankOfFreeAbelianFactor( ) const {
    return look()->rankOfFreeAbelianFactor();
  }
  // Returns rank of the free part.

  
  VectorOf<Integer> invariants( ) const {
    return look()->invariants();
  }
  // Return the invariants m_i such that m_i | m_{i+1}. All m_i > 1.

  
  VectorOf<AbelianWord> oldToNewGens() const {
    return look()->oldToNewGens();
  }
  // Returns images of the old generators in terms of the
  // new set of generators.
  
  VectorOf<AbelianWord> newToOldGens() const {
    return look()->newToOldGens();
  }
  // Returns images of the new generators in terms of the
  // new set of generators.
  
  AbelianGroup getCanonicalSmithPresentation( ) const {
    return ( new AbelianGroupRep( look()->getCanonicalSmithPresentation() ) );
  }
  // Returns the cyclic decomposition as an object of the class AbelianGroup.
  // In that object the new generators and the old generators are the same;
  // the cyclic decomposition, invariants, the rank of free part and
  // transformation vectors are already computed.


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Methods and operators which deal with the group:                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  Integer order() const {
    return look()->order();
  }
  // Returns order of the group, 0 means infinity.


  bool isTrivial() const {
    return look()->isTrivial();
  }
  // Returns true if the group is trivial.


  bool isFinite() const { return ( order() != 0 ); } 
  // Returns true if the group is finite.

  
  bool isInfinite() const { return !isFinite(); }
  // Returns true if the group is infinite.

  
  bool isFree() const {
    return look()->isFree();
  }
  // Returns true if the group is free.

  
  bool isomorphicTo( const AbelianGroup& G) const {
    return look()->isomorphicTo( *G.look() );
  }
  // Returns true if the group is isomorphic to G.


  AbelianGroup computeIntegralHomology( int n ) const{
    return ( new AbelianGroupRep( look()->computeIntegralHomology( n ) ) );
  }
  // Returns the H_n group of this one.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Methods and operators which deal with subgroups:                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Integer orderOfTheTorsionSubgroup( ) const {
    return look()->orderOfTheTorsionSubgroup();
  }
  // Returns order of the torsion subgroup.

  AbelianSGPresentation makeSubgroupPresentation(const VectorOf<Word>& vG) const;

  // Makes presentation of subgroup 

  VectorOf<Word> findSubgroupIsolator(const VectorOf<Word>& vG) const{
      return look()->findSubgroupIsolator(vG);
  }
  // *** Time consuming algorithm ! ***
  // Returns the generators of Isolator of given subgroup

  VectorOf<Word> findVirtFreeComplementOfSG(const VectorOf<Word>& vG) const{
      return look()->findVirtFreeComplementOfSG(vG);
  }
  // *** Time consuming algorithm ! ***
  // returns generators of virtual free complement, of a given subgroup

  VectorOf<Word> joinSubgroups(const VectorOf<Word>& vG1,const VectorOf<Word>& vG2) const{
      return look()->joinSubgroups(vG1,vG2);
  }
  // *** Time consuming algorithm ! ***
  // Returns the generators of join subgroup of given groups
  
  VectorOf<Word> findSubgIntersection( const VectorOf<Word>& vG1 ,
				       const VectorOf<Word>& vG2 , 
				       File& file ) const
  {
    return look()->findSubgIntersection(vG1,vG2,file);
  }
  // Returns the generators for intersection of given subgroups  
  
  bool isPureCyclSubgroup(const Word& w) const{
     return look()->isPureCyclSubgroup(w);
  }
  // True if given cyclic subgroup is pure
 
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Methods which deal with group elements:                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool areEqual(const Word& u, const Word& v) const {
    return look()->areEqual(u,v);
  }
  // Returns true if u and v are equal.
  

  bool isTrivial( const Word& w ) const {
    return look()->isTrivial(w);
  }
  // Word problem.

  
  Integer orderOfElt( const Word& w ) const {
    return look()->orderOfElt(w);
  }
  // Returns the order of an element, 0 means infinity.


  AbelianWord newToOldGens( const AbelianWord& w ) const {
    return look()->newToOldGens(w);
  }
  // Converts an element from the new generators to the old ones.

  AbelianWord oldToNewGens( const AbelianWord& w ) const {
    return look()->oldToNewGens(w);
  }
  // Converts an element from the old generators to the new ones.

  AbelianWord findEltPrimeForm(const Word& w) const{
    return look()->findEltPrimeForm(w);
  }
  // Converts an element from the old generators to the prime form ones.


  AbelianWord pBlockOfElt( const AbelianWord& w,Integer p )const{
    return look()->pBlockOfElt(w,p);
  }
  // Returns a p-block of an element. 
  
  AbelianWord pBlockOfElt( const Word& w,Integer p )const{
    return look()->pBlockOfElt( findEltPrimeForm(w) , p );
  }
  // Returns a p-block of an element. 
  
      
  Integer pHeightOfElt(const Word& w, const Integer& p) const{
    return look()->pHeightOfElt(w,p);
  }
  // *** Time consuming algorithm ! ***
  // Returns a p-height of a given alement.
 
  Integer powerOfEltInSubgroup(const Word& w,const VectorOf<Word>& sGroup) const{
      return look()->powerOfEltInSubgroup(w,sGroup);
  }
  // Returns the power of element in subgroup

  bool isEltProperPower(const Word& w) const{
      return look()->isEltProperPower(w);
  }
  // *** Time consuming algorithm ! ***
  // True if element is proper power

  void abelianMaximalRoot(const Word& w, Word& maxRoot, Integer& maxExp) const{
       look()->abelianMaximalRoot(w, maxRoot, maxExp);
  }
  // *** Time consuming algorithm ! ***
  // Compute the maximal root of an element. Put maximal root in  maxRoot and
  // maximal exponent in maxExp. If maxExp is 0, than there is no maximal root fpr
  // this element

  AbelianWord primeFormInOldGens(const AbelianWord& w) const{
      return look()->primeFormInOldGens(w);
  }
  // Converts an element in prime form  to the old generators.
 
  int isPowerOfSecond(const Word& word1, const Word& word2) const{
      return look()->isPowerOfSecond(word1,word2);
  }
  // *** Time consuming algorithm ! ***
  // If word1 is a power of word2, returns the power, if not returns 0
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Methods which deal with morphisms:                                 //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Bool isEpimorphism(const VectorOf<Word>& V) const{
      return look()->isEpimorphism(V);
  }
  // *** Time consuming algorithm ! ***
  // Returns TRUE iff generating vector `V' defines an 
  // epimorphism of this group. The length of `V' should be
  // equal to the number of generators of this group. 

  int orderOfAuto(const VectorOf<Word>& V) const{
      return look()->orderOfAuto(V);
  }
  // *** Time consuming algorithm ! ***
  // Returns the order of Automorphism defined by V. 
  // Does not make check is it automorphism or not!!!
  // Returns -1 if infinite (temporary, if order > 1000)

  VectorOf<Word> inverseAuto(const VectorOf<Word>& V) const{
    return look()->inverseAuto(V);
  }
  // *** Time consuming algorithm ! ***
  // Returns the inverse  of automorphism, defined by V. 
  // Does not make check is it automorphism or not!!!

  VectorOf<Word> fixedPointsOfAuto( const VectorOf<Word>& v ) const{
    return look()->fixedPointsOfAuto( v );
  }
  // Returns the generating set for the subgroup of fixed points of the 
  // automorphism defined by v.
  // Does not make check is it automorphism or not!!!
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // I/O:                                                                //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend ostream& operator << ( ostream& ostr, const AbelianGroup& G )
  {
    G.look()->printOn(ostr);
    return ostr;
  }
  // Print the cyclic decomposition in the form, like Z^5 x Z_2 x Z_4^2 ...

  
  void printWordInNewGens( ostream& ostr, const AbelianWord& w) const {
    look()->printWordInNewGens(ostr, w);
  }
  // Print a word in the new generators in additive notation.

  void printInPrimaryForm(ostream& ostr, const AbelianWord& aw) const{
      look()->printInPrimaryForm(ostr,aw);
  }
  // Print a word in primary form: f1 + 2 p2 ... (aw must be in prime form)

  void printPrimaryDec( ostream& ostr) const{
      look()->printPrimaryDec(ostr);
  }
  // Print the prymary decomposition in the form, like Z^5 x Z_2 x Z_4 x Z_3 ...

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend ostream& operator < ( ostream& ostr, const AbelianGroup& G )
  {
    G.look()->write(ostr);
    return ostr;
  }
  
  friend istream& operator > ( istream& istr, AbelianGroup& G )
  {
    G.change()->read(istr);
    return istr;
  }


protected:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Protected functions:                                                //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  friend AbelianSGPresentationRep AbelianGroupRep::
                       makeSubgroupPresentation(const VectorOf<Word>& vG)const;
  AbelianGroup( AbelianGroupRep* newrep ) 
    : ObjectOf<AbelianGroupRep>(newrep) { }
  // Special wrapping constructor to wrap new representations

};

#endif
