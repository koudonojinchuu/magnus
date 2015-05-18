// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: debug file for Todd-Coxeter algoritm
//
// Principal Authors: Alexey Myasnikov
//
#include "global.h"
#include "error.h"
#include "Map.h"
#include "FPGroup.h"

#include "CosetEnumerator.h"

int main( )
{
  FPGroup G;
  cout << "\n\nEnter a group presentation, e.g.,\n";
  cout << "  <x,y;x^2=y^3>\n" << endl;
  Chars errStr = cin >> G;
  if (errStr.length() > 0){
    cout << errStr;
    return 1;
  }
  Word w;
  VectorOf<Word> wv;
  cout << endl << "Enter subgroup generators. " << 
    "Empty word to finish :" <<  endl;
  while (true){
    w = G.readWord(cin,errStr);
    if (w.length()==0)
      break;
    wv.append(w);
  }
  if (wv.length()>0)
    {
      CosetEnumerator ce(G,wv);
      cout << "\nIndex of the subgroup is equal to " << ce.enumerate()<<endl;
     
      PermutationRepresentation pr = ce.permutationRepresentation();
      
      VectorOf<Word> v = pr.getRepresentatives();
      for (int i = 0;i<v.length();i++)
	{
	  G.printWord(cout,v[i]);
	  cout << endl;
	}
    }
  else{
    CosetEnumerator ce(G);
    cout << "\nOrder of the group is equal to " << ce.enumerate()<<endl;
    VectorOf<Word> v = ce.schreierRepresentatives();
    for (int i = 0;i<v.length();i++){
       G.printWord(cout,v[i]);
       cout << endl;
    }
    /*   if ()
    PermutationRepresentation pr = ce.permutationRepresentation();
    pr.printOn(G.namesOfGenerators(),cout);
    
    w = G.readWord(cin,errStr);
    if (w.length()==0)
      return;    
    G.printWord(cout, pr.representativeOf(w ));
    */
  }

}
  







