// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: test-KBM.
//
// Principal Author: Sarah Rees
//
// Status: in progress
//
// Revision History:
//
// XI/94: RN: Brought into line with project guidelines.

#include "KBMachine.h"
#include "RKBPackage.h"
#include "FPGroup.h"
#include "Set.h"
#include "Word.h"


// This runs the rkbp for a bit, builds a Knuth Bendix reduction machine
// prints it out and reduces 10 input words with it (printing the reduced words).
// The input consists of a presentation followed by 10 words, 
// each terminated by a comma.

main()
{
  do {
    FPGroup G;
    cin >> G;
    if (G.numberOfGenerators()==0) break;
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
    cout << "Running rkbp with parameter " << 2*maxRelLen << endl;
    int par = 2*maxRelLen;
    rkbp.runKB(par, -1, par, par);
    KBMachine M = rkbp.KnuthBendixMachine();
    M.setName("KBMachine");
    M.printOn(cout);
    M.oldFormatPrintOn(cout);
    int len;
    while (0!=1){
      Chars errmesg;
      cout << "Enter word to be reduced (terminated with a comma)." << endl;
      Word u = G.readWord(cin, errmesg);
      Word v = u;
      G.printWord(cout, u);
      if (u.length()==0) { cout << endl; break;}
      cout << " rewrites using index automaton as ";
      M.rewrite(u);
      G.printWord(cout, u);
      cout << endl;
      cout << " and using rkbp as ";
      rkbp.oneOffRewrite(v);
      G.printWord(cout, v);
      cout << endl;
      Bool ans = M.rewrite(u);
    }
  } while (0!=1);
}

