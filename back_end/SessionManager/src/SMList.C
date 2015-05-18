// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of class SMList, SMListIterator
//
// Principal Author: Alexei Myasnikov
//
// Status: in progress
//
// Revision History:
//

#include "SMEnumerator.h"
#include "SMSubgroup.h"
#include "Subgroup.h"
#include "Timer.h"
#include "AutoEnumerator.h"
#include "SMList.h"
#include "SMListIterator.h"
#include "MagnusHome.h"
#include <sys/mman.h>
#include <fcntl.h>

//--------------------------------------------------------------------------//
//------------------------------- SMList  ----------------------------------//
//--------------------------------------------------------------------------//

template <class T>
SMList<T>::SMList( EnumeratorProblem<T>& P, const Chars heritage )
  : AlgebraicObject( heritage ),
    theParent( P.getParentObject() ),
    theData( P.getData() )
{
  

  // Check the parent type 
  Chars theParentType(  theParent.typeID() );
  
  if ( theParentType != Chars(SMFPGroup::type()) && 
       theParentType != Chars(SMSubgroup::type()) )
    error("SMList::SMList(SMFPGroup& , const Chars, Chars , bool ) :"
	  "wrong type of the parent object.");

  theLic = new LIC( oid(), theData.numberOfElements() );
}

template <class T>
SMList<T>::SMList( const SMFPGroup& ao, const SMListData& d, const Chars heritage )
  : AlgebraicObject( heritage ),
    theParent( ao ),
    theData( d )
{
  
  // Check the parent type 
  Chars theParentType(  theParent.typeID() );
  
  if ( theParentType != Chars(SMFPGroup::type()) && 
       theParentType != Chars(SMSubgroup::type()) )
    error("SMList::SMList(AlgebraicObject& ao, const Chars& f, const Chars heritage  ) :"
	  "wrong type of the parent object.");  

  theLic = new LIC( oid(), theData.numberOfElements() );
}

   
template <class T>
SMList<T>::~SMList() 
{
    delete theLic;
}

template <class T>
void SMList<T>::printProperties(ostream& ostr) const
{
  ostr << DataPair( Parent( oid() ), Object( theParent ) );
}


template <class T>
void SMList<T>::viewStructure(ostream& ostr) const
{
  ostr
    << "viewBuilder objectView viewParameters {viewID "
    << oid()
    << " heritage {"
    << heritage()
    << "}}";
}

template <class T>
void SMList<T>::printDefinition(ostream& ostr) const
{

}

template <class T>
const SMFPGroup& SMList<T>::getGroup() const
{
  if (Chars(  theParent.typeID() ) == Chars(SMFPGroup::type()) )
    return (SMFPGroup& )theParent;
  else
    return (SMFPGroup& )((SMSubgroup& )theParent).getParent();
  
}








// ------------------------------- LIC ------------------------------------- //


LIC::LIC(OID list_oid, int num) :
  listOID( list_oid ),
  number_of_elements( num ),
  wpStatus( NULL ),
  abStatus( NULL ),
  centStatus( NULL )
{
  wpStatus = new status_type( num );
  abStatus = new status_type( num );
  centStatus = new status_type( num );
}


LIC::~LIC( )
{
  if ( wpStatus != NULL )
    delete wpStatus;
  if ( abStatus != NULL )
    delete abStatus;
  if ( centStatus != NULL )
    delete centStatus;}

///////////////////////////////////////////////////////////////
//                                                           //
//  Is Trivial                                               //
//                                                           //
///////////////////////////////////////////////////////////////


bool LIC::hasTrivialStatusBuffer( ) const
{
    if ( wpStatus != NULL )
         return true;
    else
         return false;
}

// pseudo constant function
Trichotomy LIC::getTrivialStatus( int i, GIC::AlgorithmID& al, Chars& e) const
{
  Trichotomy ret = wpStatus->getStatus( i, al );
  if ( ret != dontknow )
    e = explanation( al );
  return ret;
}

void LIC::setTrivialStatus( int i, Trichotomy is_trivial,GIC::AlgorithmID al )
{
  wpStatus->setStatus( i, is_trivial, al );
}

bool LIC::IsTrivialChecked() const
{
  return wpStatus->isChecked();
}

///////////////////////////////////////////////////////////////
//                                                           //
//  Is Abelian                                               //
//                                                           //
///////////////////////////////////////////////////////////////

bool LIC::hasAbelianStatusBuffer( ) const
{
    if ( abStatus != NULL )
      return true;
    else
      return false;
}

Trichotomy LIC::getAbelianStatus( int i, GIC::AlgorithmID& al, Chars& e) const
{
  Trichotomy ret = abStatus->getStatus( i, al );
  if ( ret != dontknow )
    e = explanation( al );
  return ret;
}

void LIC::setAbelianStatus( int i, Trichotomy is_abelian,GIC::AlgorithmID al )
{
  abStatus->setStatus( i, is_abelian, al );
}

bool LIC::IsAbelianChecked() const
{
  return abStatus->isChecked();
}


///////////////////////////////////////////////////////////////
//                                                           //
//  Is Central                                               //
//                                                           //
///////////////////////////////////////////////////////////////

bool LIC::hasIsCentralStatusBuffer( ) const
{
    if ( centStatus != NULL )
      return true;
    else
      return false;
}

Trichotomy LIC::getIsCentralStatus( int i, GIC::AlgorithmID& al, Chars& e) const
{
  Trichotomy ret = centStatus->getStatus( i, al );
  if ( ret != dontknow )
    e = explanation( al );
  return ret;
}

void LIC::setIsCentralStatus( int i, Trichotomy is_central,GIC::AlgorithmID al )
{
  centStatus->setStatus( i, is_central, al );
}

bool LIC::IsCentralChecked() const
{
  return centStatus->isChecked();
}



Chars LIC::explanation( GIC::AlgorithmID a) const
{
  Chars ret = "";
  if ( a == GIC::NORM_CLOSURE ) {
    ret =  "labels loop in normal closure approximation";
    
  } else if ( a == GIC::AB_INV ) {
    //    if ( theList->getGroup().gic.isAbelian() == yes ) 
    //  ret =  "this follows from the canonical decomposition";
    // else
      ret =  "this follows from an inspection of the "
	"abelianization of the group";   

  } else if ( a == GIC::REWR_SYSTEM ) {
    ret =  "according to the confluent rewriting system";

  } else if ( a == GIC::AUT_STRUCTURE ) {
    ret = "according to the difference machine";
    
 } else if ( a == GIC::GENETIC ) {
    ret =  "according to the genetic algorithm";

 } else if ( a == GIC::FREE ) {
    ret = "";
    
 } else {
   error("Chars SMListWPPrinResultArcer::explanation( GIC::AlgorithmID a) :"
	 " improper algorithm ID.");
 }
  if ( ret.length() > 0 )
    ret +=".";
  return ret;
}


// -------------------- LIC::status_type -------------------- //


//@njz
//LIC::status_type::status_type( int s ):
//  size( s ),
//  status_buffer( NULL )
LIC::status_type::status_type( int arg_s ):
  size( arg_s ),
  status_buffer( NULL )
//
{
#if ( SAFETY > 0 )
  if ( status_buffer != NULL )
    error("LIC::status_type::status_type( int s ):"
	  " buffer has been initialized already");
#endif

  char* tmpFileName = tempnam(MagnusTmp::magnusTmp(),NULL);
  
  status_file = open(tmpFileName,O_RDWR | O_CREAT, S_IWUSR | S_IRUSR  );

  if ( status_file < 0 )
    error("LIC::status_type::status_type( int s ):"
	  "error couldn't open a file.");


  int c = 0;
  char s[1];
  s[0] = '\0';
  for ( int i = 0;i<size ;i++)
    c += write(status_file,s,1);

  free(tmpFileName);
  
  status_buffer = (char*)mmap(0, size, 
			      PROT_READ | PROT_WRITE, MAP_SHARED, 
			      status_file, 0); 
    
  if ( status_buffer < 0 )
    error("LIC::status_type::status_type( int s ) :"
	  "error couldn't map the file.");
}

LIC::status_type::~status_type()
{
  if ( status_buffer != NULL ) {
    munmap(status_buffer, size );
    close( status_file );
  }
}

Trichotomy LIC::status_type::getStatus( int i, GIC::AlgorithmID& al ) const
{
#if ( SAFETY > 0 )
  if ( i < 0 || i >= size ) 
    error(" LIC::status_type::getStatus( int, GIC::AlgorithmID&, Chars& ) :"
	  " index out of bounds");
#endif
  if ( status_buffer[i] == '\0' )
    return dontknow;
  if ( status_buffer[i] > 0 ){
    al = GIC::AlgorithmID( status_buffer[i] );
    return yes;
  } else {
    al = GIC::AlgorithmID( -status_buffer[i] );
    return no;
  }  
}
void LIC::status_type::setStatus( int i, Trichotomy t, 
				  GIC::AlgorithmID al  )
{
 
  if ( t == dontknow )
    return;
  
#if ( SAFETY > 0 )
  if ( i < 0 || i >= size ) 
    error("LIC::status_type::setStatus( ... ):"
	  " index out of bounds");
#endif
  if ( status_buffer[i] == '\0' ){
    char flag = (char )al;
    if ( t == no )
      flag = -flag;
    status_buffer[i] = flag;  
  }  
}
  
bool LIC::status_type::isChecked() const
{
  for (int i = 0; i< size; i++)
    if (status_buffer[i] == '\0' )
      return false;

  return true; 
}

template class SMList<Word>;
template class SMList<Subgroup>;
template class SMList<Map>;

//@njz:
//SMListType<Word> SMList<Word>::theType;
//SMListType<Map> SMList<Map>::theType;
//SMListType<Subgroup> SMList<Subgroup>::theType;
//
//EnProbType<Word> EnumeratorProblem<Word>::theType;
//EnProbType<Map> EnumeratorProblem<Map>::theType;
//EnProbType<Subgroup> EnumeratorProblem<Subgroup>::theType;
//EnProbType<NoType> EnumeratorProblem<NoType>::theType;

template <> SMListType<Word> SMList<Word>::theType = SMListType<Word>();
template <> SMListType<Map> SMList<Map>::theType = SMListType<Map>();
template <> SMListType<Subgroup> SMList<Subgroup>::theType = SMListType<Subgroup>();

template <> EnProbType<Word> EnumeratorProblem<Word>::theType = EnProbType<Word>();
template <> EnProbType<Map> EnumeratorProblem<Map>::theType = EnProbType<Map>();
template <> EnProbType<Subgroup> EnumeratorProblem<Subgroup>::theType = EnProbType<Subgroup>();
template <> EnProbType<NoType> EnumeratorProblem<NoType>::theType = EnProbType<NoType>();
//
