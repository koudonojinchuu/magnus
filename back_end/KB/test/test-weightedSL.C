// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: test-weightedSL.
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
    Bool provedConfluent = no;
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

    WordOrder word_order("WtShortLex",order,weight);

    KBmagPackage kbmag(G.namesOfGenerators(),G.getRelators(),word_order);

    if ( !kbmag.sanityCheck() ) {
      error("kbmag failed sanity check.\n");
    }

    RKBPackage rkbp(G.namesOfGenerators(),G.getRelators(),word_order);

    if ( !rkbp.sanityCheck() ) {
      error("RKBPackage failed sanity check.\n");
    }

    SetOf<Word> diffs;
    SetOf<Word> oldDiffs;
    int numRules = 0, oldNumRules = 0;
    int maxRelLen=0;
    SetOf<Word> relators = G.getRelators();
    SetIterator<Word> I(relators);
    while (!I.done()){
      Word w = I.value();
      int l = w.length();
      if (l>maxRelLen) maxRelLen = l;
      I.next();
    }
    int par = 2*maxRelLen;
    while (par <= 10*maxRelLen) {
      cout << "Running rkbp with parameter "<< par << "."<<endl;
      rkbp.runKB(par,-1,par,par);
      diffs = rkbp.wordDifferences();
      numRules = rkbp.currentNumOfRules();
      cout << "Found "<<numRules<<" rules so far, and " <<
                        diffs.cardinality()<<" word differences." << endl;
      if (rkbp.isProvedConfluent()){
        provedConfluent = yes;
        cout << "Rewriting system is proved to be confluent"<< endl;
        break;
      }
      else if (diffs == oldDiffs){ 
       if (numRules > 2*oldNumRules || equality==yes) break;
       equality = yes;
      }
      else equality = no;
      oldDiffs = diffs;
      oldNumRules = numRules;
      par += 5;
    }
    if (par > 10*maxRelLen){
      cout << "Set of word differences didn't stablilise."<< endl;
      continue;
    }
    DiffMachine D=rkbp.differenceMachine(diffs);
    DiffMachine DS;
    kbmag.setDifferenceMachine(D,2);

    Bool abort=no;
    Trichotomy checks=no;
    int loop=0;
    GroupDFSA WA;
    GenMult GM;

    while (abort==no) {
      loop++;
      if (loop>1) cout << "Pass no. " << loop <<" though loop."<< endl;
      cout << "Word difference machine has "<<
               D.getNumStates() <<" states."<< endl;
      DS=D;
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
        cout << "Failed to build multipliers. Looping"<< endl;
        D = kbmag.differenceMachine(2);
        kbmag.setDifferenceMachine(D,2);
        continue;
      } 
      GM = kbmag.generalMultiplier();
      GM.setName("GM");
      checks = kbmag.gpcheckmult();
      if (checks==dontknow){ 
        cout << "Unable to complete check of multipliers. Aborting."<< endl;
        abort=yes; break;}
      if (checks==no){
        cout << "gpcheckmult returns no. Looping."<< endl;
        D = kbmag.differenceMachine(2);
      }
      else {
        cout << "gpcheckmult return yes."<< endl;
        break;
      }
    }
    if (abort==no && provedConfluent==yes)
      cout << "Automaticity is now proved by confluence, but we'll check axioms."<< endl;
    if (abort==no && kbmag.gpaxioms()==yes){
      cout << "Axioms check."<< endl;
      cout << "Group "<< G << " is proved weighted shortlex automatic"<<endl;
      cout << "with weights " << weight << " and order " << order << endl;
      cout << "Minimized word acceptor, with "<<
              WA.getNumStates()<< " states:-"<< endl;
      WA.printOn();
      cout << "General multiplier:-"<< endl;
      GM.printOn();
    }
    else { 
      cout << "Group "<< G << " is not proved weighted shortlex automatic"<<endl;
      cout << "with weights " << weight << endl;
      cout << "and order " << order << endl;
    }
  } while (0!=1); 
}

