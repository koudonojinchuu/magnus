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
    cout << G << endl;
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
    WordOrder word_order("WtLex",order,weight);
    DiffMachine D;
    D.readFrom();
    D.printOn(cout);
    while (0!=1){
      Chars errmesg;
      cout << "Enter word to be reduced (terminated with a comma)." << endl;
      Word u = G.readWord(cin, errmesg);
      Word v = u;
      G.printWord(cout, u);
      if (u.length()==0) { cout << endl; break;}
      cout << " rewrites as ";
      D.rewrite(u,word_order);
      G.printWord(cout, u);
      cout << endl;
    }
  } while (0!=1);
}

