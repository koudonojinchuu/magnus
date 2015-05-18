// Copyright (C) 1998 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Interface of class DataBase State
//
// Principal Author: Dmitry Pechkin
//
// Status: in progress
//
// Revision History:
//


//
//  
// 
#ifndef _DATABASE_MANAGER_H_
#define _DATABASE_MANAGER_H_

#include "Chars.h"
#include "Vector.h"
#include "OID.h"

enum DB2FE_MESSAGE { NO_MESSAGE,        MSG_NEW_FILENAME,  
		     MSG_NEW_DATABASE,  MSG_OPEN_DATABASE,
		     MSG_SAVE_DATABASE, MSG_SAVE_DATABASE_AS,
		     MSG_SAVE_ON_CLOSE, MSG_CLOSE_DATABASE,
		     MSG_ADD_OBJECTS,   MSG_GET_OBJECTS
};

enum FE2DB_MESSAGE { DB_NO_EVENT,
		     DB_NEW, DB_OPEN, DB_SAVE, DB_SAVE_AS, DB_CLOSE, 
		     DB_ADD_OBJECTS, DB_GET_OBJECTS, 
		     DB_CANCEL, DB_SUCCESS, DB_FAILURE, 
		     DB_YES, DB_NO, DB_SELECT_OBJECTS, DB_OBJECT_DEFINITION 
};

struct DBEvent {
  FE2DB_MESSAGE key;
  Chars str;
  DBEvent() : key(DB_NO_EVENT), str() {}
};

ostream& operator<<( ostream& ostr, const DBEvent& event );

class DBState 
{
public:

  // Standard constructors, operator= and destructor.

  DBState( ) : theNextState(0) { }
  // State( const State *nstate, const State *onfailure );

  DBState( const DBState& state );
  const DBState& operator=( const DBState& state );
  virtual ~DBState();

  // Accessors

  virtual void init( ) { }
  //virtual void Event getEvent( ) const = 0;
  virtual void handleEvent( const DBEvent& event ) = 0;

  const DBState* nextState( ) const { return theNextState; }
  virtual DBState* clone() const = 0;

  virtual bool isFinal( ) const = 0;
  virtual DB2FE_MESSAGE initMessage() const { return NO_MESSAGE; }

  virtual void printOn( ostream& ostr ) const { ostr << "DBState"; }
protected:
  //void setNextState( const DBState *nstate ) { theNextState = nstate; }

  //private:
  DBState *theNextState;
  //DBState *theFailure;
};


class MainState : public DBState 
{
public:
  MainState( ) : DBState() { }
  //MainState( const DBState *nstate, const DBState *onFailure );
};

class IntermediateState : public DBState
{
public:
  IntermediateState( ) : theFailure(0), theSecondState(0) { }
  IntermediateState( DBState *onFailure, DBState *secondState )
    : theFailure(onFailure), theSecondState(secondState) {}

  IntermediateState( const IntermediateState& state );
  ~IntermediateState();
  const IntermediateState& operator=( const IntermediateState& state );

  bool isFinal( ) const { return false; }

  void printOn( ostream& ostr ) const { ostr << "IntermediateState"; }
protected:

  void handleFailure();
  void useSecondState( DBState *state );

  DBState *theFailure;
  DBState *theSecondState;
};


//
// User defined states
//

class DatabaseClosed : public MainState
{
public:
  DatabaseClosed() { }
  //DatabaseClosed( DBState *nstate, DBState *onFailure );
  
  bool isFinal( ) const { return true; }

  DB2FE_MESSAGE initMessage() const { return MSG_CLOSE_DATABASE; }
  void handleEvent( const DBEvent& event );
  DBState *clone() const { return new DatabaseClosed(*this); }

  void printOn( ostream& ostr ) const { ostr << "DatabaseClosed"; }
};

class DatabaseSaved : public MainState
{
public:
  DatabaseSaved() { }
  //DatabaseSaved( DBState *nstate, DBState *onFailure );

  bool isFinal( ) const { return true; }

  void handleEvent( const DBEvent& event );
  DBState *clone() const { return new DatabaseSaved(*this); }
  void printOn( ostream& ostr ) const { ostr << "DatabaseSaved"; }
};

class DatabaseModified : public MainState
{
public:
  DatabaseModified() { }
  //DatabaseModified( DBState *nstate, DBState *onFailure );

  bool isFinal( ) const { return false; }

  void handleEvent( const DBEvent& event );
  DBState *clone() const { return new DatabaseModified(*this); }
  void printOn( ostream& ostr ) const { ostr << "DatabaseModified"; }
};




class DatabaseCreating : public IntermediateState
{
public:
  DatabaseCreating() { }
  DatabaseCreating( DBState *onFailure, DBState *secondState = 0 )
    : IntermediateState(onFailure,secondState) {}

  DB2FE_MESSAGE initMessage() const { return MSG_NEW_FILENAME; }
  void handleEvent( const DBEvent& event );
  DBState *clone() const { return new DatabaseCreating(*this); }
  void printOn( ostream& ostr ) const { ostr << "DatabaseCreating"; }
};

class DatabaseOpening : public IntermediateState
{
public:
  DatabaseOpening() { }
  DatabaseOpening( DBState *onFailure, DBState *secondState = 0 )
    : IntermediateState(onFailure,secondState) {}

  DB2FE_MESSAGE initMessage() const { return MSG_OPEN_DATABASE; }
  void handleEvent( const DBEvent& event );
  DBState *clone() const { return new DatabaseOpening(*this); }
  void printOn( ostream& ostr ) const { ostr << "DatabaseOpening"; }
};

class DatabaseSaving : public IntermediateState
{
public:
  DatabaseSaving() { }
  DatabaseSaving( DBState *onFailure, DBState *secondState = 0 )
    : IntermediateState(onFailure,secondState) {}

  DB2FE_MESSAGE initMessage() const { return MSG_SAVE_DATABASE; }
  void handleEvent( const DBEvent& event );
  DBState *clone() const { return new DatabaseSaving(*this); }
  void printOn( ostream& ostr ) const { ostr << "DatabaseSaving"; }
};


class DatabaseSavingAs : public IntermediateState
{
public:
  DatabaseSavingAs() { }
  DatabaseSavingAs( DBState *onFailure, DBState *secondState = 0 )
    : IntermediateState(onFailure,secondState) {}

  DB2FE_MESSAGE initMessage() const { return MSG_SAVE_DATABASE_AS; }
  void handleEvent( const DBEvent& event );
  DBState *clone() const { return new DatabaseSavingAs(*this); }
  void printOn( ostream& ostr ) const { ostr << "DatabaseSavingAs"; }
};

class DatabaseClosing : public IntermediateState
{
public:
  DatabaseClosing() { }
  DatabaseClosing( DBState *onFailure, DBState *secondState = 0 )
    : IntermediateState(onFailure,secondState) {}

  DB2FE_MESSAGE initMessage() const { return MSG_SAVE_ON_CLOSE; }
  void handleEvent( const DBEvent& event );
  DBState *clone() const { return new DatabaseClosing(*this); }
  void printOn( ostream& ostr ) const { ostr << "DatabaseClosing"; }
};

class DatabaseAddingObjects : public IntermediateState
{
public:
  DatabaseAddingObjects() { }
  DatabaseAddingObjects( DBState *onFailure, DBState *secondState = 0 )
    : IntermediateState(onFailure,secondState) {}

  DB2FE_MESSAGE initMessage() const { return MSG_ADD_OBJECTS; }
  void handleEvent( const DBEvent& event );
  DBState *clone() const { return new DatabaseAddingObjects(*this); }
  void printOn( ostream& ostr ) const { ostr << "DatabaseAddingObjects"; }
};

class DatabaseGettingObjects : public IntermediateState
{
public:
  DatabaseGettingObjects() { }
  DatabaseGettingObjects( DBState *onFailure, DBState *secondState = 0)
    : IntermediateState(onFailure,secondState) {}

  DB2FE_MESSAGE initMessage() const { return MSG_GET_OBJECTS; }
  void handleEvent( const DBEvent& event );
  DBState *clone() const { return new DatabaseGettingObjects(*this); }
  void printOn( ostream& ostr ) const { ostr << "DatabaseGettingObjects"; }
};


inline DBState *copyState( const DBState *state )
{
  return (state ? state->clone() : 0 );
}

inline ostream& operator<<(ostream& ostr, const DBState& state)
{
  state.printOn( ostr );
  return ostr;
}


//
//  Class DatabaseObjectType
//

class DatabaseObjectCategory {
public:
  enum Category
  {
    NO_CATEGORY = -1, 
    // Group types -- 
    FP, FREE, ABELIAN, NILPOTENT, SMALL_CANCELLATION, ONE_RELATOR,
    AP_FREE, AP_FREE_CYCLIC, FREE_NILPOTENT, HNN_FREE, 
    ONE_RELATOR_WITH_TORSION,
    // Other object types
    SUBGROUP, WORD, MAP, MAP2, HOMOMORPHISM, HOMOMORPHISM2,
    EQUATION, EQUATION2, SET_OF_WORDS, VECTOR_OF_WORDS, 
  };

  DatabaseObjectCategory(Category cath = NO_CATEGORY) :theCategory(cath) {}

  Chars str() const { return DatabaseObjectCategory::str(theCategory); }
  Category id() const { return theCategory; }

  static Chars str(const Category cath);

  static Chars getSMObjectCategory( class SMObject *smo );


  friend istream& operator>>(istream& istr, DatabaseObjectCategory& cath ) {
    int i;
    istr >> i;
    cath.theCategory = (DatabaseObjectCategory::Category)i;
  }

  friend ostream& operator<<(ostream& ostr, 
			     const DatabaseObjectCategory& cath ) {
    ostr << (int)cath.theCategory;
  }

private:
  Category theCategory;
};


//
//  Class DatabaseObjectSmith
//

class SMObject;

class DatabaseObjectSmith {
public:
  // Default ctor, copy ctor, operator=, dtor supplied by compiler.

  void checkinObjectFromDatabase(istream& istr);

private:

  void reset();
  void checkinObjectProperties( istream& istr );
  void checkinObjectDefinition( istream& istr );

  SMObject* checkinFPGroup(istream& istr, DatabaseObjectCategory type );
  SMObject* checkinWord(istream& istr);
  SMObject* checkinSubgroup(istream& istr);
  SMObject* checkinSetOfWords(istream& istr);
  SMObject* checkinVectorOfWords(istream& istr);
  SMObject* checkinMap(istream& istr, DatabaseObjectCategory type);
  SMObject* checkinHomomorphism(istream& istr, DatabaseObjectCategory type);
  SMObject* checkinEquation(istream& istr, DatabaseObjectCategory type);

  void parseError(const Chars& str) { theError = str; }

  // data members:

  Chars theError;
  Chars theObjectName;
  VectorOf<SMObject *> theExperiment;
  ListOf<OID> dependencies;
};

// 
//   Class DatabaseManager
//

class DatabaseManager 
{
public:

  DatabaseManager(); //( const Chars& filename );

  ~DatabaseManager( );

  void takeControl( );

  enum ManagerState { WORKING, STOPPING, STOPPED };

  ManagerState state() const { return theExternalState; }

  Chars getFileName() const { return theFileName; } 
  Chars getSecondaryFileName() const;

  static void printGlobalMessageTemplate( ostream& ostr );

  //private:
  //friend void SessionManager::readMessage(istream& istr);
  void forceToFinish();

  //private:
  //friend void SessionManager::readMessages( );
  void readMessage( istream& istr );

private:
  friend void DatabaseCreating::handleEvent( const DBEvent& event );
  friend void DatabaseSaving::handleEvent( const DBEvent& event );
  friend void DatabaseSavingAs::handleEvent( const DBEvent& event );
  void setFileName( const Chars& filename );

private:
  DatabaseManager( const DatabaseManager& );
  // Hidden, not to implement.

  DatabaseManager& operator=( const DatabaseManager& );
  // Hidden, not to implement.

  void postMessage( const DB2FE_MESSAGE message ) const;

  //static Chars getObjectCategory( SMObject *smo ); 
  static void printDatabaseObjectCategories( ostream& ostr );

  // Data members

  DBState *theCurrentState;
  Chars  theFileName;
  ManagerState theExternalState;
  DBEvent event;
};

extern DatabaseManager TheDatabaseManager;
#endif
