
#include "APofFreeGroups.h"
#include "NormalRandomWord.h"

void testManually() 
{
  AmalgProductOfFreeGroups AP;
  
  cout << "Enter an amalgamated product of free group "
    "(e.g < <a,b>*<c,d>; a = c>): "
       << endl;
  Chars errMesg = cin >> AP; 
  if( errMesg.length() > 0 ) {
    cout << errMesg << endl;
    return;
  }
  cout << "You entered: " << AP << endl << endl;

  cout << "Testing the amalgamated product of free groups: " << endl;
  // within loop:
  while(1)  {
    cout << "Enter your word (or 1 to quit): " << endl;
    Word w = AP.readWord(cin, errMesg);
    if( errMesg.length() > 0 )  {
      cout << endl << errMesg << endl;
      return;
    }
    
    cout << "You entered: ";
    AP.printWord(cout, w);
    cout << endl;
    
    if( w.length() == 0 ) {
      cout << "empty word entered. Exiting..\n";
      break;
    }
    
    // print its decomposition
    VectorOf<Word> deco = AP.decompose(w);
    cout << "Decomposition is: ";
    AP.printVectorOfWords(cout, deco);
    cout << endl;
    
    // print its reduced decomposition
    VectorOf<Word> rdeco = AP.reducedDecomposition(w);
    cout << "Reduced decomposition is: ";
    AP.printVectorOfWords(cout, rdeco);
    cout << endl;
    
    // print its normal decomposition
    VectorOf<Word> ndeco = AP.normalDecomposition(w);
    cout << "Normal decomposition is: ";
    AP.printVectorOfWords(cout, ndeco);
    cout << endl;
    
    // check:
    // equality problem for reduced and word
    cout << "Equality test (w, rdeco) ";
    if( AP.areEqual(w, compose(rdeco)) == YES )
      cout << "passed" << endl;
    else cout << "failed" << endl;
    
    // equality problem for normal and word
    cout << "Equality test (w, ndeco) ";
    if( AP.areEqual(w, compose(ndeco)) == YES )
      cout << "passed" << endl;
    else cout << "failed" << endl;
    
    // equality problem for normal and reduced
    cout << "Equality test (ndeco, rdeco) ";
    if( AP.areEqual(compose(ndeco), compose(rdeco)) == YES )
      cout << "passed" << endl;
    else cout << "failed" << endl;
  }
}



void testRandom()  
{
  
  cout << "This test generates random amalgamated products of free groups with"
    " one relator.\n";
  
  int seed;
  cout << "Enter an initial seed: ";
  cin >> seed;

  UniformRandom ur(seed);

  VectorOf<Chars> f1_names(ur.rand(1,5));
  int genNameBase = 'a';
  for(int i = 0; i < f1_names.length(); ++i)
    f1_names[i] = Chars( genNameBase+i );

  VectorOf<Chars> f2_names(ur.rand(1,5));
  genNameBase = 'a' + f1_names.length();
  for(int i = 0; i < f2_names.length(); ++i)
    f2_names[i] = Chars( genNameBase+i );
	
  FreeGroup F1( f1_names );
  FreeGroup F2( f2_names );

  NormalRandomWord nrw1( 
			F1.numberOfGenerators(),
			5 /* meanLength */, 
			3 /* lengthStddev */,
			seed /* lengthSeed */,
			seed+100 /* generatorSeed */
			);
  NormalRandomWord nrw2( 
			F2.numberOfGenerators(),
			5 /* meanLength */, 
			3 /* lengthStddev */,
			seed+200 /* lengthSeed */,
			seed+300 /* generatorSeed */
			);

  VectorOf<Word> F1_sub(1);
  VectorOf<Word> F2_sub(1);
  F1_sub[0] = nrw1.word().freelyReduce();
  F2_sub[0] = nrw2.word().freelyReduce();

  AmalgProductOfFreeGroups AP(F1, F2, F1_sub, F2_sub);

  cout << "Random generated AP group is " << AP << endl << endl;


  int meanLength, lengthStddev, lengthSeed, generatorSeed;

  cout << "Now enter parameters for random word to testing:" << endl;
  cout << "mean length of word = "; cin >> meanLength;
  cout << "word length stddev  = "; cin >> lengthStddev;
  cout << "word length seed    = "; cin >> lengthSeed;
  cout << "generator seed      = "; cin >> generatorSeed;

  cout << endl << "You entered: meanLength=" << meanLength 
       << ", lengthStddev=" << lengthStddev << ", lengthSeed=" << lengthSeed 
       << ", genSeed=" << generatorSeed << endl << endl;

  NormalRandomWord randWord(AP.numberOfGenerators(), meanLength, lengthStddev,
			    lengthSeed, generatorSeed);

  {
    int maxEquTests = 10/*1000*/;
    cout << "Testing word problem (reduced form and normal form)." << endl;
    int passed = 0;
    int failed = 0;
    for(int i=0; i<maxEquTests; i++)  {
      Word w = randWord.word();
			
      Word rform = AP.reducedFormOf(w);
      Word nform = AP.normalFormOf(w);
			
      if( AP.areEqual(w, rform) != yes ||
	  AP.areEqual(w, nform) != yes ||
	  AP.areEqual(rform, nform) != yes ) {
	cout << "Failed equality test for the word ";
	AP.printWord(cout, w);
	cout << endl;
	++failed;
      }
      else
	++passed;
    }
    cout << "equality test's final result:" << endl;
    cout << " + " << passed << " tests passed.\n";
    if( failed ) cout << " - " << failed << " tests failed.\n";
		
  }

  {
    cout << "Test of `maximal root' algorithm (part I)" << endl;
    int maxRootTests = 1000;
    int passed = 0;
    int failed = 0;

    for(int i=0; i<maxRootTests; i++)  {
      Word w = randWord.word();
      Word root;
      int power = 0;
      AP.maximalRoot(w, root, power);
      Word u = AP.raiseToPower(root,power);
      if( AP.areEqual(w, u) != yes ) {
	//				cout << "-*failure*- ";
	++failed;
      }
      else
	++passed;
      /*
	cout << "the word ";
	AP.printWord(cout, w); 
	cout << " with root ";
	AP.printWord(cout, root);
	cout << " and power = " << power << endl;
	*/
    }
    cout << "maximalRoot::part I final results:" << endl;
    cout << " + " << passed << " tests passed.\n";
    if( failed ) cout << " - " << failed << " tests failed.\n";
  }		

  {
    cout << "Test of `maximal root' algorithm (part II)" << endl;
    int maxRootTests = 1000;
    int passed = 0;
    int failed = 0;
    UniformRandom ur(seed);
    for(int i=0; i<maxRootTests; i++)  {
      Word wRoot = randWord.word();
      int wPower = ur.rand(0,20);
      Word w = Word(AP.raiseToPower(wRoot, wPower)).freelyReduce();
      Word root;
      int power = 0;
      AP.maximalRoot(w, root, power);
      Word u = AP.raiseToPower(root,power);
      bool equalFailure = false;
      if( AP.areEqual(w, u) != yes ) {
	cout << "-*failure equality*- ";
	equalFailure = true;
	++failed;
      }
      if( power < wPower ) {
	if( equalFailure ) 
	  cout << endl;
	else
	  ++failed;
	cout << "-+failure power*-";
      }
      else
	++passed;

      cout << "the word ";
      AP.printWord(cout, w); 
      cout << " with root ";
      AP.printWord(cout, root);
      cout << " and power = " << power << endl;
    }
    cout << "maximalRoot::part II final results:" << endl;
    cout << " + " << passed << " tests passed.\n";
    if( failed ) cout << " - " << failed << " tests failed.\n";
  }		

}


void testAPORGroup() 
{
  AmalgProductOfFreeGroups AP;

  cout << "Enter an amalgamated product of free group with one relator "
    "(e.g < <a,b>*<c,d>; a = c>): "
       << endl;
  Chars errMesg = cin >> AP; 
  if( errMesg.length() > 0 ) {
    cout << errMesg << endl;
    return;
  }
  cout << "You entered: " << AP << endl << endl;

  cout << "Testing the amalgamated product of free groups: " << endl;
  // within loop:
  while(1)  {
    cout << "Enter your word (or 1 to quit): " << endl;
    Word w = AP.readWord(cin, errMesg);
    if( errMesg.length() > 0 )  {
      cout << endl << errMesg << endl;
      return;
    }

    cout << "You entered: ";
    AP.printWord(cout, w);
    cout << endl;

    if( w.length() == 0 ) {
      cout << "empty word entered. Exiting..\n";
      break;
    }

    // print its decomposition
    VectorOf<Word> deco = AP.decompose(w);
    cout << "Decomposition is: ";
    AP.printVectorOfWords(cout, deco);
    cout << endl;

    // print its reduced decomposition
    VectorOf<Word> rdeco = AP.reducedDecomposition(w);
    cout << "Reduced decomposition is: ";
    AP.printVectorOfWords(cout, rdeco);
    cout << endl;

    // print its normal decomposition
    VectorOf<Word> ndeco = AP.normalDecomposition(w);
    cout << "Normal decomposition is: ";
    AP.printVectorOfWords(cout, ndeco);
    cout << endl;

    // print its cyclic normal decomposition
    VectorOf<Word> cdeco;
    Word conj;
    AP.cyclicDecomposition(w, cdeco, conj);
    cout << "Cyclic normal decomposition is: ";
    AP.printVectorOfWords(cout, cdeco);
    cout << endl;
		
    { // check APGroup::maximalRoot(...)
      Word root;
      int power;
      AP.maximalRoot(w, root, power);
      cout << "Maximal root is ";
      AP.printWord(cout, root);
      cout << ", power is " << power << endl;
      Word ww = root;
      for(int i = 1; i < power; ++i)
	ww = ww * root;
      Word nww = AP.normalFormOf(ww);
      Word nw  = AP.normalFormOf(w);
      if( nw != nww ) {
	cout << "  * error: normal form of w is ";
	AP.printWord(cout, nw);
	cout << "\n        normal form of root^power is ";
	AP.printWord(cout, nww);
	cout << endl;
      }
    }

    // check:
    // equality problem for reduced and word
    cout << "Equality test (w, rdeco) ";
    if( AP.areEqual(w, compose(rdeco)) == YES )
      cout << "passed" << endl;
    else cout << "failed" << endl;

    // equality problem for normal and word
    cout << "Equality test (w, ndeco) ";
    if( AP.areEqual(w, compose(ndeco)) == YES )
      cout << "passed" << endl;
    else cout << "failed" << endl;

    // equality problem for normal and reduced
    cout << "Equality test (ndeco, rdeco) ";
    if( AP.areEqual(compose(ndeco), compose(rdeco)) == YES )
      cout << "passed" << endl;
    else cout << "failed" << endl;

    cout << endl << endl;
  }

}

void main()
{
  // testAPORGroup();
  testManually();
  // testRandom();
}
