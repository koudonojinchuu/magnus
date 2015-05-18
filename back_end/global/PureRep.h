// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of class PureRep
//
// Description:
//
//   PureRep is an abstract base class for representations used in
//   pointer wrapping schemes
//
// Usage:
//
//   Use as a private base class of representations wrapped with
//   BaseObjectOf; make BaseObjectOf a friend; give the class
//   a protected ctor Class(Rep*) : BaseObjectOf(Rep*)
//   @stc needs to be written up better with an example
//   @stc maybe the pointer wrappers should go in a seperate dir with
//   @stc explanations and examples
//
// Principal Author: Stephane Collart
//
// Status: Complete
//
// Bugs:
//
// Revision History:
//

#ifndef _PURE_REP_H_
#define _PURE_REP_H_

#include "RefCounter.h"


struct PureRep : public RefCounter {

// This class is a `pure object representation' class, encapsulating the
// reference counter part of the representation. Normally it serves as
// the root base of a hierarchy of representations with virtual
// functions.

public :

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors                                                        //
  //                                                                     //  
  /////////////////////////////////////////////////////////////////////////

  // default ctor provided by compiler (initialises ref count)

  // copy ctor provided by compiler (resets ref count)

  virtual ~PureRep( ) { }
  // make the destructor virtual to ensure that each derived representation
  // gets deleted as what it really is

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Representation Stuff                                                //
  //                                                                     //  
  /////////////////////////////////////////////////////////////////////////

  virtual PureRep* clone( ) const = 0;
  // Should be implemented as a member which gives a new `deep-copy'
  // by each representation derived from this; normally, by making it
  // new CopyConstrutor(*this) with CopyConstructor doing `deep-dopy'
  // (`' because it is enough for the copy constructor to do logical
  // deep copy).

};

#endif

