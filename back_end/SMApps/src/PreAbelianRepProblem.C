#include "PreAbelianRepProblem.h"
#include "OutMessages.h"
#include "FEData.h"

void PreAbelianRepProblem::takeControl( )
{
  int i, j, k;
  for(i=0; i<m; i++) g[i] = Word(Generator(i+1));
  for(i=0; i<m; i++) x[i] = g[i];
  for(i=0; i<m; i++) z[i] = g[i];
  SetIterator<Word> I(G.getRelators()); 
  for(j=n-1; j>=0; j--,I.next()) y[j] = I.value();
  for(i=0; i<m; i++) for(j=0; j<n; j++)
    {
      a[i*n+j]=y[j].exponentSum(Generator(i+1));
    }
  f << "In the following, we use Nielsen transformation to obtain another set of generators <x_1,...,x_m> and another set of relators <R_1,...,R_n> such that" << endl;
  f << "    R_i = x_i^{d_i}*Q_i, for i = 1, ... , min(m,n)," << endl;  
  f << "    R_i = Q_i,           for i = min(m,n)+1,...,m," << endl;
  f << "where 0 < d_i | d_{i+1} and Q_i has zero exponent sum on each x_i." << endl;
  f << "The (i,j)-th elment of the matrix is the exponent sum of R_j on x_i" << endl; 
  show();
  for(i=0; i<m;)
    {
      if((k=findmin(i))==-1) break;
      j = k%n;
      k /= n;
      swap(i,j,'c');
      swap(i,k,'r');
      if(a[i*n+i]<0)
	{
	  for(j=i; j<n; j++) a[i*n+j] *= -1;
	  x[i] = x[i].inverse();
	  for(j=0; j<m; j++) z[j] = z[j].replaceGeneratorWithWord(Generator(i+1),g[i].inverse()); 
	}
      int b = 0;
      for(j=i+1; j<n; j++) b += plus(i,j,-a[i*n+j]/a[i*n+i],'c');
      for(j=i+1; j<m; j++) b += plus(i,j,-a[j*n+i]/a[i*n+i],'r');
      if(b==0) for(j=i+1; j<m; j++) for(k=i+1; k<n; k++) if(a[j*n+k]%a[i*n+i])
	{
	  plus(i, j, -1, 'r');
	  plus(i, k, a[j*n+k]/a[i*n+i], 'c');
	  b = 1; show();
	}
      if(b==0) i++;
    }
  VectorOf<Chars> name1 = G.namesOfGenerators();
  for(i=0; i<m; i++) name1[i] += "1"; 
  k = m<n?m:n;
  VectorOf<Word> w = y;
  for(j=0; j<k; j++) w[j] = pow(x[j], -a[j*n+j]) * w[j];
  LogMessage("New Generators: ").send();
  for(i=0; i<m; i++)
    {
      LogMessage msg;
      msg << name1[i];
      msg << " = ";
      G.printWord(msg, x[i].freelyReduce());
      msg.send();
    }
  for(i=0; i<m; i++)
    {
      LogMessage msg;
      msg << G.nameOfGenerator(i);
      msg << " = ";
      FPGroup(name1).printWord(msg, z[i].freelyReduce());
      msg.send();
    }
  LogMessage("New Relations: ").send();
  for(j=0; j<n; j++)
    {
      LogMessage msg;
      msg << "R" << j+1 << " = ";
      G.printWord(msg, y[j].freelyReduce());
      if(j<k)
	{
	  msg << " = " << name1[j] << "^" << a[j*n+j] << " * ";
	  FPGroup(name1).printWord(msg, w[j].replaceGenerators(z).freelyReduce());
	}
      msg.send();
    }
  LogMessage msg;
  msg << Link("Click here to see the intermediate steps: ", "PreAbelianRepProblem", f.getFileName());
  msg.send();
}

int PreAbelianRepProblem::findmin(int i)
{
  int j, k;
  k = i*n+i;
  for(j=k+1; j<m*n; j++) if(a[k]==0||a[j]&&abs(a[j])<abs(a[k])) k=j;
  return a[k]?k:-1;
}

int PreAbelianRepProblem::plus(int L1, int L2, int mult, char type) 
{
  if(mult==0||L1<0||L2<0) return 0;//unchanged
  if(type=='r')
    {
      if(L1>=m||L2>=m) return 0;
      x[L1] *= pow(x[L2], -mult);
      for(int i=0; i<m; i++) z[i] = z[i].replaceGeneratorWithWord(Generator(L1+1), g[L1]*pow(g[L2],mult));
      for(int i=0; i<n; i++) a[L2*n+i] += a[L1*n+i] * mult;
    }
  else if(type=='c')
    {
      if(L1>=n||L2>=n) return 0;
      y[L2] *= pow(y[L1], mult);
      for(int i=0; i<m; i++) a[i*n+L2] += a[i*n+L1] * mult;
    }
  show();
  return 1;//changed
}

void PreAbelianRepProblem::swap(int L1, int L2, char type)
{
  int i, j;
  if(L1==L2||L1<0||L2<0) return;
  if(type=='r')
    {
      if(L1>=m||L2>=m) return;
      Word w = x[L1];
      x[L1] = x[L2];
      x[L2] = w;
      VectorOf<Word> v = VectorOf<Word>(m);
      for(i=0; i<m; i++) v[i] = g[i];
      v[L1] = g[L2];
      v[L2] = g[L1];
      for(i=0; i<m; i++) z[i] = z[i].replaceGenerators(v);
      for(j=0; j<n; j++)
	{
	  i = a[L1*n+j];
	  a[L1*n+j] = a[L2*n+j];
	  a[L2*n+j] = i;
	}
    }
  else if(type=='c')
    {
      if(L1>=n||L2>=n) return;
      Word w = y[L1];
      y[L1] = y[L2];
      y[L2] = w;
      for(i=0; i<m; i++)
	{
	  j = a[i*n+L1];
	  a[i*n+L1] = a[i*n+L2];
	  a[i*n+L2] = j;
	}
    }
  show();
}

Word PreAbelianRepProblem::pow(const Word &w, int mult)
{
  Word result = Word();
  if(mult<0) 
    {
      for(int i=mult; i<0; i++) result *= w;
      return result.inverse();
    }
  else
    { 
      for(int i=0; i<mult; i++) result *= w;
      return result;
    }
}

void PreAbelianRepProblem::show(void)
{
  f << endl << "--- Step " << c++ << " ---";
  f << endl << "Generators: ";
  for(int i=0; i<m; i++) x[i] = x[i].freelyReduce();
  G.printVectorOfWords(f, x);
  f << endl << "Relations: ";
  for(int j=0; j<n; j++) y[j] = y[j].freelyReduce();
  G.printVectorOfWords(f, y);
  for(int i=0; i<m; i++)
    {
      f << endl;
      for(int j=0; j<n; j++) f << "    " << a[i*n+j];
    }
  f << endl;
}
