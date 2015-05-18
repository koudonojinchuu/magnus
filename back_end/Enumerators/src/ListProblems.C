// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of class SMListJoin
//
// Principal Author: Alexei Myasnikov
//
// Status: in progress
//
// Revision History:
//

#include "ListProblems.h"
#include "OutMessages.h"
#include "SMListIterator.h"



// --------------------- SMListJoinARCer ------------------------ //


template <class T>
void SMListJoinARCer<T>::setArguments( const SMList<T>& l1,
				       const SMList<T>& l2)
{
  l1Data = new SMListData( l1.getData() );
  l2Data = new SMListData( l2.getData() );
}

template <class T>
void SMListJoinARCer<T>::runComputation( )
{
#if SAFETY > 0
  if( ! l2Data )
    error( "void WordsOfLengthARCer::runComputation( ) : "
	   "data must be initialized first");
#endif

  l1Data->setReadMode();
  l2Data->setReadMode();

  joinData.setWriteMode();

  for ( int i=0;i < l1Data->numberOfElements(); i++ )
    joinData << EnumWriteChars( l1Data->getElementOf( i ) ) << "\n\n";
  
  for ( int i=0;i < l2Data->numberOfElements(); i++ )
    joinData << EnumWriteChars( l2Data->getElementOf( i ) ) << "\n\n" ;

  l1Data->closeCurrentMode();
  l2Data->closeCurrentMode();
  joinData.closeCurrentMode();
}


template <class T>
void SMListJoinARCer<T>::writeResults( ostream& out ) 
{
  out < joinData;
}


template <class T>
void SMListJoinARCer<T>::readResults( istream& in )
{
  in > joinData;
}


// ----------------------------- SMListJoin ------------------------- //

template <class T>
SMListJoin<T>::SMListJoin( SMList<T>& sml1,SMList<T>& sml2 )
  : Supervisor( true ),
    list1( sml1 ),
    list2( sml2 ),
    arcer(*this),
    started( false )
{ 
  arcer.setArguments(sml1,sml2);
}


template <class T>
void SMListJoin<T>::takeControl( )
{
  if (freeARCs() > 0){
    
    if (arcer.takeControl()){
      Chars view_window_name;
      if ( T::type() == Word::type() ){
	view_window_name = "WEnumerator";
      } else if ( T::type() == Map::type() ){
	view_window_name = "RandomAutomorphisms";
      } else if ( T::type() == Subgroup::type() ){
	view_window_name = "SGEnumerator";
      } 
      
      SMList<T>* smo = 
	new  SMList<T>(list1.getGroup(),
		       arcer.getJoinData(),
		       Text("Join of") + Name(list1) +
		       Text("and") + Name(list2)
		       ); 
      
      ListOf<OID> dependencies( smo->getGroup() );
      CheckinMessage( *smo, "", dependencies ).send();
      
      LogMessage msg( *smo );
      msg << Link( Chars("Click here to see the contents of ") + 
		   Text( Name( *smo) ),
		   view_window_name,
		   smo->getData().getDataFileName() 
		   );
      msg << ".";
      msg.send();
      
      adminTerminate();
      return;
    } else
      usedOneARC();
  }
}



template <class T>
void SMListJoin<T>::viewStructure(ostream& ostr) const
{
  
  ProblemView pv( ostr, oid(),Text("Compute the join of two lists"),
		  Text("Compute the join of") + Name(list1) +Text("and")+
		  Name(list2),
		  helpID("SMListJoin",
			 list1.getGroup()),
		  Text("join of lists")
		  );
  
  pv.startItemGroup();

  pv.add( Text("For the use of this problem"), ThisARCSlotID(), 50 );
  pv.done();
  
}
// -------------------- SMList ------------------------ //

template <class T>
void MakeSMListOf<T>::takeControl()
{
  // Some specific type attributes
  Chars elements;
  Chars view_window_name;
  if ( T::type() == Word::type() ){
    elements = "words";
    view_window_name = "WEnumerator";
  } else if ( T::type() == Map::type() ){
    elements = "maps";
    view_window_name = "RandomAutomorphisms";
  } else if ( T::type() == Subgroup::type() ){
    elements = "subgroups";
    view_window_name = "SGEnumerator";
  }

  SMList<T>* smo = 
    new   SMList<T>(enumerator,
		       Chars("List of ")+elements+Chars(" from \\\"") + 
		       Chars(Text(Name(enumerator)))+Chars("\\\"")
		       ); 
  
  
  ListOf<OID> dependencies( smo->getGroup() );
  CheckinMessage( *smo, "", dependencies ).send();
  
  // enumerator.getParentObject().addDependent( smo->oid() );

  LogMessage msg( *smo );
  msg << Link( Chars("Click here to see the contents of ") + 
	       Text( Name( *smo) ),
	       view_window_name,
	       smo->getData().getDataFileName() 
	       );
  msg << ".";
  msg.send();

  LogMessage msg1( *smo );
  msg1 << "There are " << smo->getData().numberOfElements() 
       << " items in " << Name( *smo ) << ".";
  msg1.send();
     
}

// -------------------------- SMListCheckerARCer --------------- //
/*


char SMListCheckerARCer::getElementInBuffer( int i ) const
{
#if ( SAFETY > 0 )
  if ( i < 0 || i >= bufferLength ) 
    error("char SMListCheckerARCer::getElementInBuffer( int i ) const:"
	  " index out of bounds");
#endif
  return theBuffer[i];
}
  
void SMListCheckerARCer::setElementInBuffer( int i, char ch )
{
#if ( SAFETY > 0 )
  if ( i < 0 || i >= bufferLength ) 
    error("char SMListCheckerARCer::getElementInBuffer( int i ) const:"
	  " index out of bounds");
#endif
  theBuffer[i] = ch;  
}

*/


template class SMListJoin<Word>;
template class SMListJoin<Subgroup>;
template class SMListJoin<Map>;
template class SMListJoinARCer<Word>;
template class SMListJoinARCer<Subgroup>;
template class SMListJoinARCer<Map>;

template class MakeSMListOf<Word>;
template class MakeSMListOf<Subgroup>;
template class MakeSMListOf<Map>;
