// Principal Author: Roger Needham

// Informal test of SubGroupGraph class


#include "FreeGroup.h"
#include "SubgroupGraph.h"
#include "NormalRandomWord.h"

SetOf<Word> readSet(FGGroup& G)
{
  SetOf<Word> result;
  Chars errMesg;

  char ch = ' ';
  while ( ch != '{' ) cin >> ch;
  while ( ch != '}' ) {
	 Word w = G.readWord(cin, errMesg);
	 if ( errMesg.length() > 0 ) cout << errMesg << endl << endl;
	 result |= w;
	 cin >> ch;
  }
  return result;
}

void printWord(const Word& w)
{
  int len = w.length();
  cout << len << endl;
  for( int i = 0; i < len; ++i ) {
    int g = ord(w[i]);
    cout << char('a' + abs(g) - 1);
    if ( g < 0 ) cout << "-1";
    else cout << '1';
  }
  cout << endl;
}

Word conjugate(const Word& w1, const Word& w2)
{
  return w2.inverse() * ( w1 * w2 );
}

Word commutator(const Word& w1, const Word& w2)
{
  return w1.inverse() * conjugate(w1, w2);
}

main( ) {
  int numGens = 5;
  int meanWordLength = 5;
  int meanSubgroupGens = 5;
  int meanNumOfTest = 50;
  int testStdDev = 5;
  int lenSeed = 1000;
  int genSeed = 22000;
  int lenStdDev = 15;
  int baseSeed;

  cout << "\nGive me a seed: ";
  cin >> baseSeed;

  FreeGroup G(numGens);
  cout << "\nFree group: " << G;

  NormalRandom NR( baseSeed );

  unsigned meanLength1 = NR.rand(meanWordLength,lenStdDev);
  unsigned meanLength2 = NR.rand(meanWordLength,lenStdDev);
  unsigned lenStdDev1 = NR.rand(lenStdDev,10);
  unsigned lenStdDev2 = NR.rand(lenStdDev,10);
  unsigned lenSeed1 = NR.rand(lenSeed,800);
  unsigned lenSeed2 = NR.rand(lenSeed,800);
  unsigned genSeed1 = NR.rand(genSeed,8000);
  unsigned genSeed2 = NR.rand(genSeed,8000);

  NormalRandomWord RW1(numGens,meanLength1,lenStdDev1,lenSeed1,genSeed1);
  NormalRandomWord RW2(numGens,meanLength2,lenStdDev2,lenSeed2,genSeed2);

  unsigned numOfWord;      // number of generators for subgroup

  while ( 1 ) {
    // make random subgroup
    do {
      numOfWord = NR.rand(meanSubgroupGens,lenStdDev);
      numOfWord %= 20;
    } while ( numOfWord == 0 );
    SetOf<Word> subgroup1;
    VectorOf<Word> subgroupV1;

    int dummy;
    int i = 0;
    while ( i < numOfWord ) {
      Word w = RW1.word().freelyReduce();
      if ( w.length() < 10 ) {
	subgroup1.adjoinElement(w);
	subgroupV1.append(w);
	printWord(w);
	cout << endl;
	++i;
      }
    }
    
    cout << "numOfWord = " << numOfWord << endl;
    cout << "To continue press `1', to stop press `0': ";
    cin >> dummy;
    if ( !dummy ) return 1;

    SubgroupGraph SG1(numGens, subgroup1);

    unsigned numOfTest = NR.rand(meanNumOfTest,testStdDev);

/*
    ////////// 1. SubgroupGraphRep::inCommutatorSGR 
    //                        &
    //            SubgroupGraphRep::isCommutator
    cout << "\nTesting SubgroupGraphRep::inCommutatorSGR,";
    cout << " SubgroupGraphRep::isCommutator" << endl;

    numOfTest %= 50;
    for ( i = 0; i < numOfTest; i++ ) {
      SetOf<Word> commutators;
      // make a set of commutators:
      int numOfTest2 = NR.rand(20,20);
      numOfTest %= 50;
      for ( int j = 0; j < numOfTest2; j++ ) {
	Word w1 = RW1.word();
	Word w2 = RW2.word();
	commutators.adjoinElement(commutator(w1,w2).freelyReduce());
      }
      Word testWord;
      SetIterator<Word> I(commutators);
      while ( !I.done ) {
	testWord = testWord*I.value();
	Word u1,u2;
	if ( !SG1.isCommutator(I.value(),u1,u2) ) {
	  cout << "\n*** SubgroupGraphRep::isCommutator failed!" << endl;
	}
      }
      if ( !SG1.inCommutatorSGR(testWord) ) {
	cout << "\n*** SubgroupGraphRep::inCommutatorSGR failed!" << endl;
	exit(1);
      }
    }
*/

    ////////// 2. SubgroupGraphRep::contains 
    cout << "\nTesting SubgroupGraphRep::contains" << endl;

    NormalRandomWord RW3(subgroupV1.length(),meanLength1,lenStdDev1,lenSeed1,genSeed1);
    numOfTest = NR.rand(meanNumOfTest,testStdDev);
    numOfTest %= 50;
    SetOf<Word> testSet;
    VectorOf<Word> testVector;
    Word testWord;
    for ( i = 0; i < numOfTest; i++ ) {
      testWord = RW3.word().replaceGenerators(subgroupV1);
      testSet.adjoinElement(testWord);
      testVector.append(testWord);
      if ( !SG1.contains(testWord) ) {
	cout << "\n*** SubgroupGraphRep::contains(Word&) failed!" << endl;
	exit(1);
      }
    }
    if ( !SG1.contains(testSet) ) {
      cout << "\n*** SubgroupGraphRep::contains(SetOf<Word>&) failed!" << endl;
      exit(1);
    }
    if ( !SG1.contains(testVector) ) {
      cout << "\n*** SubgroupGraphRep::contains(VectorOf<Word>&) failed!" << endl;
      exit(1);
    }

    ////////// 3. SubgroupGraphRep::equalTo 
    ////////// 4. SubgroupGraphRep::conjugateInSubgroup
    cout << "\nTesting SubgroupGraphRep::equalTo,";
    cout << " SubgroupGraphRep::conjugateInSubgroup" << endl;

    numOfTest = NR.rand(meanNumOfTest,testStdDev);
    numOfTest %= 10;
    for ( i = 0; i < numOfTest; i++ ) {
      cerr << "Going for " << i << endl;
      SetOf<Word> equalSet;
      VectorOf<Word> equalVector;
      SetIterator<Word> I(subgroup1);
      while ( !I.done() ) {
	equalVector.append(I.value());
	I.next();
      }
      unsigned numOfChaos = NR.rand(meanNumOfTest,testStdDev);
      numOfChaos %= 10;
      for ( int j = 0; j < numOfChaos; j++ ) {
	unsigned index1 = NR.rand(meanNumOfTest,testStdDev);
	index1 %= equalVector.length();
	unsigned index2 = NR.rand(meanNumOfTest,testStdDev);
	index2 %= equalVector.length();
	if ( index1 != index2 ) {
	  equalVector[index1] = ((Word)equalVector[index1]*(Word)equalVector[index2]).freelyReduce();
	}
	else equalVector[index1] = ((Word)equalVector[index1]).inverse();
      }
      for ( j = equalVector.length()-1; j >= 0; j-- ) {
	equalSet.adjoinElement(((Word)equalVector[j]).freelyReduce());
      }
      if ( !SG1.equalTo(equalSet) ) {
        cout << "\n*** SubgroupGraphRep::equalTo failed!" << endl;
	exit(1);
      }

      SetIterator<Word> J(equalSet);
      SetOf<Word> conjSet;
      Word aW;
      Word conj1 = RW1.word();
      while ( !J.done() ) {
	Word tw = J.value();
	Word conj2 = RW1.word();
	if (!SG1.conjugateInSubgroup(((Word)tw.conjugateBy(conj2)).freelyReduce(),aW)) {
	  cout << "\n*** SubgroupGraphRep::conjugateInSubgroup(Word&,... failed!" << endl;
	  exit(1);
	}
	if(!SG1.contains(((Word)tw.conjugateBy(conj2).conjugateBy(aW)).freelyReduce())) {
          cout << "\n*** SubgroupGraphRep::conjugateInSubgroup(Word&,...) || SubgroupGraphRep::contains failed!" << endl;
          exit(1);
        }
	conjSet.adjoinElement(((Word)tw.conjugateBy(conj1)).freelyReduce());
	J.next();
      }
      // if ( !SG1.conjugateInSubgroup(conjSet,aW) ) {
      //   cout << "\n*** SubgroupGraphRep::conjugateInSubgroup(SetOf<Word>&,...) failed!" << endl;
      //   exit(1);
      // }
      // SetOf<Word> eqS;
      // SetIterator<Word> J1(conjSet);
      // while ( !J1.done() ) {
      //   eqS.adjoinElement(((Word)J1.value().conjugateBy(aW)).freelyReduce());
      //   J1.next();
      // }
      // if ( !SG1.equalTo(eqS) ) {
      //   cout << "\n*** SubgroupGraphRep::equalTo || SubgroupGraphRep::conjugateInSubgroup(SetOf<Word>&,...) failed!" << endl;
      //   exit(1);
      // }

      
    }

    ////////// 4. SubgroupGraphRep::powerInSubgroup
    cout << "\nSubgroupGraphRep::powerInSubgroup" << endl;

  }
}
