#include "AutoEnumerator.h"
#include "SMEnumerator.h"
#include "OutMessages.h"
#include "SMFPGroup.h"
#include "viewStructure.h"
#include "SMListIterator.h"
#include "SMMap.h"
#include <signal.h>

// ---------------------- AutEnumeratorARCer --------------------------- //

void AutEnumeratorARCer2::setArguments( FreeGroup group, int avgNumbers, int n)
{
  if (randAuto) delete randAuto;
  randAuto = new RandomAutoInFree( group, avgNumbers );
  number = n;
}

void AutEnumeratorARCer2::enumerate( )
{
#if SAFETY > 0
  if( randAuto == 0 ) return;
  //    error( "void AutEnumeratorARCer::runComputation( ) : "
  //	   "The enumerator has not been initialized yet");
#endif
  Map m;
  int order;
  while (current < number) {
    if (submitSignalRecieved()) return;
    current++;
    if (!isFinite) m = randAuto->getAutomorphism();
    else m = randAuto->getFiniteAutomorphism(order);
    if (isAbelian) {
      VectorOf<Word> v = m.generatingImages();
      for(int i=0; i<v.length(); i++) 
	v[i] = AbelianWord(m.range().numberOfGenerators(), v[i]).getWord();
      m.setGeneratingImages(v);
    }
    theData << EnumWriteMap( m ) << "\n";
    if (isFinite) theData << Chars("The order: ") + Chars(order) << "\n";
    theData << "\n";
  }
}

void AutEnumeratorARCer2::writeResults( ostream& out ) 
{
  out < current < theData;
}


void AutEnumeratorARCer2::readResults( istream& in )
{
  in > current > theData;
}

// ---------------------- AutoEnumeratorProblem ----------------------- //

AutoEnumeratorProblem::AutoEnumeratorProblem(SMFPGroup& b)
  : EnumeratorProblem< Map >( b ),
    theGroup(b),
    arcer( *this, theData )
{
  arcer.isFinite = false;
  arcer.isAbelian = theGroup.gic.isAbelian()==yes;
}

void AutoEnumeratorProblem::takeControl( )
{
  if (freeARCs() > 0) {
    if (arcer.takeControl()){
      adminSuspend();
      return;	
    }
    usedOneARC();
  }
}

void AutoEnumeratorProblem::viewStructure(ostream& ostr) const
{
  
   EnumertatorProblemView pv(oid(),"Automorphism enumerator",
			     Text("Randomly enumerate automorphisms of") +
			     Name( theGroup ),
			     helpID("AutoEnumeratorProblem",theGroup),
			     Text("Aut en") );
   pv.addParameter(EditInteger("P1","Number of automorphisms",1,10,0,10000));
   pv.addParameter(EditInteger("P2","Average number of Nielsen generators",0,10,0,50));
   pv.addProblem( Subproblem("Sp1",ThisARCSlotID(),
			     Text("For the use of enumerator"),40));
   submit(ostr, pv);
}

FiniteAutoEnumeratorProblem::FiniteAutoEnumeratorProblem(SMFPGroup& b)
  : EnumeratorProblem< Map >( b ),
    theGroup(b),
    arcer( *this, theData )
{
  arcer.isFinite = true;
  arcer.isAbelian = theGroup.gic.isAbelian()==yes;
}

void FiniteAutoEnumeratorProblem::takeControl( )
{
  if (freeARCs() > 0){
    if (arcer.takeControl()) {
      adminSuspend();
      return;	
    }
    usedOneARC();
  }
}

void FiniteAutoEnumeratorProblem::viewStructure(ostream& ostr) const
{
 
  EnumertatorProblemView pv(oid(),"Automorphism enumerator",
			    Text("Randomly enumerate automorphisms of")
			    + Name(theGroup) + Text("of finite order"),
			    helpID("FiniteAutoEnumeratorProblem", theGroup),
			    Text("Fin aut en") );
   pv.addParameter(EditInteger("P1","Number of automorphisms",1,10,0,1000000));
   pv.addParameter(EditInteger("P2","Average number of Nielsen generators",0,10,0,50));
   pv.addProblem( Subproblem("Sp1",ThisARCSlotID(),
			     Text("For the use of enumerator"),40));
   
  

   submit(ostr, pv);
}

// --------------------- IsMapInListARCer ------------------------ //

void IsMapInListARCer::setArguments( const SMList<Map>& l, const Map& m)
{
  theIterator = new SMListIterator<Map>(l);
  theMap = m;
}

void IsMapInListARCer::runComputation( )
{
#if SAFETY > 0
  if( !theIterator )
    error( "void IsMapInListARCer::runComputation( ) : "
	   "fileIterator must be initialized first");
#endif
  for (theIterator->reset();!theIterator->EOL();theIterator->nextCell()){
    
    if ( theMap == theIterator->getData() ){
      result = true;
      return;
    }
  }
  result = false;
}

void IsMapInListARCer::writeResults( ostream& out ) 
{
  out < result;
}

void IsMapInListARCer::readResults( istream& in )
{
  in > result;
}

bool IsMapInListARCer::isInList() 
{
#if SAFETY > 0
  if( !theIterator )
    error( "bool IsWordInListARCer::isInList() : "
	   "fileIterator must be initialized first");
#endif
  return result;
}

// ----------------------------- IsMapInList ------------------------- //

IsMapInList::IsMapInList(SMList<Map>& sml,const SMMap& map)
  : Supervisor( true ),
    theList( sml ),
    theMap( map ),
    arcer(*this)
{ 
  arcer.setArguments(sml, map.getMap());
}

void IsMapInList::takeControl( )
{
  if (freeARCs() > 0) {
    if (arcer.takeControl()) {
      LogMessage msg( *this, theMap );
      msg << Name( theList );
      if (arcer.isInList()) msg << " contains ";
      else msg << " does not contain ";
      msg << Name( theMap ) << ".";
      msg.send();
      adminTerminate();
      return;
    }
    usedOneARC();
  }
}

void IsMapInList::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(),Text("Containment problem"),
		  Text("Determine whether") + Name(theMap)
		  + Text("lies in") + Name(theList),
		  helpID("IsMapInList", theList.getGroup()),
		  Text(Name(theMap)) + Text("is in") +
		  Name(theList) + Text("?") );
  pv.startItemGroup();
  pv.add( Text("For the use of this problem"), ThisARCSlotID(), 50 );
  pv.done();
}
