// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes EquationProblem,
//           EquationSolver, QuickEquationSolver, EquationRandomSolutions
//
// Principal Author: Eugene Paderin, Dmitry Pechkin
//
// Status: in progress
//
// Revision History:
//

#include "QuadEquationSolver.h"
#include "OutMessages.h"
#include "viewStructure.h"
#include "QEqnSolutions.h"
#include "SolutionsEnum.h"


// ---------------------- EquationBasicSolutions ------------------------- //

void EquationSolverARCer::setArguments( SMEquation& equation, 
  const Chars& aBasicFileName, const Chars& aStabFileName,
  bool buildBasicSolutions,   bool buildStabilizer )
{
  F = equation.getFreeGroup();
  word = equation.getWord();
  numberOfVariables = equation.numberOfVariables();
  workOnBasicSolutions = buildBasicSolutions;
  workOnStabilizer = buildStabilizer;
  basicFileName = aBasicFileName;
  stabFileName  = aStabFileName;

}

void EquationSolverARCer::runComputation( )
{
  File basicFile( basicFileName );
  File stabFile ( stabFileName  );

  QEqnSolutionsInFreeGroup solver( F, word, numberOfVariables );
  
  solver.startComputation();
  
  if( !workOnBasicSolutions )
    solver.disableBuildingSolutions();
  
  if( !workOnStabilizer )
   solver.disableBuildingRegStab();
  
  do { 
    
    solver.continueComputation();
    
    while ( solver.haveNewSolution() ) {
      basicFile << solver.getSolution() << endl << ready << endl;
      solver.nextSolution();
    }
    
    while( solver.haveNewStabGenerator() ) {
      stabFile << solver.getStabGenerator() << endl << ready << endl;
      solver.nextStabGenerator();
    }
    
  } while ( ! solver.isComputationDone() );

  basicFile << end;
  stabFile << end;

  // Save summary results of computation to return the parent through IPC.
  numOfBasicSolutions    = solver.basicSolutions().length();
  numOfRegStabGenerators = solver.regStabGenerators().length();
}

void EquationSolverARCer::writeResults( ostream& ostr ) 
{
  ostr < numOfBasicSolutions < ' ' < numOfRegStabGenerators; 
}

void EquationSolverARCer::readResults( istream& istr ) 
{
  istr > numOfBasicSolutions > numOfRegStabGenerators;
}

int EquationSolverARCer::numberOfBasicSolutions( ) const 
{ 
  return numOfBasicSolutions; 
}

int EquationSolverARCer::numberOfRegStabGenerators( ) const 
{ 
  return numOfRegStabGenerators; 
}

// -------------------- EquationSolver ----------------------------------- //


EquationSolver::EquationSolver( EquationProblem& equationProblem )
  : equation( equationProblem.getEquation() ), arcer( *this ),
    workOnBasicSolutions( true ), workOnStabilizer( true ),
    basicFileName( equationProblem.getBasicSolutionsFileName() ), 
    stabFileName( equationProblem.getRegStabGeneratorsFileName() ),
    watchBasicSolutions( equationProblem.getBasicSolutionsFileName(), 2000 ),
    watchRegStabGenerators( equationProblem.getRegStabGeneratorsFileName(), 2000 )
{ 
  if( equation.eic.haveAllBasicSolutions()  )
    workOnBasicSolutions = false;
  if( equation.eic.haveAllRegStabGenerators() )
    workOnStabilizer = false;

  arcer.setArguments( equation, basicFileName, stabFileName,
		      workOnBasicSolutions, workOnStabilizer );
  adminStart();
}


EquationSolver::EquationSolver( QuickEquationProblem& equationProblem )
  : equation( equationProblem.getEquation() ), arcer( *this ),
    workOnBasicSolutions( true ), workOnStabilizer( false ),
    basicFileName( equationProblem.getBasicSolutionsFileName() ), 
    stabFileName( File().getFileName(/*dummy file name*/) ), 
    watchBasicSolutions( equationProblem.getBasicSolutionsFileName(), 2000 ),
    watchRegStabGenerators( stabFileName, 2000 )
{ 
  if( equation.eic.haveAllBasicSolutions()  )
    workOnBasicSolutions = false;

  arcer.setArguments( equation, basicFileName, stabFileName,
		      workOnBasicSolutions, workOnStabilizer );

  adminStart();
}


void EquationSolver::terminate( ) 
{
  if( foundBasicSolutions.length() > 0 )
    equation.eic.addBasicSolutions( foundBasicSolutions );
  if( foundRegStabGenerators.length() > 0 )
    equation.eic.addRegStabGenerators( foundRegStabGenerators );
}

void EquationSolver::updateBasicSolutions( ) 
{
  Endomorphism bsol( equation.getFreeGroup() );
  Chars fname = File().getFileName();
  fstream f( fname, ios::in | ios::out ); 

  while( watchBasicSolutions.changed() ) {
    Chars str = watchBasicSolutions.getline();

    if( str.length() == 0 ) continue;

    if( str[0] != '\n' ) {
      str = Chars("{ ") + str + Chars(" }");
      f.seekp( 0, ios::beg );
      f << str << endl << flush;
      f.seekg( 0, ios::beg );
      Chars errMesg = f >> bsol;
      if( errMesg.length() > 0 ) {
	error("Bad endomorphism on input in "
	      "EquationProblem::updateBasicSolutions()");
      }
      foundBasicSolutions.append( bsol );
    }   
  }

  if( foundBasicSolutions.length() > 10 ) {
    equation.eic.addBasicSolutions( foundBasicSolutions );
    foundBasicSolutions = ListOf<Endomorphism>();
  }

  f.close();
  unlink( fname );
}


void EquationSolver::updateRegStabGenerators( ) 
{
  Automorphism stabgen( equation.getFreeGroup() );
  Chars fname = File().getFileName();
  fstream f( fname, ios::in | ios::out ); 

  while( watchRegStabGenerators.changed() ) {
    Chars str = watchRegStabGenerators.getline();

    if( str[0] != '\n' ) {
      str = Chars("{ ") + str + Chars(" } ");
      f.seekp( 0, ios::beg );
      f << str << endl << flush;
      f.seekg( 0, ios::beg );
      Chars errMesg = f >> stabgen;
      if( errMesg.length() > 0 ) {
	error("Bad automorphism on input in "
	      "EquationProblem::updateRegStabGenerators()");
      }
      foundRegStabGenerators.append( stabgen );
    }   
  }
  
  if( foundRegStabGenerators.length() > 50 ) {
    equation.eic.addRegStabGenerators( foundRegStabGenerators );
    foundRegStabGenerators = ListOf<Automorphism>();
  }

  f.close();
  unlink( fname );
}

void EquationSolver::takeControl( )
{
  if( freeARCs() > 0 ) {

    bool arcerDone = arcer.takeControl();

    if( arcerDone ) {
      watchBasicSolutions.setInterval( 0 );
      watchRegStabGenerators.setInterval( 0 ); 

      #ifdef SYSV
        sleep(1); // @db: don't have usleep under Solaris
      #else    
	usleep(20);
      #endif   
    }
    
    if( workOnBasicSolutions ) 
      updateBasicSolutions();  
    
    if( workOnStabilizer ) 
      updateRegStabGenerators();

    if( arcerDone ) {
      LogMessage msg( *this, equation );
      msg << "Solving the equation is finished. Found " 
	  << arcer.numberOfBasicSolutions() << " basic solutions and " 
	  << arcer.numberOfRegStabGenerators() << " RegStab generators.";
      msg.send();

      if( workOnBasicSolutions ) 
	equation.eic.putHaveAllBasicSolutions( true );
      
      if( workOnStabilizer )
	equation.eic.putHaveAllRegStabGenerators( true );
	
      //LogMessage msg2( *this, theEquation );
      //msg2 << "Rank of the equation is " << theEquation.solver().rank();
      //msg2.send();
      
      adminTerminate();
      return;
    }

    usedOneARC();
  }

}

// -------------------- Random Solutions ---------------------------------- //

void EquationRandomSolutionARCer::setArguments( SMEquation& eq, 
						const Chars& aRandomFileName,
						const Chars& aBasicFileName,
						const Chars& aStabFileName )
{
  equation       = &eq;
  randomFileName = aRandomFileName;
  basicFileName  = aBasicFileName;
  stabFileName   = aStabFileName;
}

void EquationRandomSolutionARCer::runComputation( )
{ 
  // debugging trick.
  //bool f = true;
  //while( f ) 
  //;

  VectorPtrOf<Endomorphism> basicSolutions( equation->eic.getBasicSolutions().length(), true );
  {
    int i = 0;
    for( ListIterator< ListOf<Endomorphism> > I = equation->eic.getBasicSolutions();
	 !I.done(); I.next() ) {
      basicSolutions[i++] = I.value();
    }
  }

  VectorPtrOf<Automorphism> regStabGenerators( equation->eic.getRegStabGenerators().length(), true );
  { 
    int i = 0; 
    for( ListIterator< ListOf<Automorphism> > I = equation->eic.getRegStabGenerators();
	 !I.done(); I.next() ) {
      regStabGenerators[i++] = I.value();
    }
  }

  bool isWatchBasicSolutions = !equation->eic.haveAllBasicSolutions();
  bool isWatchRegStabGenerators = !equation->eic.haveAllRegStabGenerators();
  
  // Setup watchers for new basic solutions and regStab generators.
  const int interval = 2000; // Check input files for changes every 2 sec.
  LogFileWatcher watchBasicSolutions( basicFileName, interval );
  LogFileWatcher watchRegStabGenerators( stabFileName, interval );
  Endomorphism bsol( equation->getFreeGroup() );
  Automorphism stabGen( equation->getFreeGroup() );

  // Setup generator of random solutions.
  GeneratorOfRandomSolutions generator( equation->getFreeGroup(), equation->getWord(),
					equation->numberOfVariables() );
  generator.setBasicSolutions( basicSolutions );
  generator.setRegStabGenerators( regStabGenerators );

  File randomFile( randomFileName );
  fstream basicSolutionsFile( File().getFileName(), ios::in | ios::out );
  fstream regStabGensFile( File().getFileName(), ios::in | ios::out );

  if( !basicSolutionsFile || !regStabGensFile )
    error("Can't create temporary files in EquationRandomSolutionARCer::runComputation()");

  do { 

    // Update basic solutions.

    if( isWatchBasicSolutions ) {
      bool newBasicSolutions = false;
      while( watchBasicSolutions.changed() ) {
	Chars str = watchBasicSolutions.getline();
	if( str[0] != '\n' ) {
	  basicSolutionsFile.seekp( 0, ios::beg );
	  basicSolutionsFile << "{ " << str << " } " << endl;
	  basicSolutionsFile.seekg( 0, ios::beg );
	  Chars errMesg = basicSolutionsFile >> bsol;
	  if( errMesg.length() > 0 )
	    error("Bad endomorphism on input in EquationRandomSolutionsARCer::runComputation");
	  basicSolutions.append( bsol );
	  newBasicSolutions = true;
	}
      }   
      if( newBasicSolutions )
	generator.setBasicSolutions( basicSolutions );
    }

    // Update RegStab generators.

    if( isWatchRegStabGenerators ) {
      bool newRegStabGenerators = false;
      while( watchRegStabGenerators.changed() ) {
	Chars str = watchRegStabGenerators.getline();
	if( str[0] != '\n' ) {
	  regStabGensFile.seekp( 0, ios::beg );
	  regStabGensFile << "{ " << str << " } " << ends;
	  regStabGensFile.seekg( 0, ios::beg );
	  Chars errMesg = regStabGensFile >> stabGen;
	  if( errMesg.length() > 0 )
	    error("Bad automorphism on input "
		  "in EquationRandomSolutionsARCer::runComputation");
	  regStabGenerators.append( stabGen );
	  newRegStabGenerators = true;
	}   
      }   
      if( newRegStabGenerators )
	generator.setRegStabGenerators( regStabGenerators );
    }

    // Generate random solutions.

    for( int i = 0; i < 100; ++i ) {
      if( generator.hasSolutions() && generator.generateSolution() ) {
	Endomorphism sol = generator.getSolution();
	randomFile << sol << endl << ready << endl;
      }
    }

  } while ( 1 );

  randomFile << end;
}


EquationRandomSolutions::EquationRandomSolutions( EquationProblem& equationProblem )
  : arcer( *this )
{
  arcer.setArguments( equationProblem.getEquation(), 
		      equationProblem.getRandomSolutionsFileName(),
		      equationProblem.getBasicSolutionsFileName(),
		      equationProblem.getRegStabGeneratorsFileName() );
  adminStart();
}

void EquationRandomSolutions::takeControl( )
{
  if( freeARCs() > 0 ) {

    if( arcer.takeControl() ) {
	adminTerminate();
	return;
    }
    usedOneARC();
  }
}



// -------------------- EquationProblem --------------------------- //

EquationProblem::EquationProblem( SMEquation& anEquation )
  : Supervisor( true ), equation( anEquation ), linksSent( false ),
    basicSolutionsFileName( File().getFileName() ),
    regStabGeneratorsFileName( File().getFileName() ),
    randomSolutionsFileName( File().getFileName() ),
    basicSubordinate( *this ), randomSubordinate( *this )
{
}

void EquationProblem::takeControl( )
{
  if( !linksSent ) {
    
    if( !equation.eic.haveAllBasicSolutions() ) {
      LogMessage msg( *this, equation );
      msg << Link( "Click here to view basic solutions found so far.",
		   "EquationBasicSolutions", basicSolutionsFileName, true
		   );
      msg.send();
    }
    
    if( !equation.eic.haveAllRegStabGenerators() ) { 
      LogMessage msg( *this, equation );
      msg << Link( "Click here to view RegStab generators found so far.",
		   "EquationRegStabGenerators", regStabGeneratorsFileName, true
		   );
      msg.send();
    }
    
    LogMessage msg( *this, equation );
    msg << Link( "Click here to view random solutions generated so far.",
		 "EquationRandomSolutions", randomSolutionsFileName, true
		 );
    msg.send();

    linksSent = true;
  }

}



void EquationProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Solving a quadratic equation"),
		  Text("Find basic solutions and stabilizer of equation ")
		  + Name( equation ),
		  helpID("EquationProblem", equation.getParent()),
		  Text("Solving ") + Name( equation )
		  );

  pv.startItemGroup();

  if( !equation.eic.isSolved() )
    pv.add(Text("To find basic solutions"), basicSubordinate.arcSlotID(), 100);
  
  pv.add(Text("To generate random solutions"), randomSubordinate.arcSlotID(), 100);
  
  pv.done();
}



// -------------------- QuickEquationProblem --------------------------- //

QuickEquationProblem::QuickEquationProblem( SMEquation& anEquation )
  : Supervisor( true ), equation( anEquation ), linkSent( false ),
    basicSolutionsFileName( File().getFileName() ),
    basicSubordinate( *this )
{
  if( equation.eic.haveAllBasicSolutions() ) {
    resultIsFast();
  }
}

void QuickEquationProblem::takeControl( )
{
  if( equation.eic.haveAllBasicSolutions() ) {
    if( !fastResult() )
      adminTerminate();
  }
  else if( !linkSent ) {    
    LogMessage msg( *this, equation );
    msg << Link( "Click here to view basic solutions found so far.",
		 "EquationBasicSolutions", basicSolutionsFileName, true
		 );
    msg.send();
    linkSent = true;
  }
}



void QuickEquationProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Quick solving a quadratic equation"),
		  Text("Find basic solutions of equation ")
		  + Name( equation ),
		  helpID("QuickEquationProblem", equation.getParent()),
		  Text("Quick solving ") + Name( equation )
		  );

  pv.startItemGroup();

  pv.add(Text("To use for this problem"), basicSubordinate.arcSlotID(), 100);

  pv.done();
}



void QEquationInFreeBasicSolutions::takeControl( )
{
  LogMessage msg( equation );

  File file;
  file.setColor( titleColor );
  file << "The basic solutions of the equation in free group." << endl << endl;
  if( equation.eic.haveAllBasicSolutions() )
    file << "(complete list)";
  else
    file << "(incomplete list)";
  file << endl;
  file.setColor( mainColor );

  ListIterator< ListOf<Endomorphism> > basicSolutions = equation.eic.getBasicSolutions();
  for( int i = 0 ; !basicSolutions.done(); basicSolutions.next() ) {
    file << ++i << ". " << basicSolutions.value() << endl;
  }

  file.setColor( titleColor );
  file << "(end of the list)" << endl;
  file << end;
  
  msg << Link( Chars("Click here to see the basic solutions of ")
	       + Text( Name(equation) ),
	       "KnownBasicSolutions",
	       file.getFileName() 
	       )
      << ".";

  msg.send();
}


void QEquationInFreeRegStabGenerators::takeControl( )
{
  LogMessage msg( equation );

  File file;
  file.setColor( titleColor );
  file << "The basic solutions of the equation in free group." << endl << endl;
  if( equation.eic.haveAllRegStabGenerators() )
    file << "(complete list)";
  else
    file << "(incomplete list)";
  file << endl;
  file.setColor( mainColor );

  ListIterator< ListOf<Automorphism> > regStabGenerators = equation.eic.getRegStabGenerators();
  for( int i = 0; !regStabGenerators.done(); regStabGenerators.next() ) {
    file << ++i << ". " << regStabGenerators.value() << endl;
  }

  file.setColor( titleColor );
  file << "(end of the list)" << endl;
  file << end;
  
  msg << Link( Chars("Click here to see the generators of RegStab of ")
	       + Text( Name(equation) ),
	       "KnownRegStabGenerators",
	       file.getFileName() 
	       )
      << ".";
  
  msg.send();
}

