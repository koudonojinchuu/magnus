#include "RandomMSCGroup.h"

/*
Word initWord(const Generator* p, int len) {
  VectorOf<Generator> v(len);
  for( int i = 0; i < len; i ++ ) v[i] = *p++;
  return Word(v);
}
*/

int main() {

/*
  const Generator gl3[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1};
  Word r3 = initWord(gl3, 9);
  cout << r3 << endl;
*/

/*
  int numOfGens = 10;
  SetOf<Word> S;
  NormalRandomWord NRM(numOfGens, 10, 5, 1234, 2345);
  for ( int i = 0; i < 10; i++ ) {
    Word w = NRM.word();
    S |= w;
  }  
  FPGroup F(numOfGens, S);
  cout << S << endl;
  cout << F << endl;
*/

/*
  const int numOfGenerators        = 20;
  const int generatorsSeed         = 1234;
  const int meanLengthOfRelators   = 10;
  const int lengthOfRelatorsStddev = 5;
  const int lengthOfRelatorsSeed   = 2345;
  const int meanNumberOfRelators   = 5;
  const int numberOfRelatorsStddev = 2;
  const int numberOfRelatorsSeed   = 3456;
  const int numOfTests = 10;
*/

  int numOfGenerators;
  int generatorsSeed;
  int meanLengthOfRelators;
  int lengthOfRelatorsStddev;
  int lengthOfRelatorsSeed;
  int meanNumberOfRelators;
  int numberOfRelatorsStddev;
  int numberOfRelatorsSeed;
  int numOfTests;

  cout << "Enter parameters of a random MSC group: " << endl;
  cout << "numOfGenerators = "; cin >> numOfGenerators;
  cout << "generatorsSeed = "; cin >> generatorsSeed;
  cout << "meanLengthOfRelators = " ; cin >> meanLengthOfRelators;
  cout << "lengthOfRelatorsStddev = "; cin >> lengthOfRelatorsStddev;
  cout << "lengthOfRelatorsSeed = "; cin >> lengthOfRelatorsSeed;
  cout << "meanNumberOfRelators = "; cin >> meanNumberOfRelators;
  cout << "numberOfRelatorsStddev = "; cin >> numberOfRelatorsStddev;
  cout << "numberOfRelatorsSeed = "; cin >> numberOfRelatorsSeed;
  cout << "number of tests = "; cin >> numOfTests;
  cout << endl;
  
  RandomMSCGroup RG( numOfGenerators, generatorsSeed,
    meanLengthOfRelators, lengthOfRelatorsStddev, lengthOfRelatorsSeed, 
    meanNumberOfRelators, numberOfRelatorsStddev, numberOfRelatorsSeed );

  for ( int i = 0; i < numOfTests; i++ ) {
    FPGroup G = RG.getRandomMSCGroup();
    
    int lambda = G.cancellationLambda();
    cout << "Random MSC group:" << endl << G << endl
       << "Lambda = " << lambda << endl;
  }
  
/*FPGroup F(G.numOfGens(), G.getRelators());
  int lambda = G.getMSCLambda();
  cout << "Random MSC group:" << endl << F << endl
       << "Lambda = " << lambda << endl;
*/     
}

