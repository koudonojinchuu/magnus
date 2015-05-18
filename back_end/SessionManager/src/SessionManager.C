// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of class SessionManager
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//
 


//#include "MagnusHome.h"
#include "SessionManager.h"
#include "TheObjects.h"
#include "ComputationManager.h"
#include "Timer.h"
#include "DatabaseManager.h"
#include "RandomDefinitionsGenerator.h"

extern DatabaseManager TheDatabaseManager;

// Need this to init SessionManager::magicCookie :

//@db system porting

#ifdef BSD
extern "C" int getpid( );
#elif defined(SYSV)
extern "C" long int getpid( );
#endif



//---------------------------------------------------------------------------//
//---------------------------- Buffers --------------------------------------//
//---------------------------------------------------------------------------//

// Helper class for entries in a chainable list of char* :

struct Buffers {
  Buffers( ) : buffers( new char*[size] ), index(0), next(0) { }
  ~Buffers( ) {
    for( int i = 0; i < index; ++i ) delete [] buffers[i];
    delete [] buffers;
    delete next;
  }
  Buffers* store(char* p) {
    buffers[index++] = p;
    if ( index == size ) {
      next = new Buffers;
      return next;
    }
    else
      return this;
  }
  void dump(char* p, int bufferSize) {
    for( int i = 0; i < index; ++i ) {
      for( int j = 0; j < bufferSize - 1; ++j )   // Skip \0's
        *p++ = buffers[i][j];
    }
    if ( next != 0 ) next->dump(p, bufferSize);
  }
  static const int size = 128;
  char** buffers;
  int index;
  Buffers* next;
};


//---------------------------------------------------------------------------//
//------------------------- SessionManager ----------------------------------//
//---------------------------------------------------------------------------//


void SessionManager::start( )
{
  initializeFrontEnd();

  while ( !quitRequested ) {

	 readMessages();

	 TheObjects::passControl();

	 takeControl();
  }
}


void SessionManager::readMessages( )
{
  while ( areMessages() ) {

	 // To protect the message stream, we read each message (which consists
	 // of a single line) into a char* and make an istrstream of that.

	 // Get the message:

	 const int bufferSize = 1024;
	 char buffer[bufferSize];        // Don't whip the heap for small messages
	 char* realBuffer = buffer;

	 cin.getline(buffer, bufferSize);
  
	 if ( cin.fail() ) {

		if ( (int)(*buffer) == 0 ) return;
		// When debugging we might redirect input from a file, which results
		// in a fail state when we get to its end. In this case, *buffer == 0

		Buffers* moreBuffers = new Buffers;
		Buffers* curBuffers = moreBuffers;
		int bufferCount = 1;

		do {
		  if ( cin.bad() )
			 error("The front end message stream has gone bad");
		  //@rn Take better emergency action
  
		  cin.clear();
		  char* tmp = new char[bufferSize];
		  cin.getline(tmp, bufferSize);
		  curBuffers = curBuffers->store(tmp);
		  ++bufferCount;
		} while ( cin.fail() );

		realBuffer = new char[bufferSize * bufferCount];
		for( int i = 0; i < bufferSize; ++i ) realBuffer[i] = buffer[i];
		moreBuffers->dump(realBuffer + bufferSize - 1, bufferSize); // Stomp \0
		delete moreBuffers;
	 }

	 // We politely ignore blank messages from the FE:

	 if ( (int)(*buffer) == 0 ) return;

    #ifdef DEBUG
	   cerr << "FrontEnd --> BackEnd :  " << realBuffer << endl << flush;
    #endif

	 //@njz
	 //	 istrstream message(realBuffer);
	 std::istrstream message(realBuffer);
	 //

	 // Process the raw message

	 int recipient;
	 message >> recipient;  //@rn Check for stream going bad

	 if ( recipient < 0 ) {
		// Special case
	  
		if ( recipient == sessionManagerOid )
		  readMessage( message );
		else if ( recipient == objectSmithOid )
		  ObjectSmith::readMessage( message );
		else if ( recipient == databaseManagerOid )
		  TheDatabaseManager.readMessage( message );
		else if ( recipient == randomDefnGeneratorOid ){
		  RandomDefinitionsGenerator::readMessage( message );
		}
		else
		  //@rn Warn of misdirected message
		  ;

	 } else {

		SMObject* smo = TheObjects::get(recipient);
		if ( smo ) {
		  smo->readMessage( message );
		} else {
		  //@rn Warn of misdirected message
		}
	 }

    // Clean up

	 if ( realBuffer != buffer ) delete realBuffer;
	 // Don't do this sooner! message is still using it.
  }
}



bool SessionManager::areMessages( )
{
  static Timer timer( 200 );

  if ( ! timer.expired() ) return false;

  timer.reset( 200 );

  // Can you BELIEVE this kludgery?
  // Of course, main sets stdin to non-blocking.

  int ch = getchar();
  if ( ch == EOF ) {
	 clearerr(stdin);
	 return 0;
  } else {
	 ungetc(ch, stdin);
	 return 1;
  }
}



void SessionManager::readMessage(istream& istr)
{
  MessageTag tag;

  istr >> (int&)tag;

  if ( tag == DELETE ) {

	 // Read list of OIDs to delete. Cumulate the dependency closure.

	 SetOf<OID> deleted;

	 int len, oid;

	 istr >> len;

	 while ( len-- ) {
		istr >> oid;
		deleted |= TheObjects::remove( OID( oid ) );
	 }

	 // Confirm the deletions.

	 cout << "confirm_delete_";

	 SetIterator<OID> I( deleted );
	 while ( !I.done() ) {
		cout << ' ' << I.value();
		I.next();
	 }

	 cout << endl << flush;

  } else if ( tag == QUIT ) {

    //@rn Implement

    if( !quitRequested ) {
      quitRequested = true;
      TheDatabaseManager.forceToFinish();
    }

  //@db experiment
 
  } else if ( tag == SET_NAME ) {
    
    int oid;
    Chars name;
    
    istr >> oid >> name;
    TheObjects::setName(oid,name);

  } else {

	 //@rn Warn of bad tag

  }

  
}

void SessionManager::takeControl( )
{
  TheDatabaseManager.takeControl();

  if( quitRequested ) {

    static bool userWarned = false;

    if( TheDatabaseManager.state() == DatabaseManager::STOPPED ) {

      for( int j = 0; j < TheObjects::theObjectsLength; ++j ) {
	SMObject* smo = TheObjects::theObjects[j];
	if ( smo != 0 && smo->isComputationManager()
	     && ((ComputationManager*)smo)->state()
	     == ComputationManager::RUNNING )
	  ((ComputationManager*)smo)->adminTerminate();
      }
      cout << "fakeQuit" << endl;

    }
    else {

      if( !userWarned ){
	Message msg;
	msg << 
	  "The database is modified. Please save the changes on the disk or "
	  "discard them by closing the database. After that the quitting "
	  "will be completed.";
	msg.send(); 
	userWarned = true;
      }

      if( TheDatabaseManager.state() != DatabaseManager::STOPPING ) {
	quitRequested = false;
      }
    }
  }

}


void SessionManager::initializeFrontEnd( )
{
  //------------------------------ Greeting ---------------------------------//

  cout << "magic_cookie_ " << magicCookie << endl;


  //-------------------------- Global Templates -----------------------------//

  cout
	 << "init_quit_msg_ {" << sessionManagerOid << ' ' << QUIT << '}' << endl

	 << "init_delete_msg_ {" << sessionManagerOid << ' ' << DELETE << '}' << endl

	 << "init_setName_msg_ {" << sessionManagerOid << ' ' << SET_NAME << '}' << endl;

  ComputationManager::printGlobalMessageTemplates( cout );
  SMFPGroup::printGlobalMessageTemplates( cout );
  DatabaseManager::printGlobalMessageTemplate( cout );

  //----------------------------- Type Names --------------------------------//

  cout
	 << "type_name_ {finitely presented group} G " << SMFPGroup::type() << endl
	 << "type_name_ {word} w " << SMWord::type() << endl
	 << "type_name_ {subgroup} H " << SMSubgroup::type() << endl
	 << "type_name_ {map} m " << SMMap::type() << endl
	 << "type_name_ {equation} E " << SMEquation::type() << endl
	 ;


  //------------------------------- Menus -----------------------------------//

  ObjectSmith::outputCheckinMenuDefns( cout );
  ObjectSmith::outputToolsMenuDefns( cout );
  ObjectSmith::outputMakeMenuDefns( cout );
  ObjectSmith::outputPackagesMenuDefns( cout );
  ObjectSmith::outputTestingMenuDefns( cout );

  //----------------------- Properties Registration  ------------------------//

  registerProperties();
  
  //--------------------------- Init Complete -------------------------------//
  
  cout << "boot_done_" << endl << flush;
  
}


//---------------------------------------------------------------------------//
//------------------------- SessionManager ----------------------------------//
//--------------------- static initializations ------------------------------//

const long SessionManager::magicCookie = getpid();
bool SessionManager::quitRequested = false;
