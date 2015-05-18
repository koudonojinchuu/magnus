// Copyright (C) 2000 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declaration of the FPNewPresentationARCer, 
//           FPNewPresentationProblem, FPImagePresentationARCer, 
//           FPImagePresentationCM, FPKernelPresentationARCer,
//           FPKernelPresentationCM, KBSupervisorCM,
//           FPKernelPresentationProblem
//           classes.
//
// Principal Authors: Denis Serbin
//
// Status: In progress
//
// Revision History:
//
// Special Notes:
//
//

#ifndef _KernelPresentationProblem_H_
#define _KernelPresentationProblem_H_

#include "Supervisor.h"
#include "KBModule.h"
#include "FEData.h"
#include "Word.h"
#include "FPGroup.h"
#include "SMVectorOfWords.h"
#include "Chars.h"
#include "PresentationProblems.h"
#include "SMHomomorphism.h"


//---------------------------FPNewPresentationARCer--------------------------//

class FPNewPresentationARCer : public ARCer 
{
public:
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  FPNewPresentationARCer( ComputationManager& boss ) : 
    ARCer( boss ),
    dntg( false )
    { } 

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( const KBMachine& kb , 
		     const FPGroup& g , 
		     const VectorOf<Word>& y );
  
  FPGroup getRetValue() { return retValue; }
  
  bool doesntGenerate() { return dntg; }
  
  // Inherited from ARCer:
  // bool takeControl( );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC interaction members:                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void runComputation( );
 
  void writeResults( ostream& );

  void readResults( istream& );

  
private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  NewPresentation N;
  FPGroup G;
  VectorOf<Word> newGens;
  bool dntg;
  
  FPGroup retValue;
};

// -----------------------FPNewPresentationProblem-------------------------- //


class FPNewPresentationProblem : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  FPNewPresentationProblem( class SMVectorOfWords& );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Front End Interfacing:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void viewStructure(ostream& ostr) const;  // overrides SMObject

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Control:                                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void takeControl( );         // overrides ComputationManager

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // State Transitions:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void start( ) { }              // overrides ComputationManager

  void terminate( ) { }          // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMVectorOfWords& theVector;
  
  bool init;
  
  FPNewPresentationARCer arcer;

  MirrorSubordinate kbSupervisor;
};



//-------------------------FPImagePresentationARCer-------------------------//

class FPImagePresentationARCer : public ARCer 
{
public:
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  FPImagePresentationARCer( ComputationManager& boss ) : 
    ARCer( boss ) 
    { } 
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  void setArguments( const FPGroup& preimage, const FPGroup& image, 
		     const VectorOf<Word>& y );
  
  FPGroup getRetValue() 
    { return retValue; }
  
  VectorOf<Word> getVectorOfImages()
    { return newImages; }
  
  // Inherited from ARCer:
  // bool takeControl( );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC interaction members:                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void runComputation( );
  
  void writeResults( ostream& );
  
  void readResults( istream& );

  
private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  ImageOfHom Im;
  
  VectorOf<Word> newImages;
  FPGroup retValue;
};

//---------------------------FPImagePresentationCM--------------------------//

class FPImagePresentationCM : public ComputationManager
{
public:
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  FPImagePresentationCM( class FPKernelPresentationProblem& );
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  VectorOf<Word> getVectorOfImages()
    { return im; }
  
  FPGroup getImage()
    { return G; }
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Control:                                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  void takeControl( );         // overrides ComputationManager

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // State Transitions:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void start( ) { }              // overrides ComputationManager
  
  void terminate( ) { }          // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMHomomorphism2& theMap;
  FPGroup G;
  VectorOf<Word> im;

  FPImagePresentationARCer arcer;
};


//-------------------------FPKernelPresentationARCer-------------------------//

class FPKernelPresentationARCer : public ARCer 
{
public:
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  FPKernelPresentationARCer( ComputationManager& boss ) 
    : ARCer( boss ) 
    { } 

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( const KBMachine& kb , const FPGroup& preimage , 
		     const FPGroup& image , const VectorOf<Word>& y );
  
  FPGroup getRetValue() { return retValue; }

  // Inherited from ARCer:
  // bool takeControl( );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC interaction members:                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void runComputation( );
 
  void writeResults( ostream& );

  void readResults( istream& );

  
private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  KernelOfHom K;

  FPGroup retValue;
};

//-------------------------FPKernelPresentationCM---------------------------//

class FPKernelPresentationCM : public ComputationManager
{
public:
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  FPKernelPresentationCM( class FPKernelPresentationProblem& );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  void setFlag( bool f )
    { flag = f; }
  
  FPGroup getResult()
    { return result; }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Control:                                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  void takeControl( );         // overrides ComputationManager

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // State Transitions:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void start( ) { }              // overrides ComputationManager
  
  void terminate( ) { }          // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  FPKernelPresentationProblem& KPP;
  bool init;
  bool flag;
  FPGroup result;
  
  FPKernelPresentationARCer arcer;
};

//----------------------------KBSupervisorCM---------------------------------//

class KBSupervisorCM : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  KBSupervisorCM( class FPKernelPresentationProblem& );
    
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setFlag( bool f )
    { flag = f; }
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Front End Interfacing:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void viewStructure(ostream& ostr) const { }  // overrides SMObject

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Control:                                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void takeControl( );         // overrides ComputationManager

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // State Transitions:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void start( ) { }              // overrides ComputationManager

  void terminate( ) { }          // overrides ComputationManager

  
private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  FPKernelPresentationProblem& KPP;
  bool init;
  bool flag;
  
  KBSupervisorARCer arcer;
};


// -----------------------FPKernelPresentationProblem----------------------- //

class FPKernelPresentationProblem : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  FPKernelPresentationProblem( class SMHomomorphism2& );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  SMHomomorphism2& getInitialHomo()
    { return theMap; }
  
  SMHomomorphism2& getHomo()
    { return *theMap2; }
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Front End Interfacing:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void viewStructure(ostream& ostr) const;  // overrides SMObject

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Control:                                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void takeControl( );         // overrides ComputationManager

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // State Transitions:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void start( ) { }              // overrides ComputationManager

  void terminate( ) { }          // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMHomomorphism2& theMap;
  SMHomomorphism2* theMap2;
  bool foundImage;
  bool foundKB;
  
  Subordinate<FPKernelPresentationProblem,FPKernelPresentationCM> kernel;
  Subordinate<FPKernelPresentationProblem,FPImagePresentationCM> image;
  Subordinate<FPKernelPresentationProblem,KBSupervisorCM> kbs;
  
};

#endif
