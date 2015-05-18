
// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of the NilpotentGroup class.
//
// Principal Author: Eugene Paderin
//
// Status: Draft
//
// Revision History:
//
// Special Notes:
//
//

#include "NilpotentGroup.h"
#include "FreeNilpotentGroupRep.h"
#include "FPNilpotentGroupRep.h"

#include "NilpCollectors.h"
#include "PolyWordIterator.h"


//=====================================================================
//================== class NilpotentGroup =============================
//=====================================================================


VectorOf<Chars> NilpotentGroup::defaultNames(int num) {

  VectorOf<Chars> names(num);

  for(int n = 0; n < num; n++)
    names[n] = (Chars)"x" + Chars(n+1);
  
  return names;
}


//----------------------------------------------------------------
//  Constructors
//----------------------------------------------------------------

// To construct a group with named generators, given nilpotency class


//FPNilpotentGroup
NilpotentGroup::NilpotentGroup(const VectorOf<Chars>& gennames, 
			       int nilclass,
			       const VectorOf<Word>& rels,
			       enum NilpotentCollector::Type colType 
			       )
  //@njz: TODO
  //  : DerivedObjectOf<FGGroup, NilpotentGroupRep> (
  //	 rels.length() > 0 ?  
  //         new FPNilpotentGroupRep(gennames, nilclass, rels, colType) :
  //         new FreeNilpotentGroupRep(gennames, nilclass, colType) 
  //	 )
  : DerivedObjectOf<FGGroup, NilpotentGroupRep> (
	 new FPNilpotentGroupRep(gennames, nilclass, rels, colType)
	 )
  //
  { }


//FreeNilpotentGroup
NilpotentGroup::NilpotentGroup(const VectorOf<Chars>& gennames, 
			       int nilclass,
			       enum NilpotentCollector::Type collectorType 
			       )
  : DerivedObjectOf<FGGroup, NilpotentGroupRep> ( 
      new FreeNilpotentGroupRep(gennames, nilclass, collectorType) )
{ }


// To construct a group with unnamed generators, given nilpotency class

//FPNilpotentGroup
NilpotentGroup::NilpotentGroup(int numOfGen,
			       int nilclass,
			       const VectorOf<Word>& rels,
			       enum NilpotentCollector::Type colType 
			       )
  //@njz: TODO
  //  : DerivedObjectOf<FGGroup, NilpotentGroupRep> (
  //         rels.length() > 0 ?							  
  //         new FPNilpotentGroupRep(defaultNames(numOfGen), nilclass, rels, colType) :
  //         new FreeNilpotentGroupRep(defaultNames(numOfGen), nilclass, colType) 
  //	 )
  : DerivedObjectOf<FGGroup, NilpotentGroupRep> (
	 new FPNilpotentGroupRep(defaultNames(numOfGen), nilclass, rels, colType)
	 )
  //
{ }

//FreeNilpotentGroup
NilpotentGroup::NilpotentGroup(int numOfGen,
			       int nilclass,
			       enum NilpotentCollector::Type collectorType 
			       )
  : DerivedObjectOf<FGGroup, NilpotentGroupRep> ( 
         new FreeNilpotentGroupRep(defaultNames(numOfGen), nilclass, collectorType) )
{ }


 

