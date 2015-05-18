#ifndef _SMITH_NORMAL_FORM1_H_
#define _SMITH_NORMAL_FORM1_H_

#include <Integer.h>
#include "Vector.h"
#include "File.h"
#include "DArray.h"

class SmithNormalForm1 {
public:
  SmithNormalForm1() {}
  SmithNormalForm1(DArray<Integer>);
  DArray<Integer> LeftFactor() const { return P; }
  DArray<Integer> LeftFactorInv() const { return P1; }
  DArray<Integer> RightFactor() const { return Q; }
  DArray<Integer> RightFactorInv() const { return Q1; }
  VectorOf<Integer> invariants() const { return inv; } 
  int rank() const { return inv.length(); }
  Chars getFileName() const { return f.getFileName(); }
private:
  File f;
  DArray<Integer> A, P, P1, Q, Q1;
  VectorOf<Integer> inv; 
  int findmin(int);
  int plus(int,int,Integer,char);
  void swap(int,int,char);
  void show(Chars,DArray<Integer>);
};

#endif
