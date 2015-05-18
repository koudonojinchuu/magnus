// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementations of classes CosetRelation, CosetRelationsSet. 
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


#include "CosetRelations.h"

int CosetRelationsSet::numberOfCollisions() const {return numOfCollisions;}

bool CosetRelationsSet::collision(int& good,int& bad)const
{
  BTreeIterator<int,int> I(theCollisions);
  I.reset();
  if (I.done()) return false;
  bad = I.getKey();
  good = I.getValue();
  return true;
}

bool CosetRelationsSet::removeCollision(int key) 
{
  if (theCollisions.remove(key)) {
    numOfCollisions--;
    //cout << "After removing key - " << key <<":"<<endl ; 
    //cout << "Number of collisions :" << numOfCollisions << endl ; 
    //theCollisions.printAll(); 
    return true;
  }
  return false;  
}

void CosetRelationsSet::addCollision(int bad,int good)
{
  int* oldGood = theCollisions.search(bad);
  if (oldGood == NULL){
    theCollisions.insert(bad,good);
    numOfCollisions++;
    //cout << "After add collision " << bad << "-" << good << endl; 
    //cout << "Number of collisions :" << numOfCollisions << endl ; 
    //    theCollisions.printAll(); 
    return;
  }
  if (*oldGood > good){
    addCollision(*oldGood,good);
    *oldGood = good;
  }

  if (*oldGood < good){
    addCollision(good,*oldGood);
  }
  //cout << "After add collision " << bad << "-" << good << endl; 
  //cout << "Number of collisions :" << numOfCollisions << endl ; 
  //theCollisions.printAll(); 
}


void CosetRelationsSet::replaceCosets(int good,int bad)
{
  int numOfGens = relations.length();
  for (int i = 0;i<numOfGens;i++){
    BTree< int,int >& tmpTree = relations.ref(i);
    int* savCoset;
    if ((savCoset = tmpTree.search(bad))!=NULL){
      //      cout << "Removing key :" << bad <<
      //	" in table for generator: "<< i+1 << endl;
      int coset = *savCoset;
      tmpTree.remove(bad);
      addRelation(good,genOfRels(i),coset);
    }
  }
}
int CosetRelationsSet::indexRelOf(const Generator& g)const
{
  int numOfGens = relations.length()/2;
  int indexOfRelations = ord(g);
  if (indexOfRelations < 0)
    indexOfRelations = abs(indexOfRelations)+numOfGens;  
  return indexOfRelations-1;   
}
Generator CosetRelationsSet::genOfRels(int i)const
{
  int numOfGens = relations.length()/2;
  int indexOfGen = i+1;
  if (indexOfGen > numOfGens)
    indexOfGen = -1*(indexOfGen-numOfGens);  
  return Generator(indexOfGen);   
}

bool CosetRelationsSet::addRelation(int first,const Generator& g,
				    int second,bool inverse)
{
  BTree<int,int>& tmpTree= relations.ref(indexRelOf(g));
  int* oldRelation = tmpTree.search(first);
  //cout << "Trying :"<< first <<"("<<g<<")="<< second<<endl;
  if (oldRelation != NULL){
    //cout << "result :"<<*oldRelation<<endl; 
    if (*oldRelation == second) return 0;
    if (*oldRelation  > second){
      addCollision(*oldRelation,second);
      *oldRelation = second;
      addRelation(second,inv(g),first,true);
    }
    else {
      addCollision(second,*oldRelation);
      if (!inverse)
	addRelation(second,inv(g),first,true);      
    }
    //tmpTree.printAll();
    return 1;
  }
  //cout << "Tree for "<<g<< endl;
  tmpTree.insert(first,second);
  //tmpTree.printAll();
  // To many searchs, id use recurrent.
  if (!inverse)
    addRelation(second,inv(g),first,true);
  return 1;
}

int CosetRelationsSet::getRelationNumber(int coset,const Generator& generator)const
{
  CosetRelationsSet* This = (CosetRelationsSet*)this;
  BTree<int,int>& tmpTree = This->relations.ref(indexRelOf(generator));
  int* oldRelation = tmpTree.search(coset);
  if (oldRelation!=NULL) return *oldRelation;
  return 0;
}


