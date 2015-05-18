// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Test of RKBPackage.
//           Should produce output identical to test-RKBPackage.mastertestout,
//           given test-RKBPackage.data as input.
//
// Principal Author: Sarah Rees
//
// Status: in progress
//
// Revision History:
//
// XI/94: RN: Brought into line with project guidelines.

#include "RKBPackage.h"
#include "FPGroup.h"
#include "Set.h"
#include "Word.h"

// This runs the RKBPackage, estimates the growth rate of the group, and then
// reduces 10 words using the rewrite function of the RKBPackage (printing the
// reduced words).
// The input is a group presentation followed by 10 words, each terminated
// by a comma.


main()
{
  do {
    FPGroup G;
    cin >> G;
    if (G.numberOfGenerators()==0) break;
    char ch;
  
    VectorOf<int> order;
    cin >> ch;
    if (ch != '[') { cerr << "Unexpected input, aborted"<<endl; exit(1);}
    do {
      int i;
      cin >> i;
      order.append(i);
      cin>> ch;
      if (ch==']') break;
      else if (ch!=','){ cerr << "Unexpected input, aborted"<<endl; exit(1);}
    } while (ch==',');
  
    VectorOf<int> weight;
    cin >> ch;
    if (ch != '[') { cerr << "Unexpected input, aborted"<<endl; exit(1);}
    do {
      int i;
      cin >> i;
      weight.append(i);
      cin>> ch;
      if (ch==']') break;
      else if (ch!=','){ cerr << "Unexpected input, aborted"<<endl; exit(1);}
    } while (ch==',');
  
    WordOrder word_order("WtShortLex",order,weight);
    RKBPackage rkbp(G.namesOfGenerators(),G.getRelators(),word_order);
  
    if ( !rkbp.sanityCheck() ) {
      error("RKBPackage failed sanity check.\n");
    }
  
    int maxRelLen = 2; // the relators xx^-1 and x^-1x are always added
    SetOf<Word> relators = G.getRelators();
    SetIterator<Word> I(relators);
    while (!I.done()){
      Word w = I.value();
      int l = w.length();
      if ( l > maxRelLen ) maxRelLen = l;
      I.next();
    }
    cout << "Running RKBPackage with parameter " << 2*maxRelLen << endl;
    rkbp.runKB(2*maxRelLen, -1, 2*maxRelLen, 2*maxRelLen);
    float* A = rkbp.growthRate(2*maxRelLen, 50, 25);     //@rn A pointer?!
    for ( int i = 1; i <= 2*maxRelLen; i++ )
      cout << "Estimated no. of words of length " << i << ": " << A[i] << endl;
    delete A;
    rkbp.enterRewriteMode();
    while (0!=1){
      Chars errmesg;
      cout << "Enter word to be reduced (terminated with a comma)." << endl;
      Word w = G.readWord(cin, errmesg);
      G.printWord(cout, w);
      if (w.length()==0) { cout << endl; break;}
      cout << " rewrites using rkbp as ";
      rkbp.rewrite(w);
      G.printWord(cout, w);
      cout << endl;
    }
    rkbp.quitRewriteMode();
  } while (0!=1);
}
