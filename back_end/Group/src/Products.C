// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of the FreeProducts class.
//
// Principal Author: Alexei Myasnikov
//
// Status: in progress
//
// Revision History:
//
// Special Notes:
//
//

#include "Products.h"
#include "conversions.h"

void FreeProduct::createGenerators()
{

  VectorOf<Chars> v1 = G1.namesOfGenerators();
  VectorOf<Chars> v2 = G2.namesOfGenerators();

  SetOf<Chars> gensSet = makeSetOf(v1);
  VectorOf<Chars> newG2Gens(v2.length());

  for (int i = 0;i<v2.length();i++) {
    int suffix = 1;
    Chars gName = v2[i];
    while ( gensSet.contains(gName) ) {
      gName += Chars(suffix);
      suffix++;
    }
    gensSet |= gName;
    newG2Gens[i] = gName;
  }

  theGenerators = concatenate(v1,newG2Gens);
  gensInitialized = true;
}

void FreeProduct::createRelators()
{
  if (!gensInitialized)
    error("void FreeProducts::createRelators(const SetOf<Word>& s1,const SetOf<Word>& s2): The generators must be initialized first");
  

  SetOf<Word> s1 = G1.getRelators();
  SetOf<Word> s2 = G2.getRelators();
  int numberOfG1Gens = G1.numberOfGenerators();
  int numberOfG2Gens = G2.numberOfGenerators();


  theRelators = s1;
  VectorOf<Word> gensOfG2(numberOfG2Gens);
  for (int i = 0; i < numberOfG2Gens;i++)
    gensOfG2[i] = Generator(numberOfG1Gens + i + 1);

  SetIterator<Word> I(s2);

  for (I.reset();!I.done();I.next())
    theRelators |= I.value().replaceGenerators( gensOfG2 );
}
  
// --------------- Maps -------------------

Map FreeProduct::mapFromG1() const
{
  VectorOf<Word> image(G1.numberOfGenerators());
  for (int i = 0;i<G1.numberOfGenerators();i++)
    image[i] = Generator(i+1);
  return Map(G1,(FPGroup)*this,image);
}

Map FreeProduct::mapFromG2() const
{
  VectorOf<Word> image(G2.numberOfGenerators());
  for (int i = 0;i<G2.numberOfGenerators();i++)
    image[i] = Generator(G1.numberOfGenerators()+i+1);
  return Map(G2,(FPGroup)*this,image);
}

Map FreeProduct::mapToG1() const
{
  VectorOf<Word> image(theGenerators.length());
  for (int i = 0;i<G1.numberOfGenerators();i++){
    image[i] = Generator(i+1);
  }
  return Map((FPGroup)*this, G1, image);
}
Map FreeProduct::mapToG2() const
{
  VectorOf<Word> image(theGenerators.length());
  for (int i = G1.numberOfGenerators();i<theGenerators.length();i++){
    image[i] = Generator(i+1-G1.numberOfGenerators());
  }
  return Map((FPGroup)*this, G2, image);
}

//    ----------------    DirectProdut  ----------------------- //


DirectProduct::DirectProduct( const FPGroup& g1, const FPGroup& g2)
  : FreeProduct(g1,g2)
{
  int numberOfG1Gens = G1.numberOfGenerators();
  int numberOfG2Gens = G2.numberOfGenerators();
  
  
  for (int i = 0;i<numberOfG1Gens;i++)
    for (int j=numberOfG1Gens;j<numberOfG1Gens+numberOfG2Gens;j++){
      theRelators |=  Word(
			   inv( Generator(i + 1)) * 
			   inv(Generator(j + 1))  *
			   Generator(i + 1) *
			   Generator(j + 1)
			   );
      
    }
}
  
