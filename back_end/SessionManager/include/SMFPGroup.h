// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declaration of class SMFPGroup
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//

#ifndef _SMFPGROUP_H_
#define _SMFPGROUP_H_


#include "AlgebraicObject.h"
#include "FPGroup.h"
#include "GIC.h"
#include "GCM.h"
#include "Supervisor.h"

//@njz:
#include "ComputationManager.h"
//

// This class is used to wrap all types of finitely presented groups,
// including free ones. The main reason is the strong typing required by
// the interface to the FE; if, e.g., free groups were a separate type,
// we would need four kinds of maps: FP -> FP, FP -> Free, etc.
// 
// Unfortunately, this leads to other hassles: to do a specialized
// computation for a certain kind of group, one must either `intercept'
// the computation request with a specialized computation manager, or
// check for the special kind in the generic computation manager.
// 
// This class also records information about the group which is true by
// the end user's definition:
// 
//   * The group was entered with an empty relator set. The more general
//     information that the group is free (on some set of generators) is
//     recorded by the GIC.
// 
//   * The group was entered as an abelian (resp. nilpotent)
//     presentation. The commutators are not stored in the relator set of
//     the `theGroup' member (unless the end user typed them). All
//     computations with abelian (resp. nilpotent) groups are intercepted
//     by specialized CMs which use the commutators implicitly.

//@rn Perhaps free is the only special case in this sense?


//---------------------------------------------------------------------------//
//--------------------------- SMFPGroup -------------------------------------//
//---------------------------------------------------------------------------//

class SMFPGroup : public AlgebraicObject
{

  //@njz:
  friend class ComputationManager;
  //

public:

  enum Checkin_Type {
	 FP, FREE, ABELIAN, NILPOTENT, SMALL_CANCELLATION, ONE_RELATOR,
	 AP_FREE, AP_FREE_CYCLIC, FREE_NILPOTENT, HNN_FREE, ONE_RELATOR_WITH_TORSION
  };

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMFPGroup(const FPGroup& G, const Chars heritage, Checkin_Type t = FP,
	    Chars def = Chars());

  SMFPGroup( ) : AlgebraicObject( "" ), gic( oid() ), theGCM( 0 ) { }
  //@rn Bogus ctor; see `But no' comment in Menu.h.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  FPGroup getFPGroup( ) const { return theGroup; }

  FreeGroup getFreePreimage( ) const { return theFreePreimage; }
  // When this group is F/R, it may be handy to have F.

  GCM& gcm() const { return *theGCM; }

  Checkin_Type getCheckinType( ) const { return checkin_type; }

  Chars getCheckinTypeStr( ) const;
  // Returns the checkin_type as a string of characters.
  
  
        InformationCenter* infoCenter()       { return &gic; }
  const InformationCenter* infoCenter() const { return &gic; }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Front End Interfacing:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  static const char* type( ) { return "SMFPGroup"; }

  const IconID iconID( ) const { return IconID::group; }  // overrides SMObject

  const char* typeID( ) const { return type(); }          // overrides SMObject

  void viewStructure(ostream& ostr) const;                // overrides SMObject

  void printProperties(ostream& ostr) const;              // overrides SMObject

  void printDefinition(ostream& ostr) const;              // overrides SMObject

  static void printGlobalMessageTemplates(ostream& ostr) {

    ostr << "init_map_gens_msg_ {${viewID} " << GENS_MAP_REQUEST
	 << " ${oid2} ${message} }" << endl;
    
  }

protected:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Restricted Access:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void readMessage(istream&);                          // overrides SMObject


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

public:

  GIC gic;

private:

  // These are the tags for messages handled by objects in this class:
  enum MessageTag {
    GENS_MAP_REQUEST
  };

  GCM* theGCM;  // TheObjects removes theGCM when this is removed.

  const FPGroup theGroup;

  const FreeGroup theFreePreimage;

  Checkin_Type  checkin_type;

  Chars definition;
  
};

#endif
