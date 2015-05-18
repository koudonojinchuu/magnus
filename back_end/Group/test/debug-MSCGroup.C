// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: informal test of MSCGroup class
//
// Principal Author: Dmitry Pechkin
//
// Status:
//
// Revision History:
//

#include "MSCGroup.h"
#include "PresentationParser.h"

ostream& operator << (ostream& o, Trichotomy t)
{
  if ( t == YES ) o << " YES ";
  else if ( t == NO ) o << " NO ";
  else o << " DONTKNOW ";

  return o;
}


void printInfo(const char *msg, MSCGroup& G) {
	cout << msg              << G << endl;
	cout <<  "order="        << G.order();
	cout << " isAbelian()="  << G.isAbelian();
	cout << " isInfinite()=" << G.isInfinite();
	cout << " isFinite()="   << G.isFinite();
	cout << " isFree()="     << G.isFree() << endl;
	cout << endl;
}

Word initWord(const Generator* p, int len) {
  VectorOf<Generator> v(len);
  for( int i = 0; i < len; i ++ ) v[i] = *p++;
  return Word(v);
}

void main()
{

  {	// Test of properties of a MSC group ( order, isAbelian, isFinite,
	// isInfinite, isFree ).
	// Test data for this section is placed in debug-MSCGroup.data
	// Testout is debug-MSCGroup.testout

	bool quit = false;
	while( !quit ) {
		MSCGroup G;
		cout << endl << "Enter a MSC group: " << endl;
		Chars errMsg = cin >> G;
		if( errMsg.length() > 0 ) {
			cout << errMsg;
			exit(0);
		}
		cout << endl;

		printInfo("You entered MSC group : ", G);

		char reply;
		cout << "Repeat (y/n)? ";
		do {
			cin >> reply;
			reply = tolower(reply);
		} while( reply != 'y' && reply != 'n' );
		if( reply == 'n' ) quit = true;
	}
  }

  {	// test MSCGroup::addRelators(), removeRelators(), setRelators()
	// There are no input data. Testout is debug-MSCGroup.testout2.


	const Generator gl3[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1};
	const Generator gl4[] = { 2, 2, 2, 2, 2, 2, 2, 2, 2 };
	const Generator gl5[] = { 1, 2, 3, -1, 2, -3, -3 };
	
	Word r3, r4, r5;
	r3 = initWord(gl3, 9);
	r4 = initWord(gl4, 9);
	r5 = initWord(gl5, 7);

	MSCGroup G3(3, r3);
	cout << "G3=" << G3 << endl;

	G3.addRelators( r4 );
	cout << "add "; G3.printWord(cout, r4); cout << ", G3="  << G3 << endl;

	G3.addRelators( r5 );
	cout << "add "; G3.printWord(cout, r5); cout << ", G3="  << G3 << endl;

	G3.removeRelators(r4);
	cout << "remove "; G3.printWord(cout, r4); cout << ", G3=" << G3 << endl;

	G3.removeRelators(r5);
	cout << "remove "; G3.printWord(cout, r5); cout << ", G3=" << G3 << endl;

	SetOf<Word> rels3;
	rels3 |= r3;
	rels3 |= r4;
	rels3 |= r5;
	G3.setRelators(rels3);
	cout << "set " << rels3 << ": G3=" << G3 << endl;

	// test constructors with different lambda value
	MSCGroup G4(3, rels3);
	cout << "given lambda=DONTKNOW, G=" << G4 << endl;

	MSCGroup G5(3, rels3, 6);
	cout << "given lambda=6, G=" << G5 << endl;

	MSCGroup G6(3, rels3, 3); // force rin-time warning message in constructor
	cout << "given lambda=3, G=" << G6 << endl;

	// MSCGroup G7(3, rels3, 7); // force run-time error in constructor

	MSCGroup F(1);
	printInfo("Free group of rank 1: ", F);

	const Generator gl1[] = { 1, 1, 1, 1, 1};
	Word r1 = initWord(gl1, 5);
	F.addRelators(r1);
	printInfo("A cyclic group of finite order: ", F);

	MSCGroup G2(2);
	printInfo("Free group of rank 2: ", G2);

	const Generator gl2[] = { 2 };
	Word r2 = initWord(gl2, 1);
	G2.addRelators(r1);
	G2.addRelators(r2);
	printInfo("MSCGroup(order=5): ", G2);

	G2.removeRelators(r1);
	printInfo("MSCGroup(order=0): ", G2);
  }

  {	// Test of wordProblem, cobnjugacyProblem of MSCGroup
	// Input data can be found in debug-MSCGroup.data3
	// Testout is debug-MSCGroup.testout3

	MSCGroup G;
	cout << "Enter a MSC represenatation, i.e. <a,b,c| a^7, b = c> : " << endl;
	Chars errMsg = cin >> G;
	if( errMsg.length() > 0 ) {
		cout << errMsg << endl;
		exit(-1);
	}
	cout << endl;
	cout << "You typed the MSC group: " << G << endl << endl;
	
	while( 1 ) {
		cout << "Type a word: ";
		Word w = G.readWord(cin, errMsg);
		if( errMsg.length() > 0 ) {
			cout << errMsg << endl;
			exit(0);
		}
		cout << endl << "You typed: ";
		G.printWord(cout, w);
		cout << endl << "wordProblem says " << G.wordProblem(w) << endl;

		cout << endl << "type a second word (for conjugacy problem): ";
		Word u = G.readWord(cin, errMsg);
		if( errMsg.length() > 0 ) {
			cout << errMsg << endl;
			exit(0);
		}
		cout << endl << "you typed: ";
		G.printWord(cout, u);
		cout << endl << "conjugacyProblem says " << G.conjugacyProblem(w,u) << endl;

	}
  }

}
