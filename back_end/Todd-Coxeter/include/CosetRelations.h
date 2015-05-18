// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definition of class CosetRelation, CosetRelationsSet.
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


#ifndef _COSET_RELATIONS_H_
#define _COSET_RELATIONS_H_

#include "Word.h"
#include "BTree.h"
#include "Vector.h"

// ----------------- CosetRelationsSet ----------------------- //
// This is internal class for Todd-Coxeter

class CosetRelationsSet {
public:
  CosetRelationsSet(int num_of_gens):
    relations(num_of_gens*2),
    numOfCollisions(0),
    theCollisions(4){}
  
  const BTree<int,int>& relationsOf(Generator i)const 
    {return relations.constref(indexRelOf(i));}
  // Returns relators, corresponding to the given generator
  // Relations are pairs in BTree of type <Key><Generator> = <Value>
  // Where Key and Value are corresponding Key and Value of a BTree and
  // Generator is the given generator.

  bool addRelation(int first,const Generator& g, int second,bool inverse=false);
  // Adds relation (relation is first*g = second) to the relations set.
  // Looks for collisions and record them if occurs

  int getRelationNumber(int ,const Generator&)const;
  // Returns the right part of relation 

  void replaceCosets(int good,int bad);
  // Replaces all cosets in relation set equal too 'bad' with 'good' 
 
  int numberOfCollisions()const;
  // Number of collisions, remain in theCollisions

  bool removeCollision(int); 
  // Removes i-th collision from theCollisions. 

  bool collision(int& good,int& bad)const;
  // True if good theCollisions is not empty.
  // Returns good and bad items of current collision

  int indexRelOf(const Generator& g)const;
  // Returns index in relations vector, corresponding 
  // to the given generator

  Generator genOfRels(int i)const;
  // returns Generator, corresponding to given index
 
  
  // I/O
  friend ostream& operator << ( ostream& ostr, const CosetRelationsSet& cr )
  {
    for (int i=0;i<cr.relations.length();i++){
      BTreeIterator<int,int> I(cr.relations.constref(i));
      for (I.reset();!I.done();I.next()){
	ostr << I.getKey() << "g" << i+1 <<"="<<I.getValue() <<", ";
      }
      ostr << endl;
    }
    return ostr;
  }  


private:
  CosetRelationsSet(const CosetRelationsSet&);
  // Copy constructor prohibited

  void addCollision(int bad,int good);
  // add collision to the collision list

  int numOfCollisions;
  // Current number of collisions

  VectorOf<BTree<int,int> > relations;
  // Set of relations

  BTree< int, int > theCollisions;
  // List of collisions
};

#endif
























