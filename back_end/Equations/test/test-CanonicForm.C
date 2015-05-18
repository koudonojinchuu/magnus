#include "QEqnSolutions.h"

void computation()
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

		if( w.length() == 0 ) return;

		QEqnSolutionsInFreeGroup equation(F, w, numOfVar);

		int maxVerticesBlock = (numOfVar*numOfVar*numOfVar/10)*100;
		int maxLoopsBlock = (numOfVar*numOfVar*numOfVar/10)*500;
		QEqnSolutionsInFreeGroup::EquationInfo lastInfo=equation.getProcessInfo();
		cout << "*** Surface auto is " << equation.toSurfaceForm() << endl;
		cout << "*** Surface form is " << equation.surfaceForm() << endl;
		cout << "*** Minimal form is " << equation.toSurfaceForm().imageOf(w).freelyReduce() << endl;
		equation.startComputation();
		while( !equation.isComputationDone() ) {
			equation.continueComputation();
			bool firstLine = true;
			while( equation.haveASolution() ) {
				if( firstLine ) {
					cout << "*** New basic solutions founded: " << endl;
					firstLine = false;
				}
				Endomorphism solution = equation.getSolution();
				if( solution.imageOf(w).freelyReduce().length() == 0 )
					cout << " + ";
				else
					cout << " - ";
				cout << solution << endl;
				equation.nextSolution();
			}
			firstLine = true;
			while( equation.haveAStabGenerator() ) {
				Endomorphism generator = equation.getStabGenerator();
				if( firstLine ) {
					cout << "=== New RegStab generators is founded:" << endl;
					firstLine = false;
				}
				if( generator.imageOf(w).freelyReduce() == w )
					cout << " * ";
				else
					cout << " ! ";
				cout << generator << endl;
				equation.nextStabGenerator();
			}
			QEqnSolutionsInFreeGroup::EquationInfo newInfo=equation.getProcessInfo();
			if( newInfo.verticesPassed-oldInfo.verticesPassed > maxVerticesPortion ||
				 newInfo.loopsPassed-oldInfo.loopsPassed > maxLoopsPortion )
				equation.debugPrint(cerr);
		}

		cout << equation << endl << endl;

		int solutionNumber;
		cout << "If you wish to see `first' N solutions type a number, or 0 to continue: ";
		cin >> solutionNumber;

	} while( w.length() > 0 );

}


void standard()
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

		if( w.length() == 0 ) exit(0);

		QEqnSolutionsInFreeGroup equation(F, w, numOfVar);

		cout << "surface form is ";
		F.printWord(cout, equation.surfaceForm());
		Word WW = equation.equation();
		Word sf = equation.toSurfaceForm().imageOf(equation.equation()).cyclicallyReduce();
		
		cout << endl << "by hand is     ";
		F.printWord(cout, sf);
		if( sf == equation.surfaceForm() )
			cout << " :: + CORRECT + ";
		else
			cout << " :: - WRONG - ";
		cout << endl;

		cout << "surface auto is " << equation.toSurfaceForm() << endl;
		cout << endl;

//		cout << equation << endl << endl;
	} while( w.length() > 0 );
}

main()
{
//	standard();
	computation();
	return 1;
}
