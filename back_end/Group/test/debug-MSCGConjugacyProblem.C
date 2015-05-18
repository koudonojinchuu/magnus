// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: informal test of MSCGConjugacyProblem class
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//

#include "iostream.h"
#include "MSCGConjugacyProblem.h"
#include "NormalRandomWord.h"

ostream& operator << (ostream& o, Trichotomy t)
{
  if ( t == YES ) o << " YES ";
  else if ( t == NO ) o << " NO ";
  else o << " DONTKNOW ";

  return o;
}

Word initWord(const Generator* p, int len) {
  VectorOf<Generator> v(len);
  for( int i = 0; i < len; i ++ ) v[i] = *p++;
  return Word(v);
}

void singleTest() {
  FPGroup F;
  cout << "Enter a MSC represenatation, i.e. <a,b,c| a^7, b = c> : " << endl;
  Chars errMsg = cin >> F;
  if( errMsg.length() > 0 ) {
    cout << errMsg << endl;
      exit(-1);
  }

  cout << endl;
  MSCGroup G(F);
  cout << "You typed the MSC group: " << F << endl << endl;

  bool quit = false;
  while( !quit ) {
    cout << "Type a word: ";
    Word w = F.readWord(cin, errMsg);
    if( errMsg.length() > 0 ) {
      cout << errMsg << endl;
      exit(0);
    }
    cout << endl << "You typed: ";
    F.printWord(cout, w);
    
    cout << endl << "type a second word: ";
    Word u = F.readWord(cin, errMsg);
    if( errMsg.length() > 0 ) {
      cout << errMsg << endl;
      exit(0);
    }

    cout << endl << "you typed: ";
    F.printWord(cout, u);

    MSCGConjugacyProblem CP(G,w,u);

    cout << endl << "conjugacyProblem says ... ";
    CP.startComputation();
    while( !CP.continueComputation() ) ;
    Trichotomy answer = CP.answer(); 
    cout<< answer << endl;
    if ( answer == YES ) {
      cout << "The conjugator: ";
      Word conjugator = CP.conjugator();
      F.printWord(cout, conjugator);
      cout << endl;
      cout << "Checking: ";
      bool bChecking = G.areEqual(w.conjugateBy(conjugator),u);
      if ( bChecking )
	cout << "success";
      else
	cout << "error";
      cout << endl << endl;
    }
    char reply;
    cout << "Repeat (y/n)? ";
    do {
      cin >> reply;
      reply = tolower(reply);
    } while( reply != 'y' && reply != 'n' );
    if( reply == 'n' ) quit = true;
    cout << endl;
  }
}

void error( const Chars& msg, const FPGroup& G, const Word& w, const Word& c,
	    const Word& u, const Word& conjugator )
{
  cout << msg << ": w = ";
  G.printWord( cout, w );
  cout << ", c = ";
  G.printWord( cout, c );
  cout << ", u = ";
  G.printWord( cout, u );
  cout << ", found conjugator = ";
  G.printWord( cout, conjugator );
  cout << endl;
}

void randomTests( )
{
  FPGroup F;
  cout << "Enter a MSC represenatation, i.e. <a,b,c| a^7, b = c> : " << endl;
  Chars errMsg = cin >> F;
  if( errMsg.length() > 0 ) {
    cout << errMsg << endl;
      exit(-1);
  }

  cout << endl;
  MSCGroup G(F);
  cout << "You typed the MSC group: " << F << endl << endl;

  int meanLen;
  cout << "Type average length of test words:";
  cin >> meanLen;
  cout << meanLen << endl;

  int numberOfTests;
  cout << "Type number of tests: ";
  cin >> numberOfTests;
  cout << numberOfTests << endl;

  NormalRandomWord rw( F.numberOfGenerators(), 
		       meanLen, 
		       meanLen/2,	// lengthStdDev
		       2345,		// lengthSeed
		       1234		// generatorSeed
		       );
    
  int failed = 0;
  int passed = 0;
  Word w,c,u;
  for( int test = 0; test < numberOfTests; ++test ) {
    w = rw.word();
    int probe = 0; 
    do {
      c = rw.word();
      u = w.conjugateBy(c);
      u = u.cyclicallyReduce();
      ++probe;
    } while( w == u && probe < 1000 );
    if( probe < 1000 ) {
      MSCGConjugacyProblem CP(G,w,u);
      CP.startComputation();
      while( !CP.continueComputation() ) ;
      Trichotomy answer = CP.answer(); 
      if ( answer == yes ) {
	Word conjugator = CP.conjugator();
	bool bChecking = G.areEqual(w.conjugateBy(conjugator),u);
	if ( bChecking )
	  ++passed;
	else {
	  error("bad conj", F, w,c,u,conjugator);
	  ++failed;
	}
      }
      else if( answer == no ) {
	error("negative answer", F, w,c,u, Word() );
	++failed;
      }
      else {
	error("no answer", F, w,c,u, Word() );
	++failed;
      }
    }
  }
  cout << endl << endl 
       << "+ " << passed << " tests passed." << endl
       << "- " << failed << " tests failed." << endl;
}

void main( )
{
  randomTests();
  //  singleTest();
}


