// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: The reference counter class for the whole project.
//
// Principal Author: Stephane Collart
//
// Status: Complete
//
// Questions:
//
// * Should some SAFETY level check for overflow of the reference
//   count? The problem is the quite high computational cost
//
// * Would it ever be necessary to provide for reference counts of more than
//   an unsigned int?
//
// Revision History:
//
// * RefCounter, as a class used purely for derivation, no longer has public
//   members; it exports its reference count manipulators protected, so that 
//

#ifndef _REF_COUNTER_H_
#define _REF_COUNTER_H_


#include "global.h"


class RefCounter {

  //@rn Lippman says that this makes every instance of ObjectOf a friend,
  //@rn but gcc 2.6.3 refuses it and all obvious permutations.
  //@rn template <class T> friend class ObjectOf;
  
//@rn Temporary kludge:
//@rn protected:
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Exported Type Handles                                               //
  //                                                                     //  
  /////////////////////////////////////////////////////////////////////////

  typedef unsigned int refCounterType;
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors                                                        //
  //                                                                     //  
  /////////////////////////////////////////////////////////////////////////

  RefCounter( ) : xrefs(0) { }
  // a new reference counter is initialised
  // with a ref count of 1 (ie 0 extra refs)

  RefCounter( const RefCounter& rc ) : xrefs(0) { }
  // for derived representation classes whose copy constructor is generated
  // by the compiler: a new (copied) representation also has an initial
  // extra ref count of 0

  // Destructor provided by compiler

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Reference Count Handles                                             //
  //                                                                     //  
  /////////////////////////////////////////////////////////////////////////

  bool lastRef( ) const { return xrefs == 0; }
  
  bool sharedRef( ) const { return xrefs != 0; }
  
  void addRef( ) const { ++((RefCounter*)this)->xrefs; }
  // addRef is logically const
  
  void delRef( ) const { --((RefCounter*)this)->xrefs; }
  // delRef is logically const

  #ifdef DEBUG
  refCounterType nxrefs( ) const { return xrefs; }
  #endif
  
private:
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members                                                        //
  //                                                                     //  
  /////////////////////////////////////////////////////////////////////////

  refCounterType xrefs; // extra references (ie 0 means one ref)

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Hidden Members                                                      //
  //                                                                     //  
  /////////////////////////////////////////////////////////////////////////

  RefCounter& operator = ( const RefCounter& );
  // disable assignment: normally representations are never assigned

};

#endif

