// Copyright (C) 1998 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the FreeProduct class.
//
// Principal Authors: Alexei Myasnikov
//
// Status: in progress
//
// Revision History:
//
// Special Notes:
//           This classes are constructors. The only purpose is to 
//   recieve two groups and return a product (FPGroup) and corresponding maps.
//

#ifndef _PRODUCTS_H_
#define _PRODUCTS_H_

#include "FPGroup.h"
#include "Map.h"

class FreeProduct
{
public:
  enum CreatError { NONE, DOUBLE_GENS };
  FreeProduct( const FPGroup& g1, const FPGroup& g2): 
    theStatus( NONE ),
    G1( g1 ),
    G2( g2 ),
    gensInitialized( false )
    {
      createGenerators();
      createRelators();
    }

  // Maps
  virtual Map mapFromG1() const;
  virtual Map mapFromG2() const;
  virtual Map mapToG1() const;
  virtual Map mapToG2() const;
  
  //  CreateError getCreateStatus() const { return theStatus; }

  operator FPGroup( ) const { return FPGroup(theGenerators, theRelators);}

protected:
  void createGenerators();
  void createRelators();

  VectorOf<Chars> theGenerators;
  SetOf<Word> theRelators;

  FPGroup G1;
  FPGroup G2;

  // Creation status
  CreatError theStatus;
private:
  FreeProduct( const FreeProduct&);
  FreeProduct& operator = ( const FreeProduct& );
  bool gensInitialized;
};


class DirectProduct : public FreeProduct
{
 public:
   DirectProduct( const FPGroup& g1, const FPGroup& g2);
         
 private:
  DirectProduct( const DirectProduct&);
  DirectProduct& operator = ( const DirectProduct& );
};
#endif

