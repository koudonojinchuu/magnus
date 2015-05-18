// Copyright (C) 1998 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Debug program for decompose problem in subgroups of free groups
//           and subgroups of finitely presented subgroups.
//
// Principal Author: Dmitry Pechkin
//
// Status: in progress
//
// Revision History:
//
// Bugs:
//
// Special Notes:
//
// Next implementation steps:
//

// Principal Author: Sergei Lyutikov


#include "DecomposeInSubgroup.h"
#include "Timer.h"

void testFreeGroup()
{
  FreeGroup F;

  cout << "\n\nType a free group presentation:\n" << endl;

  Chars errMesg = cin >> F;
  if ( errMesg.length() > 0 ) {
    cout << errMesg << endl << endl;
    return;
  }
  cout << "\n\nYou typed:\n" << F << endl << endl;

  cout << "Type a set of generating words of a subgroup:" << endl;

  SetOf<Word> S = F.readSetOfWords(cin, errMesg);
  if ( errMesg.length() > 0 ) {
    cout << errMesg << endl << endl;
    return;
  }
  cout << "\nyou typed:\n" << S;

  VectorOf<Word> V;

  SetIterator<Word> I(S);
  while ( !I.done() ) {
    V.append( I.value().freelyReduce() );
    I.next();
  }

  DecomposeInSubgroupOfFreeGroup D( F.rank(), V );

  // TEST: Decompose subgroup elements:

  Word w;
  do {
    cout << "Type a testing word (or empty word 1 for quit):" << endl;
    
    w = F.readWord( cin, errMesg );
    if ( errMesg.length() > 0 ) {
      cout << errMesg << endl << endl;
      return;
    }
    cout << "You typed: ";
    F.printWord( cout, w );
    cout << endl;

    w = w.freelyReduce();

    Word deco = D.decompose( w );
    
    cout << "The test word in original subgroup generators "
	 << "is expressed as follows:" << endl << "w = " << deco << endl;

    if( D.checkDecomposition( w ) != yes ) {
      cout << "** The algorithm produced WRONG answer." << endl;
    }

  }
  while ( w.length() != 0 );

}

void printProduct( ostream& ostr, const ProductOfRelatorConjugates& deco,
		   const FPGroup& G )
{
  for( int i = 0; i < deco.length(); ++i ) {
    ostr << ( i ? " * " : " = " ) << "(";
    G.printWord( ostr, deco[i].relator );
    ostr << ") ^ (";
    G.printWord( ostr, deco[i].conjugator );
    ostr << ")" << endl;
  }
  ostr << endl;
}

void testFPGroup()
{
  FPGroup G;

  cout << "\n\nType a finitely presented group:\n" << endl;

  Chars errMesg = cin >> G;
  if ( errMesg.length() > 0 ) {
    cout << errMesg << endl << endl;
    return;
  }
  cout << "\n\nYou typed:\n" << G << endl << endl;

  cout << "Type a set of generating words of a subgroup:" << endl;

  SetOf<Word> S = G.readSetOfWords(cin, errMesg);
  if ( errMesg.length() > 0 ) {
    cout << errMesg << endl << endl;
    return;
  }
  cout << "\nyou typed:\n" << S;

  VectorOf<Word> V;

  SetIterator<Word> I(S);
  while ( !I.done() ) {
    V.append( I.value().freelyReduce() );
    I.next();
  }

  DecomposeInSubgroupOfFPGroup D( G, V );

  // TEST: Decompose subgroup elements:

  Word w;
  do {
    cout << "Type a testing word (or empty word 1 for quit):" << endl;
    
    w = G.readWord( cin, errMesg );
    if ( errMesg.length() > 0 ) {
      cout << errMesg << endl << endl;
      return;
    }
    cout << "You typed: ";
    G.printWord( cout, w );
    cout << endl;

    w = w.freelyReduce();

    while( !D.contains(w) )
      D.expandGraph();

    SubgroupGraph &graph = (SubgroupGraph&)D.graph();
    cout << "w in nielsen words = " 
	 << graph.inNielsenWords(w) << endl;
    cout << "nielsen basis = " << graph.nielsenBasis() << endl;

    Word deco = D.decompose( w );
    
    cout << "The test word in original subgroup generators "
	 << "is expressed as follows:" << endl << "w = " << deco << endl;

    //    if( D.checkDecomposition( w ) != yes ) {
    cout //<< "** The algorithm produced WRONG answer." << endl
	   << "   full decomposition is: " << endl
	   << "   " << D.fullDecomposition(w) << endl;
    printProduct( cout, D.fullDecomposition(w), G );
      //    }

    ofstream ostr("out1");
    ostr< D;
    ostr.close();

    ifstream istr("out1");
    Timer timer(1000000);
    bool read = true;
    while( read ) {
      read = ! D.readPiece( istr, timer);
      timer.reset(1000000);
    }
    
    ostr.open("out2");
    ostr < D;
    ostr.close();

  }
  while ( w.length() != 0 );

}


main()
{
  // testFreeGroup();
  testFPGroup();
}
