// Copyright (C) 2000 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declaration of the PresentationsOfSubgroup 
//           classes.
//
// Principal Authors: Denis Serbin
//
// Status: Useable
//
// Revision History:
//
// Special Notes:
//
//

#include "Subgroup.h"
#include "FPGroup.h"
#include "File.h"
#include "FreeGroup.h"
#include "CosetEnumerator.h"
#include "SGofFreeGroup.h"
#include "PresentationsOfSubgroup.h"

// This function is used by PresentationsOfSubgroup::makePresentation.
SetOf<Word> SchreierRepresentatives( SGofFreeGroup& H1 )
{
  // constructing of Schreier system:
  // for every word of Nielsen basis and it's inverse we take all initial
  // subwords of length <= 1/2 of word's length and put them to the set T1;
  // if the word is of even length so it can be presented as 'a b^-1'
  // where a and b are of the same length ,and  one of them say 'a' (or both) 
  // is an isolated part i.e. any word from Nielsen basis or its inverse can't   
  // have 'a' as an initial part; so for every word of even length we remove 
  // such isolated part from T1 and we get the set T - Schreier transversal 
  
  FreeGroup F = H1.parentGroup();
  VectorOf<Word> b = H1.nielsenBasis();
  
  SetOf<Word> Sch(F.makeIdentity()); // Schreier transversal
  
  SetOf<Word> isolated(F.makeIdentity()); // the set of isolated parts
  
  Word w,invw,u,invu;
  int length = b.length();
  
  // collecting isolated parts
  for( int i = 0 ; i <  length ; i++ )
    {
      w = b[i];
      int wlen = w.length();
      int half = wlen / 2;
      
      if( !(wlen % 2) ) 
	{
	  // if word's length is even we take it's initial segment of
	  // half length and compare it with initial segments of all
	  // words in Nielsen basis and sum number of its occurrencies
	  
	  Word a = w.initialSegment(half);
	  
	  int count = 0;
	  for( int j = 0 ; j < b.length() ; j++ )
	    {
	      u = b[j];
	      invu = F.inverseOf(w);
	      
	      if(u.length() >= half && (u.initialSegment(half) == a ||
		 invu.initialSegment(half) == a))
		count++;
	    }
	  // if number of occurrencies is less then 2 then this part is
	  // isolated; if not we take as an isolated part the terminal 
	  // segment ^-1.
	  if( count < 2 )
	    isolated.adjoinElement(a);
	  else
	    {
	      invw = F.inverseOf(w);
	      Word b = invw.initialSegment(half);
	     	      
	      isolated.adjoinElement(b);
	    }
	}
    }
  
  // constructing of T
  for( int i = 0 ; i < length ; i++ )
    {
      w = b[i];
      invw = F.inverseOf(w);
      
      int wlen = w.length();
      int half = wlen / 2;
      
      for( int j = 0 ; j < half ; j++ )
        {
          u = w.initialSegment(j);
	  Sch.adjoinElement(u);
          
	  u = invw.initialSegment(j);
	  Sch.adjoinElement(u);
        }
      
      if( wlen % 2 )
        {
          u = w.initialSegment(half);
	  Sch.adjoinElement(u);
          
	  u = invw.initialSegment(half);
	  Sch.adjoinElement(u);
        }
      else
        {
          u = w.initialSegment(half);
      	  
	  if( !isolated.contains(u) )
	    Sch.adjoinElement(u);
	  else
	    {
	      u = invw.initialSegment(half);
	      Sch.adjoinElement(u);
	    }
        }
    }
 
  return Sch;
}

// This function is used by PresentationsOfSubgroup::makePresentation.
SGofFreeGroup constructPreimage(const FreeGroup& F,const VectorOf<Word>& sGens,const SetOf<Word>& groupRels)
{
  SetIterator<Word> J(groupRels);
  VectorOf<Word> basis(sGens);
  SetOf<Word> s; 
  Word w;
  int currlen = 0,oldlen,count = 0;
  
  SGofFreeGroup H(F,basis);

  // finding of some preimage of finite index for given subgroup
  while( true )
    {
      w = F.getN_thElement(count);
      oldlen = currlen;
      currlen = w.length();
      if( !currlen || ( oldlen != currlen ) ) 
	// if the rank of F is 1 then fo getting a full preimage we just 
	// conjugate group relators with representatives
	if( F.rank() == 1 )
	  {
	    SetIterator<Word> I(groupRels);
	    while(!I.done())
	      {
		w = I.value();
		if( !H.contains(w) ) 
		  {
		    w = w.freelyReduce();
		    basis.append(w);
		    H = SGofFreeGroup(F,basis);
		  }
		I.next();
	      }
	    break;
	  }
	else
	  {
	    // if F's rank > 1 we check current approximation for having
	    // a finite index; if it has we stop,if not we continue
	    // to approximate 
	    if( !( (H.rank() - 1) % (F.rank() - 1) ) ) 
	      {
		s = SchreierRepresentatives(H);
		if( (F.rank() - 1) * s.cardinality() == (H.rank() - 1) )
		  {
		    J.reset();
		    SetIterator<Word> I(s);
		    while(!J.done())
		      {
			I.reset();
			while(!I.done())
			  {
			    w = J.value();
			    Word u = I.value();
			    u = F.inverseOf(u);
			    
			    w = F.conjugateBy(w,u);
			    if( !H.contains(w) ) 
			      {
				w = u.freelyReduce();
				basis.append(w);
				H = SGofFreeGroup(F,basis);
			      }
			    I.next();
			  }
			J.next();
		      }
		    break;
		  }     
	      }
	  }
      // to finish constructing of full preimage we add group relators
      // conjugated by representatives
      J.reset();
      while( !J.done() )
	{
	  Word u = F.conjugateBy(J.value(),w);
          if( !H.contains(u) ) 
	    {
	      u = u.freelyReduce();
	      basis.append(u);
	      H = SGofFreeGroup(F,basis);
	    }
	  J.next();
	}
      count++;
    }
  
  return H;
}

// This function is used by PresentationsOfSubgroup::makePresentation.
SetOf<Word> findRels(SGofFreeGroup& H,const SetOf<Word>& groupRels,
                     const VectorOf<Word>& sch,File &tmpOutput)
{
  FreeGroup F = H.parentGroup();
  SetOf<Word> subgroupRels;
  FreeGroup Q(H.rank());
  
  SetIterator<Word> J(groupRels);

  tmpOutput << "Subgroup relators:" << endl;
  // according to RS-process we conjugate all group relators with Schreier
  // representatives and rewrite them in subgroup generators.
  Word u,w;
  int count = 1 , count1 = 1;
  while(!J.done())
    {
      for( int i = 0 ; i < sch.length() ; i++ ) 
        {
          w = J.value();
	  u = sch[i];
          u = F.inverseOf(u);
	  
	  w = F.conjugateBy(w,u);
          w = w.freelyReduce();
	  u = H.inNielsenWords(w);
	  
	  tmpOutput << "u" << count1 << " = t(s" << i + 1 << ",r" << count << ") = ";
	  
	  Q.printWord(tmpOutput,u);
	  
	  tmpOutput << " = ";
	  
	  F.printWord(tmpOutput,w);
	  tmpOutput << endl;
	  
	  subgroupRels.adjoinElement(u);
	  count1++;
	}
      J.next();
      count++;
    }
  
  tmpOutput << endl;
  return subgroupRels;
}

// This function is used by PresentationsOfSubgroup::makePresentation.
void transform(SetOf<Word>& subgroupRels,int& rank,int *gens,File& tmpOutput)
{
  // this function performs simple transformations of presentation
  // that we obtain yet; for example if we have in relators some letter
  // we exclude this letter from all relators and decrease rank of 
  // subgroup and so on.
  
  FreeGroup F(rank);
  int count = 0;
             
  // at first we cyclically reduce obtained subgroup relators
  SetIterator<Word> I(subgroupRels);
  Word w,u;
  while( !I.done() )
    {
      w = I.value();
      subgroupRels.removeElement(w);
      w = w.cyclicallyReduce();

      if( w.length() > 0 )
	subgroupRels.adjoinElement(w);
      
      I.next();
    }
  
  // then we exclude relators which have some letter that occurs once 
  // and rewrite others
  int *vect = new int[rank];

  I.reset();
  while( !I.done() )
    {
      w = I.value();
      
      Generator g,invg;
      int num,flag = false;
      Word e,inve;
      
      for( int t = 0 ; t < rank ; t++ )
	{
	  vect[t] = 0;
	  vect[t] = w.numberOfOccurrences( Generator(t+1) );
	}
	  
      for( int t = 0 ; t < w.length() ; t++ )
	{
	  g = w[t];
	  int a = g.hash();
	  num = ( a > 0 ) ? a : -a;  
	  
	  if( vect[num - 1] == 1 )
	    {
	      flag = true;
	      Word tmp1 = w.initialSegment(t);
	      Word tmp2 = w.terminalSegment(w.length() - t - 1);
	      
	      e = F.inverseOf(tmp1) * F.inverseOf(tmp2);
	      e = e.freelyReduce();
	      inve = F.inverseOf(e);
	      invg = Generator( -g.hash() );
	      
	      gens[num - 1] = 0;
	      count++;
	      
	      break;
	    }
	}
      
      // rewriting of other relators
      SetIterator<Word> J(subgroupRels);
      while(!J.done() && flag)
	{
	  w = J.value();
	  subgroupRels.removeElement(w);
	  
	  Word w1;
	  for( int t = 0 ; t < w.length() ; t++ )
	    {
	      if( w[t] != g && w[t] != invg )
		w1 *= w[t];
	      if( w[t] == g )
		w1 *= e;
	      if( w[t] == invg )
		w1 *= inve;
	    }
	  
	  w = w1;
	  w = w.freelyReduce();
	  
	  if( w.length() > 0 )
	    subgroupRels.adjoinElement(w);
	  
	  J.next();
	}
  
      if( flag )
	{
	  VectorOf<Word> v;
	  
	  I = SetIterator<Word>(subgroupRels);
	  while(!I.done())
	    {
	      v.append(I.value());
	      I.next();
	    }
	  subgroupRels.removeAllElements();
	  
	  v = F.nielsenBasis(v);
	  
	  for( int t = 0 ; t < v.length() ; t++ )
	    subgroupRels.adjoinElement(v[t]);
	  
	  I = SetIterator<Word>(subgroupRels);
	  continue;
	}
      
      I.next();
    }
  
  // because of deleting some letters from subgroup generators we must
  // substitute unsuitable letters in subgroup relators with new ones
  int next = 1;
  for( int i = rank ; i > rank - count ; i-- )
    {
      if( gens[i-1] == 0 )
	continue;
      
      for( ; gens[next-1] ; next++ );
      
      gens[next-1] = i;
      gens[i-1] = 0;
      Generator x(i),invx(-i);
      Generator y(next),invy(-next);
      SetIterator<Word> J(subgroupRels);
      while( !J.done() )
	{
	  w = J.value();
	  if( w.numberOfOccurrences(x) )
	    { 
	      subgroupRels.removeElement(w);
	      w = w.replaceGeneratorWithWord(x,y); 
	      w = w.replaceGeneratorWithWord(invx,invy); 
	      subgroupRels.adjoinElement(w);
	    }
	  J.next();
	}
      next++;
    }
  
  // finally we check wether some relators are conjugates
  // if so we exclude one relator in such pair
  I = SetIterator<Word>(subgroupRels);
  while( !I.done() )
    {
      w = I.value();
      subgroupRels.removeElement(w);
      w = w.cyclicallyReduce();
      subgroupRels.adjoinElement(w);
      
      int flag = false;
      SetIterator<Word> J(subgroupRels);
      while(!J.done())
	{
	  u = J.value();
	  Word v;
	  
	  if( ( u != w && F.conjugacyProblem(u,w,v) == YES ) 
	      || u == F.inverseOf(w) )
	   {
	     subgroupRels.removeElement(u);
	     flag = true;
	   }
	  J.next();
	}
      
      if( flag )
	{
	  I = SetIterator<Word>(subgroupRels);
	  continue;
	}
      
      I.next();
    }
  
  // output in file
  
  rank -= count;
  
  delete [] vect;
}

Word rewrite( const PermutationRepresentation& pr, const VectorOf<Word>& b,
	      const FreeGroup& F, const Word& w )
{
  Word h;
  Word rw;
   
  for( int i = 0 ; i < w.length() ; i++ )
    {
      Word h = pr.representativeOf(w.initialSegment(i));
      Word h1 = pr.representativeOf(w.initialSegment(i+1));
      Word s = h * w[i] * F.inverseOf(h1);
      s = s.freelyReduce();
      
      int k = b.indexOf(s);
      
      Word g;
      
      if( k < 0 )
	{
	  s = F.inverseOf(s);
	  k = b.indexOf(s);
	  
	  if( k >= 0 )
	    g = Generator(-(k+1));
	  
	}
      else
	g = Generator(k+1);
      
      rw *= g;
    }
  return rw;
}

PresentationsOfSubgroup::PresentationsOfSubgroup( const Subgroup& s )
  : H( s ),
    theGenerators( s.generators() ),
    G( H.parentGroup() )
{ }
  
FPGroup PresentationsOfSubgroup::makePresentation( File &tmpOutput )
{
  VectorOf<Chars> groupGens = G.namesOfGenerators();
  FreeGroup F( groupGens );
  SetOf<Word> groupRels = G.getRelators();
 
  // constructing of the preimage
  SGofFreeGroup preimageOfH = constructPreimage( F, theGenerators, groupRels );
  
  // getting the Schreier transversal
  SetOf<Word> s = SchreierRepresentatives( preimageOfH ); 
  
  tmpOutput << "Index for the preimage: " << s.cardinality() << endl;
  tmpOutput << "Schreier transversal for the preimage:" << endl;
  
  VectorOf<Word> sch;
  
  SetIterator<Word> I(s);
  while(!I.done())
    {
      sch.append( I.value() );
      I.next();
    }
  
  for( int t = 0 ; t < sch.length() ; t++ )
    {
      tmpOutput << "s" << t+1 << " = ";
      F.printWord( tmpOutput,sch[t] );
      tmpOutput << endl;
    }      
  
  tmpOutput << endl << "Schreier generators of the subgroup:" << endl;
  VectorOf<Word> basis = preimageOfH.nielsenBasis();
  for( int t = 0 ; t < basis.length() ; t++ )
    {
      int len = basis[t].length();
      Word q = basis[t].initialSegment( len / 2 );
      
      if( sch.indexOf(q) >= 0 )
	{
	  tmpOutput << "x" << t+1 << " = " << "j(" << "s" << sch.indexOf(q) + 1 << ",";
	  F.printWord(tmpOutput,basis[t].subword( len/2 + 1 , len/2 + 1 ));
	  tmpOutput << ") = ";
	}
      else
	{
	  Word q1 = basis[t].initialSegment( len / 2 - 1 );
	  tmpOutput << "x" << t+1 << " = " << "j(" << "s" << sch.indexOf(q1) + 1 << ",";
	  F.printWord(tmpOutput,basis[t].subword( len/2 , len/2 + 1 ));
	  tmpOutput << ") = ";
	}
      F.printWord(tmpOutput,basis[t]);
      tmpOutput << endl;
    }      
  
  tmpOutput << endl;
  
  // finding subgroup rels
  SetOf<Word> subgroupRels = findRels( preimageOfH, groupRels, 
				       sch, tmpOutput );
  
  int rank = preimageOfH.rank();
  
  int *gens = new int[rank];
  for( int k = 1 ; k < rank + 1 ; k++ )
    gens[k-1] = k;
  
  // transformations
  transform( subgroupRels, rank, gens, tmpOutput );
 
  tmpOutput << "Number of generators of the subgroup after Tietze transformations: " << rank << endl;
  tmpOutput << "New generators of subgroup expressed in terms of given ones:" << endl;
  
  FreeGroup Q( rank );
  VectorOf<Word> nb = preimageOfH.nielsenBasis();
  for( int t = 1 ; t < rank + 1 ; t++ )
    {
      Generator g(t);
      Q.printWord( tmpOutput, Word(g) );
      tmpOutput << " = ";
      Word p = nb[ gens[ t-1 ] - 1 ];
      F.printWord( tmpOutput, p );
      tmpOutput << endl;
    }
  
  SetIterator<Word> K( subgroupRels );
  tmpOutput << endl << "Subgroup relators after Tietze transformations and renumbering: " <<  endl;
  int newcount = 1;
  while( !K.done() )
    {
      tmpOutput << "u" << newcount << " = ";
      Q.printWord( tmpOutput, K.value() );
      tmpOutput << endl;
      
      K.next();
      newcount++;
    }
  
  FPGroup N = FPGroup( rank, subgroupRels );

  tmpOutput << endl << "Subgroup presentation:" << endl;
  tmpOutput << N << endl;
  
  tmpOutput << end;
  delete [] gens;

  return FPGroup( rank, subgroupRels );
}  


FPGroup PresentationsOfSubgroup::makePresentationTC( const PermutationRepresentation &pr, File &tmpOutput )
{
  VectorOf<Chars> groupGens = G.namesOfGenerators();
  FreeGroup F( groupGens );
  SetOf<Word> groupRels = G.getRelators();
  
  VectorOf<Word> sch = pr.getRepresentatives();
  SetOf<Word> discarded;
  
  tmpOutput << "Index for the preimage: " << sch.length() << endl;
  tmpOutput << "Schreier transversal for the preimage:" << endl;
  
  for( int t = 0 ; t < sch.length() ; t++ )
    {
      tmpOutput << "s" << t+1 << " = ";
      F.printWord( tmpOutput, sch[t] );
      tmpOutput << endl;
    }      
  
  tmpOutput << endl << "Schreier generators of the subgroup:" << endl;
  VectorOf<Word> basis;
  Word w;
  // constructing of RS-basis for subgroup
  int count = 1;
  for( int i = 0 ; i < F.rank() ; i++ ) 
    {
      Word x = Generator(i+1);
      
      for( int r = 0 ; r < sch.length() ; r++ )
	{
	  Word u = sch[r];
	  Word tmp = pr.representativeOf( u * x );
	  w = u * x * F.inverseOf( tmp );
	  w = w.freelyReduce();
	  
	  if( w.length() ) 
	    {
	      basis.append(w);
	      tmpOutput << "x" << count << " = j(s" << r + 1 << ",";
	      F.printWord( tmpOutput, x );
	      tmpOutput << ") = ";
	      F.printWord( tmpOutput, w );
	      tmpOutput << endl;
	      count++;
	    }
	}
    }
  
  tmpOutput << endl;
  
  SGofFreeGroup H1( F, basis );
  
  SetOf<Word> subgroupRels;
  FreeGroup Q( basis.length() );
  
  SetIterator<Word> J( groupRels );

  // constructing of subgroup relators
  tmpOutput << "Subgroup relators:" << endl;
  
  count = 1;
  int count1 = 1;
  while(!J.done())
    {
      for( int i = 0 ; i < sch.length() ; i++ ) 
        {
          w = J.value();
	  Word u = sch[i];
          u = F.inverseOf(u);
	  
	  w = F.conjugateBy(w,u);
          w = w.freelyReduce();
	  
	  tmpOutput << "u" << count1 << " = t(s" << i + 1 << ",r" << count << ") = ";
	  
	  u = rewrite(pr,basis,F,w);
	  Q.printWord(tmpOutput,u); 
	  tmpOutput << " = ";
	  
	  F.printWord(tmpOutput,w);
	  tmpOutput << endl;
	  
	  subgroupRels.adjoinElement(u);
	  count1++;
	}
      
      count++;
      J.next();
    }
  
  tmpOutput << endl;
  
  int rank = basis.length();
  
  int *gens = new int[rank];
  for( int k = 1 ; k < rank + 1 ; k++ )
    gens[k-1] = k;
  
  // transformations
  transform( subgroupRels, rank, gens, tmpOutput );
  
  tmpOutput << "Number of generators of the subgroup after Tietze transformations: " << rank << endl;
  tmpOutput << "New generators of subgroup expressed in terms of given ones:" << endl;
  
  for( int t = 1 ; t < rank + 1 ; t++ )
    {
      Generator g( t );
      Q.printWord( tmpOutput, Word( g ) );
      tmpOutput << " = ";
      Word p = basis[ gens[ t-1 ] - 1 ];
      F.printWord( tmpOutput , p );
      tmpOutput << endl;
    }

  SetIterator<Word> I( subgroupRels );
  tmpOutput << endl << "Subgroup relators after Tietze transformations and renumbering: " <<  endl;
  int newcount = 1;
  while( !I.done() )
    {
      tmpOutput << "u" << newcount << " = ";
      Q.printWord( tmpOutput, I.value() );
      tmpOutput << endl;
      
      I.next();
      newcount++;
    }
  
  FPGroup N = FPGroup( rank, subgroupRels );

  tmpOutput << endl << "Subgroup presentation:" << endl;
  tmpOutput << N << endl;
  
  tmpOutput << end;
  delete [] gens;

  return FPGroup( rank, subgroupRels );
}  


FPGroup PresentationsOfSubgroup::makePurePresentationTC(const PermutationRepresentation &pr)
{
  VectorOf<Chars> groupGens = G.namesOfGenerators();
  FreeGroup F( groupGens );
  SetOf<Word> groupRels = G.getRelators();
  
  VectorOf<Word> sch = pr.getRepresentatives();
  
  VectorOf<Word> basis;
  Word w;
  // constructing of RS-basis for subgroup
  int count = 1;
  for( int i = 0 ; i < F.rank() ; i++ ) 
    {
      Word x = Generator(i+1);
      
      for( int r = 0 ; r < sch.length() ; r++ )
	{
	  Word u = sch[r];
	  Word tmp = pr.representativeOf(u * x);
	  w = u * x * F.inverseOf(tmp);
	  w = w.freelyReduce();
	  
	  if( w.length() ) 
	    {
	      basis.append(w);
	      count++;
	    }
	}
    }
  
  SGofFreeGroup H1( F, basis );
  
  SetOf<Word> subgroupRels;
  
  SetIterator<Word> J( groupRels );

  // constructing of subgroup relators
  
  count = 1;
  int count1 = 1;
  while(!J.done())
    {
      for( int i = 0 ; i < sch.length() ; i++ ) 
        {
          w = J.value();
	  Word u = sch[i];
          u = F.inverseOf(u);
	  
	  w = F.conjugateBy(w,u);
          w = w.freelyReduce();
	  
	  u = rewrite(pr,basis,F,w);
	  
	  subgroupRels.adjoinElement(u);
	  count1++;
	}
      
      count++;
      J.next();
    }
  
  /*  cout << "aaaaaa" << endl;
  
      int rank = basis.length();
      
      int *gens = new int[rank];
      for( int k = 1 ; k < rank + 1 ; k++ )
      gens[k-1] = k;
      
      File f;
      transform(subgroupRels,rank,gens,f);
      
      cout << "sss" << endl;
      
      delete [] gens; */

  return FPGroup( basis.length() , subgroupRels );
}  


Word PresentationsOfSubgroup::rewriteWord( const PermutationRepresentation& pr , const Word& w )
{
  VectorOf<Chars> groupGens = G.namesOfGenerators();
  FreeGroup F( groupGens );

  VectorOf<Word> sch = pr.getRepresentatives();
  
  VectorOf<Word> basis;
  Word w1;
  
  int count = 1;
  for( int i = 0 ; i < F.rank() ; i++ ) 
    {
      Word x = Generator(i+1);
      
      for( int r = 0 ; r < sch.length() ; r++ )
	{
	  Word u = sch[r];
	  Word tmp = pr.representativeOf(u * x);
	  w1 = u * x * F.inverseOf(tmp);
	  w1 = w1.freelyReduce();
	  
	  if( w1.length() ) 
	    {
	      basis.append(w1);
	      count++;
	    }
	}
    }
  
  SGofFreeGroup H1( F, basis );
  
  if( H1.contains(w) )
    return rewrite( pr, basis, F, w );
  else
    return Word();
}




