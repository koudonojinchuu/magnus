// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Test of KBmagPackage.
//           Should produce output identical to test-KBmagPackage.mastertestout,
//           given test-KBmagPackage.data as input.
//
// Principal Author: Sarah Rees
//
// Status: in progress
//
// Revision History:
//

#include "KBmagPackage.h"
#include "FPGroup.h"
#include "Set.h"
#include "Word.h"
#include "DiffMachine.h" 
#include "GenMult.h" 
#include "WordOrder.h"


main()
{
  FPGroup G;
  VectorOf<int> order;
  cin >> G;
  char ch;
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
  WordOrder word_order("ShortLex",order);
  KBmagPackage kbmag(G.namesOfGenerators(),G.getRelators(),word_order,20);
  if ( !kbmag.sanityCheck() ) {
    error("kbmag failed sanity check.\n");
  }
  if (kbmag.autgroup()==yes){
    cout << "Group "<< G << " is proved shortlex automatic"<<endl;
    GroupDFSA WA = kbmag.wordAcceptor();
    WA.setName("Word_acceptor");
    WA.printOn();
    GenMult GM = kbmag.generalMultiplier();
    GM.setName("General_multiplier");
    GM.printOn();
    DiffMachine D1 = kbmag.differenceMachine(1);
    D1.setName("1stDiffMachine");
    D1.printOn();
    DiffMachine D2 = kbmag.differenceMachine(2);
    D2.setName("2ndDiffMachine");
    D2.printOn();
  }
  else 
    cout << "Group "<< G << " is not proved shortlex automatic"<<endl;
  cout << endl;
  
  FPGroup G2;
  cin >> G2;
  KBmagPackage kbmag2(G2.namesOfGenerators(),G2.getRelators());
  if ( !kbmag2.sanityCheck() ) {
    error("kbmag2 failed sanity check.\n");
  }
  if (kbmag2.kbprog()==yes && kbmag2.gpmakefsa()==yes && kbmag2.gpaxioms()==yes){
    cout << "Group "<< G2 << " is proved shortlex automatic"<<endl;
    GroupDFSA WA = kbmag2.wordAcceptor();
    WA.setName("Word_acceptor");
    WA.printOn();
    GenMult GM = kbmag2.generalMultiplier();
    GM.setName("General_multiplier");
    GM.printOn();
    DiffMachine D1 = kbmag2.differenceMachine(1);
    D1.setName("1stDiffMachine");
    D1.printOn();
    DiffMachine D2 = kbmag2.differenceMachine(2);
    D2.setName("2ndDiffMachine");
    D2.printOn();
  }
  else 
    cout << "Group "<< G2 << " is not proved shortlex automatic"<<endl;
  cout << endl;
  FPGroup G3;
  cin >> G3;
  KBmagPackage kbmag3(G3.namesOfGenerators(),G3.getRelators());
  if ( !kbmag3.sanityCheck() ) {
    error("kbmag3 failed sanity check.\n");
  }
  if (kbmag3.kbprog()>0){
    Bool abort=NO;
    int loop=0;
    do {
      loop++;
      cout << "Trying to built automata."<< endl;
      if (loop>1) cout << "Pass no. " << loop <<" though loop."<< endl; 
      if (kbmag3.gpwa()!=yes || kbmag3.gpgenmult()!=yes) abort=YES; 
      if (abort){ cout << "Failed, giving up!"<< endl; break;}
      GroupDFSA WA = kbmag3.wordAcceptor();
      WA.setName("Word_acceptor");
      WA.printOn();
      GenMult GM = kbmag3.generalMultiplier();
      GM.setName("GeneralMultiplier");
      GM.printOn();
      DiffMachine D1 = kbmag3.differenceMachine(1);
      D1.setName("1stDiffMachine");
      D1.printOn();
      DiffMachineRep D2 = kbmag3.differenceMachineRep(2);
      D2.setName("2ndDiffMachine");
      D2.printOn();
    } while (kbmag3.gpcheckmult()==no);
    if (abort==NO && kbmag3.gpaxioms()==yes)
      cout << "Group "<< G3 << " is proved shortlex automatic"<<endl;
    else 
      cout << "Group "<< G3 << " is not proved shortlex automatic"<<endl;
   }
   GroupDFSARep WA2 = kbmag3.wordAcceptorRep();
   DiffMachineRep D3 = kbmag3.differenceMachineRep(2);
   KBmagPackage kbmag4(G3.namesOfGenerators(),G3.getRelators());
   kbmag4.setWordAcceptor(WA2);
   kbmag4.setDifferenceMachine(D3,2);
   kbmag4.gpgenmult();
   DFSA F;
   F.readFrom();
   kbmag4.minimize(F);
   F.printOn();
   GroupDFSARep F2;
   F2.readFrom();
   kbmag4.minimize(F2);
   F2.printOn();
   GroupDFSA M1;
   M1.readFrom();
   GroupDFSA M2;
   M2.readFrom();
   GroupDFSA M3;
   kbmag4.gpcomp(M1,M2,M3);
   M3.printOn();
   GroupDFSARep N1;
   N1.readFrom();
   GroupDFSARep N2;
   N2.readFrom();
   GroupDFSARep N3;
   kbmag4.gpcomp(N1,N2,N3);
   N3.printOn();
} 
