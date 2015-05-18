// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: test-Diff.
//
// Principal Author: Sarah Rees
//
// Status: in progress
//
// Revision History:
//

#include "RKBPackage.h"
#include "DiffMachine.h"
#include "FPGroup.h"
#include "Set.h"
#include "Word.h"

//This runs the rkbp for a bit on a  given group and builds the associated 
//difference machine,and prints it out.
// The input is simply a presentation.
main()
{
  do {
    FPGroup G;
    cin >> G;
    if (G.numberOfGenerators()==0) break;
    VectorOf<int> order;
    VectorOf<int> weight;
    char ch;

    while (((ch = cin.peek()) != EOF) && isspace(ch)) ch = cin.get();

    if (ch == '[') { 
      ch = cin.get();
      do {
        int i;
        cin >> i;
        order.append(i);
        cin>> ch;
        if (ch==']') break;
        else if (ch!=','){ cerr << "Unexpected input, aborted"<<endl; exit(1);}
      } while (ch==',');
    
      while (((ch = cin.peek()) != EOF) && isspace(ch)) ch = cin.get();
      if (ch == '[') {
        ch = cin.get();
        do {
          int i;
          cin >> i;
          weight.append(i);
          cin>> ch;
          if (ch==']') break;
          else if (ch!=','){ cerr << "Unexpected input, aborted"<<endl; exit(1);}
        } while (ch==',');
      }
    }
    WordOrder word_order("ShortLex",2*G.numberOfGenerators());
    if (order.length() && weight.length()==0){
      WordOrder wo("ShortLex",order);
      word_order = wo;
    }
    else if (order.length() && weight.length()){
      WordOrder wo("WtShortLex",order,weight);
      word_order = wo;
    }

    RKBPackage rkbp(G.namesOfGenerators(),G.getRelators(),word_order);
  
    if ( !rkbp.sanityCheck() ) {
      error("RKBPackage failed sanity check.\n");
    }
  
    int maxRelLen=0;
    SetOf<Word> relators = G.getRelators();
    SetIterator<Word> I(relators);
    while (!I.done()){
      Word w = I.value();
      int l = w.length();
      if (l>maxRelLen) maxRelLen = l;
      I.next();
    }
    cout << "Running rkbp with parameter "<< 2*maxRelLen << endl;
    rkbp.runKB(2*maxRelLen,-1,2*maxRelLen,2*maxRelLen);
    SetOf<Word> differences = rkbp.wordDifferences();
    DiffMachine D=rkbp.differenceMachine(differences);
    D.setName("DiffMachine");
    D.printOn(cout);
    DiffMachine D2=rkbp.differenceMachine();
    if (D!=D2) cout << "The two difference machines aren't equal."<< endl;
    while (0!=1){
      Chars errmesg;
      cout << "Enter word to be reduced (terminated with a comma)." << endl;
      Word u = G.readWord(cin, errmesg);
      Word v = u;
      G.printWord(cout, u);
      if (u.length()==0) { cout << endl; break;}
      cout << " rewrites using computed difference machine and "<<
       word_order.getOrderType() << "word order as ";
      D.rewrite(u,word_order);
      G.printWord(cout, u);
      cout << endl;
      if (word_order.getOrderType()=="WtShortLex"){ 
        u=v;
        WordOrder word_order2("WtLex",order,weight);
        D.rewrite(u,word_order2);
      }
      cout << " and using weightedLex as ";
      G.printWord(cout, u);
      cout << endl;
      cout << " and using rkbp as ";
      rkbp.oneOffRewrite(v);
      G.printWord(cout, v);
      cout << endl;
    }
  } while (0!=1);
}

