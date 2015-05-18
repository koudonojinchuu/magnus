#include "QEqnSolutions.h"
#include "SolutionsEnum.h"

main()
{

  cout << "*** Solves quadratic equation in free group ***\n";
  cout << "First generators of a free group are variables, others are constants.\n";
  FreeGroup F;
  cout << "\n\nEnter a free group (e.g. <x,y,z>) : ";
  Chars errMesg = cin >> F;
  if( errMesg.length() > 0 ) {
    cout << errMesg << endl;
    exit(0);
  }
  cout << "You typed: " << F << endl << endl;

  int numOfVar;
  cout << "Enter a number of variables = ";
  cin  >> numOfVar;

  Word w;
  do {
    cout << "\nType a quadratic equation as word (e.g. x^-1 a b x a^-1 b^-1)\n";
    cout << "or empty word (1) for quit: ";
    w = F.readWord(cin, errMesg);
    if( errMesg.length() > 0 ) {
      cout << errMesg << endl;
      exit(0);
    }
    cout <<"\nYou typed: ";
    F.printWord(cout, w);
    cout << endl << endl;

    if( w.length() == 0 ) return 1;

    QEqnSolutionsInFreeGroup equation(F, w, numOfVar);

    int maxVerticesPortion = (numOfVar*numOfVar*numOfVar/10)*100;
    int maxLoopsPortion = (numOfVar*numOfVar*numOfVar/10)*500;
    QEqnSolutionsInFreeGroup::EquationStatus oldStatus=equation.getProcessStatus();

    cout << "Surface auto is " << equation.toSurfaceForm() << endl;
    cout << "Surface form is " << equation.surfaceForm() << endl;
    cout << "Minimal form is " << equation.toSurfaceForm().imageOf(w).freelyReduce() << endl;

    equation.startComputation();

    char reply = 'y';
    cout << "Enable building solutions (y/n)? "; 
    cin >> reply;
    cout << reply;
    if( toupper(reply) == 'N' ) equation.disableBuildingSolutions();

    cout << "Enable building RegStab (y/n)? "; 
    cin >> reply;
    cout << reply;
    if( toupper(reply) == 'N' ) equation.disableBuildingRegStab();

    while( !equation.isComputationDone() ) {
      equation.continueComputation();
      bool firstLine = true;
      while( equation.haveNewSolution() ) {
	if( firstLine ) {
	  cout << "*** Basic solutions are found: " << endl;
	  firstLine = false;
	}
	Endomorphism solution = equation.getSolution();
	if( solution.imageOf(w).freelyReduce().length() == 0 )
	  cout << " + "; // mark right solution
	else
	  cout << " - "; // mark wrong solution
	cout << solution << endl;
	equation.nextSolution();
      }
      firstLine = true;
      while( equation.haveNewStabGenerator() ) {
	Endomorphism generator = equation.getStabGenerator();
	if( firstLine ) {
	  cout << "=== RegStab generators are found:" << endl;
	  firstLine = false;
	}
	if( generator.imageOf(w).freelyReduce() == w )
	  cout << " * "; // mark right generator
	else
	  cout << " ! "; // mark wrong generator
	cout << generator << endl;
	equation.nextStabGenerator();
      }

      QEqnSolutionsInFreeGroup::EquationStatus newStatus=equation.getProcessStatus();
      if(newStatus.verticesPassed-oldStatus.verticesPassed>maxVerticesPortion||
	 newStatus.loopsPassed-oldStatus.loopsPassed > maxLoopsPortion ) {
	equation.debugPrint(cerr);
	cerr << endl;
	oldStatus = newStatus;
      }
    }
    cerr << "summary: ";
    equation.debugPrint(cerr);
    cerr << endl;

    cout << equation << endl << endl;

    cout << "Do you want to enumerate solutions (y/n) ? ";
    cin >> reply; 
    cout << "(" << reply << ")" << endl;
    if( toupper(reply) == 'Y' ) {
      int maxSolution;
      cout << "Enter a number of solutions to see: ";
      cin >> maxSolution; 
      cout << "(" << maxSolution << ")" << endl;
      GeneratorOfRandomSolutions solutions(F, w, numOfVar);

      Automorphism prefixPath = equation.prefixMap();
      Automorphism prefixPathInv = prefixPath.inverse();
      
      // Make a full set of basic solutions of the equation;
      VectorPtrOf<Endomorphism> basicSolutions = equation.basicSolutions();
      for( int i = basicSolutions.length()-1; i >= 0; --i ) {
	Endomorphism phi = prefixPath | basicSolutions[i];
	phi.reduceGenImages();
	basicSolutions[i] = phi;
      }
      solutions.setBasicSolutions( basicSolutions );

      // Make a full set of generators of regular stabilzer of the equation;
      VectorPtrOf<Automorphism> regStabGenerators = equation.regStabGenerators();
      for( int i = regStabGenerators.length()-1; i >= 0; --i ) {
	Automorphism phi = prefixPath | regStabGenerators[i] | prefixPathInv;
	phi.reduceGenImages();
	regStabGenerators[i] = phi;
      }
      solutions.setRegStabGenerators( regStabGenerators );

      for(int i = 0; i<maxSolution && solutions.hasSolutions() 
	    && solutions.generateSolution(); i++) {
	Endomorphism solution = solutions.getSolution() ;
	cout << "i = " << i << ", ";
	if( solution.imageOf(w).cyclicallyReduce().length() > 0 ) 
	  cout << " <wrong> ";
	cout << solution << endl;
      }
    }
  } while( w.length() > 0 );

  return 1;
}
