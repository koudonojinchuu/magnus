// Copyright (C) 2000 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Declaration of class RandomDefinitionsGenerator
//
// Principal Author: Alexei Miasnikov
//
// Status: in progress
//
// Revision History:
//


#ifndef _RANDOM_DEFINITIONS_H_
#define _RANDOM_DEFINITIONS_H_

#include "RandomNumbers.h"
#include "FPGroup.h"
#include "Chars.h"
#include "ViewContents.h"
#include <iostream.h>

// ------------------------------------  RandomDefinitionGenerate -----------------------------------
class RandomDefinitionGenerate
{
public:
  RandomDefinitionGenerate() : gw("Parameters") { }

  virtual void readParameters(istream& istr); 
  virtual void requireParameters() = 0; 
  virtual void printDefinition() = 0;
protected:  
  GroupWindow gw;
};

class RandomWordGenerate : public RandomDefinitionGenerate
{
public:
  RandomWordGenerate(istream& is, const FPGroup& p, Chars id);
  void requireParameters(); 
  void printDefinition();

private:

  RandomWordGenerate(const RandomWordGenerate &);
  // just for safety sake

  Chars generateWord( );

  istream& istr; 
  FPGroup parent; 
  Chars theID;
  UniformRandom ur;
  int averagelength;
};

// ------------------------------------  RandomCollectionOfWordsGenerate -----------------------------------

class RandomCollectionOfWordsGenerate : public RandomDefinitionGenerate
{
public:
  RandomCollectionOfWordsGenerate(istream& is, const FPGroup& p, Chars id, bool isS = false);
  void requireParameters(); 
  void printDefinition();

private:

  RandomCollectionOfWordsGenerate(const RandomCollectionOfWordsGenerate &);
  // just for safety sake

  Chars generateCollection( );

  istream& istr; 
  FPGroup parent; 
  Chars theID;
  UniformRandom ur;
  int averagelength;
  int numberOfWords;
  bool isSet;
};

// ------------------------------------  RandomMapGenerate -----------------------------------

class RandomMapGenerate : public RandomDefinitionGenerate
{
public:
  RandomMapGenerate(istream& is, const FPGroup& p, Chars id);
  void requireParameters(); 
  void printDefinition();

private:

  RandomMapGenerate(const RandomMapGenerate &);
  // just for safety sake

  Chars generateMap( );

  istream& istr; 
  FPGroup parent; 
  Chars theID;
  UniformRandom ur;
  int averagelength;

};
// ------------------------------------  RandomGroupGenerate -----------------------------------

class RandomGroupGenerate : public RandomDefinitionGenerate
{
public:
  RandomGroupGenerate(istream& is, Chars id, bool iF = false, bool iN = false, bool iOR = false);
  void requireParameters(); 
  void printDefinition();

private:

  RandomGroupGenerate(const RandomGroupGenerate &);
  // just for safety sake

  Chars generateGroup( );

  istream& istr; 
  Chars theID;
  int maxGens;
  int maxRels;
  int averageRels;
  int nilpClass;

  bool isFree;
  bool isNilpotent;
  bool isOR;
};

// ------------------------------------  RandomSCGroupGenerate -----------------------------------

class RandomSCGroupGenerate : public RandomDefinitionGenerate
{
public:
  RandomSCGroupGenerate(istream& is, Chars id);
  void requireParameters(); 
  void printDefinition();

private:

  RandomSCGroupGenerate(const RandomSCGroupGenerate &);
  // just for safety sake

  Chars generateSCGroup( );

  istream& istr; 
  Chars theID;
  int maxGens;
  int maxRels;
  int averageRels;

};

//---------------------------------------------------------------------------//
//--------------------- RandomDefinitionsGenerator --------------------------//
//---------------------------------------------------------------------------//


class RandomDefinitionsGenerator

{
  
 private:
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  RandomDefinitionsGenerator( );
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Public Members Touched by Friends:                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  friend class SessionManager;
  
  static void readMessage(istream& istr);
  // Used by class SessionManager.
  // this function reads message from the front end which is request for 
  // a definition of an algebraic object. Format of the message is following:
  //   "object_type  window_name", here object type is a string identifying the 
  // object to be defined and window_name - is a  name for a text window where
  // definition will be displayed.
  
  static Chars generateFPGroupPresentation();
    
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  static RandomDefinitionGenerate* defGenerator;
};

#endif
