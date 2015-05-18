// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Informal test of DoubleCosetGraph class.
//
// Principal Author: Dmitry Bormotov
//
// Status:
//
// Revision History:
//

#include "FPGroup.h"
#include "GraphConjugacyProblem.h"


int main ( )
{
  FPGroup G;
  cout << "Type a finite group presentation:" << endl;
  Chars errMesg = cin >> G;

  if( errMesg.length() > 0 ) {
    cout << errMesg << endl;
    return 1;
  }
  
  cout << "You typed:" << endl << G << endl;

  cout << "Type a word u: ";
  Word u = G.readWord(cin, errMesg);
  if( errMesg.length() > 0 ) {
    cout << errMesg << endl;
    return 1;
  }
  u = u.freelyReduce();

  cout << "Type a word v: ";
  Word v = G.readWord(cin, errMesg);
  if( errMesg.length() > 0 ) {
    cout << errMesg << endl;
    return 1;
  }
  v = v.freelyReduce();
  
  GraphConjugacyProblem GCP(G,u,v);
  GCP.startComputation();

  Generator g(1);
  SubgroupGraph theGraph( G.numberOfGenerators(), G.getRelators() );
  
  int i = 0, k = 0;
  bool bFirst = true;
  
  while( !GCP.done() )	{

    if( GCP.theNewGraphIsNeeded() ) {

      cout << "Constructing the new graph number " << k++ << " ... ";

      if( !bFirst ) {
	
	theGraph.joinConjugate(g);
	int o = ord(g);
	if ( o > 0 )
	  o = -o;
	else
	  o = 1 - o;
	if ( o > G.numberOfGenerators() ) o = 1;
	g = Generator(o);
      }
      else
	bFirst = false;

      cout << "finished." << endl;
      
      cout << i++ << endl;
      GCP.continueComputation(theGraph);
    }
    else {
      cout << i++ << endl;
      GCP.continueComputation();
    }
  }
  
  cout << "These words are conjugate !" << endl << endl;
  cout << "The conjugator = ";
  G.printWord( cout, GCP.getConjugator() );
  cout << endl << endl;
}

