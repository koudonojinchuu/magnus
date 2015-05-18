// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes RankOfSubgroup, RankOfSubgroupProblem, RankOfSubgroupARCer
//
// Principal Author: Alexander Ushakov
//
// Status: in progress
//
// Revision History:
//


#include "Set.h"
#include "RankOfSubgroup.h"
#include "OutMessages.h"
#include "viewStructure.h"
//#include "GeneralWhitehead.h"
#include "SMSubgroup.h"

//################################################

ProperSubgroupEnumerator::ProperSubgroupEnumerator(const FreeGroup& F, const VectorOf<Word>& words) :
  theGroup( F ),
  theWords( words ),
  subgroup( 0 ),
  rank( theGroup.rank() )
{
  int t, cur, w;

  genOfGroup = VectorOf<Word>(rank);

  for( t=0 ; t<rank ; ++t ) 
    genOfGroup[t] = Word(Generator(t+1));

  //qVert - number of vertices
  qVert = 1;
  for( t=0 ; t<words.length() ; ++t ) 
    qVert += words[t].length()-1;

  setToVert  = new int[qVert];
  vertToWord = new int[qVert];
  vertToPos  = new int[qVert];

  for( t=0,cur=0,w=0 ; t<qVert ; ++t,++cur ) {
    setToVert[t] = UNKNOWN;
    vertToWord[t] = w;
    vertToPos[t]  = cur;
    if( cur==words[w].length()-1 ) {
      ++w;
      cur = 0;
    }
  }

  partition  = new int[qVert];
  _partition = new int*[qVert];
  for( t=0 ; t<qVert ; ++t ) {
    partition[t] = UNKNOWN;
    _partition[t] = new int[qVert];
  }
  lCells = new LC[qVert];

  nVert = 0;
  nSet = 0;
  qSet = 0;

  qWord = theWords.length();
  //generators of new subgroup
  genOfSG = VectorOf<Word>( qVert+qWord , theWords );
  buildSG( );
}


ProperSubgroupEnumerator::~ProperSubgroupEnumerator()
{
  for( int t=0 ; t<qVert ; ++t ) 
    delete []_partition[t];
  delete []_partition;

  delete []setToVert;
  delete []vertToWord;
  delete []vertToPos;
  delete []lCells;
  if( subgroup )
    delete subgroup;
}


bool ProperSubgroupEnumerator::getNextProperSubgroup(SGofFreeGroup& sg)
{
  while( nVert>=0 ) {

    if( nSet<=qSet ) {
      if( stepTo() ) {
	if( nVert==qVert ) {
  
	  /*
	  static int _t = 0;	  
	  file << _t << ": {";
	  for( int t=0; t<qVert; ++t) 
	    file << partition[t] << " , ";
	  file << "}" << endl;
	  ++_t;
	  */

	  VectorOf<Word> vt = subgroup->nielsenBasis( );
	  sg = SGofFreeGroup( theGroup , vt );

	  stepBack();
	  ++nSet;
	  return true;
	}
      }
    }
    else {
      stepBack();
    }
    ++nSet;
  }
  return false;
}


bool ProperSubgroupEnumerator::stepTo()
{
  int t;

  if( partition[nVert]!=UNKNOWN ) {

    //
    if( nSet>partition[nVert] ) {
      nSet = qSet;
      return false;
    }
    nSet = partition[nVert];

    // debug checkout
    if( setToVert[nSet]==UNKNOWN ) {
      file << "Error !!! " << endl;
      //error
    }

    // remember step
    for( t=0 ; t<qVert ; ++t ) 
      _partition[nVert][t] = partition[t];
    partition[nVert] = nSet;
    
    lCells[nVert].qSet = qSet;
    
    nSet = -1;
    ++nVert;
    return true;  
  }
  else {

    // 
    for( t=0 ; t<qVert ; ++t ) 
      _partition[nVert][t] = partition[t];

    //
    partition[nVert] = nSet;

    // check references to vertice
    for( t=0 ; t<qVert ; ++t) 
      if( partition[t]==nVert )
	partition[t] = nSet;
    
    lCells[nVert].qSet = qSet;
    if( nSet==qSet ) // mem. first vertice of set
      setToVert[qSet++] = nVert;
    else {
      // add new generator
      genOfSG[qWord++] = getWord(setToVert[nSet], nVert);
      buildSG( );
    }
    nSet = -1;
    ++nVert;

    if( !setRefs() || subgroup->contains(genOfGroup) ) {
      stepBack();
      return false;
    }

    return true;
  }
}


void ProperSubgroupEnumerator::stepBack()
{
  int t;
  --nVert;

  if( nVert>=0 ) {

    nSet = partition[nVert];

    for( t=0 ; t<qVert ; ++t ) 
      partition[t] = _partition[nVert][t];

    if( qSet > lCells[nVert].qSet ) 
      setToVert[qSet] = UNKNOWN;
    else 
      if( partition[nVert]==UNKNOWN ) {
	--qWord;
	buildSG( );
      }

    qSet = lCells[nVert].qSet;
  }
}


Word ProperSubgroupEnumerator::getWord(int x, int y)
{
  int t;
  int w1 = vertToWord[x];
  int w2 = vertToWord[y];
  int p1 = vertToPos[x];
  int p2 = vertToPos[y];
  VectorOf<Generator> vect(p1 + theWords[w2].length()-p2 );

  for( t=0 ; t<p1 ; ++t ) 
    vect[t] = theWords[w1][t];
  for( t=0 ; t<theWords[w2].length()-p2 ; ++t )
    vect[p1+t] = theWords[w2][p2+t];

  /*
  file << "getWord(" << x << "," << y << ")" << " -> " ;
  theGroup.printWord(file, Word(vect) );
  file << endl;
  */

  return Word(vect);
}


void ProperSubgroupEnumerator::buildSG( )
{
  if( subgroup )
    delete subgroup;

  subgroup = new SGofFreeGroup( theGroup , VectorOf<Word>(qWord, genOfSG) );
}


bool ProperSubgroupEnumerator::setRefs( )
{
  int t, t1, t2;
  Word w;
  int ns1;
  int ns2;

  for( t1=0 ; t1<qVert; ++t1 ) {
    ns1 = partition[t1];
    if( ns1==UNKNOWN || setToVert[ns1]==t1 ) {

      for( t2=t1+1 ; t2<qVert; ++t2 ) {
	ns2 = partition[t2];
	if( ns2==UNKNOWN || setToVert[ns2]==t2 ) {
	  
	  w = getWord(t1, t2);
	  if( subgroup->contains(w) ) {

	    // 1.	    
	    if( ns1!=UNKNOWN && ns2!=UNKNOWN )
	      return false;
	    
	    // 2.
	    if( ns1!=UNKNOWN ) {
	      partition[t2] = ns1;
	      for( t=t2 ; t<qVert ; ++t )
		if( partition[t]==t2 )
		  partition[t]=ns1;
	      continue;
	    }
	    
	    // 3.
	    if( ns2!=UNKNOWN ) {
	      file << "Waring !!! 'ns2!=UNKNOWN'" << endl;
	      continue;
	    }
	    
	    // 4. 
	    // ns1==UNKNOWN && ns2==UNKNOWN
	    partition[t2] = ns1;
	    for( t=t2 ; t<qVert ; ++t )
	      if( partition[t]==t2 )
		partition[t]=ns1;
	    
	  }
	}
      }
    }
  }

  return true;
}



// -------------------- RankOfSubgroupArcer --------------------------- //


RankOfSubgroupARCer::RankOfSubgroupARCer(ComputationManager& boss, 
		   const class SMFPGroup& group,  
		   const SGofFreeGroup& sg ) :
  ARCer ( boss ),
  theGroup ( group ),
  theSubgroup ( sg ),
  answer ( dontknow ),
  PSE ( new ProperSubgroupEnumerator( theGroup.getFreePreimage( ) , sg.generators() ) )
{
  
}

RankOfSubgroupARCer::~RankOfSubgroupARCer( )
{
  delete PSE;
}

void RankOfSubgroupARCer::runComputation()
{
  lookingup();
}


void RankOfSubgroupARCer::writeResults( ostream& out ) 
{
  out < answer;
}


void RankOfSubgroupARCer::readResults( istream& in )
{
  in > answer;
}


bool RankOfSubgroupARCer::isAFreeFactorIn( SGofFreeGroup& g ) const
  // check whether subgroup 'theSubgroup' is a free factor in subgroup g
  // subgroup g must contains subgroup 'theSubgroup'
{
  // rewrite subgroup g
  const VectorOf<Word> basis = g.nielsenBasis( );
  const SGofFreeGroup  nielsGroup(theGroup.getFreePreimage(), basis);

  // rewrite generators of 'theSubgroup' in a new generators of subgroup g
  int numGen = theSubgroup.generators().length( );
  VectorOf<Word> newGen( numGen );
  for( int t=0 ; t<numGen ; ++t ) 
    newGen[t] = expressWordInSubgroupGenerators(nielsGroup, theSubgroup.generators()[t]);

  FreeGroup      freeGroup( basis.length() );
  SGofFreeGroup sg(freeGroup, newGen);
  return sg.isAFreeFactor( );
}


void RankOfSubgroupARCer::printSG( ostream& file, const SGofFreeGroup& subgroup )
{
  VectorOf<Word> vect(subgroup.generators());

  file << "<< ";
  for( int t=0 ; t<vect.length() ; ++t ) {
    if( t )
      file << " , ";
    theGroup.getFreePreimage().printWord(file, vect[t]);
  }
  file << " >>" << endl;

  file << endl;
}


void RankOfSubgroupARCer::lookingup()
{
  SGofFreeGroup sg( theGroup.getFreePreimage() , VectorOf<Word>() );
  File file( getFileName() );
  SetOf<SGofFreeGroup> subgroups;

  while( PSE->getNextProperSubgroup( sg ) ) {

    if( subgroups.contains(sg) ) 
      continue;
    
    subgroups.adjoinElement(sg);  
    if( isAFreeFactorIn(sg) ) {
      static int _t = 0;
      ++_t;
      file << _t << " :  ";
      printSG( file , sg );
    }

  }

  answer = true;
}


// -------------------- RankOfSubgroupProblem --------------------------- //


RankOfSubgroupProblem::RankOfSubgroupProblem(const SMSubgroup& sg)
    : Supervisor( true ), 
      theSubgroup( sg ), 
      rankOfSubgroup( *this ),
      linkHasBeenSent( false )      
{ 

}
 
const SMSubgroup& RankOfSubgroupProblem::getSubgroup( ) const 
{ 
  return theSubgroup; 
}



void RankOfSubgroupProblem::takeControl( )
{
  if( !linkHasBeenSent ) {
  
    linkHasBeenSent = true;
    
    LogMessage msg( *this, theSubgroup );
    msg << Link( Text("Click here to see the rank of ") + Name( theSubgroup ),
		"RankOfSubgroup", rankOfSubgroup->getFileName(), true);
    msg.send();

  }

  Trichotomy answer = rankOfSubgroup->getAnswer();
  if ( rankOfSubgroup.exists() && answer!=dontknow) {
    
    LogMessage msg( *this, theSubgroup );
    msg.send();
    
    adminTerminate();
    return;
  }

}


void RankOfSubgroupProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Rank"),
		  Text("Compute rank of ") + Name( theSubgroup )
		  + " in "
		  + Name( Parent( theSubgroup ) ) + "?",
		  helpID("RankOfSubgroupProblem", theSubgroup.getParent()),
		  "Rank"
		  );

  pv.startItemGroup();

  pv.add(Text("For the use of the algorithm"), rankOfSubgroup.arcSlotID(), 100);

  pv.done();
}



// ------------------------ RankOfSubgroup ------------------------------ //


RankOfSubgroup::RankOfSubgroup(RankOfSubgroupProblem& problemObject)
    : theSubgroup( problemObject.getSubgroup() ),
      theGroup( theSubgroup.getParent().getFreePreimage() ),
      theARCer(*this, problemObject.getSubgroup().getParent(), problemObject.getSubgroup().getSubgroup())
{
  adminStart();
}


RankOfSubgroup::~RankOfSubgroup( )
{

}


void RankOfSubgroup::takeControl( )
{

  if ( freeARCs() > 0 ) {

    if ( theARCer.takeControl() ) {

      adminTerminate();
      return;
    }   

    usedOneARC();
  }

}
