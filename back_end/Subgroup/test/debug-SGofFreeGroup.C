// Principal Author: Sergei Lyutikov


#include "SGofFreeGroup.h"

main()
{
  FreeGroup G;

  cout << "\n\nType a free group presentation:\n" << endl;

  Chars errMesg = cin >> G;

  if ( errMesg.length() > 0 ) cout << errMesg << endl << endl;
  else cout << "\n\nYou typed:\n" << G << endl << endl;

  cout << "Type a set of generating words:" << endl;

  SetOf<Word> S = G.readSetOfWords(cin, errMesg);
  if ( errMesg.length() > 0 ) cout << errMesg << endl << endl;
  cout << "\nyou typed:\n" << S;

  VectorOf<Word> V;

  SetIterator<Word> I(S);
  while ( !I.done() ) {
    V.append( I.value().freelyReduce() );
    I.next();
  }

  SGofFreeGroup SF(G,V);

  // TEST: 1. Subgroup structure methods:

  cout << "\nTEST subgroup structure methods:" << endl;
  cout << "\nIs normal: " << SF.isNormal() << endl;
  cout << "\nNormalizer: " << SF.normalizer() << endl;
  cout << "\nNielsen basis: " << SF.nielsenBasis() << endl;
  cout << "\nRank of subgroup: " << SF.rank() << endl;
  cout << "\nWhitehead basis: " << SF.findWhiteheadBasis() << endl;


  // TEST: 2. Subgroup elements:

  cout << "Type a set of testing words:" << endl;

  SetOf<Word> S2 = G.readSetOfWords(cin, errMesg);
  if ( errMesg.length() > 0 ) cout << errMesg << endl << endl;
  cout << "\nyou typed:\n" << S2;

  SetIterator<Word> J(S2);
  while ( !J.done() ) {
    Word temp = J.value().freelyReduce();
    cout << "\nWord in free group's generators: " << temp << endl;
    cout << "\nRight Schreier representative: " << 
      SF.rightSchreierRepresentative(temp) << endl;
    cout << "is in the subgroup: ";
    Bool cont = SF.contains(temp);
    if ( cont ) {
      cout << "YES" << endl;
      Word niel = SF.inNielsenWords(temp);
      cout << "in Nielsen set of generators: " << niel << endl;
    }
    else cout << "NO" << endl;
    Word conjugator;
    Bool conj = SF.conjugateInSubgroup(temp,conjugator);
    cout << "conjugate in the subgroup: ";
    if ( conj ) {
      cout << "YES" << endl;
      cout << "Here is a conjugator: " << conjugator << endl;
    }
    else cout << "NO" << endl;
    long power = SF.powerInSubgroup(temp);
    cout << "power in the subgroup: ";
    if ( power ) {
      cout << "YES" << endl;
      cout << "power: " << power << endl;
    }
    else cout << "NO" << endl;

    J.next();
  }


  // TEST: 3. Automorphisms.

  cout << "Type a set of generating words:" << endl;

  SetOf<Word> S3 = G.readSetOfWords(cin, errMesg);
  if ( errMesg.length() > 0 ) cout << errMesg << endl << endl;
  cout << "\nyou typed:\n" << S3;


  VectorOf<Word> V2;

  SetIterator<Word> I2(S3);
  while ( !I2.done() ) {
    V2.append( I2.value().freelyReduce() );
    I2.next();
  }

  if ( V2.length() != G.rank() ) error("*** is test-SGofFre...");

  cout << "is an automorphism: ";
  Bool aut = G.isAutomorphism(V2);
  if ( aut ) {
    cout << "YES" << endl;
    cout << "is an inner automorphism: ";
    if ( G.isInnerAutomorphism(V2) ) cout << "YES" << endl;
    else cout << "NO" << endl;
    cout << "inverse automorphism: " << G.inverseAutomorphism(V2) << endl;
  }
  else cout << "NO" << endl;

  return 1;
}
