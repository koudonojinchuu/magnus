#include "OneRelatorGroupWithTorsion.h"
#include "ShortenByRelators2.h"
#include "File.h"

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

void testPowerOfElt()
{
  FPGroup H;
  cout << "Testing power of element problem in one-relator group with torsion." << endl << endl
       << "Enter an one-relator group (e.g. < a,b,c | (a b)^5 >: ";
  Chars errMesg = cin >> H;
  if( errMesg.length() > 0 ) {
    cout << errMesg << endl;
    return;
  }

  OneRelatorGroupWithTorsion G(H);
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

    cout << "Order of ";
    G.printWord( cout, w );
    ProductOfRelatorConjugates prodDeco;
    Word st, x;
    int order = G.powerOfElt( w, st, x, prodDeco );
    cout << " is " << order << " and word's decomposition is ( ";
    G.printWord( cout, st);
    cout << " )^( ";
    G.printWord( cout, x);
    cout << " ) * " << prodDeco << endl << endl;
  }
}


void testMaximalRoot()
{
  FPGroup H;
  cout << "Testing maximal root problem in one-relator group with torsion."
       << endl << endl
       << "Enter an one-relator group (e.g. < a,b,c | (a b)^5 >: ";

  Chars errMesg = cin >> H;
  if( errMesg.length() > 0 ) {
    cout << errMesg << endl;
    return;
  }

  OneRelatorGroupWithTorsion G(H);
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

    Word root;
    int power;
    Trichotomy answer = G.maximalRoot( w, root, power );
    if( answer == dontknow ) {
      cout << "The given word is ";
      G.printWord( cout, w );
      cout << ", " << endl << "the root is ";
      G.printWord( cout, root );
      cout << endl << "The answer cannot be determined." << endl;
    }
    else {
      if( G.areEqual( G.raiseToPower(root,power), w ) != yes )
	cout << "WRONG ANSWER:";
      
      cout << "The given word is ";
      G.printWord( cout, w );
      cout << ", " << endl << "the root is ";
      G.printWord( cout, root );
      cout << "," << endl << "the maximal power = " << power << endl;
    }
  }
}


void testConjugacyProblem()
{
  FPGroup H;
  cout << "Testing conjugacy problem in one-relator group with torsion."
       << endl << endl
       << "Enter an one-relator group (e.g. < a,b,c | (a b)^5 >: ";

  Chars errMesg = cin >> H;
  if( errMesg.length() > 0 ) {
    cout << errMesg << endl;
    return;
  }

  OneRelatorGroupWithTorsion G(H);
  cout << "You entered: " << G << endl << endl;

  Word w;
  while( 1 ) {
    cout << "Type a first word or empty word (1) to quit) : ";
    Word u = G.readWord( cin, errMesg );
    if( errMesg.length() > 0 ) {
      cout << errMesg << endl;
      return;
    }
    if( u.length() == 0 ) 
      break;

    cout << "Type a second word or empty word (1) to quit) : ";
    Word v = G.readWord( cin, errMesg );
    if( errMesg.length() > 0 ) {
      cout << errMesg << endl;
      return;
    }
    if( v.length() == 0 ) 
      break;

    Word conj;
    bool answer = G.conjugacyProblem( u, v, conj );
    cout << endl << " wp = " <<  u.conjugateBy(conj) * v.inverse() << endl;
    if( answer != G.areEqual( u.conjugateBy(conj), v )  )
      cout << "WRONG ANSWER:";

    cout << "The words are" << ( answer ? " " : " not" ) 
	 << "conjugated, the conjugator word is ";
    G.printWord( cout, conj );
    cout << "." << endl;
  }
}

#define file cout

printWordReductionDetails( const FPGroup& G, const Word& w )
{
  ShortenByRelators2 S( G.getRelators() );
  ProductOfRelatorConjugates prodDeco;
  Word trivial = S.expressWordInConjugatesOfRelators( w, prodDeco );
  Word u = w;

  file << prodDeco << endl;

  for( int i = prodDeco.length() - 1; i >= 0; --i ) {
    Word relator = prodDeco[i].relator;
    Word conjugator = prodDeco[i].conjugator;

    int shift = relator.agreementLength( conjugator );
    conjugator = conjugator.subword( shift, conjugator.length() );

    int tail = tailAgreementLength( u, conjugator );
    int bLen = conjugator.length() - tail;
    int aLen = relator.length() - bLen;

    file << "Using the relator ";
    // file.setColor( titleColor );
    G.printWord( file, relator );
    //file.setColor( mainColor );
    file << " : " << endl;
    if( u.length()-tail-aLen > 0 ) 
      G.printWord( file, u.initialSegment(u.length()-tail-aLen) );
    file << ' ';
    file << "_"; // file.setColor( titleColor );
     G.printWord( file, u.subword(u.length()-tail-aLen, u.length()-tail) );
    file << "_"; //file.setColor( mainColor );
    file << ' ';
    if( tail > 0 )
      G.printWord( file, u.terminalSegment( tail ) );
    file << " --> ";
    if( u.length()-tail-aLen > 0 ) 
      G.printWord( file, u.initialSegment(u.length()-tail-aLen) );
    if( bLen > 0 ) {
      file << "_"; //file.setColor( titleColor );
      file << ' ';
      G.printWord( file, conjugator.initialSegment( bLen ) );
      file << "_"; //file.setColor( mainColor );
    }
    file << ' ';
    if( tail )
      G.printWord( file, u.terminalSegment( tail ) );
    file << endl << endl;

    u =  u.initialSegment(u.length()-tail-aLen) 
      * conjugator.initialSegment( bLen )
      * u.terminalSegment( tail );

    if( u.freelyReduce().length() != u.length() ) {
      file << "Freely reducing :" << endl;
      G.printWord( file, u );
      file << " --> ";
      u = u.freelyReduce();
      G.printWord( file, u );
      file << endl << endl;
    }
  }

  file << "Got the empty word. Reduction is finished." << endl;
  file << end;
}

void testShortening()
{
  FPGroup G;
  cout << "Testing MSC-reduction algorithm" << endl << endl
       << "Enter an MSC-group (e.g. < a,b,c | (a b)^5 >: ";

  Chars errMesg = cin >> G;
  if( errMesg.length() > 0 ) {
    cout << errMesg << endl;
    return;
  }

  while( 1 ) {
    cout << "Type a word or empty word (1) to quit) : ";
    Word u = G.readWord( cin, errMesg );
    if( errMesg.length() > 0 ) {
      cout << errMesg << endl;
      return;
    }
    if( u.length() == 0 ) 
      break;
    printWordReductionDetails( G, u );
  }

}

void showFile( const Chars& filename, ostream& ostr )
{
  ifstream istr( filename );
  if( !istr )
    return;

  while( !istr.eof() ) {
    ostr.put( (char)istr.get() );
  }
}

void testWordProblem()
{
  FPGroup H;
  cout << "Testing word problem in one-relator group with torsion.\n\n"
       << "Enter an one-relator group with torsion (e.g. <a,b,c | (a b)^5> ):";
  Chars errMesg = cin >> H;
  if( errMesg.length() > 0 ) {
    cout << errMesg << endl;
    return;
  }

  OneRelatorGroupWithTorsion G(H);
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

    ProductOfRelatorConjugates prodDeco;
    Trichotomy trivial = G.wordProblem( w, prodDeco );
    //    if( trivial == yes ) {
      DetailedReport report( H );
      report.printDehnTransformationDetails( w );
      showFile( report.getFileName(), cout );
      //    }
  }

}


main()
{
  testWordProblem();
  // testPowerOfElt();
  // testMaximalRoot();
  // testConjugacyProblem();
  //testShortening();
}
