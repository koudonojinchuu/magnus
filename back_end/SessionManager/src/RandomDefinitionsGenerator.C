// Copyright (C) 2000 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of class RandomDefinitionsGenerator
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//


#include "RandomDefinitionsGenerator.h"
#include "NormalRandomWord.h"
#include "FreeGroup.h"
#include "Chars.h"
#include "OID.h"
#include "SMFPGroup.h"
#include "Vector.h"
#include "RandomMSCGroup.h"

#include "OutMessages.h"

void RandomDefinitionGenerate::readParameters(istream& istr)
{
  
  int n;
  istr > n;
  
  Chars type = "";
  Chars name;
  istr >> type;
  
  while ( type != Chars("{EOF_PARAM}")) { 
    
    istr >> name; 
        if (type == Chars("int") || type == Chars("RadioButtonGroup")
	|| type == Chars("text") || type == Chars("bool")) {
      Chars errMsg;
      gw.setValue(istr,errMsg, name);
      if (errMsg.length() > 0 ) {
	//ParseParamErrorMessage(bossOID,name,type,errMsg).send();
	//haveErrors = true;
	error("Error occured when parsin parameter");
      }
    } else {
     
      error("void ParameterStructure::read( istream& istr ) : "
	    "unexpected type of a parameter");
    }

    istr >> type;
   }
  
}

// ------------------------------- RandomWordGenerate -------------------------------------------


RandomWordGenerate::RandomWordGenerate(istream& is, const FPGroup& p, Chars id) :
  RandomDefinitionGenerate(), 
  istr(is),
  parent(p),
  averagelength(10),
  theID(id)
{

  gw.add(EditInteger("AL","Average word length",1,10,1,1000));
}

void RandomWordGenerate::requireParameters()
{
  cout << "simpleParameters {-4 PARAM} " << theID <<  " { Generate random word } ";
  gw.submit(cout);
  cout << endl << flush;

}

void RandomWordGenerate::printDefinition()
{
 
  averagelength = gw.getValue("AL");
  
 
  cout << "show_defn_  defn "
       << " {" << generateWord() << "} "
       << " w  {" << theID << "} "
       << endl << flush;

 
  LogMessage msg;
  msg << "Average length used : " << averagelength;
  msg.send();
  
}

Chars RandomWordGenerate::generateWord( )
{
  
  //@njz
  //  strstream str;
  std::strstream str;
  //
  
  NormalRandomWord nw(parent.numberOfGenerators(), averagelength, 
		      ur.rand(0,averagelength/5),
		      ur.rand(10,100000),ur.rand(10,100000));
  
  
  parent.printWord(str, nw.word().freelyReduce());

  str << '\0';
  
  Chars returnStr(str.str());
  
  return returnStr;
}

 int numberOfWords;
  bool isSet;

// ------------------------------- RandomCollectionOfWordsGenerate -------------------------------------------

RandomCollectionOfWordsGenerate::RandomCollectionOfWordsGenerate(istream& is, const FPGroup& p, Chars id, bool isS) :
  RandomDefinitionGenerate(), 
  istr(is),
  parent(p),
  averagelength(10),
  theID(id),
  numberOfWords(10),
  isSet( isS )
{
  gw.add(EditInteger("AL","Average word length",1,10,1,1000));
  gw.add(EditInteger("nW","Number of words",1,10,1,1000));
}

void RandomCollectionOfWordsGenerate::requireParameters()
{
  cout << "simpleParameters {-4 PARAM} " << theID <<  " { Generate random words } ";
  gw.submit(cout);
  cout << endl << flush;

}

void RandomCollectionOfWordsGenerate::printDefinition()
{
 
  averagelength = gw.getValue("AL");
  numberOfWords = gw.getValue("nW");
  
 
  cout << "show_defn_  defn "
       << " {" << generateCollection() << "} "
       << " w  {" << theID << "} "
       << endl << flush;

 
  LogMessage msg;
  msg << "Average length used : " << averagelength << ", number of words : " << numberOfWords;
  msg.send();
  
}

Chars RandomCollectionOfWordsGenerate::generateCollection( )
{
  
  //@njz
  //  strstream str;
  std::strstream str;
  //
  
  NormalRandomWord nw(parent.numberOfGenerators(), averagelength, ur.rand(0,averagelength/5),
		      ur.rand(10,100000),ur.rand(10,100000));

  // generate set of words
  if (isSet){
    SetOf<Word> s;
    // if ()
    while (s.cardinality() < numberOfWords ){
      s |= nw.word().freelyReduce();
    }
    /*    SetIterator<Word> I(s);
    for (I.reset();!I.done();I.next()) {
       parent.printWord(str,I.value());
       
    }*/
    parent.printSetOfWords(str,s);

  } else {  // just a tuple of words
    str << "{ ";
    for (int i=0;i<numberOfWords;i++){
      parent.printWord(str, nw.word().freelyReduce());
      if (i < numberOfWords - 1 ) str << ", ";
    }
    str <<  " }";
  }

  str << '\0';
  Chars returnStr(str.str());
  return returnStr;
}

// ------------------------------------- RandomGroupGenerate ----------------------------

RandomGroupGenerate::RandomGroupGenerate(istream& is, Chars id, bool iF, bool iN, bool iOR):
  RandomDefinitionGenerate(), 
  istr(is),
  theID(id),
  maxGens(10),
  maxRels(10),
  averageRels(10),
  nilpClass(3),
  isFree(iF),
  isNilpotent(iN),
  isOR(iOR)
{

  gw.add(EditInteger("mG","Maximal number of generators",1,10,1,100));
  if ( !isFree ) {
    if (!isOR )
      gw.add(EditInteger("mR","Maximal number of relators",1,10,1,100));
    gw.add(EditInteger("aR","Average length of relators",1,10,1,1000));
  }

  if (isNilpotent) 
     gw.add(EditInteger("cN","Nilpotency class",1,3,1,20));

}

void RandomGroupGenerate::requireParameters()
{
  cout << "simpleParameters {-4 PARAM} " << theID <<  " { Generate group presentation } ";
  gw.submit(cout);
  cout << endl << flush;

}

void RandomGroupGenerate::printDefinition()
{
 
  maxGens = gw.getValue("mG");
  if (!isFree){
    if (!isOR )
      maxRels = gw.getValue("mR");
    averageRels = gw.getValue("aR");
  }
  if (isNilpotent) 
    nilpClass =  gw.getValue("cN");

  cout << "show_defn_  defn "
       << " {" << generateGroup() << "} "
       << " w  {" << theID << "} "
       << endl << flush;
  
  
  LogMessage msg;
  msg << "A random presentation generated.";
  msg.send();
  

}


Chars RandomGroupGenerate::generateGroup()
{
  
  UniformRandom ur;

  //@njz
  //  strstream str;
  std::strstream str;
  //

  int nOfGens = ur.rand(2,maxGens);
  int nOfRels = 1;
  if (!isOR)
    nOfRels = ur.rand(2,maxRels);
  
  // generate generators and print them into output string
  VectorOf<Chars> genNames(nOfGens);

  if (isNilpotent)
    str << nilpClass;
  str << "< ";

  for (int i=0;i<nOfGens;i++){
    genNames[i] = Chars("x") + Chars(i+1);
    str << genNames[i];
    if (i < nOfGens-1)
      str << ", ";
  }
  
  if (!isFree) {
    // make a separator 
    str << " ; ";
    
    // generate relations and print them in the output
    FreeGroup f(genNames);
    
    for (int i=0;i<nOfRels;i++){
      NormalRandomWord nw(nOfGens, averageRels, ur.rand(0,averageRels/5),
			  ur.rand(10,100000),ur.rand(10,100000));
      f.printWord(str, nw.word().freelyReduce());
      if ( i< nOfRels-1 )
	str << ", ";
    }
  }
  str << " >" << '\0';
  Chars returnStr(str.str());
  
  return returnStr;
}


// ------------------------------------- RandomSCGroupGenerate ----------------------------

RandomSCGroupGenerate::RandomSCGroupGenerate(istream& is, Chars id):
  RandomDefinitionGenerate(), 
  istr(is),
  theID(id),
  maxGens(10),
  maxRels(10),
  averageRels(10)
{

  gw.add(EditInteger("mG","Maximal number of generators",1,10,1,100));
  gw.add(EditInteger("mR","Maximal number of relators",1,10,1,100));
  gw.add(EditInteger("aR","Average length of relators",1,10,1,1000));
}

void RandomSCGroupGenerate::requireParameters()
{
  cout << "simpleParameters {-4 PARAM} " << theID <<  " { Generate group presentation } ";
  gw.submit(cout);
  cout << endl << flush;

}

void RandomSCGroupGenerate::printDefinition()
{
   
  maxGens = gw.getValue("mG");
  maxRels = gw.getValue("mR");
  averageRels = gw.getValue("aR");

  cout << "show_defn_  defn "
       << " {" << /*generateSCGroup()*/ "aaaaaaaaaaaaaaaaaaaa" << "} "
       << " w  {" << theID << "} "
       << endl << flush;
  

  LogMessage msg;
  msg << "A random presentation of a small cancelation group generated.";
  msg.send();
  

}


Chars RandomSCGroupGenerate::generateSCGroup()
{
  
  UniformRandom ur;

  //@njz
  //  strstream str;
  std::strstream str;
  //

  int nOfGens = ur.rand(2,maxGens);
  int nOfRels = ur.rand(2,maxRels);
  
  // generate group and print them into output string

  RandomMSCGroup rscg( nOfGens,  ur.rand(10,100000),
		       averageRels,  ur.rand(0,averageRels/5),  ur.rand(10,100000), 
		       nOfRels, ur.rand(0,nOfRels/5),  ur.rand(10,100000)
		       );
  
  str << rscg.getRandomMSCGroup()  << '\0';

  Chars returnStr(str.str());
  
  return returnStr;
}


//---------------------------------------------------------------------------//
//---------------------------- RandomDefinitionsGenerator -------------------//
//---------------------------------------------------------------------------//

void RandomDefinitionsGenerator::readMessage(istream& istr)
{
  Chars def;
  Chars param;
  
  istr >> def;
  
  if ( def == Chars("FP") || def == Chars("ABELIAN") || def == Chars("FREE") || def == Chars("OR")  ||
       def == Chars("NILPOTENT") || def == Chars("FREE_NILPOTENT") ) {

    bool isFree = false;
    bool isNilpotent = false;
    bool isOR = false;

    if ( def == Chars("OR") )
      isOR = true;

    if ( def == Chars("NILPOTENT") ||  def == Chars("FREE_NILPOTENT") )
      isNilpotent = true;

    if ( def == Chars("FREE") ||  def == Chars("FREE_NILPOTENT") )
      isFree = true;

    int garbage;
    istr >> garbage;
    
    istr >> param;
    if (defGenerator)
      delete defGenerator;
     
    if (def  == Chars("SC")) {
      //defGenerator = new RandomGroupGenerate(istr, param, isFree, isNilpotent );
      //defGenerator->requireParameters();
      //defGenerator = new RandomSCGroupGenerate(istr, param );
      //defGenerator->requireParameters();
    } else {
      defGenerator = new RandomGroupGenerate(istr, param, isFree, isNilpotent, isOR );
      defGenerator->requireParameters();
    }

    /*    
  } else if ( def == Chars("SC") ){
  
    int garbage;
    istr >> garbage;
    
    istr >> param;
    if (defGenerator)
      delete defGenerator;
    
    defGenerator = new RandomSCGroupGenerate(istr, param );
    defGenerator->requireParameters();
    */
  } else if ( def == Chars("WORD")){
    int garbage;
    istr >> garbage;
    
    OID o;
    istr > o;
    istr >> param;
    
    
    SMFPGroup* parent = (SMFPGroup*)TheObjects::get( o );
    
    if (defGenerator)
      delete defGenerator;

    defGenerator = new RandomWordGenerate(istr,parent->getFPGroup(), param );
    defGenerator->requireParameters();

  }  else if ( def == Chars("SET_OF_WORDS") ||  def == Chars("VECTOR_OF_WORDS") || 
	       def == Chars("SUBGROUP") ){
    
    int garbage;
    istr >> garbage;
    
    OID o;
    istr > o;
    istr >> param;
    
    
    SMFPGroup* parent = (SMFPGroup*)TheObjects::get( o );
    
    if (defGenerator)
      delete defGenerator;

    bool isSet = false;
    
    if ( def == Chars("SET_OF_WORDS") )
      isSet = true;
    
    defGenerator = new RandomCollectionOfWordsGenerate(istr,parent->getFPGroup(), param, isSet );
    defGenerator->requireParameters();    
    
  } else if ( def == Chars("PARAM") ) {
    Chars garbage;
    istr >> garbage;
    
    if (!defGenerator)
      error("Sending parameter values before parameter request");
    
    defGenerator->readParameters(istr);
    defGenerator->printDefinition();

  }  else if (def == Chars("MAP") || def == Chars("EQUATION")|| def == Chars("SYS_OF_EQUATION")
	      || def == Chars("SC") ) {
    Warning warn;
    warn << "RandomDefinitionsGenerator::readMessage(istream& istr) : " <<  def << " not implemeted yet.";
    warn.send();
  } else
    error( "RandomDefinitionsGenerator::readMessage(istream& istr) : unrecognized command.");
}


RandomDefinitionGenerate*  RandomDefinitionsGenerator::defGenerator = NULL;
