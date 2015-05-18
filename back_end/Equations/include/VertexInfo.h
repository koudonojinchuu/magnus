// Copyright (C) 2000 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of class VertexInfo
//
// Principal Author: Copied from Dmitry Pechkin's QEqnSolutions.C
//
// Status: Useable.
//
// Revision History:
//
// Special Notes:
//
// * VertexInfo has been moved to a separate file to ease class
//   template instantiation under gcc 2.95 on Solaris.


#ifndef _VertexInfo_H_
#define _VertexInfo_H_

#include "Word.h"
#include "NielsenTransformations.h"


// We can represent elimination process as a directed acyclic graph.
// There is an edge from word A to  word B marked with elementary
// singular endo R if B = R(A).
//
// In fact, the graph is represented as list of VertexInfo structures.

struct VertexInfo {

  // no default constructor

  VertexInfo(const Word& w, int i, const SingularEndo& se) :
    word(w), lastVar(i), eliminator(se) {}

  // copy constructor, operator=, and destructor provided by compiler

  friend inline bool operator==(const VertexInfo& x, const VertexInfo& y) {
    return (x.word == y.word && x.lastVar == y.lastVar && x.eliminator == y.eliminator);
  }

  friend inline ostream& operator<<(ostream& o, const VertexInfo& x) {
    error("called operator<<( ostream&, const VertexInfo& ) "
	  "which is prohibited");
    return o;
  }

  // data

  Word word;
  // a word with some variables eliminated

  int lastVar;
  // last variable eliminated; the word was derived from its parent
  // by eliminating lastVar
  // this prevents from considering the same vertex twice

  SingularEndo eliminator;
  // eliminating endo -- maps the root to the word
};

#endif
