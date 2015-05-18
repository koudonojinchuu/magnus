// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definition of class PermutationRepresentation, CosetEnumerator.
//
// Principal Author: Alexey Myasnikov
//
// Status: Useable
//
// Usage:
//
// Revision History:
//
//


#ifndef _COSET_ENUMERATOR_H_
#define _COSET_ENUMERATOR_H_

#include "CosetTables.h"
#include "FPGroup.h"

// ------------------------- PermutationRepresentation --------------//
class PermutationRepresentation {
public:  
  PermutationRepresentation():permTable(NULL), numberOfGens(0) { }

  PermutationRepresentation(const PermutationRepresentation& p);
  
  ~PermutationRepresentation() { 
    if (permTable)
      delete [] permTable;
  }
  
  PermutationRepresentation& operator = ( const PermutationRepresentation& p);

  void printOn(const VectorOf<Chars>& n, ostream& ostr)const;
  // prints permutations : x1 = (1 2 5) (7 8) ...
  // n - names of generators Xi

  Word representativeOf(const Word& )const;
  //returns a representative of w

  const VectorOf<Word>& getRepresentatives()const{
    return representatives;
  }
  // returns vector of representatives

  bool isEmpty()const { return  permTable == NULL;}

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend ostream& operator < (ostream& ostr,
			      const PermutationRepresentation& PR )
  {
    ostr < PR.numberOfGens;
    ostr < PR.representatives;
    for (int i=0;i<PR.numberOfGens;i++)
      ostr < PR.permTable[i];
    return ostr;
  }
  
  friend istream& operator > ( istream& istr, PermutationRepresentation& PR)
  {
    istr > PR.numberOfGens;
    istr > PR.representatives;
    if (PR.permTable)
      delete [] PR.permTable;
    PR.permTable = new ListOf< VectorOf<int> >[PR.numberOfGens];
    for (int i=0;i<PR.numberOfGens;i++)
      istr > PR.permTable[i];    
    return istr;
  }
 
private:
  friend class CosetEnumerator;
  PermutationRepresentation(int numOfGens):
    numberOfGens(numOfGens),
    permTable(new ListOf< VectorOf<int> >[numOfGens])   
    {}

  void addCycle(int g,const VectorOf<int>& v);
  
  void setRepresentatives(const VectorOf<Word>& r){
    if (r.length()==0)
      error("void setRepresentatives(const VectorOf<Word>& r) : "
	    "wrong parameter");
    representatives = r;
  }

  int searchNext(const Generator& g,int coset)const;

  ListOf< VectorOf<int> >* permTable;    
  int numberOfGens;
  VectorOf<Word> representatives;
};

// ------------------------- CosetEnumerator --------------//

class CosetEnumerator {
public:

  // First constructors check for group(subgroup) be infinite
  // if so happens, than no coset enumeration proceeded, and
  // no representatives are built. So before asking for
  // representaties or permutations check order(index) be
  // more than 0.

  CosetEnumerator(const FPGroup& group); 
  // Constructor for order of a group problem
  CosetEnumerator(const FPGroup& group,const VectorOf<Word>& subgroup);
  // Constructor for index of a subgroup problem


  ~CosetEnumerator();
  /////////////////////////////////////////////////////////////////////// 
  //                                                                   //
  // Methods                                                           //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////

  int enumerate();
  // Returns a order of the group, or index of the subgroup
  // in the group. It depends of which constructor was called
  
  const VectorOf<Word> schreierRepresentatives()const{
    if (schreierRepres.length() == 0)
      error("const VectorOf<Word> schreierRepresentatives()const :"
	    "cosets have to be enumerated first.");
    return schreierRepres;
  }
  // Returns Schreier representatives. makeRepresentatives() has to be
  // called before.

  const PermutationRepresentation& permutationRepresentation()const{
    return thePermutationRepresentation;
  }
  // Returns Schreier representatives. makeRepresentatives() has to be
  // called before.

  /////////////////////////////////////////////////////////////////////// 
  //                                                                   //
  // Operators                                                         //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////
  

private:
  bool addCoset(int coset_num);
  // Adds a new coset to tables
  void  iterateTables( );
  // Filles tables
  void removeCollisions( );
  // removes current collisions, which exist in
  // set of relations
  bool infinite()const;
  // True if an abelianization of the group is infinite
  CosetEnumerator(const CosetEnumerator& );
  // Copy constructor is not allowed
  void makeRepresentatives(); 
  // fills schreierRepres vector, with Schreier representatives
  void makePermutations();

  CosetRelationsSet theRelationSet;
  // set of the ralations. It`s unique
  CosetTable** tables;
  // array of all coset tables
  FPGroup theGroup;
  VectorOf<Word> theSubgroup;
  // Generators of the subgroup (if exists)
  int theOrder;
  // computed order of the group or index of the subgroup
  bool haveFastSolution;
  // True if have fast sulution (I made some 
  //fast checks in constructors)
  int numberOfGroupRelators;
  // Number of tables made for group relators
  int numberOfTables;
  // Number of tables 
  VectorOf<Integer> cosetNumbers;
  VectorOf<Word> schreierRepres;
  PermutationRepresentation thePermutationRepresentation;
  BTree<int,int> cosets2repres;

};
#endif































































