// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: 
//
// Principal Authors: Alexei Myasnikov.
//

#include "global.h"
#include "conversions.h"
#include "NilpotentGroup.h"
#include "SGOfNilpotentGroup.h"

void main( )
{
  FPGroup G;
  cout << "Enter nilpotentcy class first then group ";
  int NilpotentcyClass;
  cin >> NilpotentcyClass;
  Chars errMsg = cin >> G;
  if (errMsg.length()>0) 
    return 1;
 
  NilpotentGroup ng(G.namesOfGenerators(),NilpotentcyClass,makeVectorOf(G.getRelators()));
  ng.initialize();
  /*  if (ng.isMalcevBasis())
    cout << "Malcev ";
  cout << "Basis :" << endl;
  ng.printBasis(cout);
  cout << endl << "The hirsch number :" << ng.theHirschNumber() << endl;
  // For FPNG only:
  int rank; int classN;
  if (ng.isFreeNilpotent(&rank,&classN))
    cout << "Is free nilpotent of rank: "<< rank <<
      " and class: " << classN<<endl;
  else
    cout << "Is not free nilpotent." << endl;
  
  cout << "Minimal nilpotentcy class: " << ng.minimalNilpotencyClass( )<< endl;
  cout << "Order of the torsion subgroup :" << 
    ng.orderOfTheTorsionSubgroup() << endl;
  PresentationForNG ngp = ng.makePresentation();
  cout << "Presentation :" << endl;
  ngp.print(cout);*/
  cout << endl << "Enter the word in terms of generators :";
  Word w = G.readWord(cin,errMsg);
  if (errMsg.length()>0) 
    return 1;
  //  cout << "Order :" << ng.orderOf(w) << endl;
    PolyWord decom =  ng.decompose(w);
  cout << endl << "Canonical decomposition :" << endl;
  cout << "     in terms of group generators: "; 
  G.printWord(cout,ng.toWord(decom));
  cout << endl;
  cout << "     in terms of BC: " << ng.toCommutatorWord(decom) << endl;
  cout << "     in terms of group basis: " << ng.asDecomposition(decom) << endl;
  cout << "Weight :" << ng.weightOf(decom) << endl;
  cout << "Order :" << ng.orderOf(decom) << endl;
  /*  cout << "Is in commutator subgroup :" << ng.isInCommutatorSubgroup(w) << endl;
  cout << "Is central :" << ng.isCentral(w) << endl;
  PolyWord root; int power;
  ng.maximalRoot(w,root,power);
  cout << "Maximal root :("; 
  G.printWord(cout,ng.toWord(root));
  cout << ")^" << power << endl;
  cout << "Is proper power :" << ng.isProperPower(decom) << endl;
  */  
  cout << endl << "Enter the word in terms of generators :";
  Word w1 = G.readWord(cin,errMsg);
  if (errMsg.length()>0) 
    return 1;
  //  ng.conjugacyProblem(w,w1);
  ng.centralizer(w);
}
  



