// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes IsWordAPE, IsWordAPEProblem, IsWordAPEARCer
//
// Principal Author: Alexander Ushakov
//
// Status: in progress
//
// Revision History:
//


#include "IsWordAPE.h"
#include "GeneralWhitehead.h"
#include "OutMessages.h"


template <> int SetData< IsWordAPEARCer::pairSG_bool >::
hashElement( const IsWordAPEARCer::pairSG_bool& pair) const
{
  return pair.hash( );
}


// -------------------- IsWordAPEArcer --------------------------- //


IsWordAPEARCer::IsWordAPEARCer(ComputationManager& boss, 
			       const class FreeGroup& group,
			       const Word& word ) :
  ARCer ( boss ),
  theGroup ( group ),
  theWord ( word ),
  answer ( dontknow ),
  theSG ( 0 ),
  haveSG ( false )
{
  VectorOf<Word> v(1); 
  v[0] = word;
  PSE = new ProperSubgroupEnumerator( theGroup , v ); 
}


IsWordAPEARCer::~IsWordAPEARCer( )
{
  if (theSG) delete theSG;
  delete PSE;
}


void IsWordAPEARCer::runComputation()
{
  lookingup();
}


void IsWordAPEARCer::writeResults( ostream& out ) 
{
  out < answer ;
  out < " " ;
  out < haveSG;
  out < " ";
  if( haveSG ) out < theSG->generators();
}


void IsWordAPEARCer::readResults( istream& in )
{
  in > answer ;
  in > haveSG;
  if( haveSG ) {
    VectorOf<Word> v;
    in > v;
    theSG = new SGofFreeGroup( theGroup , v );
  }
}


void IsWordAPEARCer::printSG( ostream& file, const SGofFreeGroup& subgroup )
{
  VectorOf<Word> vect(subgroup.generators());

  file << "{ ";
  for( int t=0 ; t<vect.length() ; ++t ) {
    if( t )
      file << " , ";
    theGroup.printWord(file, vect[t]);
  }
  file << " }" << endl;

  file << endl;
}


void IsWordAPEARCer::lookingup()
{
  VectorOf<Word> v;
  SGofFreeGroup sg( theGroup , v );

  File file( getFileName() );

  // check for prime in a given free group
  GeneralWhitehead GW( theGroup , false );
  GW.startComputation( theWord );
  while ( !GW.continueComputation( ) );
  if( GW.extendsToFreeBasis() ) {
    file << "The word ";
    theGroup.printWord(file, theWord);
    file << " is primitive in the group " 
	 << theGroup
	 << endl;  
    answer = no;
    return;
  }

  file << "The word ";
  theGroup.printWord( file, theWord );
  file << " is not primitive in the group " 
       << theGroup
       << endl << endl;  
  
  SetOf<pairSG_bool> setSG;
  int count = 0;

  file << "New set of subgroups to check : " << endl << endl; 

  // enumerate subgroups
  while( PSE->getNextProperSubgroup( sg ) ) {

    ++count;
    bool cf = false;
    SetIterator<pairSG_bool> iterator(setSG);

    // check whether current subgroup contains in any subgroup getting earlier
    for( ;!iterator.done() ; iterator.next() ) { 

      if( iterator.value().getSG().contains(sg) ) {
	cf = true;
	break;
      }
      if( sg.contains(iterator.value().getSG( )) ) 
	setSG.removeElement( iterator.value() );

    }
    if( cf )
      continue;
    
    //write number of current subgroup & subgroup;
    static int _t = 0;
    ++_t;
    file << _t << " :  ";
    printSG( file , sg );

    setSG.adjoinElement( pairSG_bool(sg) );

    if( count>=period ) {
      count -= period;

      // check for primitive
      file << "Check whether the word is primitive in the list of subgroups " 
	   << "above:" 
	   << endl << endl;

      iterator = SetIterator<pairSG_bool> (setSG);
      for( ; !iterator.done() ; iterator.next() ) {
	if( iterator.value().getChecked() ) 
	  continue;

	setSG.removeElement(iterator.value());
	pairSG_bool pair(iterator.value().getSG( ), true);
	setSG.adjoinElement(pair);

	if( !checkForPrimitive( iterator.value().getSG() ) ) {
	  answer = no;
	  haveSG = true;
	  theSG = new SGofFreeGroup(iterator.value().getSG( ) );
	  return;
	}
	
      } //check for primitive
      
      file << "New set of subgroups to check : " << endl << endl; 

    } // (count > period)

  }
  
  SetIterator<pairSG_bool> iterator(setSG);
  file << "Check whether the word is primitive in the list of subgroups " 
       << "above:" 
       << endl << endl;
  while( !iterator.done() ) {

    if( iterator.value().getChecked( ) ) {
      iterator.next();
      continue;
    }
    if( !checkForPrimitive( iterator.value().getSG( ) ) ) {
      answer = no;
      haveSG = true;
      theSG = new SGofFreeGroup(iterator.value().getSG( ) );
      return;
    }
    iterator.next();
  }

  answer = yes;
}


bool IsWordAPEARCer::checkForPrimitive(SGofFreeGroup sg)
{ 
  bool result = false;

  // my free group
  const FreeGroup myGroup = theGroup;
  // checked word
  const Word word = theWord;
  // length of the word 
  const int length = word.length();

  // rebuild SG
  sg = SGofFreeGroup( myGroup , sg.nielsenBasis( ) );
  // rebuild word to SG
  Word newWord = expressWordInSubgroupGenerators( sg , word );
  // compute rank of the subgroup
  int rank = sg.rank( );
  
  FreeGroup freeGroup(rank);
  
  File file( getFileName( ) );

  //fast check
  {
    int i;
    int *exp = new int[rank];
    for( i=0 ; i<rank ; ++i ) exp[i] = 0;
    for( i=0 ; i<newWord.length() ; ++i )
      exp[ abs( newWord[i] )-1 ]++;

    for( i=0 ; i<rank ; ++i ) 
      if( exp[i]==1 )
	result = true;

    delete []exp;
  }

  // check
  if( !result ) {
    GeneralWhitehead GW( freeGroup , false );
    GW.startComputation( newWord );
    while ( !GW.continueComputation( ) );

    if ( GW.extendsToFreeBasis() ) result = true;
    else result = false;
  }

  // make note in file
  {
    int t;
    VectorOf<Word> v = sg.generators();
    file << "  Subgroup << ";
    for( t=0; t<v.length(); ++t ) {
      if( t ) file << " ; ";
      myGroup.printWord(file, v[t]);
    }
    file << " >> ";
    file << endl;
    
    file << "  The word expressed in the generators of the subgroup: '";
    freeGroup.printWord( file , newWord );
    file << "'" << endl;
    file << "  Result: ";
    if( !result ) file << "not" ;
    file << " primitive. " << endl << endl;
  }

  return result;
}


// -------------------- IsWordAPEProblem --------------------------- //


IsWordAPEProblem::IsWordAPEProblem(const SMWord& w)
    : Supervisor( true ), 
      theWord( w ), 
      isWordAPE( *this ),
      linkHasBeenSent( false )      
{ 

}
 

void IsWordAPEProblem::takeControl( )
{
  if( !linkHasBeenSent ) {
  
    linkHasBeenSent = true;
    
    LogMessage msg( *this, theWord );
    msg << Link("Click here to obtain details of the computation.",
		"IsWordAPE", isWordAPE->getFileName(), true);
    msg.send();

  }

  Trichotomy answer = isWordAPE->getAnswer();
  if ( isWordAPE.exists() && answer!=dontknow) {
    
    LogMessage msg( *this, theWord );
    if (answer == yes)
      {
	msg << Name(theWord) << " is almost primitive in "
	    << Name(theWord.getParent()) << ".";
      }
    else {
      if( isWordAPE->haveSG() ) {

	msg << Name(theWord) << " is not almost primitive in "
	    << Name(theWord.getParent()) << " because it is not primitive in "
	    << "the subgroup  { ";
	VectorOf<Word> v;
	SGofFreeGroup sg( theWord.getParent().getFreePreimage(), v);
	isWordAPE->getSG(sg);
	v = sg.generators();
	int n;
	for( n=0;n<v.length();++n ) {
	
	  if( n ) msg << " , ";
	  theWord.getParent().getFreePreimage().printWord(msg, v[n]);
	}
	msg << " } ";
      }
      else 
	msg << Name(theWord) << " is primitive in " << Name(theWord.getParent());
    }
    msg.send();
    
    adminTerminate();
    return;
  }

}


void IsWordAPEProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Almost primitive element problem"),
		  Text("Is") + Name( theWord )
		  + "almost primitive in"
		  + Name( Parent( theWord ) ) + "?",
		  helpID("IsWordAPEProblem", theWord.getParent()),
		  "APE ?"
		  );

  pv.startItemGroup();

  pv.add(Text("For the use of the algorithm"), isWordAPE.arcSlotID(), 100);

  pv.done();
}


// ------------------------ IsWordAPE ------------------------------ //


IsWordAPE::IsWordAPE(IsWordAPEProblem& problemObject)
    : theWord( problemObject.getWord() ),
      theGroup( theWord.getParent().getFreePreimage() ),
      theARCer(*this, theGroup, problemObject.getWord().getWord())
{
  adminStart();
}


IsWordAPE::~IsWordAPE( )
{

}


void IsWordAPE::takeControl( )
{

  if ( freeARCs() > 0 ) {

    if ( theARCer.takeControl() ) {

      adminTerminate();
      return;
    }   

    usedOneARC();
  }

}







