// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of class SMEquation
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
#include "MagnusHome.h"
#include "SMListIterator.h"
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

bool SUSPEND_SIGNAL_RECEIVED = false;

#ifdef BSD
extern "C"
void suspend_signal_handler(int sig, int code, struct sigcontext* scp,
			     char* addr)
#else
extern "C"
void suspend_signal_handler(int sig)
#endif

{
  if (sig == SIGUSR1)
    SUSPEND_SIGNAL_RECEIVED = true;
}

const Type NoType::theNoType = Type( Type::unique() );


//--------------------------------------------------------------------------//
//--------------------------- SMListData  ----------------------------------//
//--------------------------------------------------------------------------//

SMListData::SMListData( ): number_of_elements(0), mode( -1 ) 
{ 
  // get name for the data file
  char* tmpFileName = tempnam(MagnusTmp::magnusTmp(),NULL);
  dataFileName = Chars( tmpFileName );
  
  // makes an empty color file for front end maybe needs to be changed
  //const char* destColor = Chars( dataFileName ) + Chars(".color");
  Chars destColor = Chars( tmpFileName ) + Chars(".color");
  ofstream outColor( destColor );
  outColor << "#0000000%000@" << flush;
  outColor.close();
  free(tmpFileName);

  // get name for the referencies file
  tmpFileName = tempnam(MagnusTmp::magnusTmp(),NULL);
  refFileName = Chars( tmpFileName );
  free(tmpFileName);
  

}

// operator for adding elements
SMListData& SMListData::operator << (const WriteEnumeratorElement& e)
{
#if SAFETY > 0
  if ( mode != ios::out )
    error("SMListData& operator << (const WriteEnumeratorElement& e) :"
	  " was not opened for read acces.");
#endif    
  number_of_elements++;
  
  dataFile << number_of_elements << ": ";
  // save begining position of an element
  char f_pos[10];
  long beg_pos = dataFile.tellg();
  sprintf( f_pos, "%.10d",beg_pos );
  refFile << f_pos << flush;

  // write element
  dataFile << e;

  // save element length
  char e_size[5];
  //@njz
  //  sprintf(e_size, "%.5d",dataFile.tellg() - beg_pos );
  sprintf(e_size, "%.5d",((int)dataFile.tellg()) - beg_pos );
  //
  refFile << e_size << flush;

  dataFile << flush;
  return *this;
}

SMListData&  SMListData::operator << (const Chars& c)
{
#if SAFETY > 0
  if ( mode != ios::out )
    error("SMListData& operator << (const Chars& c) :"
	  " was not opened for read acces.");
#endif    

  dataFile << c;
  return *this;
}


  // returns i-th element as a character. 
  // Iterator must parse it to the corresponding data type
Chars SMListData::getElementOf( int i )
{
  int beg_pos = getBegPosOf( i );
  int size = getLengthOf( i );

  Chars ret;
  
  char* el = new char[size + 1];

  dataFile.seekg( beg_pos );
  dataFile.read( el, size );

  el[size] = '\0';
  ret = Chars( el );
  delete [] el;
  return ret;   
}


  // appends a list to the end 
void SMListData::append( SMListData& sd )
{
#if SAFETY > 0
  if ( mode != ios::out )
    error( "SMListData::append( ... ) : "
	   " was not opened for write access." );  
#endif   
  int n = sd.numberOfElements();
  for (int i = 0;i < n; i++ ){
    number_of_elements++;
    
    dataFile << number_of_elements << ": ";
    // save begining position of an element
    char f_pos[10];
    long beg_pos = dataFile.tellg();
    sprintf( f_pos, "%.10d",beg_pos );
    refFile << f_pos << flush;
    
    // write element
    dataFile << sd.getElementOf( i );
    
    // save element length
    char e_size[5];
    //@njz
    //    sprintf(e_size, "%.5d",dataFile.tellg() - beg_pos );
    sprintf(e_size, "%.5d",((int)dataFile.tellg()) - beg_pos );
    //
    refFile << e_size << flush;

  }
}

void SMListData::openFiles( int m )
{
  mode = m;
  //@njz: 
  //  dataFile.open(dataFileName,mode);
  dataFile.open(dataFileName,(ios::openmode)mode);
  //
#if SAFETY > 0
  if( !dataFile )
      error( "SMListData::openFiles( int ) : "
	     "Cannot open the data file." );
#endif    

  //@njz: 
  //  refFile.open(refFileName,mode);
  refFile.open(refFileName,(ios::openmode)mode);
  //
#if SAFETY > 0
  if( !refFile )
      error( "SMListData::openFiles( int ) : "
	     "Cannot open the referencies file." );
#endif 
}

void SMListData::setWriteMode()
{
  closeCurrentMode();
  openFiles( ios::out );
}
void SMListData::setReadMode()
{
  closeCurrentMode();
  openFiles( ios::in );  
}
void SMListData::closeCurrentMode()
{
  //@njz: check if still open? -TODO
  //
  dataFile.close();
  refFile.close();
  mode = -1;
}

SMListData::SMListData( const SMListData& sd ):
  number_of_elements( sd.number_of_elements ), 
  mode( -1 ),
  dataFileName( sd.dataFileName ),
  refFileName( sd.refFileName )
{
  
}


void SMListData::doHardCopy( const SMListData& sd )
{
  closeCurrentMode();

  number_of_elements = sd.number_of_elements;

  // copy the file with enumerations
  char* tmpFileName = tempnam(MagnusTmp::magnusTmp(),NULL);
  Chars fName = sd.dataFileName;
  const char* tmpStr = fName;
  int pid;
  
  if ( ( pid = fork() ) == 0 ) {
    setpgid(0,0);
    
    if ( execlp( "cp", "",tmpStr ,tmpFileName, NULL) == -1 )
      error("SMListData( const SMListData& sd, int m = ios::in ):"
	    " can't create a copy of the data file.");
    
    _exit(0);
  }
  
  while( kill(pid, 0) == 0 ) {
    sleep(100); 
  }
  
#ifdef BSD    
  int status;
  wait(&status);
#endif
  
  // make an empty color file
  //const char* destColor = Chars( tmpFileName ) + Chars(".color");
  Chars destColor = Chars( tmpFileName ) + Chars(".color");
  ofstream outColor( destColor );
  outColor << "#0000000%000@" << flush;
  outColor.close();
  
  // save the name of the data file
  dataFileName = Chars( tmpFileName );
  free( tmpFileName );
  // Copy references file
  tmpFileName = tempnam(MagnusTmp::magnusTmp(),NULL);
  fName = sd.refFileName;
  tmpStr = fName;
  
  if ( ( pid = fork() ) == 0 ) {
    setpgid(0,0);
    
    if ( execlp( "cp", "",tmpStr ,tmpFileName, NULL) == -1 )
      error("SMListData( const SMListData& sd, int m = ios::in ):"
	    " can't create a copy of the offset file.");
    
    _exit(0);
  }
  
  while( kill(pid, 0) == 0 ) { sleep(10); }
  
#ifdef BSD    
  status;
  wait(&status);
#endif
  // save the name of the offset file
  refFileName = Chars( tmpFileName );
  free( tmpFileName );
}
  
  // gives offset of the i-th element 
int SMListData::getBegPosOf( int i)
{
#if SAFETY > 0
  if ( mode != ios::in )
    error( "SMListData::getBegPosOf( int ) : "
	   " was not opened for read access." );      
  if( i < 0 || i >= number_of_elements )
    error( "SMListData::getBegPosOf( int ) : "
	   " wrong index." );
#endif    
  
  refFile.seekg( i*10 + i*5 );
  char str_pos[11]; 
  refFile.read( str_pos, 10 );
  str_pos[10] = '\0';
  int f_pos = atoi( str_pos );
  return f_pos;
}


  // returns the length of an i-th element
int SMListData::getLengthOf( int i)
{
#if SAFETY > 0
  if ( mode != ios::in )
    error( "SMListData::getLengthOf( int ) : "
	   " was not opened for read access." );      
  if( i < 0 || i >= number_of_elements )
    error( "SMListData::getLengthOf( int ) : "
	   " wrong index." );
#endif    
  
  refFile.seekg( i*10 + i*5 + 10 );
  char str_pos[6];  
  refFile.read( str_pos, 5 );
  str_pos[5] = '\0';
  int size = atoi( str_pos );
  return size;
}


//--------------------------------------------------------------------------//
//----------------------- EnumeratorARCer ----------------------------------//
//--------------------------------------------------------------------------//

void EnumeratorARCer::runComputation( )
{
  SUSPEND_SIGNAL_RECEIVED = false;
  if (signal( SIGUSR1, suspend_signal_handler) == SIG_ERR)
    error("void EnumeratorARCer::runComputation( ) :"
	  "Can`t catch signal");
  enumerate();
}

bool EnumeratorARCer::submitSignalRecieved()const
{
  return  SUSPEND_SIGNAL_RECEIVED;
}

void EnumeratorARCer::submitSignal( )
{
  if( pid < 0 ) return;
  if (bInterrupted == true)
    kill(-pid, SIGCONT);
  kill(-pid, SIGUSR1);

}
/*
//---------------------------------------------------------------------------//
//-------------------------- SMEnumerator -----------------------------------//
//---------------------------------------------------------------------------//

template <class T> 
SMEnumerator<T>::SMEnumerator(AlgebraicObject& P, const Chars heritage)
  : AlgebraicObject( heritage ),
    theParent( P )

{ }

template <class T> 
SMEnumerator<T>::~SMEnumerator() 
{
}
template <class T> 
void SMEnumerator<T>::printProperties(ostream& ostr) const
{
  ostr << DataPair( Parent( oid() ), Object( theParent ) );
}


template <class T> 
void SMEnumerator<T>::viewStructure(ostream& ostr) const
{
  ostr
    << "viewBuilder objectView viewParameters {viewID "
    << oid()
    << " heritage {"
    << heritage()
    << "}}";
}

template <class T> 
void SMEnumerator<T>::printDefinition(ostream& ostr) const
{

}

template class SMEnumerator<Word>;
template class SMEnumerator<Map>;
template class SMEnumerator<NoType>;
*/
//---------------------------------------------------------------------------//
//-------------------------- EnumeratorProblem ------------------------------//
//---------------------------------------------------------------------------//



