#include "OneRelatorGroup.h"
#include "SubgroupOfOneRelatorGroup.h"

void printRelatorConjugates( ostream& ostr, const FGGroup& G, 
  const ProductOfRelatorConjugates& prodDeco )
{
  for( int i = 0; i < prodDeco.length(); ++i ) {
    if( i ) ostr << " * ";
    if( prodDeco[i].relator.length() > 1 )  ostr << "(";
    G.printWord( ostr, prodDeco[i].relator );
    if( prodDeco[i].relator.length() > 1 )  ostr << ")";
    ostr << "^(";
    G.printWord( ostr, prodDeco[i].conjugator );
    ostr << ")";
  }

}

void testWordProblem()
{
  FPGroup H;
  cout << "Testing word problem in one-relator group." << endl << endl
       << "Enter an one-relator group (e.g. < a,b,c | a b c >: ";
  Chars errMesg = cin >> H;
  if( errMesg.length() > 0 ) {
    cout << errMesg << endl;
    return;
  }

  OneRelatorGroup G(H);
  cout << "You entered: " << G << endl << endl;

  Word w;
  while( 1 ) {
    cout << "Type a word or empty word (1) to quit) : ";
    w = G.readWord( cin, errMesg );
    if( errMesg.length() > 0 ) {
      cout << errMesg << endl;
      return;
    }
    if( w.length() == 0 ) 
      break;

    cout << "Word ";
    G.printWord( cout, w );
    ProductOfRelatorConjugates prodDeco;
    const bool keepDetails = true; //false; //true;
    Trichotomy answer = G.wordProblem( w, keepDetails, prodDeco );
    cout << " is " << ( answer == yes ? "" : "not " ) << "identity," << endl
	 << " decomposition of word in relator conjugates is: " << endl;
    printRelatorConjugates( cout, G, prodDeco );
    cout << endl << "**************************" << endl << endl;

    if( answer == yes ) {
      Word w_check;
      for( int i = 0; i < prodDeco.length(); ++i ) {
	w_check *= prodDeco[i].relator.conjugateBy( prodDeco[i].conjugator );
      }
      if( w_check.freelyReduce() != w.freelyReduce() ) {
	cout << "THIS IS WRONG ANSWER!" << endl << "freely reduced word is ";
	G.printWord( cout, w_check.freelyReduce() );
	cout << endl << endl;
      }
    }

  }
}

void testSubgroupContainment( )
{
  FPGroup H;
  cout << "Testing containment problem in one-relator group." << endl << endl
       << "Enter an one-relator group (e.g. < a,b,c | a b c >: ";
  Chars errMesg = cin >> H;
  if( errMesg.length() > 0 ) {
    cout << errMesg << endl;
    return;
  }

  OneRelatorGroup G(H);
  cout << "You entered: " << G << endl << endl;

  cout << "Type a list of generators of a subgroup (empty word as end of list): " 
       << endl;
  VectorOf<Word> gens;
  while( 1 ) {
    Word g = G.readWord( cin, errMesg );
    if( g.length() == 0 ) break;
    if( errMesg.length() > 0 ) {
      cout << errMesg << endl;
      return;
    }
    gens.append( g );
  }

  SubgroupOfOneRelatorGroup S( G, gens );
  cout << endl << endl << "The subgroup is " << S << endl;

  Word w;
  while( 1 ) {
    cout << "Type a word or empty word (1) to quit) : ";
    w = G.readWord( cin, errMesg );
    if( errMesg.length() > 0 ) {
      cout << errMesg << endl;
      return;
    }
    if( w.length() == 0 ) 
      break;

    cout << "Word ";
    G.printWord( cout, w );

    ProductOfRelatorConjugates prodDeco;
    const bool keepDetails = false; //true;
    bool answer = S.contains( w, keepDetails );
    prodDeco = S.getIdentityProductDecomposition( w );
    if( answer ) {
      Word wbasis = S.rewriteFromGroupToSubgroupGenerators( w );

      cout << " in basis is " << wbasis << endl;

      Word w_check = S.rewriteFromSubgroupToGroupGenerators( wbasis );
      for( int i = 0; i < prodDeco.length(); ++i ) {
	w_check *= prodDeco[i].relator.conjugateBy( prodDeco[i].conjugator );
      }
      if( w_check.freelyReduce() != w.freelyReduce() ) {
	cout << "THIS IS WRONG ANSWER!" << endl << "freely reduced word is ";
	G.printWord( cout, w_check.freelyReduce() );
	cout << endl << endl;
      }

    }
    cout << " decomposition of word in relator conjugates is: " << endl;
    printRelatorConjugates( cout, G, prodDeco );
    cout << endl << "**************************" << endl << endl;

  }
}


main()
{
  testWordProblem();
  //  testSubgroupContainment();
}
