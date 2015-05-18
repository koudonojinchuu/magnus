// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the FPGroup class.
//
// Principal Authors: Alexey Myasnikov
//
// Status: in progress
//
// Revision History:
//
//
// Special Notes:
//
//

#include "AbelianSGPresentation.h"
#include "File.h"

Word AbelianSGPresentationRep::fromSGPGensToSGGens(const Word& w) const
{ 
  int i,j,m,n;
  m = theSGPGens.height();
  VectorOf<Integer> u(m);
  for(i=0; i<m; i++) u[i] = w.exponentSum(Generator(i+1));
  n = theSGPGens.width();
  VectorOf<Integer> v(n);
  for(j=0;j<n;j++) for(v[j]=i=0;i<m;i++) v[j] += u[i]*theSGPGens[i][j];
  return AbelianWord(v).getWord();
}

bool AbelianSGPresentationRep::fromSGGensToSGPGens(const Word& w,Word& wInNew) const
{
  int i, j, m, n;
  m = theSGPGens.height();
  n = theSGPGens.width();
  VectorOf<Integer> u(n);
  for(i=0; i<n; i++) u[i] = w.exponentSum(Generator(i+1));
  VectorOf<Integer> v(n);
  for(j=0; j<n; j++) {
    for(v[j]=i=0; i<n; i++) v[j] += u[i]*theSGPGensInv[i][j];
    if(j<m) {
      if(v[j]%theInvariants[j]==0) v[j] /= theInvariants[j];
      else return false;
    }
    else if(v[j]!=0) return false;
  }
  v.shrink(m);
  wInNew = AbelianWord(v).getWord();
  return true;
}

void AbelianSGPresentationRep::makeFile( )
{
  File file;
  VectorOf<AbelianWord> cyclicGens = newToOldGens();
  int numofnewgens = cyclicGens.length();
  int rank = rankOfFreeAbelianFactor();
  char s[10];
  Word tmpWord;
  file.setColor(titleColor);
  file << "New generators of the subgroup expressed in terms of the given generators:" << endl;  
  file.setColor(mainColor);
  for( int i = 0; i < rank; ++i ) {
    file << "f" << i+1 << " = ";
    tmpWord =  fromSGPGensToSGGens(cyclicGens[i].getWord());
    theParent.getFPGroup().printWord(file,theParent.oldInAbelianForm(tmpWord).getWord() );
    file  << endl;
  }
  for( int i = rank; i < numofnewgens; ++i ) {
    file << "t" << i - rank + 1 << " = ";
    tmpWord =  fromSGPGensToSGGens(cyclicGens[i].getWord());
    theParent.getFPGroup().printWord(file,theParent.oldInAbelianForm(tmpWord).getWord() );
    file << endl;
  }
  file << endl;
  file.setColor(titleColor);
  file << "Given generators expressed in terms of the generators for the canonical decomposition of the subgroup:" << endl;  
  file.setColor(mainColor);
  for( int i = 0; i < theSGGens.length(); ++i ) {
    theParent.getFPGroup().printWord( file,theSGGens[i] );
    file << " = ";
    if (fromSGGensToSGPGens(theSGGens[i],tmpWord))
      printWordInNewGens(file,oldToNewGens(oldInAbelianForm(tmpWord)));   
    file << endl;
  } 
  file << end;
  theFileName = file.getFileName();
}

void AbelianSGPresentationRep::makeFileOfPDGens( )
{
  File file;
  char s[10];
  int rank = rankOfFreeAbelianFactor();
  VectorOf<Integer> newGens(numOfGensInTorsionPartOfPD()+rank);
  file.setColor(titleColor);
  file << "New generators of the subgroup expressed in terms of the given generators:" << endl;  
  file.setColor(mainColor);
  Word tmpWord;
  for( int i = 0; i < numOfGensInTorsionPartOfPD()+rank; ++i ) {
    if(i<rank)
      file << "f" << i+1 << " = ";
    else
      file << "p" << i+1-rank << " = ";
    newGens[i] = 1;  
    tmpWord =  fromSGPGensToSGGens(primeFormInOldGens( AbelianWord(newGens) ).getWord());
    theParent.getFPGroup().printWord(file,theParent.oldInAbelianForm(tmpWord).getWord() );
    newGens[i] = 0;
    file << endl;
  }
  file << endl;
  file.setColor(titleColor);
  file << "Given generators expressed in terms of the generators for the primary decomposition of the subgroup:" << endl;  
  file.setColor(mainColor);
  for( int i = 0; i < theSGGens.length(); ++i ) {
    theParent.getFPGroup().printWord( file,theSGGens[i] );
    file << " = ";
    if (fromSGGensToSGPGens(theSGGens[i],tmpWord))
         printInPrimaryForm(file,findEltPrimeForm(tmpWord));   
    file << endl;
  }    
  file << end;
  theFileNameOfPD = file.getFileName();
}
