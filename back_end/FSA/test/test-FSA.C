// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Test of FSA hierarchy.
//
// Principal Author: Sarah Rees
//
// Status: in progress
//
// Revision History:
//

#include "FPGroup.h"
#include "DFSA.h"
#include "DFSARep.h"
#include "Vector.h"
#include "Chars.h"
main() {
  FPGroup G;
  Chars errMesg = cin >> G;
  GroupDFSA M(G.namesOfGenerators());
  M.readFrom();
  M.printOn();
  int N=G.numberOfGenerators();
  VectorOf<Chars> alphabet;
  for (int i=1;i<=N;i++){
    Generator g(i);
    Chars sym=G.nameOfGenerator(g);
    alphabet.append(sym);
    sym += "^-1";
    alphabet.append(sym);
  }
  DFSA M2(alphabet);
  M2.readFrom();
  M2.printOn();
  GroupDFSA M3; // no alphabet specified for this one
  M3.readFrom();
  M3.printOn();
  DFSA M4; // no alphabet specified for this one
  M4.readFrom();
  M4.printOn();
  //GroupDFSA M5(G.namesOfGenerators(),0,2);
  GroupDFSARep M5("",G.namesOfGenerators());
  M5.readFrom();
  M5.printOn();
  for (i=1;i<=M5.getNumSymbols();i++){
    Generator g = M5.getSymbol(i);
    cout << "Symbol no. "<< i << ":";
    G.printGenerator(cout,g);
    cout << endl;
  }
  GroupDFSARep M6 = M5;
  M6.setName("donkey");
  M6.printOn();
  if (M2==M4) cout << "M and M5 match,but ought not to."<< endl;
  if (!(M5==M6)) cout << "M5 and M6 are not the same, 
     although they should be."<< endl;
  VectorOf<DFSARep> VV;
}
