// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes Ctor<n>, Menu<n>
//
// Principal Author: Roger Needham, Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//
// * 11/10/96 EP added stuff for SMVectorOfWords
//
// * 96/10/09 @dp added ReadFreeGetNextN_thElementItem::construct(SMObject*,
//     Chars&,istream&).
//   96/10/10 @dp added ReadTerminalSegmentItem::construct(SMObject*,Chars&,
//     istream&).
//   96/10/10 @dp added ReadSegmentOfWordItem::construct(SMObject*,Chars&,
//     istream&).
//   void Menu::DefineInt2::print(ostream& ostr) const
//   //@db temporary implementation


#include "Chars.h"
#include "Menu.h"
#include "ObjectSmith.h"
#include "SessionManager.h"
#include "TheObjects.h"
#include "OutMessages.h"
#include "EquationParser.h"
#include "EqSystemParser.h"
#include "Permutation.h"

// Includes required for special cases:

#include "TheObjects.h"
#include "SMFPGroup.h"
#include "SMWord.h"
#include "SMEquation.h"
#include "SMSubgroup.h"
#include "SMMagnusBreakdown.h"
#include "SMMap.h"
#include "fastProblems.h"
#include "HomologyProblem.h"
#include "Type.h"
#include "PackagesData.h"
#include "ObjectFactory.h"

//---------------------------------------------------------------------------//
//------------------------------ Ctor<n> ------------------------------------//
//---------------------------------------------------------------------------//


void Ctor::newDependent(SMObject* smo, OID oid) const
{
  //@db smo->addDependent( oid );
  // addDependent() is now called by CheckinMessage 
}


SMObject* Ctor::get(int oid) const
{
  return TheObjects::get( OID(oid) );
}


void CtorAux0::readFrom(istream& istr) const {
  Chars name;

  SMObject* smo = construct( name, istr );
  if ( smo ) {
    if ( smo->displayInFE() ) {
      ListOf<OID> dependencies;
      CheckinMessage( *smo, name, dependencies ).send();
    }
  }
}


void CtorAux1::readFrom(istream& istr) const
{
  int i;

  istr >> i;
  SMObject* arg1 = get( i );
  if ( arg1 == 0 ) return;
  Chars name;

  SMObject* smo = construct( arg1, name, istr );
  if ( smo ) {
    newDependent( arg1, *smo );
    if ( smo->displayInFE() ) {
      ListOf<OID> dependencies( *arg1 );
      CheckinMessage( *smo, name, dependencies ).send();
    }
    //@rn Factor out somewhere
  }
}


void CtorAux2::readFrom(istream& istr) const
{
  int i;

  istr >> i;
  SMObject* arg1 = get( i );
  if ( arg1 == 0 ) return;
  istr >> i;
  SMObject* arg2 = get( i );
  if ( arg2 == 0 ) return;
  Chars name;
  SMObject* smo;
  if ( transposeARGS ) smo = construct( arg2, arg1, name, istr );
  else smo = construct( arg1, arg2, name, istr );
  if ( smo ) {
    newDependent( arg1, *smo );
    newDependent( arg2, *smo );
    if ( smo->displayInFE() ) {
      ListOf<OID> dependencies( *arg1 );
      dependencies.append( *arg2 );
      CheckinMessage( *smo, name, dependencies ).send();
    }
    //@rn Factor out somewhere
  }
}


void CtorAux3::readFrom(istream& istr) const
{
  int i;

  istr >> i;
  SMObject* arg1 = get( i );
  if ( arg1 == 0 ) return;
  istr >> i;
  SMObject* arg2 = get( i );
  if ( arg2 == 0 ) return;
  istr >> i;
  SMObject* arg3 = get( i );
  if ( arg3 == 0 ) return;
  Chars name;
  SMObject* smo = construct( arg1, arg2, arg3, name, istr );
  if ( smo ) {
    newDependent( arg1, *smo );
    newDependent( arg2, *smo );
    newDependent( arg3, *smo );
    if ( smo->displayInFE() ) {
      ListOf<OID> dependencies( *arg1 );
      dependencies.append( *arg2 );
      dependencies.append( *arg3 );
      CheckinMessage( *smo, name, dependencies ).send();
    }
    //@rn Factor out somewhere
  }
}


void CtorAux4::readFrom(istream& istr) const
{
  int i;

  istr >> i;
  SMObject* arg1 = get( i );
  if ( arg1 == 0 ) return;
  istr >> i;
  SMObject* arg2 = get( i );
  if ( arg2 == 0 ) return;
  istr >> i;
  SMObject* arg3 = get( i );
  if ( arg3 == 0 ) return;
  istr >> i;
  SMObject* arg4 = get( i );
  if ( arg4 == 0 ) return;
  Chars name;
  SMObject* smo = construct( arg1, arg2, arg3, arg4, name, istr );
  if ( smo ) {
    newDependent( arg1, *smo );
    newDependent( arg2, *smo );
    newDependent( arg3, *smo );
    newDependent( arg4, *smo );
    if ( smo->displayInFE() ) {
      ListOf<OID> dependencies( *arg1 );
      dependencies.append( *arg2 );
      dependencies.append( *arg3 );
      dependencies.append( *arg4 );
      CheckinMessage( *smo, name, dependencies ).send();
    }
    //@rn Factor out somewhere
  }
}



//---------------------------------------------------------------------------//
//------------------------------- Ctor --------------------------------------//
//--------------------------- special cases ---------------------------------//


SMObject* ReadSMFPGroup::construct(Chars& name, istream& istr) const
{
  istr >> name;
  FPGroup G;
  Chars err = istr >> G;
  if ( err.length() == 0 ) {
    if( G.getRelators().cardinality() == 0 )
      return new SMFPGroup( G, "User defined free group", SMFPGroup::FREE );
    if ( G.getRelators().cardinality() == 1 )
      return new SMFPGroup( G, "User defined one relator group",
			    SMFPGroup::ONE_RELATOR );
    return new SMFPGroup( G, "User defined finitely presented group" );
  } else {
    ParseErrorMessage( name, err ).send();
    return 0;
  }
}


SMObject* ReadSMFreeGroup::construct(Chars& name, istream& istr) const
{

  istr >> name;

  FreeGroup G;
  Chars err = istr >> G;
  if ( err.length() == 0 ) {
    return new SMFPGroup( FPGroup( G.namesOfGenerators() ),
			  "User defined free group", SMFPGroup::FREE );
  } else {
    ParseErrorMessage( name, err ).send();
    return 0;
  }
}


SMObject* ReadSMAbelianGroup::construct(Chars& name, istream& istr) const
{
  istr >> name;
  FPGroup G;
  Chars err = istr >> G;
  if ( err.length() == 0 ) {
    SMFPGroup* smo = new SMFPGroup( G, "User defined abelian group", 
				    SMFPGroup::ABELIAN );
    smo->gic.putIsAbelian();
    return smo;
  } else {
    ParseErrorMessage( name, err ).send();
    return 0;
  }
}


SMObject* ReadSMNilpotentGroup::construct(Chars& name, istream& istr) const
{
  istr >> name;
  int theClass;
  istr >> theClass;
  FPGroup G;
  Chars err = istr >> G;
  if ( err.length() == 0 ) {
    if( G.getRelators().cardinality() > 0 ){
      SMFPGroup* smo = new SMFPGroup( G, "User defined nilpotent group", 
				      SMFPGroup::NILPOTENT );
      smo->gic.putIsNilpotent( theClass );
      return smo;
    } else {
      SMFPGroup* smo = new SMFPGroup( FPGroup(G.namesOfGenerators()),
				      "User defined free nilpotent group",
				      SMFPGroup::FREE_NILPOTENT );
      FreeGroup FG(G.namesOfGenerators());
      smo->gic.putIsFreeNilpotent( FG, theClass );
      return smo;
    }
  } else {
    ParseErrorMessage( name, err ).send();
    return 0;
  }
}

SMObject* ReadSMFreeNilpotentGroup::construct(Chars& name, istream& istr) const
{
  istr >> name;
  int theClass;
  istr >> theClass;
  FreeGroup G;
  Chars err = istr >> G;
  if ( err.length() == 0 ) {
    SMFPGroup* smo = new SMFPGroup( FPGroup(G.namesOfGenerators()),
				    "User defined free nilpotent group",
				    SMFPGroup::FREE_NILPOTENT );
    smo->gic.putIsFreeNilpotent( G, theClass );
    return smo;
  } else {
    ParseErrorMessage( name, err ).send();
    return 0;
  }
}

SMObject* ReadSMORGroup::construct(Chars& name, istream& istr) const
{
  istr >> name;
  FPGroup G;
  Chars err = istr >> G;
  if ( err.length() == 0 ) {
    if ( G.getRelators().cardinality() == 1 ) {
      SetIterator<Word> I = G.getRelators();
      Word r = I.value().cyclicallyReduce();
      if( r.isProperPower() ) 
	return new SMFPGroup( G, "User defined one relator group with torsion",
			      SMFPGroup::ONE_RELATOR_WITH_TORSION );
      else return new SMFPGroup( G, "User defined one relator group",
				 SMFPGroup::ONE_RELATOR );
    } else {
      ParseErrorMessage(name,"0 {the presentation is not one relator}").send();
      return 0;		
    }
  } else {
    ParseErrorMessage( name, err ).send();
    return 0;
  }
}


SMObject* ReadSMSmallCancGroup::construct(Chars& name, istream& istr) const
{
  istr >> name;
  FPGroup G;
  Chars err = istr >> G;
  if ( err.length() == 0 ) {
    int lambda = G.cancellationLambda();
    if ( lambda < 6 && lambda != 0 ) {
      //@njz
      //      ostrstream o;
      std::ostrstream o;
      //
      o << "0 {This group is only C'(1/" << lambda << ").}" << ends;
      ParseErrorMessage( name, o.str() ).send();
      o.freeze(0);
      //@rn If message is queued, `freeze' allows str to be deleted.
      return 0;
    } else {
      //@njz
      //      ostrstream o;
      std::ostrstream o;
      //
      if ( lambda == 0 ) o << "User defined C'(0) group" << ends;
      else o << "User defined C'(1/" << lambda << ") group" << ends;
      SMFPGroup* smo = new SMFPGroup(G,o.str(),SMFPGroup::SMALL_CANCELLATION);
      o.freeze(0);
      smo->gic.putHaveMSC( G, lambda );
      return smo;
    }
  } else {
    ParseErrorMessage( name, err ).send();
    return 0;
  }
}

SMObject* ReadSMPermutation::construct( Chars& name, istream& istr ) const
{
  Permutation p;
  istr >> name;
  Chars errMsg = istr >> p;
  if( errMsg.length() == 0 )
    return new SMPermutation( p, "User defined permutation" );
  else {
    ParseErrorMessage( name, Chars(0) + " {" + errMsg + ".}" ).send();
    //    ParseErrorMessage( name, Chars(P.cursorPos()) 
    //+ " {" + errMsg + ".}" ).send();
    return 0;
  }
}

SMObject* ReadSMWord::construct(SMObject* a1, Chars& name, istream& istr) const
{
  SMFPGroup& parent = *(SMFPGroup*)a1;
  istr >> name;
  FGGroup G = parent.getFPGroup();
  SetOf<OID> S = TheObjects::dependencyClosure(parent);
  SetOf<OID> O;
  SetIterator<OID> I(S);
  for(; !I.done(); I.next()) {
    SMObject* smo = TheObjects::get(OID(I.value()));
    if( Chars(smo->typeID()) == Chars("SMWord")) O |= I.value();
  }
  VectorOf<OID> oids = makeVectorOf(O);
  int newNamesLen = oids.length();
  VectorOf<Chars> newNames(newNamesLen);
  for( int i = 0; i < newNamesLen; ++i )
    newNames[i] = TheObjects::name(oids[i]);
  VectorOf<Chars> names = concatenate(G.namesOfGenerators(), newNames);
  FreeGroup F(names);
  Word w;
  Chars err;
  w = F.readWord(istr, err);
  if ( err.length() == 0 ) {
    int numOfOldGens = G.numberOfGenerators();
    int vLen = names.length();
    VectorOf<Word> v(vLen);
    for( int i = 0; i < numOfOldGens; ++i )  v[i] = Word(Generator(i+1));
    for( int i = numOfOldGens; i < vLen; ++i )
      v[i] = ((SMWord*)(TheObjects::get(oids[i-numOfOldGens])))->getWord();
    Map M(F,G,v);
    w = M.imageOf(w);
    return new SMWord( parent,w,Text("User defined word in the generators of")
		       + Name( parent ) );
  } else {
    ParseErrorMessage( name, err ).send();
    return 0;
  }
}


SMObject*
ReadSMEquation::construct(SMObject* a1, Chars& name, istream& istr) const
{
  //@rn This expects the equation to be quadratic.
  
  SMFPGroup& parent = *(SMFPGroup*)a1;
  istr >> name;
  FGGroup G = parent.getFPGroup();
  EquationParser P( istr );
  VectorOf<Chars> newNames;
  Chars err;
  Word E = P.parseEquation( G.namesOfGenerators(), newNames, err );  
  if ( err.length() == 0 ) {

    // Check for quadraticness

    int numOfVar = newNames.length() - G.numberOfGenerators();
    for( int i = 0; i < numOfVar; ++i ) {
      int count = E.numberOfOccurrences( Generator( i + 1 ) );
      if ( count != 2 && count != 0 ) {
	ParseErrorMessage( name, "0 {The equation is not quadratic}" ).send();
	return 0;
      }
    }
    return new SMEquation( parent, FreeGroup( newNames ), E, numOfVar,
			   Text("User defined equation in") + Name( parent ) );
  } else {
    ParseErrorMessage( name, err ).send();
    return 0;
  }
}

SMObject*
ReadSMEquation2::construct(SMObject* a1, Chars& name, istream& istr) const
{
  //@rn This expects the equation to be quadratic.
  
  SMFPGroup& parent = *(SMFPGroup*)a1;
  istr >> name;
  FGGroup G = parent.getFPGroup();
  EquationParser P( istr );
  VectorOf<Chars> newNames;
  Chars err;
  Word E = P.parseEquation( G.namesOfGenerators(), newNames, err );  
  if ( err.length() == 0 ) {
    int numOfVar = newNames.length() - G.numberOfGenerators();
    return new SMEquation2( parent, FreeGroup( newNames ), E, numOfVar,
			    Text("User defined equation in") + Name(parent) );
  } else {
    ParseErrorMessage( name, err ).send();
    return 0;
  }
}

SMObject*
ReadSMEqSystem::construct(SMObject* a1, Chars& name, istream& istr) const
{
  SMFPGroup& parent = *(SMFPGroup*)a1;
  istr >> name;
  FGGroup G = parent.getFPGroup();
  EqSystemParser ESP( istr );
  VectorOf<Chars> newNames;
  Chars err;
  VectorOf<Word> system = ESP.parseEqSystem( G.namesOfGenerators(), newNames, err );
  if ( err.length() == 0 ) {
      //cout << "no err";
    int numOfVar = newNames.length() - G.numberOfGenerators();
    return new SMEqSystem( parent, FreeGroup( newNames ), system, numOfVar,
			   Text("User defined system of equations in") 
			   + Name( parent ) );
  } else {
      //cout << "err";
    ParseErrorMessage( name, err ).send();
    return 0;
  }
}

SMObject*
ReadSMSubgroup::construct(SMObject* a1, Chars& name, istream& istr) const
{
  SMFPGroup& parent = *(SMFPGroup*)a1;
  istr >> name;
  FPGroup G = parent.getFPGroup();
  Chars err;
  VectorOf<Word> generators = G.readVectorOfWords(istr, err);
  if ( err.length() == 0 ) {
    return new SMSubgroup( parent, 
			   SGofFreeGroup(parent.getFreePreimage(), generators),
			   Text("User defined subgroup of") + Name( parent ) );
  } else {
    ParseErrorMessage( name, err ).send();
    return 0;
  }
}


SMObject*
ReadSMSetOfWords::construct(SMObject* a1, Chars& name, istream& istr) const
{
  SMFPGroup& parent = *(SMFPGroup*)a1;
  istr >> name;
  FPGroup G = parent.getFPGroup();
  Chars err;
  SetOf<Word> generators = G.readSetOfWords(istr, err);
  if ( err.length() == 0 ) 
    return new SMSetOfWords( parent, generators, Text("User defined set of words in") + Name( parent ) );
  else {
    ParseErrorMessage( name, err ).send();
    return 0;
  }
}

SMObject*
ReadSMVectorOfWords::construct(SMObject* a1, Chars& name, istream& istr) const
{
  SMFPGroup& parent = *(SMFPGroup*)a1;
  istr >> name;
  FPGroup G = parent.getFPGroup();
  Chars err;
  VectorOf<Word> generators = G.readVectorOfWords(istr, err);
  if ( err.length() == 0 )
    return new SMVectorOfWords( parent, generators, Text("User defined tuple of words in") + Name( parent ) );
  else {
    ParseErrorMessage( name, err ).send();
    return 0;
  }
}


SMObject* ReadSMMap::construct(SMObject* a1, Chars& name, istream& istr) const
{
  SMFPGroup& domain = *(SMFPGroup*)a1;
  istr >> name;
  FPGroup G = domain.getFPGroup();
  Map m( G, G );
  Chars err = istr >> m;
  if ( err.length() == 0 )
    return new SMMap( domain, domain, m,
		      Text("User defined map from the generators of")
		      + Name( domain ) + "to" + Name( domain ) );
  else {
    ParseErrorMessage( name, err ).send();    
    return 0;
  }
}

SMObject* ReadSMMap2::construct(SMObject* a1, SMObject* a2,
				Chars& name, istream& istr) const
{
  SMFPGroup& domain = *(SMFPGroup*)a1;
  SMFPGroup& range = *(SMFPGroup*)a2;
  istr >> name;
  FPGroup G = domain.getFPGroup();
  FPGroup H = range.getFPGroup();
  Map m( G, H );
  Chars err = istr >> m;
  if ( err.length() == 0 )
    return new SMMap( domain, range, m,
		      Text("User defined map from the generators of")
		      + Name( domain ) + "to" + Name( range ) );
    
  else {
    ParseErrorMessage( name, err ).send();
    return 0;
  }
}


SMObject* ReadSymmetricGroup::construct(Chars& name, istream& istr) const
{
  int n;
  istr >> n;
  SymmetricGroupFactory S(n);
  return S.factory();
}


SMObject*
ReadSMMagnusBreakdown::construct(SMObject* a1, Chars& name, istream& istr)const
{
  SMFPGroup& G = *(SMFPGroup *)a1;
  return new SMMagnusBreakdown( G );
}


SMObject*
ReadPowerOfMapItem::construct(SMObject* a1, Chars& name, istream& istr) const
{
  int p;
  istr >> p;
  return new PowerOfMap( *(SMMap*)a1, p );
}


SMObject*
ReadPHeight::construct(SMObject* a1, Chars& name, istream& istr) const
{
  int p;
  istr >> p;
  return new AbelianPHeightOfEltProblem( *(SMWord*)a1, p );
}

SMObject*
ReadHomologyItem::construct(SMObject* a1, Chars& name, istream& istr) const
{
  int d;
  istr >> d;
  return new HomologySupervisor( *(SMFPGroup*)a1, d );
}

SMObject*
ReadHomologyItem1::construct(SMObject* a1, Chars& name, istream& istr) const
{
  int d;
  istr >> d;
  return new AbelianIntegralHomologyProblem( *(SMFPGroup*)a1, d );
}

SMObject*
ReadAutEnumItem::construct(SMObject* a1, Chars& name, istream& istr) const
{
  int p;
  istr >> p;
  return new AutEnumerator( *(SMFPGroup*)a1, p );
}


SMObject*
ReadFinAutEnumItem::construct(SMObject* a1, Chars& name, istream& istr) const
{
  int p;
  istr >> p;
  return new AutEnumerator( *(SMFPGroup*)a1, p ,true);
}

SMObject*
ReadInitialSegmentItem::construct(SMObject* a1,Chars& name,istream& istr) const
{
  int p;
  istr >> p;
  return new InitialSegmentOfWord( *(SMWord*)a1, p );
}

SMObject*
ReadTerminalSegmentItem::construct(SMObject* a1,Chars& name,istream& istr)const
{
  int p;
  istr >> p;
  return new TerminalSegmentOfWord( *(SMWord*)a1, p );
}

SMObject*
ReadSegmentOfWordItem::construct(SMObject* a1, Chars& name, istream& istr)const
{
  int start;
  istr >> start;
  int length  = 1;
  // @dp TO BE INSERTED: istr >> length;

  SMWord *smo = (SMWord*)a1;
  if( start > smo->getWord().length() ) {
    Message message("Warning");
    message << "The starting position should be less than " 
	    << smo->getWord().length()+1 << ".";
    message.send();
    return 0;
  }
  return new SegmentOfWord( *(SMWord*)a1, start, length);
}


SMObject*
ReadMakeNilpotentQuotientItem::construct(SMObject* a1, Chars& name,
					 istream& istr) const
{
  int p;
  istr >> p;
  return new MakeNilpotentQuotient( *(SMFPGroup*)a1, p );
}


SMObject*
ReadMakeQuotientItem::construct(SMObject* a1, Chars& name, istream& istr) const
{
  SMFPGroup& parent = *(SMFPGroup*)a1;
  istr >> name;
  FPGroup G = parent.getFPGroup();
  Chars err;
  SetOf<Word> relators = G.readSetOfWords(istr, err);
  
  if ( err.length() == 0 )
    return new MakeQuotient( *(SMFPGroup*)a1, relators );
  else {
    ParseErrorMessage( name, err ).send();
    return 0;
  }
}


SMObject*
ReadFreeGetN_thElementItem::construct(SMObject* a1, Chars& name, 
				      istream& istr) const
{
  int p;
  istr >> p;
  return new FreeGetN_thElement( *(SMFPGroup*)a1, p );
}

SMObject*
ReadFreeGetNextN_thElementItem::construct(SMObject* a1, Chars& name, 
					  istream& istr) const
{
  int p;
  istr >> p;
  return new FreeGetNextN_thElement( *(SMWord*)a1, p );
}


SMObject*
ReadIsNilpotentProblemItem::construct(SMObject* a1, Chars& name,
				      istream& istr) const
{
  int p;
  istr >> p;
  return new IsNilpotentProblem( *(SMFPGroup*)a1, p );
}


SMObject*
ReadIsSGNilpotentItem::construct(SMObject* a1, Chars& name, istream& istr)const
{
  int p;
  istr >> p;
  return new IsSGNilpotent( *(SMSubgroup*)a1, p );
}

SMObject*
ReadLCStermProblem::construct(SMObject* a1, Chars& name, istream& istr) const
{
  int p;
  istr >> p;
  return new NGLCStermGensProblem( *(SMFPGroup*)a1, p );
}

SMObject* 
ReadNthPower::construct(SMObject* a1, Chars& name, istream& istr) const
{
  int p;
  istr >> p;
  return new IsWordNthPower( *(SMWord*)a1, p );
}

SMObject* 
ReadNormalApproximation::construct(SMObject* a1, Chars& name, istream& istr) const
{
  int p;
  istr >> p;
  return new NormalApproximationProblem( *(SMSubgroup*)a1, p );
}

// ----------------- Package Management ------------------- //

SMObject*
ReadPackage::construct(Chars& name, istream& istr) const
{
  Chars tname;
  Chars command;
  int checkin_selected;
  Type::notype n;
  Type object_selected(n);
  istr > tname > command > checkin_selected;
  object_selected.readMeFrom( istr );
  int nOfParam;
  istr > nOfParam;
  ListOf<ParamRecord> pList;
  for (int i=0;i<nOfParam;i++){
    Chars pname;
    istr > pname;
    Chars ptype;
    istr > ptype;
    ParamRecord::PARAM_TYPE pt;
    if (ptype == Chars("Integer")) pt = ParamRecord::INT;
    else if (ptype == Chars("String")) pt = ParamRecord::STRING;
    else pt = ParamRecord::BOOL;
    pList.append( ParamRecord( pname, pt) );
  }
  return new AddPackage(tname,command,checkin_selected,object_selected,pList);
}

SMObject*
ReadEditPackage::construct(Chars& name, istream& istr) const
{
  Type::notype n;
  Type id(n);
  istr >> id;
  int i;
  istr >> i;
  Chars c;
  istr >> c;
  return new EditPackage( id,i, c );
}

SMObject*
ReadGroupPackageID::construct(SMObject* a1, Chars& name, istream& istr) const
{
  int id;
  istr >> id;
  return new RunPackage( *(SMFPGroup*)a1, id );
}

SMObject*
ReadWordPackageID::construct(SMObject* a1, Chars& name, istream& istr) const
{
  int id;
  istr >> id;
  return new RunPackage( *(SMWord*)a1, id );
}

SMObject*
ReadSubgroupPackageID::construct(SMObject* a1, Chars& name, istream& istr)const
{
  int id;
  istr >> id;
  return new RunPackage( *(SMSubgroup*)a1, id );
}

SMObject*
ReadMapPackageID::construct(SMObject* a1, Chars& name, istream& istr) const
{
  int id;
  istr >> id;
  return new RunPackage( *(SMMap*)a1, id );
}

SMObject*
ReadHomoPackageID::construct(SMObject* a1, Chars& name, istream& istr) const
{
  int id;
  istr >> id;
  return new RunPackage( *(SMHomomorphism*)a1, id );
}

SMObject*
ReadWordWordPackageID::construct(SMObject* a1,SMObject* a2,
				 Chars& name, istream& istr) const
{
  int id;
  istr >> id;
  return new RunPackage( *(SMWord*)a1, *(SMWord*)a2, id );
}

SMObject*
ReadSubgroupSubgroupPackageID::construct(SMObject* a1,SMObject* a2,
					 Chars& name, istream& istr) const
{
  int id;
  istr >> id;
  return new RunPackage( *(SMSubgroup*)a1, *(SMSubgroup*)a2, id );
}

SMObject*
ReadSubgroupWordPackageID::construct(SMObject* a1,SMObject* a2,
				     Chars& name, istream& istr) const
{
  int id;
  istr >> id;
  return new RunPackage( *(SMSubgroup*)a1, *(SMWord*)a2, id );
}


//---------------------------------------------------------------------------//
//-------------------------- Menu::Action -----------------------------------//
//---------------------------------------------------------------------------//


void Menu::DefineFPGroup::print(ostream& ostr) const
{
  ostr << "_defineFPGroup_ {Define a finitely presented group}";
}


void Menu::DefineFreeGroup::print(ostream& ostr) const
{
  ostr << "_defineFreeGroup_ {Define a free group}";
}


void Menu::DefineAbelianGroup::print(ostream& ostr) const
{
  ostr << "_defineAbelGroup_ {Define an abelian group}";
}


void Menu::DefineNilpotentGroup::print(ostream& ostr) const
{
  ostr << "_defineNilpotentGroup_ {Define a nilpotent group}";
}

void Menu::DefineFreeNilpotentGroup::print(ostream& ostr) const
{
  ostr << "_defineFreeNilpotentGroup_ {Define a free nilpotent group}";
}

void Menu::DefineSmallCancGroup::print(ostream& ostr) const
{
  ostr << "_defineSCGroup_ {Define a small cancellation group}";
}


void Menu::DefineORGroup::print(ostream& ostr) const
{
  ostr << "_defineORGroup_ {Define an one-relator group}";
}

void Menu::DefinePermutation::print(ostream& ostr) const
{
  ostr << "_definePermutation_ {Define a permutation}";
}

void Menu::DefineWord::print(ostream& ostr) const
{
  ostr
	 << "_defineWord_ {Define a word in the generators of " << Name( 1 ) << '}';
}

void Menu::DefineEquation::print(ostream& ostr) const
{
  ostr
	 << "_defineEquation_ {Define an equation in " << Name( 1 ) << '}';
}

void Menu::DefineEquation2::print(ostream& ostr) const
{
  ostr
	 << "_defineEquation_ {Define an equation in " << Name( 1 ) << '}';
}

void Menu::DefineEqSystem::print(ostream& ostr) const
{
  ostr
    << "_defineSystemOfEquations_ {Define a system of equations in " << Name( 1 ) << '}';
}

void Menu::DefineSubgroup::print(ostream& ostr) const
{
  ostr
	 << "_defineSubgroup_ {Define a subgroup of " << Name( 1 ) << '}';
}

void Menu::DefineSetOfWords::print(ostream& ostr) const
{
  ostr
    << "_defineSetOfWords_ {Define a set of words in "
    << Name( 1 ) << '}';
}

void Menu::DefineVectorOfWords::print(ostream& ostr) const
{
  ostr
    << "_defineVectorOfWords_ {Define a tuple of words in "
    << Name( 1 ) << '}';
}


void Menu::DefineMap::print(ostream& ostr) const
{
  ostr << "_defineMap_ "<< Oid( 1 ) << " " << Oid( 1 );
}


void Menu::DefineMap2::print(ostream& ostr) const
{
  ostr << "_defineMap_ " << Oid( 1 ) << " " << Oid( 2 );
}


void Menu::DefineInverseMap2::print(ostream& ostr) const
{
  ostr << "_defineMap_ " << Oid( 2 ) << " " << Oid( 1 );
}


void Menu::DefineInt::print(ostream& ostr) const
{
  ostr
	 << "_defineInt_ {" << prompt << "} " << lower << ' ';
  if ( suppliedUpper )
	 ostr << upper;
  else
	 ostr << "{}";
}

void Menu::DefinePackage::print(ostream& ostr) const
{
  ostr << "_definePackage_ {Add new package } ";
  Packages::printObjectNames( ostr );
  Packages::printCheckinTypes( ostr );
}

void Menu::DefineEditPackage::print(ostream& ostr) const
{
  ostr << "_editPackage_ { Edit packages } ";
  Packages::printCheckinTypes( ostr );
  Packages::printObjectFileNames( ostr );
}

void Menu::PackageID::print(ostream& ostr) const
{
  ostr
    << "_packageID_ {package id " << "} " << theID;
}

void Menu::DefineInt2::print(ostream& ostr) const
{

  ostr << "_defineInt_ {" << prompt[0] << "} " << boundInt[0].lowerBound << ' ';
  if ( boundInt[0].haveUpperBound )
    ostr << boundInt[0].upperBound;
  else
	 ostr << "{}";

  return;

  //  for( int i = 0; i < 2; ++i ) {
  //
  //    cerr << "prompt[" << i << "] = " << prompt[i] << endl; // @dp debug print.
  //    int i = 0;
  //
  //    ostr << "_defineInt_ {" << prompt[i] << "} " << boundInt[i].lowerBound << ' ';
  //    if ( boundInt[i].haveUpperBound )
  //      ostr << boundInt[i].upperBound;
  //    else
  //      ostr << "{}";
  //    ostr << " ; ";
  //  }

}


void Menu::DefineSetOfRelators::print(ostream& ostr) const
{
  ostr << "_defineSetOfRelators_ {Define a set of words}";
}


//---------------------------------------------------------------------------//
//------------------------------ Menu<n> ------------------------------------//
//---------------------------------------------------------------------------//


Menu::Menu(Menu::MenuKind mk) : state( INIT )
{
  if ( mk == CHECKIN )
    cout << "menu_defn_ menu checkin signature {";
  else if( mk == TOOLS )
    cout << "menu_defn_ menu tools signature {";
  else if( mk == MAKE ) 
    cout << "menu_defn_ menu make signature {";
  else if( mk == DATABASE )
    cout << "menu_defn_ menu database signature {";
  else if( mk == PACKAGES )
    cout << "menu_defn_ menu packages signature {";
  else if( mk == TESTING )
    cout << "menu_defn_ menu testing signature {";
}


void Menu::startItemGroup( )
{
  if ( state == DONE ) error("Continued writing to closed menu");
  if ( state == ITEM_GROUP ) cout << "}} ";  // Close previous
  cout << "{1 {";
  state = ITEM_GROUP;
}


void Menu::startItemGroup(const Expression& E)
{
  if ( state == DONE ) error("Continued writing to closed menu");
  if ( state == ITEM_GROUP ) cout << "}} ";  // Close previous
  cout << "{{" << E << "} {";
  state = ITEM_GROUP;
}


void Menu::separator( )
{
  if ( state != ITEM_GROUP )
	 error("Tried to add item to menu without starting itemGroup first");
  cout << " s ";
}


void Menu::done( )
{
  if ( state == DONE ) error("Continued writing to closed menu");
  cout << "}}}" << endl << flush;
  state = DONE;
}


void Menu::add(Ctor* ctor, const Text& text, const Action& action)
{
  if ( state != ITEM_GROUP )
	 error("Tried to add item to menu without starting itemGroup first");

  cout << "{{" << text << "} {";
  action.print( cout );
  cout << "} {"
  
  // Do return message:

		 << SessionManager::getObjectSmithOid() << ' '
    
		 << ObjectSmith::registerCallBack( ctor )
  
		 << "}} ";
}


void Menu::addDisabled(const Text& text)
{
  if ( state != ITEM_GROUP )
	 error("Tried to add item to menu without starting itemGroup first");

  // @db comment out the next line to hide disabled menu items
  cout << "{{" << text << "} {} {}} ";
}


void Menu::startCascade(const Text& text)
{
  if ( state != ITEM_GROUP )
	 error("Tried to add item to menu without starting itemGroup first");

  cout << "{c {" << text << "} {";
}


void Menu::closeCascade( )
{
  if ( state != ITEM_GROUP )
	 error("Tried to add item to menu without starting itemGroup first");

  cout << "}} ";
}
