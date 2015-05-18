#include "SmithNormalForm1.h"

SmithNormalForm1::SmithNormalForm1(DArray<Integer> M)
{
  int i,j,k;
  A = M;
  P = DArray<Integer>(A.height(),A.height());
  P1 = DArray<Integer>(A.height(),A.height());
  Q = DArray<Integer>(A.width(),A.width());
  Q1 = DArray<Integer>(A.width(),A.width());
  for(i=0; i<A.height(); i++) P[i][i] = P1[i][i] = 1;
  for(i=0; i<A.width(); i++) Q[i][i] = Q1[i][i] = 1;
  show("A = ", A); 
  show("P = ", P); show("P^-1 = ", P1); 
  show("Q = ", Q); show("Q^-1 = ", Q1);
  for(i=0; i<A.height()&&i<A.width(); ) {
    if((k=findmin(i))==-1) break;
    j = k%A.width();
    k /= A.width();
    swap(i,j,'c');
    swap(i,k,'r');
    if(A[i][i]<0) {
      for(j=i; j<A.width(); j++) A[i][j] *= -1; 
      for(j=0; j<A.height(); j++) { P[j][i] *= -1; P1[i][j] *= -1; }
    }
    int ok = 0;
    for(j=i+1; j<A.width(); j++) ok += plus(i,j,-A[i][j]/A[i][i],'c');
    for(j=i+1; j<A.height(); j++) ok += plus(i,j,-A[j][i]/A[i][i],'r');
    if(ok==0) for(j=i+1; j<A.height(); j++) for(k=i+1; k<A.width(); k++) 
      if(A[j][k]%A[i][i]!=0) {
	plus(i, j, -1, 'r');
	plus(i, k, A[j][k]/A[i][i], 'c');
	ok = 1;
	show("A = ", A); 
	show("P = ", P); show("P^-1 = ", P1); 
	show("Q = ", Q); show("Q^-1 = ", Q1);
      }
    if(ok==0) { inv.append(A[i][i]); i++; }
  }
  show("The original matrix is : \n", M);
  show("The left transformation matrix is : \n", P);
  show("The right transformation matrix is : \n", Q);
  show("The Smith normal form is : \n", A);
}

int SmithNormalForm1::findmin(int i)
{
  int m,n,p,q;
  m = n = i; 
  for(p=i; p<A.height(); p++) for(q=i; q<A.width(); q++)
    if(	A[p][q]!=0&&(A[m][n]==0||abs(A[p][q])<abs(A[m][n])) ) { m = p; n = q; }
  return A[m][n]==0?-1:m*A.width()+n;
}

int SmithNormalForm1::plus(int L1, int L2, Integer mult, char type) 
{
  int i;
  if(mult==0||L1<0||L2<0) return 0; 
  if(type=='r') {
    if(L1>=A.height()||L2>=A.height()) return 0;
    for(i=0; i<A.width(); i++) A[L2][i] += A[L1][i]*mult;
    for(i=0; i<A.height(); i++) 
      { P[i][L1] -= P[i][L2]*mult;  P1[L2][i] += P1[L1][i]*mult; }
    show("A = ", A); show("P = ", P); show("P^-1 = ", P1); 
  }
  else if(type=='c') {
    if(L1>=A.width()||L2>=A.width()) return 0;
    for(i=0; i<A.height(); i++) A[i][L2] += A[i][L1]*mult;
      for(i=0; i<A.width(); i++)
	{ Q[L1][i] -= Q[L2][i]*mult; Q1[i][L2] += Q1[i][L1]*mult; }
      show("A = ", A); show("Q = ", Q); show("Q^-1 = ", Q1); 
    }
  return 1;
}

void SmithNormalForm1::swap(int L1, int L2, char type)
{
  int i;
  Integer s;
  if(L1==L2||L1<0||L2<0) return;
  if(type=='r') { 
    if(L1>=A.height()||L2>=A.height()) return;
    for(i=0; i<A.width(); i++) {
      s = A[L1][i]; A[L1][i] = A[L2][i]; A[L2][i] = s; 
    }
    for(i=0; i<A.height(); i++) {
      s = P[i][L1]; P[i][L1] = P[i][L2]; P[i][L2] = s; 
      s = P1[L1][i]; P1[L1][i] = P1[L2][i]; P1[L2][i] = s;
    }
    show("A = ", A); show("P = ", P); show("P^-1 = ", P1); 
  }
  else if(type=='c') {
    if(L1>=A.width()||L2>=A.width()) return;
    for(i=0; i<A.height(); i++) { 
      s = A[i][L1]; A[i][L1] = A[i][L2]; A[i][L2] = s; 
    }
    for(i=0; i<A.width(); i++) {
      s = Q[L1][i]; Q[L1][i] = Q[L2][i]; Q[L2][i] = s;
      s = Q1[i][L1]; Q1[i][L1] = Q1[i][L2]; Q1[i][L2] = s; 
    }
    show("A = ", A); show("Q = ", Q); show("Q^-1 = ", Q1);
  }
}

void SmithNormalForm1::show(Chars msg, DArray<Integer> M)
{
  f << endl << msg;
  for(int i=0; i<M.height(); i++) {
    for(int j=0; j<M.width(); j++) f << "\t" << M[i][j];
    f << endl;
  }
}
