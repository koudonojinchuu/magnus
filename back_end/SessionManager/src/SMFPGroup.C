// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of class SMFPGroup
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//


#include "SMFPGroup.h"
#include "viewStructure.h"


//---------------------------------------------------------------------------//
//--------------------------- SMFPGroup -------------------------------------//
//---------------------------------------------------------------------------//

SMFPGroup::SMFPGroup(const FPGroup& G, const Chars heritage, Checkin_Type t,
		     Chars def)
  : AlgebraicObject( heritage ),
         theGroup( G ),
	 theFreePreimage( G.namesOfGenerators() ),
	 gic( oid() ),
	 theGCM( new GCM( *this ) ),
	 checkin_type( t ),
         definition( def )
{
  addDependent( *theGCM );
}


void SMFPGroup::printProperties(ostream& ostr) const
{
  ostr << DataPair( CheckinType( *this ), checkin_type )
       << DataPair( Oid( *this ), Object( *this ) );
}


void SMFPGroup::viewStructure(ostream& ostr) const // overrides SMObject
{
  ObjectView( ostr, oid(), heritage() );
}


void SMFPGroup::printDefinition(ostream& ostr) const // overrides SMObject
{
  //@rn  Problem for things like abelian and nilpotent presentations, since
  //@rn  the commutators are not stored as relators.
  
  if( definition.length() == 0) {
    ostr << theGroup;
  } else {
    ostr << definition;
  }
}  


Chars SMFPGroup::getCheckinTypeStr( ) const
{
  switch ( checkin_type ) {
    
  case FP :                 return "FP";
  case FREE :               return "FREE";
  case ABELIAN :            return "ABELIAN";
  case NILPOTENT :          return "NILPOTENT";
  case SMALL_CANCELLATION : return "SMALL_CANCELLATION";
  case ONE_RELATOR :        return "ONE_RELATOR";
  case ONE_RELATOR_WITH_TORSION : return "ONE_RELATOR_WITH_TORSION";
  case AP_FREE :            return "AP_FREE";
  case AP_FREE_CYCLIC :     return "AP_FREE_CYCLIC";
  case FREE_NILPOTENT :     return "FREE_NILPOTENT";
  case HNN_FREE :           return "HNN_FREE";
  default:    error("Chars SMFPGroup::getCheckinTypeStr( ) const : "
	    "unknown chekin type.");
  }
}

void SMFPGroup::readMessage(istream& istr)
{
  MessageTag tag;
  
  istr >> (int&)tag;

 

  if ( tag == GENS_MAP_REQUEST ) {
    Chars msg;
    int cursorPos = 0;
    int oid2;
    Chars out;

    istr >> oid2;
    istr > msg;
 
    if ( getFPGroup().numberOfGenerators() > 0) {
      VectorOf<Chars> v = getFPGroup().namesOfGenerators();
      
      for (int i=0;i<v.length()-1;i++) {
	out += Chars(" ") + v[i] + " ->  ,\\n";
      }
      out +=  Chars(" ") + v[v.length()-1] + " ->  \\n";
      cursorPos =  v[0].length() + 5;
    }
    cout << "defineMapDialog {Define a map from " << Name( *this ) << " to "
	 << Name( OID(oid2) ) << "} " << '{' << msg << "} "<< "{} " << "\"{\\n"
	 << out  << "}\" " << "{} " << cursorPos; 
    cout << endl << flush;
  }
}
