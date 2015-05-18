// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of mixin class FEData
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//


#include "FEData.h"
#include "SMObject.h"


//---------------------------------------------------------------------------//
//-------------------------- FEData::(Reps) ---------------------------------//
//---------------------------------------------------------------------------//


FEData::KeyRep::KeyRep(int i)
  : index( i ), text( 0 ), isConstant( true ), subExpression( 0 )
{ }


FEData::KeyRep::KeyRep(int i, const char* t)
  : index( i ), text( t ), isConstant( false ), subExpression( 0 )
{ }


FEData::KeyRep::KeyRep(OID oid, const char* t)
  : index( oid.unwrap() ), text( t ), isConstant( true ), subExpression( 0 )
{ }


FEData::KeyRep::KeyRep(ExpressionRep* se, const char* t)
  //@njz: initialization of 'index' added
  //  : text( t ), subExpression( se )
  : index( 0 ), text( t ), subExpression( se )
  //
{ }


void FEData::KeyRep::printOn(ostream& ostr) const
{
  if ( subExpression ) {
	 ostr << "$A(";
	 subExpression->printOn( ostr );
	 ostr << ',' << text << ')';
  } else if ( isConstant ) {
	 if ( text == 0 )
		ostr << index;
	 else
		ostr << "$A(" << index << ',' << text << ')';
  } else {
	 if ( text == 0 )
		ostr << "$x" << index;
	 else
		ostr << "$A($x" << index << ',' << text << ')';
  }
}


FEData::JoinRep::JoinRep(FEData::ExpressionRep* a1,
								 FEData::ExpressionRep* a2,
								 const char* j
								 )
  : lhs(a1), rhs(a2), junctor(j)
{ }


FEData::JoinRep::~JoinRep( )
{
  lhs->getHence();
  rhs->getHence();
}
	 

void FEData::JoinRep::printOn(ostream& ostr) const
{
  ostr << '(';
  lhs->printOn( ostr );
  ostr << junctor;
  rhs->printOn( ostr );
  ostr << ')';
}


FEData::NotRep::NotRep(FEData::ExpressionRep* a) : expr(a) { }


FEData::NotRep::~NotRep( )
{
 expr->getHence();
}


void FEData::NotRep::printOn(ostream& ostr) const
{
  ostr << '!';
  expr->printOn( ostr );
}


FEData::NameRep::NameRep(FEData::ExpressionRep* a) : expr(a) { }


FEData::NameRep::~NameRep( )
{
 expr->getHence();
}


void FEData::NameRep::printOn(ostream& ostr) const
{
  ostr << "$A(";
  expr->printOn( ostr );
  ostr << ",name)";
}


//---------------------------------------------------------------------------//
//------------------------ FEData::Expression -------------------------------//
//---------------------------------------------------------------------------//


ostream& operator << ( ostream& ostr, const FEData::Expression& E)
{
  E.theRep->printOn( ostr );
  return ostr;
}


FEData::Expression
FEData::Expression::join(const FEData::Expression& E, const char* junctor) const
{
  return
	 Expression( new JoinRep( theRep->gimme(), E.theRep->gimme(), junctor ) );
}


//---------------------------------------------------------------------------//
//------------------------- FEData::DataPair --------------------------------//
//---------------------------------------------------------------------------//


FEData::DataPair& FEData::DataPair::operator = (const FEData::DataPair& dp)
{
  theKey = dp.theKey;
  theDatum = dp.theDatum;
}


ostream& operator << ( ostream& ostr, const FEData::DataPair& dp)
{
  ostr << "{ key {" << dp.theKey << "} datum {" << dp.theDatum << "}} ";
  return ostr;
}


//---------------------------------------------------------------------------//
//--------------------------- FEData::Text ----------------------------------//
//---------------------------------------------------------------------------//


FEData::Text::Text(const char* t)
{
  ostrstr << t;
}


FEData::Text::Text(const Expression& E)
{
  ostrstr << E;
}
     

FEData::Text::operator Chars( ) const
{
  //@njz
  //  ostrstream& o = (ostrstream&)ostrstr;  // Cast away const-ness.
  std::ostrstream& o = (std::ostrstream&)ostrstr;  // Cast away const-ness.
  //
  o << ends;
  Chars result( o.str() );
  o.freeze(0);
  return result;
}


FEData::Text& FEData::Text::operator + (const FEData::Text& t)
{
  ostrstr << ' ' << t;
  return *this;
}


FEData::Text& FEData::Text::operator + (const FEData::Expression& E)
{
  ostrstr << ' ' << E;
  return *this;
}


ostream& operator << (ostream& ostr, const FEData::Text& t)
{
  //@njz
  //  ostrstream& o = (ostrstream&)t.ostrstr;  // Cast away const-ness.
  std::ostrstream& o = (std::ostrstream&)t.ostrstr;  // Cast away const-ness.
  //
  o << ends;
  ostr << o.str();
  o.freeze(0);
  return ostr;
}


//--------------------------------------------------------------------------//
//-------------------------- FEData::Name ----------------------------------//
//--------------------------------------------------------------------------//

FEData::Name::Name(int i)
  : Expression( new NameRep( Object( i ).theRep->gimme() ) )
{ }


FEData::Name::Name(OID o)
  : Expression( new NameRep( Object( o ).theRep->gimme() ) )
{ }


FEData::Name::Name(const FEData::Expression& E)
  : Expression( new NameRep( E.theRep->gimme() ) )
{ }


// ------------------------- FEData::Link ---------------------------------- //


FEData::Link::Link( const Chars& text, const Chars& problemName,
		    const Chars& fileName, bool isDynamic )
  : Text( Chars("@") + text + "@" + problemName + "@" + fileName + "@"
	  + (isDynamic ? "1" : "0") + "@" )
{ }
