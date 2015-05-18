// Contents: Declaration of class TurnerProperSubgroupEnumerator
//
// Principal Author: Alexander Ushakov
//
// Status: in progress
//
// Revision History:
//

#ifndef _TURNER_PROPER_SUBGROUP_ENUMERATOR_H_
#define _TURNER_PROPER_SUBGROUP_ENUMERATOR_H_


//#include "Supervisor.h"
//#include "SMSetOfWords.h"
//#include "SMWord.h"
#include "FreeGroup.h"
#include "File.h"
#include "SGofFreeGroup.h"


// ------------------------  ProperSubgroupEnumerator ------------------------------ //


class ProperSubgroupEnumerator
{
public:
  ProperSubgroupEnumerator(const FreeGroup& F, const VectorOf<Word>& words);
  ProperSubgroupEnumerator(const FreeGroup& F, const Word& word);
 ~ProperSubgroupEnumerator();

  ProperSubgroupEnumerator(const ProperSubgroupEnumerator& PSE);
  int operator == (const ProperSubgroupEnumerator& PSE);
  // hidden

  bool getNextProperSubgroup(SGofFreeGroup& sg);

  Chars getFileName() { 
        return file.getFileName(); 
  }

private:
  const VectorOf<Word> theWords;
  const FreeGroup theGroup;

  VectorOf<Word> genOfGroup;
  // generators of free group

  const int rank;
  // rank of the FreeGroup

  bool stepTo();
  void stepBack();
  bool setRefs();
  void buildSG( );
  Word getWord(int x, int y);

  File file;
  // debug file

  SGofFreeGroup *subgroup;
  // current subgroup
  VectorOf<Word> genOfSG;
  // generators of current subgroup
  int qWord;
  // generator quantity


  int nVert;
  // num of current vertice
  int nSet;
  // num of current set

  int *partition;
  int **_partition;
  // partition 

  int qSet;
  // set quantity 
  int qVert;
  // vertice quantity

  int *setToVert;
  int *vertToWord;
  int *vertToPos;

  struct LC {
    int qSet;
  }* lCells;

  enum { UNKNOWN=-1 };
};


#endif

