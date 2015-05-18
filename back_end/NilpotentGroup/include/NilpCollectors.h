// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definition of classes CollectorToTheLeft
//
//
// Principal Author: Eugene Paderin
//
// Status: In progress
//
// Revision History:
//
// Special Notes:
//

#ifndef _NILP_COLLECTORS_H_
#define _NILP_COLLECTORS_H_

#include "NilpotentCollector.h"


//==========================================================================
//=================== class CollectorToTheLeft =============================
//==========================================================================

class CollectorToTheLeft: public NilpotentCollector
{

public:

  //------------------------------------------------------------------------
  //   Constructors and initializers
  //------------------------------------------------------------------------

  CollectorToTheLeft(int numgen, int nilclass, bool initialize = true)
    : NilpotentCollector(numgen, nilclass, initialize)
  { 
    //cout << "This is a collector to the left." << endl;
  }

  CollectorToTheLeft(const BasicCommutators& bc)
    : NilpotentCollector(bc)
  { }

  
  //copy constructor, destructor and assignment operator provided by compiler

  virtual NilpotentCollector::Type type() const { 
    return NilpotentCollector::TO_THE_LEFT; 
  }
  // the collector type

  virtual void collectingProcess( PolyWord& pw ) const;
  // collects the given PolyWord. The result is stored in the same PolyWord.

  virtual NilpotentCollector * clone() const {
    return new CollectorToTheLeft(*this);
  }

  // method to clone the object


};



//==========================================================================
//=================== class CollectorFromTheLeft =============================
//==========================================================================

class CollectorFromTheLeft: public NilpotentCollector
{

public:

  //------------------------------------------------------------------------
  //   Constructors and initializers
  //------------------------------------------------------------------------

  CollectorFromTheLeft(int numgen, int nilclass, bool initialize = true)
    : NilpotentCollector(numgen, nilclass, initialize)
  { 
    //cout << "This is a collector from the left." << endl;
  }

  CollectorFromTheLeft(const BasicCommutators& bc)
    : NilpotentCollector(bc)
  { }

  
  //copy constructor, destructor and assignment operator provided by compiler

  virtual NilpotentCollector::Type type() const { 
    return NilpotentCollector::FROM_THE_LEFT; 
  }
  // the collector type

  virtual void collectingProcess( PolyWord& pw ) const;
  // collects the given PolyWord. The result is stored in the same PolyWord.

  virtual NilpotentCollector * clone() const {
    return new CollectorFromTheLeft(*this);
  }

};


#endif


