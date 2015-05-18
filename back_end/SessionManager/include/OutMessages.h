// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declarations of class OutMessage, and derivatives
//           ARCUpdate, CheckinMessage, FEDataUpdate, LogMessage,
//           ParseErrorMessage, StateTransition, Warning
//
// Principal Author: Roger Needham
//
// Status: useable
//
// Revision History:
//
// 01/25/98 @am added ParseParamErrorMessage class. It reports to
// the front end about parsing errors in parameters.
//
// 01/27/2000 @am added PackageInfoMessage class. 

#ifndef _OUTMESSAGES_H_
#define _OUTMESSAGES_H_

#include <stdio.h>
#include <iostream.h>
#include "Chars.h"
#include "List.h"
#include "ARC.h"
#include "ARCSlotID.h"
#include "FEData.h"
#include "VectorPtr.h"

//---------------------------------------------------------------------------//
//--------------------------- OutMessage ------------------------------------//
//---------------------------------------------------------------------------//

// At some point we should use a hidden ostream, not cout.

class OutMessage
{
public:

  void send( ) const;

protected:

  OutMessage( ) { } //@rn gcc 2.6.3 claims I need this...

  virtual void print(ostream& ostr) const = 0;
  // Used by class SessionManager.

private:

  OutMessage( const OutMessage& );
  // Hidden, not to be implemented.
};


//---------------------------------------------------------------------------//
//--------------------------- LogMessage ------------------------------------//
//---------------------------------------------------------------------------//

//@njz
//class LogMessage : public OutMessage, protected FEData, public ostrstream
class LogMessage : public OutMessage, protected FEData, public std::ostrstream
//
{
public:

  // Do *NOT* mess with the ostrstream directly, unless you like debugging.
  // It is imported publicly only to get the existing << operators.

  // The following ctors initialize a message to the logs of the views of
  // the indicated OIDs (no OID means the session log only).
  // message_level == 1 means the message should be echoed to the session log;
  // message_level == 2 means the message is only for the addressed log.

  LogMessage(int message_level = 1);

  LogMessage(OID o, int message_level = 1);

  LogMessage(OID o1, OID o2, int message_level = 1);

  LogMessage(OID o1, OID o2, OID o3, int message_level = 1);

  LogMessage( const char* msg, int message_level = 1);

  LogMessage(OID o, const char* msg, int message_level = 1);

protected:

  void print(ostream& ostr) const;  // overrides OutMessage

private:

  int num_addressees;

  VectorPtrOf<OID> oids; //[3];
  
  int level;
};


//---------------------------------------------------------------------------//
//------------------------ ParseErrorMessage --------------------------------//
//---------------------------------------------------------------------------//


class ParseErrorMessage : public OutMessage
{
public:

  ParseErrorMessage(const Chars name, const Chars diagnosis);
  // The FE requires that `diagnosis' start with an int indicating the
  // position of the error, followed by a ' '. Use 0 if no other value
  // is appropriate.

protected:

  void print(ostream& ostr) const;  // overrides OutMessage

private:

  const Chars theName;
  const Chars theDiagnosis;
};

//---------------------------------------------------------------------------//
//------------------------ PackageInfoMessage ---------------------------//
//---------------------------------------------------------------------------//

class PackageInfoMessage : public OutMessage
{
public:
  
  PackageInfoMessage(const Chars& name, const Chars& command, 
                     const Chars& ch_type, const Chars& obj);
protected:
  
  void print(ostream& ostr) const;  // overrides OutMessage
  
private:
  
  const Chars theChType;
  const Chars theName;
  const Chars theObject;
  const Chars theCommand;
 
};

//---------------------------------------------------------------------------//
//------------------------ ParseParamErrorMessage ---------------------------//
//---------------------------------------------------------------------------//

class ParseParamErrorMessage : public OutMessage
{
public:
  
  ParseParamErrorMessage(OID o,const Chars& name, 
			 const Chars& type,
			 const Chars& msg);
protected:
  
  void print(ostream& ostr) const;  // overrides OutMessage
  
private:
  
  OID theOID;
  const Chars theType;
  const Chars theName;
  const Chars theMsg;
  
};

//---------------------------------------------------------------------------//
//------------------------ ParseParamOk -------------------------------------//
//---------------------------------------------------------------------------//

class ParseParamOk : public ParseParamErrorMessage
{
public:
  
  ParseParamOk(OID o):
    ParseParamErrorMessage(o,"Parameters","","none") { }
};

//---------------------------------------------------------------------------//
//------------------------- CheckinMessage ----------------------------------//
//---------------------------------------------------------------------------//

class CheckinMessage : public OutMessage
{
public:

  CheckinMessage(const class SMObject& smo,
		 const Chars name,
		 const ListOf<OID> dependencies,
		 bool bPrintDefinition = true
		 );

  // Copy ctor and dtor supplied by compiler

protected:

  void print(ostream& ostr) const;  // overrides OutMessage

private:

  const class SMObject& theSMO;
  const Chars theName;
  const ListOf<OID> theDependencies;
  bool BPrintDefinition;

};


//---------------------------------------------------------------------------//
//-------------------------- FEDataUpdate -----------------------------------//
//---------------------------------------------------------------------------//

class FEDataUpdate : public OutMessage, protected FEData
{
public:
  
  FEDataUpdate( const Key& key, const Expression& datum )
    : theKey( key ), theDatum( datum )
  { }

protected:
  
  void print(ostream& ostr) const;  // overrides OutMessage

private:

  const Key theKey;
  const Expression theDatum;
};


//---------------------------------------------------------------------------//
//------------------------- StateTransition ---------------------------------//
//---------------------------------------------------------------------------//

class StateTransition : public OutMessage
{
public:

  enum Kind { START, SUSPEND, RESUME, TERMINATE, STALLED };

  StateTransition(Kind k, OID o) : kind( k ), oid( o ) { }

protected:
  
  void print(ostream& ostr) const;  // overrides OutMessage

private:

  const Kind kind;
  const OID oid;
};


//---------------------------------------------------------------------------//
//---------------------------- ARCUpdate ------------------------------------//
//---------------------------------------------------------------------------//

class ARCUpdate : public OutMessage
{
public:

  ARCUpdate(OID o, ARCSlotID a, ARC v) : oid( o ), asi( a ), value( v ) { }

protected:
  
  void print(ostream& ostr) const;  // overrides OutMessage

private:

  OID oid;
  ARCSlotID asi;
  ARC value;

};


//---------------------------------------------------------------------------//
//----------------------------- Warning -------------------------------------//
//---------------------------------------------------------------------------//

//@njz
//class Warning : public OutMessage, protected FEData, public ostrstream
class Warning : public OutMessage, protected FEData, public std::ostrstream
//
{
  // Do *NOT* mess with the ostrstream directly, unless you like debugging.
  // It is imported publicly only to get the existing << operators.

protected:
  
  void print(ostream& ostr) const;  // overrides OutMessage

};


//----------------------------- Message -------------------------------------//


//@njz
//class Message : public OutMessage, protected FEData, public ostrstream
class Message : public OutMessage, protected FEData, public std::ostrstream
//
{

public:
  
  Message( Chars title = "Information" ) : theTitle( title ) { }

protected:
  
  void print(ostream& ostr) const;  // overrides OutMessage

private:

  Chars theTitle;
};

//-------------------------- InvokingMessage --------------------------------//


//@njz
//class InvokingMessage : public OutMessage, protected FEData, public ostrstream
class InvokingMessage : public OutMessage, protected FEData, public std::ostrstream
//
{

public:
  
  InvokingMessage( ) { }

protected:
  
  void print(ostream& ostr) const;  // overrides OutMessage
};


#endif
