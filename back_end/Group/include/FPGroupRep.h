// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the FPGroupRep class.
//           The name is abbreviated to fit in a library.
//
// Principal Authors: Stephane Collart, Roger Needham
//
// Status: in progress
//
// Revision History:
//
// 12/16/94 rn Added provisional data member word_problem, for experimentation.
//          The wordProblem method now consults it. The ctors had to change too.
//
//  5/01/95 rn removed the WordProblem data member.
//
// Discussion:
//
// * The problem of storing flags: many flags signalling properties of
//   groups imply that information was computed in addition to the
//   setting of the flag: for small cancellation, the value of lamda,
//   for free, the rank, etc.
//   Storing both a flag and the value of some information can be
//   redundant, making consistent manipulation more cumbersome.
//   The helper class FPGroupRep::MetricSmallCancellationLambda is a
//   tentative and partial answer to this, by providing both value
//   and setting handles, but keeping only one effective data record.
//   Note that the problem of storing additional information generated
//   in computations, such as eg. more relators, is left open by this.
//
// * Is it really appropriate for a group to accumulate information
//   about itself? There seems to be a danger of redundancy of
//   facilities if eg. an FPGroup both remembers it is a MSCGroup,
//   and behaves as such, and at the same time a full blown MSCGroup
//   class is provided with much the same abilities.
//
// Bugs:
//
// * isTrivialElt(Elt) calls wordProblem(Word), implying the assumption
//   the arg is always an actual word: this seems highly questionable.
//   
// Special Notes:
//
//

#ifndef _FIN_PRES_GROUP_REP_H_
#define _FIN_PRES_GROUP_REP_H_


#include <Integer.h>
#include "FGGroupRep.h"
#include "File.h"

struct FPGroupRep : FGGroupRep {

  // Methods dealing with the finitely presented group rep:

// Constructors:

  // Copy constructor and operator= provided by compiler (do deep copy).
  
  FPGroupRep( int ngens )
  : FGGroupRep(ngens),
	 isMetricSmallCancellation(MetricSmallCancellationLambda::infinity)
  { }
  // Make a rep with ngens unnamed generators, no relators.

  FPGroupRep( const VectorOf<Chars>& gennames )
  : FGGroupRep( gennames ),
	 isMetricSmallCancellation(MetricSmallCancellationLambda::infinity)
  { }
  // Make a rep with named generators, no relators.

  FPGroupRep( int ngens, const SetOf<Word>& rels )
  : FGGroupRep(ngens),
	 relators(rels)
  { }
  // Make a rep with ngens unnamed generators, given relators.
  
  FPGroupRep( const VectorOf<Chars>& gennames, const SetOf<Word>& rels )
  : FGGroupRep( gennames ),
    relators(rels)
  { }
  // Make a rep with named generators, given relators.
  
  // Destructor provided by compiler.

// Accessors / Manipulators:

  virtual SetOf<Word>& setRelators( const SetOf<Word>& r ) {
	isMetricSmallCancellation.reset();
	relators = r;
	return relators;
	// @stc can be condensed to return relators = r if and when set
	// assignment-like operators are made to return refs
  }

  virtual SetOf<Word>& addRelators( const SetOf<Word>& r ) {
    isMetricSmallCancellation.reset();
	relators |= r;
    return relators;
    // @stc can be condensed to return relators |= r if and when set
    // assignment-like operators are made to return refs
  }
 
  virtual SetOf<Word>& removeRelators( const SetOf<Word>& r ) {
    isMetricSmallCancellation.reset();
    relators -= r;
    return relators;
    // @stc can be condensed to return relators -= r if and when set
    // assignment-like operators are made to return refs
  }

//
// Representation methods:
//
  
  PureRep* clone( ) const { return new FPGroupRep(*this); }
  // overrides FGGroupRep::clone()

  static const Type theFPGroupType;

  static Type type( ) { return theFPGroupType; }
  // dominates FGGroupRep::type()

  Type actualType( ) const { return type(); }
  // overrides FGGroupRep::actualType();

//
// Methods dealing with the properties of the group:
//

  int order( ) const;
  Trichotomy isTrivial( ) const;
  Trichotomy isFinite( ) const;
  Trichotomy isInfinite( ) const;
  Trichotomy isAbelian( ) const;
  virtual Trichotomy isFree( ) const;
  bool compare( const GroupRep* G ) const;

//
// I/O:
//
  
  void printOn(ostream&) const;
  GroupRep* readFrom(istream&, Chars&) const;

  virtual void printRelators(ostream&) const;

//
// Methods dealing with group elements: //@rn move many to .C
//
  
  // Iherited from FGGroupRep:
  // virtual Elt makeIdentity( ) const;
  // virtual Bool isSyntacticIdentity( const Elt& e) const;

  virtual Trichotomy isTrivialElt( const Elt& e ) const {

	return wordProblem(e);
  }
  // @stc see bugs in the banner

  Trichotomy areEqual(const Elt& e1, const Elt& e2) const {
	 return wordProblem( multiply( e1, e2.inverse() ) );
  }

  // Inherited from FGGroupRep:
  // virtual Elt firstElt( ) const;
  // virtual Elt nextElt( const Elt& e ) const;
  // virtual Elt multiply( const Elt& e1, const Elt& e2 ) const;
  // virtual Elt inverseOf( const Elt& e ) const;
  // virtual Elt raiseToPower(const Elt&, int) const;
  // virtual Elt conjugateBy(const Elt&, const Elt&) const;
  // virtual Elt commutator(const Elt&, const Elt&) const;

  Elt eval( const Word& w ) const {
	 
    #if SAFETY > 0
	   if ( ord(w.maxOccurringGenerator()) > theNumberOfGenerators )
		  error("tried to evaluate Word with no interpretation in FreeGroup");
    #endif
		return w.freelyReduce(); //@rn appropriate?
  }
  // overrides FGGroupRep::eval()


  Trichotomy wordProblem( const Word& w ) const {
	 return dontknow;
  }
  // overrides FGGroupRep::wordProblem()

  Trichotomy conjugacyProblem( const Word& u, const Word& v ) const {
	 return DONTKNOW;
  }
  // overrides FGGroupRep::conjugacyProblem()

  Word shortenByRelators(const Word& w) const;

  Chars productOfCommutators(const Word& w,File& file);
  
  Chars productOfSquares(const Word& w,File& file);
  
  Integer decideOrder(FPGroupRep* Gr) const;
  //@rn very temporary

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
    
  inline void write( ostream& ostr ) const
  {
    FGGroupRep::write(ostr);
    ostr < relators;
    ostr < isMetricSmallCancellation;
    ostr < '\n'; //@dp temp
  }

  inline void read( istream& istr )
  {
    FGGroupRep::read(istr);
    istr > relators;
    istr > isMetricSmallCancellation;
  }

// Data members:

  SetOf<Word> relators;


  // Trichotomy isMetricSmallCancellation; // @stc
  //@rn
  // a) For what lambda?
  // b) Requires constructing symmetrized set of relators.
  //    Leave this group's relator set symmetrized for subsequent calls
  //    to shortenByRelators (which means Dehn's algorithm in this case)?
  //    How to protect them from alteration?
  // @stc presumably, to know isMetricSmallCancellation, lambda must be
  // computed; therefore, lambda should be stored, and this should
  // test the value of lambda. The relators can't be enhanced, since
  // that would make this group formally another group, ie. surprise
  // for the user. The question remains open whether and how a group
  // should keep alternative representations for itself.

  class MetricSmallCancellationLambda {
  // simple helper class, flagging the small cancellation property and
  // simultanously storing the lambda value; increases consistency,
  // reduces memory consumption
    int lambda_val;
	// @stc tentative internal coding convention:
	// -1: DONTKNOW (initial value);
	//  1: NO (not a small cancellation group);
	//  0: lambda is infinite;
	//  2: the value of lambda.
  public:
	MetricSmallCancellationLambda( ) : lambda_val(dontknow) { }
	MetricSmallCancellationLambda( int l ) : lambda_val(l) { }

 /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend ostream& operator < ( ostream& ostr, const MetricSmallCancellationLambda& iMSC)
  {
    ostr < iMSC.lambda_val;
    return ostr;
  } 
  
  friend istream& operator > ( istream& istr, MetricSmallCancellationLambda& iMSC )
  {
    istr > iMSC.lambda_val;
    return istr;
  }

    Trichotomy operator()( ) const {
      switch (lambda_val) {
		case dontknow: return DONTKNOW;
        case  1: return NO;
		default: return YES;
	  }
	}
	int lambda( ) const {
		#if SAFETY > 0
		if (lambda_val < 0) error("Tried to take undefined lambda in"
			" FPGroupRep::MetricSmallCancellationLambda::lambda()");
		#endif
		return lambda_val;
	}
	void setLambda( int l ) {
		#if SAFETY > 0
        if (l < 0) error("Tried to set bad lambda in"
            " FPGroupRep::MetricSmallCancellationLambda::setLambda()");
        #endif
        lambda_val = l;
	}
	void reset( ) { lambda_val = dontknow; }
	enum { dontknow = -1, infinity = 0 }; // export constants
	// @stc it might be preferable to make dontknow and infinity
	// class objects or classes themselves.
  };

  MetricSmallCancellationLambda isMetricSmallCancellation;
  // flags the property and stores the value of lambda at the same time

};

#endif

