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

#include "FreeGroup.h"
#include "DoubleCosetGraph.h"


Word initWord(const Generator* p, int len) {
  VectorOf<Generator> v(len);
  for( int i = 0; i < len; i ++ ) v[i] = *p++;
  return Word(v);
}


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


int main ( )
{

  FreeGroup F;
  cout << "Type a free group presentation:" << endl;
  Chars errMesg = cin >> F;

  if( errMesg.length() > 0 ) {
    cout << errMesg << endl;
    return 1;
  }
  cout << "You typed:" << endl << F << endl;

  cout << "Enter a set of generators of subgroup H:" << endl;
  SetOf<Word> SH = F.readSetOfWords(cin, errMesg);
  if( errMesg.length() > 0 ) {
    cout << errMesg << endl;
    return 1;
  }
  cout << "You typed:" << endl << SH << endl;
  
  SubgroupGraph H(F.rank(), SH);

  int len;
  int HMaxGenLen = 0;
  SetIterator<Word> I(SH);
  for( ; !I.done(); I.next() )
    if( ( len = ( I.value() ).length() ) > HMaxGenLen )
      HMaxGenLen = len;
  
  
  cout << "Enter a set of generators of subgroup K:" << endl;
  SetOf<Word> SK = F.readSetOfWords(cin, errMesg);
  if( errMesg.length() > 0 ) {
    cout << errMesg << endl;
    return 1;
  }
  cout << "You typed:" << endl << SK << endl;
  
  SubgroupGraph K(F.rank(), SK);

  int KMaxGenLen = 0;
  I = SetIterator<Word>(SK);
  for( I.reset(); !I.done(); I.next() )
    if( ( len = I.value().length() ) > KMaxGenLen )
      KMaxGenLen = len;

  
  cout << "Enter the word U: " << endl;
  Word U = F.readWord(cin, errMesg);
  if( errMesg.length() > 0 ) {
    cout << errMesg << endl;
    return 1;
  }
  cout << "You typed:" << endl << U << endl;
  
  H.debugPrint(cout);
  DoubleCosetGraph DCG(H, U, K, HMaxGenLen, KMaxGenLen);
  DCG.debugPrint();

  cout << "Type a word: ";
  Word w = F.readWord(cin, errMesg);
  if( errMesg.length() > 0 ) {
    cout << errMesg << endl;
    return 1;
  }

  Word W = w.freelyReduce();
  cout << "You typed:" << endl << W << endl << endl;

  if( DCG.contains(W) ) 
    cout << "This word IS in graph.";
  else
    cout << "This word is NOT in graph.";
  cout << endl;
}

