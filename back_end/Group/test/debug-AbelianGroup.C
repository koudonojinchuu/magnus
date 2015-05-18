#include <iostream.h>
#include <Integer.h>
#include "Map.h"

#include "AbelianGroup.h"


main ( ) {
  cout << "\n\nEnter a group presentation, e.g.,\n";
  cout << "  <x,y;x^2=y^3>\n" << endl;

  FPGroup G;
  Chars errMsg = cin >> G;

  if ( errMsg.length() > 0 ) {
    cout << errMsg << endl << endl;
    return 1;
  }

  cout << "\n\nYou entered:\n";
  cout << G << endl << endl;

  cout << "Computing abelianization..." << endl;
  AbelianGroup A(G);
  A.computeCyclicDecomposition();

  cout << "OldToNew: ";
  VectorOf<AbelianWord> oldToNew = A.oldToNewGens();
  for( int i = 0; i < oldToNew.length(); ++i ) {
    cout << " ";
    G.printWord(cout, oldToNew[i].getWord());
  }
  cout << endl << endl;
  
  cout << "NewToOld: ";
  VectorOf<AbelianWord> newToOld = A.newToOldGens();
  for( int i = 0; i < newToOld.length(); ++i ) {
    cout << " ";
    G.printWord(cout, newToOld[i].getWord());
  }
  cout << endl << endl;
 
  
//  cout << "CD: " << A.getCanonicalSmithPresentation() << endl << endl;

/*
  cout << "OldToNewGens: " << Map( G, CD.getFPGroup(), A.oldToNewGens() )
       << endl << endl;
  cout << "NewToOldGens: " << Map( CD.getFPGroup(), G, A.newToOldGens() )
       << endl << endl;
*/      
  bool quit = false;
  while( !quit ) {
    

// -------------------- the wordProblem() test -------------------- //
    
/*
    cout << "Enter a word: " << endl;
    Word w = G.readWord(cin, errMsg);
    if( errMsg.length() > 0 ) {
      cout << errMsg << endl;
      exit(0);
    }

    cout << "Solving word problem..." << endl << endl;
    bool answer = A.wordProblem(w);
    cout << "word problem: " << ( answer ? "YES" : "NO") << endl;
*/

    
// -------------------- the orderOfElt() test -------------------- //

    
/*
    cout << "Enter a word: " << endl;
    Word w = G.readWord(cin, errMsg);
    if( errMsg.length() > 0 ) {
      cout << errMsg << endl;
      exit(0);
    }

    cout << "Computing the order of element..." << endl << endl;
    Integer order = A.orderOfElt(w);
    cout << "the order: ";
    if ( order == Integer(0) )
      cout << "infinity";
    else cout << order;
    cout << endl;
    
    char reply;
    cout << "Repeat (y/n)? ";
    do {
      cin >> reply;
      reply = tolower(reply);
    } while( reply != 'y' && reply != 'n' );
    if( reply == 'n' ) quit = true;
*/

    
// --- the orderOfTheTorsionSubgroup() and the orderOfAbelianGroup tests --- //

/*
    cout << "Computing the order of the Torsion Subgroup..." << endl << endl;
    Integer order = A.orderOfTheTorsionSubgroup();
    cout << "the order: " << order << endl << endl;
    
    cout << "Computing the order of the Abelian Group..." << endl << endl;
    order = A.order();
    cout << "the order: " << order << endl;

    quit = true;
*/

// ----------------------- the getRelators() test -------------------------- //

/*    
    cout << "Relators:" << endl;
    cout << A.getRelators() << endl;
    quit = true;
*/    


// -------------------------- the evaluate() test -------------------------- //
    

    cout << "Enter a word: " << endl;
    Word w = G.readWord(cin, errMsg);
    if( errMsg.length() > 0 ) {
      cout << errMsg << endl;
      exit(0);
    }

    cout << "Rewrited word: " << endl;
    AbelianWord r = A.oldToNewGens( AbelianWord(G.numberOfGenerators(), w) );
    
    A.printWordInNewGens(cout, r);
    cout << endl;
  }
}
