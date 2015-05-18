// Copyright (C) 2000 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the KernelOfHom, ImageOfHom, NewPresentation 
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

#include "FPGroup.h"
#include "KBMachine.h"
#include "conversions.h"
#include "FreeGroup.h"
#include "PresentationProblems.h"
#include "PresentationsOfSubgroup.h"

FPGroup KernelOfHom::getKernelPresentation()
{
  NewPresentation N( range , kb , images ); 
  FPGroup G = N.getNewPresentation( );
  
  // constructing of Schreier transversal
  
  VectorOf<Word> tmp = makeVectorOf( G.getRelators() );
  FreeGroup T( G.numberOfGenerators() );
  
  VectorOf<Word> tmp1 = T.nielsenBasis( tmp );
  
  CosetEnumerator ce( G , tmp1 );
  
  int ind = ce.enumerate();
  
  PermutationRepresentation pr = ce.permutationRepresentation();
  VectorOf<Word> sch = pr.getRepresentatives();
  
  // constructing of basis for kernel

  VectorOf<Word> basis;
  Word w;
  
  int count = 1;
  
  int i;
  
  for( i = 0 ; i < G.numberOfGenerators() ; i++ ) 
    {
      Word x = Generator(i+1);
      
      for( int r = 0 ; r < sch.length() ; r++ )
	{
	  Word u = sch[r];
	  Word tmp = pr.representativeOf(u * x);
	  w = u * x * G.inverseOf(tmp);
	  w = w.freelyReduce();
	  
	  if( w.length() ) 
	    {
	      basis.append(w);
	      count++;
	    }
	}
    }
  
  File f;
  PresentationsOfSubgroup PS( Subgroup( domain , basis ) );
  FPGroup res = PS.makePresentation( f );
  
  int n = res.numberOfGenerators();
  SetOf<Word> rels = res.getRelators();
  
  return FPGroup( n , rels );
}  

  
FPGroup ImageOfHom::getImageOfHomo( VectorOf<Word>& newhom )
{
  Subgroup s( range , images ); 
  
  CosetEnumerator forS( range , images );
  
  int ind = forS.enumerate();
  
  if( ind == 1 )
    {
      newhom = images;
      return range;
    }
  
  PermutationRepresentation prforS = forS.permutationRepresentation();
  
  PresentationsOfSubgroup PS( s );
  FPGroup G = PS.makePurePresentationTC( prforS );
  
  VectorOf<Word> nv;

  int i;
  for( i = 0 ; i < images.length() ; i++ )
    {
      Word w = PS.rewriteWord( prforS , images[i] );
      nv.append( w );
    }

  newhom = nv;
  
  int n = G.numberOfGenerators();
  SetOf<Word> rels = G.getRelators();
  
  return FPGroup( n , rels );
}


FPGroup NewPresentation::getNewPresentation()
{
  FreeGroup F( newBasis.length() );
  
  VectorOf<Word> x( G.numberOfGenerators() );
  
  int i;
  
  for( i = 0 ; i < G.numberOfGenerators() ; i++ )
    {
      int count = 1;
      bool flag = false;
      
      while( !flag )
	{
	  Word cur = F.getN_thElement( count );
	  Word xcur;
	  
	  for( int k = 0 ; k < cur.length() ; k++ )
	    {
	      Generator g( cur[k] );
	      if( g.hash() > 0 )
		xcur *= newBasis[ g.hash() - 1 ];
	      else
		xcur *= newBasis[ -g.hash() - 1 ].inverse();
	    }
	  
	  Word tmp = xcur * inv( Generator( i + 1 ) );
	  tmp = tmp.freelyReduce();
	  
	  kb.rewrite( tmp );
	  
	  if( tmp.length() == 0 )
	    {
	      x[i] = cur;
	      flag = true;
	    }
	  
	  count++;
	}
    }
  
  VectorOf<Chars> newNames( newBasis.length() );
  
  for( i = 0 ; i < newBasis.length() ; i++ )
    newNames[i] = Chars("y") + Chars( i + 1 );
  
  SetOf<Word> newRels;
  
  SetIterator<Word> I( G.getRelators() );
  while( !I.done() )
    {
      Word w = I.value();
      Word nw;
      for( int j = 0 ; j < w.length() ; j++ )
	{
	  Generator g( w[j] );
	  if( g.hash() > 0 )
	    nw *= x[ g.hash() - 1 ];
	  else
	    nw *= Word( x[ -g.hash() - 1 ] ).inverse();
	}
      
      nw = nw.freelyReduce();
      
      if( nw.length() && !newRels.contains( nw.inverse() ) )
	newRels.adjoinElement( nw );
      
      I.next();
    }

  int j1;
  
  for( int i1 = 0 ; i1 < newBasis.length() ; i1++ )
    {
      Word w = newBasis[i1];
      Word nw;
      
      for( j1 = 0 ; j1 < w.length() ; j1++ )
	{
	  Generator g( w[j1] );
	  
	  if( g.hash() > 0 )
	    nw *= x[ g.hash() - 1 ];
	  else
	    nw *= Word( x[ -g.hash() - 1 ] ).inverse();
	}
      
      nw = nw * inv( Generator( i1 + 1 ) );
      nw = nw.freelyReduce();
      
      if( nw.length() && !newRels.contains( nw.inverse() ) )
	newRels.adjoinElement( nw );
    }

  return FPGroup( newNames , newRels );
}





