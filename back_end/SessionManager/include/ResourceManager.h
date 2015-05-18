// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Declaration of class ResourceManager
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//

//@rn Give some thought to access restrictions and efficiency.


#ifndef _RESOURCEMANAGER_H_
#define _RESOURCEMANAGER_H_


#include "ARC.h"
#include "ARCSlotID.h"
#include "OID.h"


//---------------------------------------------------------------------------//
//------------------------- ResourceManager ---------------------------------//
//---------------------------------------------------------------------------//

class ResourceManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  ResourceManager(OID oid);

  ~ResourceManager( );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  OID getOID( ) const;
  // Since this class is imported by an SMObject, we need to store the
  // OID here, too.

  ARC freeARCs( ) const;
  // Returns the number of liquid, i.e., spendable ARCs available to this.

  bool workingFor(OID oid) const;
  // Returns true if this has 1 or more ARCs from oid.

  bool isNeeded() const;


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Control:                                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void freeze( );
  // Says that this may not spend any ARCs, and any ARCs allocated by this
  // are also frozen.

  void liquefy( );
  // Reverses the effect of `freeze' (which need not have been called).

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Transactions:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void allocate(ResourceManager& recipient, ARC arcs);
  // This wishes to give `arcs' to `recipient', to spend as `recipient'
  // likes.

  void acceptAllocation(OID benefactor,
								ARCSlotID asi,
								ARC arcs,
								bool overrides = false
								);
  // `benefactor' wants to give us `arcs'. We are told to which ARCSlotID
  // of the OID to report spending. When `overrides', we clear any record
  // of allocation from `oid', and substitute this one.

  void usedARCs(ARC arcs);
  // A derivative informs this that is has spent `arcs'.

  void usedOneARC( );
  // Commonly used shortcut.

  void usedMemory(int kilobytes) { }  //@rn Not yet supported.


private:

  bool isLiquid(OID oid) const;
  // We will record in a central location which objects in this class
  // are frozen. This aids in determining whether a Resources record
  // is free for use.

  ResourceManager(const ResourceManager&);
  // Hidden, not to be implemented.

  ResourceManager& operator = (const ResourceManager&);
  // Hidden, not to be implemented.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  struct Resources
  {
	 Resources(OID oid, ARCSlotID asi, ARC arcs, Resources* n) :
	   benefactor(oid),
		theARCs(arcs),
		theARCSlotID(asi),
		memory(-1),        // No limit
		next(n)
	 { }

	 ~Resources( ) { delete next; }

	 OID benefactor;
	 ARCSlotID theARCSlotID;
	 ARC theARCs;
	 int memory;

	 Resources* next;
  };


  Resources* toUse;
  // These are the Resources gotten from elsewhere.

  Resources* lastDrawnFrom;
  // Pointer into toUse of last Resources drawn from.

  OID theOID;
  // The oid of the ComputationManager for which this manages Resources.

};

#endif
