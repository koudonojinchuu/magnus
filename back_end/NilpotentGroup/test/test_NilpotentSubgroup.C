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

int main( )
{
  FPGroup G;
  cout << "Enter nilpotentcy class first then group ";
  int NilpotentcyClass;
  cin >> NilpotentcyClass;
  Chars errMsg = cin >> G;
  if (errMsg.length()>0) 
    return 1;
 
  NilpotentGroup ng(G.namesOfGenerators(),
		    NilpotentcyClass,makeVectorOf(G.getRelators()));
  ng.initialize();
  VectorOf<Word> vw;
  Word w;
  for (int i=1;true;i++){
    cout << endl << "Enter the "<<i<<" generator of subgroup"<< endl;
    cout << "Empty word to finish: ";
    w = G.readWord(cin,errMsg);
    if (w.length()==0)
      break;
    if (errMsg.length()>0) 
      return 1;
    vw.append(w);
  }
  SGOfNilpotentGroup sg(ng,vw);
  sg.initBasis();
  sg.printBasis(cout);
  cout << "The Hirsch number :" << sg.theHirschNumber() << endl;
  cout << "Index in parent group :" << sg.index() << endl;
  cout << "Is Trivial :" << sg.isTrivial() << endl;
  cout << "Is Central :" << sg.isCentral() << endl;
  cout << "Is normal :" << sg.isNormal() << endl;
  cout << "Is abelian :" << sg.isAbelian() << endl;
  cout << "Subgroup class :" << sg.subgroupClass() << endl;
  cout << "Generators of normal closure :" << endl;
  vw = sg.normalClosureGens();
  for (int i=0;i<vw.length();i++){
    G.printWord(cout,vw[i]);
    cout << endl;
  }
  PresentationForSNG sgp = sg.makePresentation();
  cout << "Presentation of subgroup :" << endl;
  sgp.print(cout);
  cout << endl << "Enter the word :";
  errMsg = "";
  w = G.readWord(cin,errMsg);
  if (errMsg.length()>0){ 
    cout << errMsg;
    return 1; 
  }
  cout << endl << "Does subgroup contain the word :" << sg.contains(w) << endl;
  PolyWord result;
  if (sg.decompose(ng.decompose(w),result))
    cout << "Decomposition in sg basis :" << sg.asDecomposition(result);
  else
    cout << "Subgroup does not contain this word";

}

 




