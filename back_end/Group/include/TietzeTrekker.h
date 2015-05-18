// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the TietzeTrekker class.
//           A TietzeTrekker is a wrapper for a BlackBox which invokes
//           Frank Rimlinger's TietzeTrek with a group presentation.
//
// Principal Author: Roger Needham
//
// Status: experimental
//
// Revision History:
//
// Next implementation steps:
//
// * TietzeTrek doesn't listen to cin. Make it.
//   - It should respond to a quit command from cin.
//   - The transformation strategy can be tweaked; make commands for that.
//
// * For safety, check for parse error message after reading Frank's
//   presentations.
//
// * The BlackBox may go bad in the middle of the run. This should be
//   detected and reported by sanityCheck(), but still allow calls to
//   knownTo*.
//
// Bugs:
//
// * If a generator in a presentation does not appear in any relator,
//   Frank doesn't know about it. Thus <a,b,c; a=b> is infinite cyclic.
//   Given Frank's flaky API, there is no good workaround. Giving a relator
//   c c^-1 doesn't work, since he freely reduces the input, and I can't
//   find where. Moreover, when a relator reduces to the empty word, it
//   triggers a bug.
//   The present kludge is to give relators like c x c^-1, x. This
//   significantly delays the discovery of very obvious facts, but
//   presumably the caller has already checked for obvious free generators.
//
// * The TietzeTrek is not commanded to exit when a TietzeTrekker is destructed.
//   It seems to go away anyhow (broken pipes), but that's too hacky.
//
// Usage:
// 
// Instantiate a TietzeTrekker on, e.g., a FPGroup G by
// 
//   TietzeTrekker TT( G.namesOfGenerators(), G.getRelators() );
// 
// Make sure that TT.sanityCheck() returns TRUE.
// There is now an asynchronous process associcated with TT which is
// continuously generating presentations, along with statements of
// certain facts (factoids) which can readily be deduced from certain
// presentations.
// Get the `next' presentation from getPresentation().
// To be `notified' of any factoids, first call getFactoid (see comment
// thereon), then call knownTo*. If you don't like the answer, call
// getFactoid again, etc.


#ifndef _TIETZE_TREKKER_H_
#define _TIETZE_TREKKER_H_


#include "BlackBox.h"
#include "FPGroup.h"


class TietzeTrekker {

public:

  TietzeTrekker(const FPGroup& G);
  // Fire up Frank Rimlinger's TietzeTrek with the presentation of G.

  TietzeTrekker(const VectorOf<Chars>& genNames, const SetOf<Word>& relators);
  // Fire up Frank Rimlinger's TietzeTrek with the given `presentation'.

  //@rn Need a destructor, which tells TietzeTrek to quit.

  Bool sanityCheck( ) { return ( status == 1 ); }
  // Says whether this TietzeTrekker thinks its initialization was successful.

  Bool getFactoid(Bool queuePresentations, long giveUpLimit = 100);
  // The first argument says whether to save any presentations
  // which are found before the next factoid. Saved ones are later returned
  // by getPresentation().
  // The second argument specifies the number of lines to read from the
  // BlackBox in search of a factoid before giving up and returning.
  // The return value says whether a new factoid was discovered.

  // The following indicate what facts have been discovered so far by 0
  // or more calls to getFactoid (FALSE means "don't know (yet)"):

  Bool knownToBeTrivial( )      { return (flags & TRIVIAL) != 0; }
  Bool knownToBeCyclic( )       { return (flags & CYCLIC) != 0; }
  Bool knownToHaveFreeFactor( ) { return (flags & HAS_FREE_FACTOR) != 0; }
  Bool knownToBeFinite( )       { return (flags & FINITE) != 0; }
  Bool knownToBeFree( )         { return (flags & FREE) != 0; }

  int getOrder( );
  // Returns the order of the group in `standard' format: -1 == DONTKNOW,
  // 0 == INFINITE, otherwise actual order. If the answer is -1, you can
  // call getFactoid (again) for another throw of the dice.

  int getRank( );
  // Returns the rank of the group AFTER knownToBeFree() has returned TRUE.
  // It is a fatal error if this is called when !knownToBeFree().

  FPGroup getPresentation( );
  // Returns another presentation of the finitely presented group
  // (representation) used to instantiate this TietzeTrekker, in the form
  // of a FPGroup.


private:

  enum { TRIVIAL = 1, CYCLIC = 2, HAS_FREE_FACTOR = 4, FINITE = 8, FREE = 16 };

  // Data members;

  ListOf<FPGroup> presentations;
  // Presentations queued by getFactoid.

  BlackBox TietzeTrek;  // The BlackBox we're wrapping.

  int flags;            // Flag what we know with values from above enum.
  int order;            // Store order if we know it.
  int rank;             // Store free rank if we know it.

  int status;           // Initialization status: 1 == good, 0 == bad.


  // Private methods:

  void initialize(const VectorOf<Chars>& genNames, const SetOf<Word>& relators);
  // Does the actual work of construction.

  void printWord(ostream& ostr, const Word& w);
  // A special word printer, to work around the ineptitudes in Frank's API.

  void printGenerator(ostream& ostr, int g);
  // Prints g as [a-zA-Z].
};

#endif
