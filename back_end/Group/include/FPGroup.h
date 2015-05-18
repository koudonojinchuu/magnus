// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the FPGroup class.
//
// Principal Authors: Stephane Collart, Roger Needham
//
// Status: in progress
//
// Revision History:
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//
// Special Notes:
//
//

#ifndef _FINITELY_PRESENTED_GROUP_H_
#define _FINITELY_PRESENTED_GROUP_H_


//#include "FreeGroup.h"
#include "FGGroup.h"
#include "FPGroupRep.h"
#include "File.h"

class FPGroup : public DerivedObjectOf<FGGroup,FPGroupRep> {

public:

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Constructors                                     //
  //                                                   //
  ///////////////////////////////////////////////////////

  // Copy constructor provided by compiler

  FPGroup( int ngens = 0 ) :
    DerivedObjectOf<FGGroup,FPGroupRep>( new FPGroupRep(ngens) ) { }
  // To construct a group of given number of unnamed generators and
  // no relators. Default ngens = 0 gives trivial group with 0
  // generators

  FPGroup( int ngens, const SetOf<Word>& rels ) :
    DerivedObjectOf<FGGroup,FPGroupRep>( new FPGroupRep(ngens, rels) ) { }
  // To construct a group of given number of unnamed generators and
  // given relators.
  
  FPGroup( const VectorOf<Chars>& gennames ) :
    DerivedObjectOf<FGGroup,FPGroupRep>( new FPGroupRep(gennames) ) { }
  // To construct a group with named generators, no relators.

  FPGroup( const VectorOf<Chars>& gennames,
								  const SetOf<Word>& rels ) :
    DerivedObjectOf<FGGroup,FPGroupRep>( new FPGroupRep(gennames, rels) ) { }
  // To construct a group with named generators, given relators.

  FPGroup( const Group& G ) : DerivedObjectOf<FGGroup,FPGroupRep>(G) { }
  // cast construtor: to cast an arbitrary group as an FP group.
  // NB. This rewraps the unchanged representation, hence is in general
  // only useful for casting a group known to be an actual FP group.

  // @stc FPGroup( const FreeGroup& G ) :
  //	FGGroup(G) { } // @stc bugged constructor
  // To make a free group into a finitely presented one.
  // @stc if you used this constructor, check your code -- all it did
  // was rewrap an Free gp rep as an FP gp.
  
  // Destructor provided by compiler.

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Accessors / Modifiers                            //
  //                                                   //
  ///////////////////////////////////////////////////////
  
  // Inherited from Group:
  //@rn how to do this systematically?
  // @stc I don't think there is any proper way to do it systematically,
  // any class anywhere may have access adjusters: thus the only way to
  // `see' the complete interface in every class is to write it down
  // exhaustively; however, each inherited method should be considered
  // inherited from its immediate parent, in order not to make the
  // validity of the statements here dependent on changes higher up.
  // it's probably better to put each method in its proper contextual place

  static Type type( ) { return FPGroupRep::type(); }
  // dominates FGGroup::type();

  // Inherited from FGGroup:
  //  Type actualType() const; // pseudo-virtual
  //  int numberOfGenerators( ) const
  //  Chars nameOfGenerator(int i) const
  //  VectorOf<Chars> namesOfGenerators( ) const

  SetOf<Word> getRelators( ) const { return look()->relators; }  

  // @stc code of the following incomplete: other flags must be reset
  // too:

  SetOf<Word> setRelators( const SetOf<Word>& r ) {
	return change()->setRelators(r);
  }

  SetOf<Word> addRelators( const SetOf<Word>& r ) {
	return change()->addRelators(r);
  }

  SetOf<Word> removeRelators( const SetOf<Word>& r ) {
	return change()->removeRelators(r);
  }

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Methods dealing with group structure             //
  //                                                   //
  ///////////////////////////////////////////////////////

  // Inherited from FGGroup:
  //  int order( ) const;

  Trichotomy isFree( ) const { return enhance()->isFree(); }
  // Returns YES if this group is free on its generators, NO if not,
  // and DONTKNOW if no answer can be determined.

  Trichotomy isMetricSmallCancellationGroup( ) const {
	computeCancellationLambda();
	return look()->isMetricSmallCancellation();
  }

  int cancellationLambda( ) const {
	computeCancellationLambda();
	return look()->isMetricSmallCancellation.lambda();
  }

  // @stc these methods for checking whether the group has small
  // cancellation are of dubious usefulness, since presumably in 
  // general the caller will want to construct symmetrised relations
  // to use in making a MSCGroup.

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Methods which deal with group elements           //
  //                                                   //
  ///////////////////////////////////////////////////////

  // Inherited from FGGroup:
  // Elt makeIdentity( ) const;
  // Bool isSyntacticIdentity(const Elt& e) const;
  // Trichotomy isTrivialElt( const Elt& e ) const; // pseudo-virtual
  // Trichotomy areEqual(const Elt& e1, const Elt& e2) const;
  // Elt firstElt( ) const;
  // Elt nextElt(const Elt& e) const;
  // Elt multiply(const Elt& e1, const Elt& e2) const;
  // Elt inverseOf(const Elt& e) const;
  // Elt raiseToPower(const Elt& e, int n) const;
  // Elt conjugateBy(const Elt& e1, const Elt& e2) const;
  // Elt commutator(const Elt& e1, const Elt& e2) const;
  // Elt eval( const Word& w ) const;
  // Trichotomy wordProblem( const Word& w ) const;
  // Trichotomy conjugacyProblem( const Word& u, const Word& v ) const;

  Word shortenByRelators(const Word& w) const {
	 return look()->shortenByRelators(w);
  }
  // Shorten w by calling shortenByRelator with relators.
  // @stc @rn: this is outdated by @db & @dp's new implementation:
	  // This does not even symmetrize the relators, much less check a
	  // small cancellation condition or test hyperbolicity  -- that
	  // is the caller's responsibility.
  // @stc the question of whether to explicitely store symmetrised
  // relations or to provide fancy indexing is raised here too
  // (see Discussion, MSCGroup.h)

  Chars productOfCommutators(const Word& w,File& file) {
    return change()->productOfCommutators(w,file);
  }
  
  Chars productOfSquares(const Word& w,File& file) {
    return change()->productOfSquares(w,file);
  }

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Fabricators:                                     //
  //                                                   //
  ///////////////////////////////////////////////////////

  // Inherited from FGGroup:
  //  Subgroup randomSubgroup( ) const;

  ///////////////////////////////////////////////////////
  //                                                   //
  //  I/O                                              //
  //                                                   //
  ///////////////////////////////////////////////////////

  // Inherited from FGGroup:

  //  Word readWord(istream& istr, Chars& errMesg) const
  //  void printWord( ostream& ostr, const Word& w ) const

  void printRelators(ostream& ostr) const {
	 look()->printRelators(ostr);
  }

 /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  // operators inhereted from Group


  ///////////////////////////////////////////////////////
  //                                                   //
  //  Private helper stuff:                            //
  //                                                   //
  ///////////////////////////////////////////////////////

private:

  void computeCancellationLambda( ) const {
    if ( look()->isMetricSmallCancellation() == DONTKNOW ) {
      SetOf<Word> S(getRelators());
      enhance()->isMetricSmallCancellation.
	setLambda(::cancellationLambda( symmetrise(S) ));
    }
    // @stc compiler complains about the rvalue tmp here -- need to
    // make a work-around to make the compiler shut up.
  }
  // @stc should the group store a copy of symmetrised relators for
  // future use? the question remains even if there is a symmetrised
  // words container class.
	
protected:
 
  // Special wrapping constructor to wrap new representations (returned
  // by eg. delegated methods) and for base initialisation by derived
  // classes:
 
  FPGroup( FPGroupRep* newrep ) : DerivedObjectOf<FGGroup,FPGroupRep>(newrep) { }
 
 
  ///////////////////////////////////////////////////////
  //                                                   //
  //  Debugging stuff                                  //
  //                                                   //
  ///////////////////////////////////////////////////////
 
private:
 
#ifdef DEBUG
//  friend int main( );
//  friend void debugPrint(ostream&, const FreeGroup& g);
#endif

};

#endif

