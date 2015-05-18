// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: test-finiteness.
//
// Principal Author: Sarah Rees
//
// Status: in progress
//
// Revision History:
//
// XI/94: RN: Brought into line with project guidelines.


#include "RKBPackage.h"
#include "KBMachine.h"
#include "FPGroup.h"
#include "Set.h"
#include "Word.h"

// This tests for finiteness/infiniteness of the group input.
// The Knuth Bendix procedure is run for a while on the rewriting system
// deduced from the group relations, and a Knuth Bendix machine is built.
// The function finiteLanguage then tests to see if
// the language of that machine is finite or infinite.
// If the language is finite, the group is proved finite.
// On the other hand,
// if the system of rewrite rules is confluent and the language
// is infinite, the group is proved infinite.
// If neither of these two situations occur,
// Knuth Bendix is run for a little longer, and the tests are
// repeated.
// This continues until either finiteness of the group is proved,
// or infiniteness is proved, or the number of rewrite rules is rather large.
// The input consists of a group presentation. 

main()
{
  FPGroup G;
  do {
    cin >> G;
    cout << "Testing for finiteness." << endl << "G = "<< G << endl;
    RKBPackage rkbp(G.namesOfGenerators(),G.getRelators());
  
    if ( !rkbp.sanityCheck() ) {
      error("RKBPackage failed sanity check.\n");
    }
  
    int maxRelLen = 0;
    SetOf<Word> relators = G.getRelators();
    SetIterator<Word> I(relators);
    while (!I.done()){
      Word w = I.value();
      int l = w.length();
      if ( l > maxRelLen ) maxRelLen = l;
      I.next();
    }
    cout << "Running RKBPackage with parameter " << 2*maxRelLen << endl;
    int par = 2*maxRelLen;
    int numRules;
    Bool finite, confluent;
    Integer size;
    do {
      rkbp.runKB(par, -1, par, par);
      confluent = rkbp.isProvedConfluent();
      numRules = rkbp.currentNumOfRules();
      cout << "Rewriting system has "<< numRules <<" rules." << endl;
      KBMachine M=rkbp.KnuthBendixMachine();
      size = M.sizeLanguage();
      if (size== -1) finite=FALSE;
      else finite = TRUE;
      if (finite==TRUE) {
        if (confluent==TRUE) 
          cout << "Group is proved finite of size " << size << endl;
        else
          cout << "Group is proved finite of size at most " << size << endl;
      }
      else if (finite==FALSE && confluent==TRUE) 
        cout << "Group is proved infinite." << endl;
      else  cout << "Language is infinite, rewriting system is not confluent."<<endl;
      par += 2;
    } while (finite==FALSE && confluent==FALSE && numRules <= 10000);
  } while (G.numberOfGenerators() > 0);
}

