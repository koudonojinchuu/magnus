// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: test-weightedLex.
//
// Principal Author: Sarah Rees
//
// Status: in progress
//
// Revision History:
//

#include "DiffMachine.h"
#include "KBmagPackage.h"
#include "RKBPackage.h"
#include "FPGroup.h"
#include "Set.h"
#include "Word.h"

main()
{
  do {
    FPGroup G;
    int i;
    cin >> G;
    if (G.numberOfGenerators()==0) break;
    VectorOf<int> weight;
    VectorOf<int> order;
    char ch;
    Bool equality = no;
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
    else for (i=1;i<=2*G.numberOfGenerators();i++) weight.append(1);

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
    }
    else for (i=1;i<=G.numberOfGenerators();i++){ 
      order.append(i); order.append(-i);
    }

    WordOrder word_order("WtLex",order,weight);
    KBmagPackage kbmag(G.namesOfGenerators(),G.getRelators(),word_order,20);
    if ( !kbmag.sanityCheck() ) {
      error("kbmag failed sanity check.\n");
    }

    Bool abort=no;
    Trichotomy checks=no;
    int loop=0;
    GroupDFSA WA;
    GenMult GM;

    DiffMachine D;
    D.readFrom();
    D.closeUnderSubstrings(1,word_order);
    cout << "Substring closed word difference machine has "<< 
           D.getNumStates() <<" states."<< endl;
    kbmag.setDifferenceMachine(D,2);

    while (abort==no) { 
      loop++; 
      if (loop>1) cout << "Pass no. " << loop <<" though loop."<< endl; 
      cout << "Word difference machine has "<< 
               D.getNumStates() <<" states."<< endl;
      cout << "Trying to build word acceptor."<< endl;
      WA = D.wordAcceptor(word_order);
      cout << "Word acceptor has "<<WA.getNumStates()<< 
                              " states before minimization,"<< endl;
      kbmag.minimize(WA);
      cout << "and "<<WA.getNumStates()<< " afterwards."<< endl;
      kbmag.setWordAcceptor(WA);
      cout << "Trying to build multipliers."<< endl;
      checks = kbmag.gpgenmult();
      if (checks==dontknow) { 
        cout << "Failed to construct multipliers. Giving up."<< endl;
        abort=yes; 
        break;
      } 
      if (checks==no){
        cout << "Failed to build multipliers. This shouldn't happen. Aborting"<< endl;
        abort = yes;
        cout << "2nd Difference Machine:-"<< endl;
        D.printOn();
        cout << "Word acceptor:-"<< endl;
        WA.printOn();
        break;
      }
      GM = kbmag.generalMultiplier();
      checks = kbmag.gpcheckmult();
      if (checks==dontknow){ 
        cout << "Unable to complete check of multipliers. Aborting."<< endl;
        abort=yes; break;}
      else if (checks==no){
        cout << "gpcheckmult returns no. Looping."<< endl;
        D = kbmag.differenceMachine(2);
        D.closeUnderSubstrings(1,word_order);
        cout << "Substring closed word difference machine has "<< 
               D.getNumStates() <<" states."<< endl;
        kbmag.setDifferenceMachine(D,2);
      }
      else {
        cout << "gpcheckmult return yes."<< endl;
        break;
      }
    } 
    if (abort==no && kbmag.gpaxioms()==yes){
      cout << "Group "<< G << " is proved weighted lex automatic"<<endl;
      cout << "with weights " << weight << " and order " << order << endl;
      cout << "Minimized word acceptor, with "<< 
              WA.getNumStates()<< " states:-"<< endl;
      WA.printOn();
      cout << "General multiplier:-"<< endl;
      GM.printOn();
    }
    else { 
      cout << "Group "<< G << " is not proved weighted lex automatic"<<endl;
      cout << "with weights " << weight << endl;
      cout << "and order " << order << endl;
    }
  } while (0!=1); 
}

