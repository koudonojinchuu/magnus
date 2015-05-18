// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the FPGroup class.
//
// Principal Authors: Alexey Myasnikov
//
// Status: in progress
//
// Revision History:
//
//
// Special Notes:
//
//

#ifndef _ABELIAN_SG_PRESENTATION_H_
#define _ABELIAN_SG_PRESENTATION_H_


#include "AbelianGroup.h"
#include "AbelianGroupRep.h"
#include "Map.h"

//--------------- AbelianSGPresentationRep -------------------------//

struct AbelianSGPresentationRep : AbelianGroupRep {

// Constructors:

  // Copy constructor and operator = provided by compiler.
  
  AbelianSGPresentationRep(const AbelianGroup& parent,
			   const VectorOf<Word>& sgGens, 
			   const FPGroup& thePresentation, 
			   const DArray<Integer>& sgPGens,
			   const DArray<Integer>& sgPGensInv,
			   const VectorOf<Integer>& invariants,
			   bool makeF )
    : AbelianGroupRep(thePresentation,makeF),
       theParent( parent ),
       theSGGens( sgGens ),
       theSGPGens( sgPGens ),
       theSGPGensInv( sgPGensInv ),
       theInvariants( invariants )
  { }
  // Make a rep. It accepts the following parameters:
  // parent - parent group of a subgroup,
  // sgGens - generators od SG in terms of parent
  // thePresentation - the presentation for a subgroup as a group
  // sgPGens - generators of presentation epressed in generators of canonical 
  // decomposition of a parent group ,
  // makeF - true if you whant to make a files with 
  // generators, the same as for AbelianGroup 

  // Destructor provided by compiler.

//
// Representation methods:
//
  
  AbelianSGPresentationRep* clone( ) const { return new AbelianSGPresentationRep(*this); }

    Word fromSGPGensToSGGens(const Word& w)const;
    // Makes the map from presentation generators to
    // generators of parent group

    bool fromSGGensToSGPGens(const Word& w,Word& wInNew)const;
   // Makes the map from  generators of parent group to
   // presentation generators, if word w is in the subgroup it
   // returns true, and put the reflection in wInNew.
   // If word w does not belong to the subgroup, it returns false.

  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
    
  inline void write( ostream& ostr ) const
  {
    AbelianGroupRep::write(ostr);
    ostr < theParent;
    ostr < theSGGens;
    ostr < theSGPGens;
    ostr < theSGPGensInv;
    ostr < theInvariants;
  }

  inline void read( istream& istr )
  {
    AbelianGroupRep::read(istr);
    istr > theParent;
    istr > theSGGens;
    istr > theSGPGens;
    istr > theSGPGensInv;
    istr > theInvariants;
 }

// Data members:
private:
  VectorOf<Word>  theSGGens;
  // The generators of subgroup i terms of parent group
  DArray<Integer> theSGPGens;
  // Generators of presentation, given in generators of
  // canonical decomposition of parent group
  DArray<Integer> theSGPGensInv;
  VectorOf<Integer> theInvariants;
  AbelianGroup    theParent;
  // Parent group

// Private methods

  void makeFile( );
  // Creates the file containing the expression of the  generators
  // of canonical decomposition in terms of the old ones.

  void makeFileOfPDGens( );
  // Creates the file containing the expression of the  generators
  // of primary decomposition in terms of the old ones.
};

//--------------- AbelianSGPresentation -------------------------//

class AbelianSGPresentation : public DerivedObjectOf<AbelianGroup,AbelianSGPresentationRep> {

public:

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Constructors                                     //
  //                                                   //
  ///////////////////////////////////////////////////////

  // Copy constructor provided by compiler


  AbelianSGPresentation() : DerivedObjectOf<AbelianGroup,AbelianSGPresentationRep> (new AbelianSGPresentationRep(AbelianGroup(FPGroup()),VectorOf<Word>(),FPGroup(),DArray<Integer>(),DArray<Integer>(),VectorOf<Integer>(),false)) { }
  
  ///////////////////////////////////////////////////////
  //                                                   //
  //  I/O                                              //
  //                                                   //
  ///////////////////////////////////////////////////////

  // Inherited from AbelianGroup:

  Word fromSGPGensToSGGens(const Word& w)const{
    return look()->fromSGPGensToSGGens(w);
  }
   // Makes the map from presentation generators to
   // generators of parent group

  bool fromSGGensToSGPGens(const Word& w,Word& wInNew)const{
    return look()->fromSGGensToSGPGens(w,wInNew);
  }
   // Makes the map from  generators of parent group to
   // presentation generators, if word w is in the subgroup it
   // returns true, and put the reflection in wInNew.
   // If word w does not belong to the subgroup, it returns false.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  // Operators inhereted from AbelianGroup
 
 
  ///////////////////////////////////////////////////////
  //                                                   //
  //  Debugging stuff                                  //
  //                                                   //
  ///////////////////////////////////////////////////////
protected:
private:
  friend  AbelianSGPresentation AbelianGroup::makeSubgroupPresentation(const VectorOf<Word>& vG) const;

  //Constructor is private because it have sence only for AbelianGroup
  // It accepts the following parameters:
  // parent - parent group of a subgroup,
  // sgGens - generators od SG in terms of parent
  // thePresentation - the presentation for a subgroup as a group
  // sgPGens - generators of presentation epressed in generators of canonical 
  // decomposition of a parent group ,
  // makeF - true if you whant to make a files with 
  // generators, the same as for AbelianGroup
  AbelianSGPresentation(const AbelianGroup& parent,
			const VectorOf<Word>& sgGens,
			const FPGroup& thePresentation,
			const DArray<Integer>& sgPGens,
			const DArray<Integer>& sgPGensInv,
			const VectorOf<Integer>& invariants,
			bool makeF = false) :
    DerivedObjectOf<AbelianGroup,AbelianSGPresentationRep>(new AbelianSGPresentationRep(parent,sgGens,thePresentation,sgPGens,sgPGensInv,invariants,makeF)) { }
  // Special wrapping constructor to wrap new representations (returned
  // by eg. delegated methods) and for base initialisation by derived
  // classes:
 
  AbelianSGPresentation( AbelianSGPresentationRep* newrep ) : 
    DerivedObjectOf<AbelianGroup,AbelianSGPresentationRep>(newrep) { } 
 
#ifdef DEBUG
  friend int main(int, char** );
#endif

};

#endif

