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
    if (order.length()!=weight.length() || 
        order.length()>2*G.numberOfGenerators()){
       cerr << "Error in input of word order data. Aborted."<< endl;
       exit(1);
     }


    WordOrder word_order("WtLex",order,weight);
    KBmagPackage kbmag(G.namesOfGenerators(),G.getRelators(),word_order,20);
    if ( !kbmag.sanityCheck() ) {
      error("kbmag failed sanity check.\n");
    }

    Bool abort=no;
    Trichotomy checks=no;
    Bool provedConfluent=no;
    int loop=0;
    GroupDFSA WA;
    GenMult GM;
    DiffMachine D,DS;

    int wt, maxwt;
    State s;
    int haltingfactor=100;
    int tidyint = 50;
    int maxeqns = 200;
    int maxstates = 1000;
    Trichotomy kb=no;
    do {
      kb=kbmag.kbprog(haltingfactor,tidyint,maxeqns,maxstates);
      if (kb==yes) break;
      else if (kb==dontknow) {
        cout << "kbprog has failed."<< endl;
        break;
      }
      else if (kb==no && maxeqns >= 262144){
        cout << "kbprog fails to stabilise with maxeqns at "<< maxeqns<< endl;
        break;
      }
      tidyint =500;
      maxeqns *= 2;
      maxstates =0;
      cout << "Changing kbprog parameters, maxeqns now set to "<<maxeqns<<
            endl;

    } while (kb==no);
    if (abort==no) {
      D = kbmag.differenceMachine(2);
      if (kb != yes){
        if (D.getNumStates()>1){
          cout << "Although kbprog has not been seen to stabilise,"<< endl;
          cout << " test will proceed with current set of word differences."<< endl;
          D.rebuildDifferenceMachine(word_order);
          D.closeUnderInverses(word_order);
        }
        else {
          cout << " Giving up."<< endl;
          cout << 
     "kbprog has failed to produce any non-trivial word differences."<< endl;
          abort=yes;
        }
      }
    }
    if (abort==no){
      cout << "Word difference machine has "<< 
               D.getNumStates() <<" states."<< endl;
      maxwt=0;
      for (s=1;s<=D.getNumStates();s++){
        wt = word_order.getWeight(D.getDifference(s));
        if (wt>maxwt) maxwt=wt;
      }
      cout << "Max weight of a word difference is "<< maxwt<< endl;
      kbmag.setDifferenceMachine(D,2);
      provedConfluent=kbmag.isProvedConfluent();
      if (provedConfluent) cout << "KB rules are confluent."<< endl;
    }

    while (abort==no) { 
      loop++; 
      if (loop>1) cout << "Pass no. " << loop <<" though loop."<< endl; 
      cout << "Word difference machine has "<< 
               D.getNumStates() <<" states."<< endl;
      DS = D;
      DS.closeUnderSubstrings(1,word_order);
      cout << "Substring closed word difference machine has "<< 
               DS.getNumStates() <<" states."<< endl;
      cout << "Trying to build word acceptor."<< endl;
      WA = DS.wordAcceptor(word_order);
      WA.setName("WA");
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
      GM.setName("GM");
      cout << "Trying to check multipliers."<< endl;
      checks = kbmag.gpcheckmult();
      if (checks==dontknow){ 
        cout << "Unable to complete check of multipliers. Aborting."<< endl;
        abort=yes; break;}
      else if (checks==no){
        cout << "gpcheckmult returns no. Looping."<< endl;
        D = kbmag.differenceMachine(2);
        cout << "Word difference machine has "<< 
               D.getNumStates() <<" states."<< endl;
        maxwt=0;
        for (s=1;s<=D.getNumStates();s++){
          wt = word_order.getWeight(D.getDifference(s));
          if (wt>maxwt) maxwt=wt;
        }
        cout << "Max weight of a word difference is "<< maxwt<< endl;
        kbmag.setDifferenceMachine(D,2);
      }
      else {
        cout << "gpcheckmult return yes."<< endl;
        break;
      }
    } 
    if (abort==no && provedConfluent==yes)
      cout << "Automaticity is now implied by confluence. But we'll check axioms."<< endl;
    if (abort==no && kbmag.gpaxioms()==yes){
      cout << "Axioms check"<< endl;
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

