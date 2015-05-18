#include "REnumerator.h"
#include "SMEnumerator.h"
#include "OutMessages.h"
#include "SMFPGroup.h"
#include "viewStructure.h"
#include "RandomNumbers.h"
#include "conversions.h"
#include "SMListIterator.h"


// ------------------- NCEnumerateTotalARCer --------------------- //


void NCEnumerateTotalARCer::enumerate( )
{

  int numOfRelators = theGroup.getRelators().cardinality();
  VectorOf<Word> relators = makeVectorOf(theGroup.getRelators());
  
  while(counter < numberOfAll) {
    currentWord = currentWord.nextInShortLex(numOfRelators);
    Word newRelator = currentWord.replaceGenerators(relators).freelyReduce();
    if (newRelator.length() > 0) {
      counter++;
      theData << EnumWriteWord( newRelator, theGroup) << "\n\n";
    }

    if (submitSignalRecieved()){
      return;
    }

  }
}

// ------------------- NCEnumerateRandomARCer --------------------- //


void NCEnumerateRandomARCer::enumerate( )
{
  

  while(counter < numberOfAll) {

    if (submitSignalRecieved()){
      return;
    }
    
    // Randomly select the number fo products of r ^ w, where
    // r - relator and w a word from the group
    int numOfProducts = 0;
    if ( typeIsUniform ) {
      numOfProducts = ur.rand(1,numberOfFactors);
    } else {
      while (numOfProducts <=0 || numOfProducts > numberOfFactors)
	numOfProducts = nr.rand(numberOfFactors/2,numberOfFactors/2);
    }

    Word newRelator;
    for (int j = 0;j<numOfProducts;j++)
      newRelator = (newRelator*getRelConj()).freelyReduce();
    
    if (newRelator.length() > 0) {
      counter++;
      theData << EnumWriteWord( newRelator, theGroup) << "\n\n";
    }
  }
}

Word NCEnumerateRandomARCer::getRelConj()
{

  // Number of the generators and their inverses
  int numOfGenerators = theGroup.numberOfGenerators()*2;

  int numOfRelators = theGroup.getRelators().cardinality();
  VectorOf<Word> relators = makeVectorOf(theGroup.getRelators());
  
  // Randomly select the length of a conjugator
  int conjLength = 0;
  if ( typeIsUniform ) {
    conjLength = ur.rand(1,maxConjLength);
  } else {
    while (conjLength <=0 || conjLength > maxConjLength)
      conjLength = nr.rand(maxConjLength/2,maxConjLength/2);
  }
  

  // Make a conjugator
  Word newConj;
  for (int j=0;j<conjLength;j++){
    int g=0;
    if ( typeIsUniform ) {
      g = ur.rand(1,numOfGenerators);
    } else {
      while (g <=0 || g > numOfGenerators)
	g = nr.rand(numOfGenerators/2,numOfGenerators/2);
    }
   
    // get inverse if it was choosen
    if ( g > numOfGenerators/2 ) {
      g = g - numOfGenerators/2;
    } else {
      g *= -1;
    }
    Generator newG(g);
    newConj *= newG;
  }
  
  // Choose a relator
  int relatorNum = -1;
  if ( typeIsUniform ) {
    relatorNum = ur.rand(0,numOfRelators-1);
  } else {
    while (relatorNum <0 || relatorNum > numOfRelators-1)
      relatorNum = nr.rand((numOfRelators-1)/2,(numOfRelators-1)/2);
  } 
  
  // new relator is equal to relator ^ newConj.
  Word newRelator = newConj * relators[relatorNum] * newConj.inverse();
  newRelator = newRelator.freelyReduce();
  return newRelator;
}

// ---------------------- NCRelatorEnumerator --------------------- //

NCRelatorEnumerator::NCRelatorEnumerator(SMFPGroup& b)
  : EnumeratorProblem< Word >( b ),
    theGroup(b),
    arcer( NULL )
{
  
}


void NCRelatorEnumerator::takeControl( )
{

  if (freeARCs() > 0){

      if (arcer->takeControl()){

	adminSuspend();
	return;	
      }
      else
	usedOneARC();
  }
}

void NCRelatorEnumerator::start( ) {              
  //    Chars selArcer = getParameters().getValue("RBG1");
  int totalNum = getParameters().getValue("P1");
  Chars fileName;
  LogMessage msg1(*this,theGroup);
  
  // if ( selArcer == Chars("RB1") ){
  int conjL;
  int factors;
  arcer = new NCEnumerateRandomARCer( *this, theData,theGroup.getFPGroup() ); 
  fileName = theData.getDataFileName();
  
  Chars randomType = getParameters().getValue("RandomRBG");
  if ( randomType == Chars("rRB1") ){
    
    ((NCEnumerateRandomARCer* )arcer)->setTypeNormal();
    conjL = getParameters().getValue("P3");
    factors = getParameters().getValue("nFact");
    
    msg1 << Link(Chars( "Click here to see the relators  of ")+
		 Text( Name(theGroup) )+
		 Chars(", enumerated using normal distribution"),
		 "REnumerator",
		 fileName,true );
    
  } else {
    
    ((NCEnumerateRandomARCer* )arcer)->setTypeUniform();
    conjL = getParameters().getValue("P4");
    factors = getParameters().getValue("uFact");
    
    msg1 << Link(Chars( "Click here to see the relators  of ")+
		 Text( Name(theGroup) )+
		 Chars(", enumerated using uniform distribution"),
		 "REnumerator",
		 fileName,true );
    
  }
  ((NCEnumerateRandomARCer* )arcer)->setTotalNumber(totalNum);
  ((NCEnumerateRandomARCer* )arcer)->setConjL(conjL);
  ((NCEnumerateRandomARCer* )arcer)->setNumOfFactors(factors);
  
  
  
  /*} else {
    arcer = new NCEnumerateTotalARCer( *this,theGroup.getFPGroup() ); 
    ((NCEnumerateTotalARCer* )arcer)->setTotalNumber(totalNum);
    fileName = ((NCEnumerateTotalARCer* )arcer)->getFileName();
    msg1 << Link(Chars( "Click here to see the relators  of ")+
    Text( Name(theGroup) ),
    "REnumerator",
    fileName,true );
    }
  */
  
  msg1.send();    
  
}

void NCRelatorEnumerator::resume( )
{              
  arcer->submitSignal();
  
  //  Chars selArcer = getParameters().getValue("RBG1");
  int totalNum = getParameters().getValue("P1");
  
  // if ( selArcer == Chars("RB1") ){
  int conjL;
  int factors;
  Chars randomType = getParameters().getValue("RandomRBG");
  if ( randomType == Chars("rRB1") ){
    
    conjL = getParameters().getValue("P3");
    factors = getParameters().getValue("nFact");   
  } else {
    
    conjL = getParameters().getValue("P4");
    factors = getParameters().getValue("uFact");   
    
  }
  
  
  ((NCEnumerateRandomARCer* )arcer)->setTotalNumber(totalNum);
  ((NCEnumerateRandomARCer* )arcer)->setConjL(conjL);
  ((NCEnumerateRandomARCer* )arcer)->setNumOfFactors(factors);
  
  /*} else {
    
    ((NCEnumerateTotalARCer* )arcer)->setTotalNumber(totalNum);
    
    }
  */
  
}

void NCRelatorEnumerator::viewStructure(ostream& ostr) const
{
  
   EnumertatorProblemView pv(oid(),"Relator enumerator",
			     "Enumerate relators using normal closure",
			     helpID("REnumeratorProblem",theGroup),
			      Text("Ncrel en")
			     );

   pv.addParameter(EditInteger("P1","Number of relators",1,100,0,1000000));

   RadioButton rRB1("rRB1","Enumerate using normal disribution");
   RadioButton rRB2("rRB2","Enumerate using uniform disribution");

   rRB1.add(EditInteger("P3","Max. length of conjugates",1,10,1,500));
   rRB2.add(EditInteger("P4","Max. length of conjugates",1,10,1,500));

   rRB1.add(EditInteger("nFact","Number of factors",1,10,1,32));
   rRB2.add(EditInteger("uFact","Number of factors",1,10,1,32));

   RadioButtonGroup randomRBG("RandomRBG");
   randomRBG.add(rRB1);
   randomRBG.add(rRB2);
   // randomRBG.addParameter(EditInteger("P2","Number of conjugates",0,10,1,1000));

   
   //RadioButton rb1("RB1","Enumerate randomly");
   //rb1.add(randomRBG);
   
   //RadioButtonGroup rbg("RBG1");

   //rbg.add(rb1);

   //   RadioButton rb2("RB2","Enumerate totally");
   //rb2.add(EditInteger("P5", "Any parameter",1) );
     
   //rbg.add(rb2);

   pv.addParameter(randomRBG);

   pv.addProblem( Subproblem("Sp1",ThisARCSlotID(),
			     Text("For the use of enumerator"),40));
   
  
   submit(ostr, pv);

}


