// Copyright (C) 2000 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declaration of KernelOfHom, ImageOfHom, NewPresentation 
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

#ifndef _PRESENTATION_PROBLEMS_H_
#define _PRESENTATION_PROBLEMS_H_

#include "FPGroup.h"
#include "FPGroupRep.h"
#include "KBMachine.h"
#include "RKBPackage.h"
#include "File.h"
#include "Subgroup.h"

class KernelOfHom 
{
 public:

  KernelOfHom( const FPGroup& d, const FPGroup& r, const KBMachine& k, const VectorOf<Word> v ) :
    domain( d ),
    range( r ),
    kb( k ),
    images( v )
    { }
  
  KernelOfHom( ) :
    domain(),
    range(),
    kb(),
    images()
    { }

  FPGroup getKernelPresentation( );
  // computes a presentation for the kernel of homomorphism
  
 private: 

  FPGroup domain;
  FPGroup range;
  KBMachine kb;
  VectorOf<Word> images;
};

class ImageOfHom 
{
 public:

  ImageOfHom( const FPGroup& d, const FPGroup& r, const VectorOf<Word> v ) :
    domain( d ),
    range( r ),
    images( v )
    { }

  ImageOfHom() :
    domain(),
    range(),
    images()
    { }
  
  FPGroup getImageOfHomo( VectorOf<Word>& v );
  // compute the image of homomorphism and returns new generators in v 

 private: 

  FPGroup domain;
  FPGroup range;
  VectorOf<Word> images;
};

class NewPresentation
{

 public:

  NewPresentation( const FPGroup& g, const KBMachine& k, const VectorOf<Word> v ) :
    G( g ),
    kb( k ),
    newBasis( v )
    { }

  NewPresentation( ) :
    G( ),
    kb( ),
    newBasis( )
    { }

  FPGroup getNewPresentation( );
  // returns a presentation for the group on given generators  
  
 private:

  FPGroup G;
  KBMachine kb;
  VectorOf<Word> newBasis;
};

#endif



