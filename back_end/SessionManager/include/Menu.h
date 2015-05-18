// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Declarations of helper classes for specifying menus
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//
// * 10/11/96 EP added stuff for SMVectorOfWords
//
//  96/10/09 @dp added struct ReadFreeGetNextN_thElementItem.
//  96/10/10 @dp added struct ReadTerminalSegmentItem, ReadSegmentOfWordItem.
//  96/10/10 @dp added struct DefineInt2, BoundedInteger.
//


#ifndef _MENU_H_
#define _MENU_H_


#include "FEData.h"
#include "SMObject.h"

//@rn These includes are required by the gcc 2.6.3 bug workaround,
//@rn discussed below:

#include "SMFPGroup.h"
#include "SMWord.h"
#include "SMEquation.h"
#include "SMSubgroup.h"
#include "SMSetOfWords.h"
#include "SMVectorOfWords.h"
#include "SMMap.h"
#include "SMMagnusBreakdown.h"
#include "SMPermutation.h"
#include "fastProblems.h"
#include "HomologyProblem.h"
#include "FreeProblems.h"
#include "IsNilpotentProblem.h"
#include "SubgroupProblems.h"
#include "OneRelatorProblems.h"
#include "PackagesSMApps.h"
#include "SMEqSystem.h"

//#include "HNNProblems.h"

//---------------------------------------------------------------------------//
//------------------------------ Ctor<n> ------------------------------------//
//---------------------------------------------------------------------------//

// A programmer may specify an end user menu item by supplying a
// `constructor wrapper', i.e., an appropriate derivative of class Ctor.
// These arrange, in cooperation with class ObjectSmith, for a callback
// when the menu item is selected. The callback reads the arguments of
// the item, and creates the required derivative of SMObject. The
// constructor wrapper does strong compile-time type checking of the
// arguments against the SMObject to be created.

// Templates cannot be class members (think about it), so these have
// to hang out in the global namespace.

struct Ctor
{
  virtual void readFrom(istream& istr) const = 0;
  // This does all input, and construction and storage of objects.

  virtual Ctor* copy( ) const = 0;
  // Used for storing an object in a derived class as a Ctor*.
  // This is necessary in order to keep the right virtual table.

protected:

  // With these here, only Ctor need be a friend of TheObjects,
  // SMObject, resp.:

  SMObject* get(int oid) const;
  // Returns the value of TheObjects::get( oid ).

  void newDependent(SMObject* smo, OID oid) const;
  // Tells smo that oid depends on it.
};


// The classes CtorAux<n> are not strictly necessary; they factor
// common code out of the Ctor<n> templates, to mitigate the effects of
// combinatorial explosion.
// The `readFrom' member knows how many oid's to read. It gets the
// corresponding SMObject*'s from TheObjects, and passes them to
// `construct' for the actual construction and storage of the new object.
// In special cases, e.g., when a parse error is possible, `construct' may
// return 0.


struct CtorAux0 : public Ctor
{
  void readFrom(istream& istr) const;            // overrides Ctor

  virtual SMObject* construct(Chars&, istream&) const = 0;
};


struct CtorAux1 : public Ctor
{
  void readFrom(istream& istr) const;            // overrides Ctor

  virtual SMObject* construct(SMObject*, Chars&, istream&) const = 0;
};


struct CtorAux2 : public Ctor
{
  CtorAux2(bool transpose = false) : transposeARGS( transpose ) { }

  void readFrom(istream& istr) const;            // overrides Ctor

  virtual SMObject* construct(SMObject*, SMObject*, Chars&, istream&) const = 0;

  bool transposeARGS;
  // Marks whether to reverse the order of the arguments.
};


struct CtorAux3 : public Ctor
{
  void readFrom(istream& istr) const;            // overrides Ctor

  virtual SMObject* construct(SMObject*, SMObject*, SMObject*, 
			      Chars&, istream&) const = 0;
};


struct CtorAux4 : public Ctor
{
  void readFrom(istream& istr) const;            // overrides Ctor

  virtual SMObject* construct(SMObject*, SMObject*, SMObject*, SMObject*, 
			      Chars&, istream&) const = 0;
};


// The following are the Ctor's to be passed as arguments to the Menu
// objects. Their purpose is to do the casting from mere SMObject*'s to
// objects of precisely the right type, and to call the right
// constructor.
// Each derives from an intermediate CtorArgs<n>, so that the classes
// Menu<n> below can check the types of the arguments.

struct CtorArgs0 : public CtorAux0 { };

template <class T>
struct Ctor0 : public CtorArgs0
{
  Ctor* copy( ) const { return new Ctor0; }          // overrides Ctor

private:

  SMObject* construct(Chars&, istream&) const {      // overrides CtorAux1
    return new T( );
  }
  // The istream& is used only by specializations of this function.
};


template <class A1> struct CtorArgs1 : public CtorAux1 { };

template <class T, class A1>
struct Ctor1 : public CtorArgs1<A1>
{
  Ctor* copy( ) const { return new Ctor1; }          // overrides Ctor

private:

  SMObject* construct(SMObject* a1, Chars&, istream&) const {
    return new T( *((A1*)a1) );
  }
  // overrides CtorAux1
  // The istream& is used only by specializations of this function.
};


template <class A1, class A2> struct CtorArgs2 : public CtorAux2
{
  CtorArgs2(bool transpose = false) : CtorAux2( transpose ) { }
};

template <class T, class A1, class A2>
struct Ctor2 : public CtorArgs2<A1,A2>
{
  Ctor2(bool transpose = false) : CtorArgs2<A1,A2>( transpose ) { }

  //@njz
  //  Ctor* copy( ) const { return new Ctor2( transposeARGS ); } // overrides Ctor
  Ctor* copy( ) const { return new Ctor2( this->transposeARGS ); } // overrides Ctor
  //

private:

  SMObject* construct(SMObject* a1, SMObject* a2, Chars&, istream&) const {
    return new T( *((A1*)a1), *((A2*)a2) );
  }
  // overrides CtorAux2
  // The istream& is used only by specializations of this function.
};


template <class A1, class A2, class A3> struct CtorArgs3 : public CtorAux3 { };

template <class T, class A1, class A2, class A3>
struct Ctor3 : public CtorArgs3<A1,A2,A3>
{
  Ctor* copy( ) const { return new Ctor3; } // overrides Ctor

private:

  SMObject* construct(SMObject* a1, SMObject* a2, SMObject* a3, 
		      Chars&, istream&) const 
  {
    return new T( *((A1*)a1), *((A2*)a2), *((A3*)a3) );
  }
  // overrides CtorAux3
  // The istream& is used only by specializations of this function.
};


template <class A1, class A2, class A3, class A4> struct CtorArgs4 :
  public CtorAux4 { };

template <class T, class A1, class A2, class A3, class A4>
struct Ctor4 : public CtorArgs4<A1,A2,A3,A4>
{
  Ctor* copy( ) const { return new Ctor4; } // overrides Ctor

private:

  SMObject* construct(SMObject* a1, SMObject* a2, SMObject* a3, SMObject* a4, 
		      Chars&, istream&) const 
  {
    return new T( *((A1*)a1), *((A2*)a2), *((A3*)a3), *((A4*)a4) );
  }
  // overrides CtorAux4
  // The istream& is used only by specializations of this function.
};

// At this point we would appear to have a satisfactory situation. The
// most common case, where a T has a constructor which takes arguments of
// types A1, A2, ... is handled automatically. Special cases are easily
// handled by specializing the member `Ctor<n>::construct' to use the
// istream&. In fact, the work of reading the `standard case' arguments
// is already done by CtorAux<n> even in special cases.
// 
// But no.
// 
// A sporadic gcc 2.6.3 bug sometimes prevents specializing a member
// function of a template class. In the perhaps overly naive hope that
// gcc will someday do templates (almost) right, we hack a workaround
// which mimics as closely as possible what we really want to do:
// 
// For each and every special case, derive a subclass of the needed
// instance of Ctor<n>. Override `construct', with the function body
// being exactly what the specialization's would have been.
// 
// An unfortunate consequence is that Ctor<n> will still try to
// instantiate the default version of `construct'. It cannot, because T
// does not have the right constructor. Indeed, the resulting compiler
// error is what this whole scheme is *designed* to cause! So, we hack
// further by putting dummy ctors in the classes T. Now I have to puke.


//---------------------------------------------------------------------------//
//------------------------------- Ctor --------------------------------------//
//--------------------------- special cases ---------------------------------//


struct ReadSMFPGroup : public Ctor0<SMFPGroup>
{
  Ctor* copy( ) const { return new ReadSMFPGroup; }          // overrides Ctor1
  SMObject* construct(Chars&, istream&) const;               // overrides Ctor1
};


struct ReadSMFreeGroup : public Ctor0<SMFPGroup>
{
  Ctor* copy( ) const { return new ReadSMFreeGroup; }        // overrides Ctor1
  SMObject* construct(Chars&, istream&) const;               // overrides Ctor1
};


struct ReadSMAbelianGroup : public Ctor0<SMFPGroup>
{
  Ctor* copy( ) const { return new ReadSMAbelianGroup; }     // overrides Ctor1
  SMObject* construct(Chars&, istream&) const;               // overrides Ctor1
};


struct ReadSMNilpotentGroup : public Ctor0<SMFPGroup>
{
  Ctor* copy( ) const { return new ReadSMNilpotentGroup; }   // overrides Ctor1
  SMObject* construct(Chars&, istream&) const;               // overrides Ctor1
};

struct ReadSMFreeNilpotentGroup : public Ctor0<SMFPGroup>
{
  Ctor* copy( ) const { return new ReadSMFreeNilpotentGroup; }// overrides Ctor1
  SMObject* construct(Chars&, istream&) const;               // overrides Ctor1
};

struct ReadSMORGroup : public Ctor0<SMFPGroup>
{
  Ctor* copy( ) const { return new ReadSMORGroup; }          // overrides Ctor1
  SMObject* construct(Chars&, istream&) const;               // overrides Ctor1
};

struct ReadSMSmallCancGroup : public Ctor0<SMFPGroup>
{
  Ctor* copy( ) const { return new ReadSMSmallCancGroup; }   // overrides Ctor1
  SMObject* construct(Chars&, istream&) const;               // overrides Ctor1
};

struct ReadSMPermutation : public Ctor0<SMPermutation>
{
  Ctor* copy( ) const { return new ReadSMPermutation; }      // overrides Ctor1
  SMObject* construct(Chars&, istream&) const;               // overrides Ctor1
};

struct ReadSMWord : public Ctor1<SMWord,SMFPGroup>, protected FEData
{
  Ctor* copy( ) const { return new ReadSMWord; }             // overrides Ctor1
  SMObject* construct(SMObject* a1, Chars&, istream&) const; // overrides Ctor1
};


struct ReadSMEquation : public Ctor1<SMEquation,SMFPGroup>, protected FEData
{
  Ctor* copy( ) const { return new ReadSMEquation; }         // overrides Ctor1
  SMObject* construct(SMObject* a1, Chars&, istream&) const; // overrides Ctor1
};

struct ReadSMEquation2 : public Ctor1<SMEquation2,SMFPGroup>, protected FEData
{
  Ctor* copy( ) const { return new ReadSMEquation2; }        // overrides Ctor1
  SMObject* construct(SMObject* a1, Chars&, istream&) const; // overrides Ctor1
};

struct ReadSMEqSystem : public Ctor1<SMEqSystem,SMFPGroup>, protected FEData
{
  Ctor* copy( ) const { return new ReadSMEqSystem; }         // overrides Ctor1
  SMObject* construct(SMObject* a1, Chars&, istream&) const; // overrides Ctor1
};

struct ReadSMSubgroup : public Ctor1<SMSubgroup,SMFPGroup>, protected FEData
{
  Ctor* copy( ) const { return new ReadSMSubgroup; }         // overrides Ctor1
  SMObject* construct(SMObject* a1, Chars&, istream&) const; // overrides Ctor1
};


struct ReadSMSetOfWords : 
  public Ctor1<SMSetOfWords,SMFPGroup>, protected FEData
{
  Ctor* copy( ) const { return new ReadSMSetOfWords; }       // overrides Ctor1
  SMObject* construct(SMObject* a1, Chars&, istream&) const; // overrides Ctor1
};

struct ReadSMVectorOfWords : 
  public Ctor1<SMVectorOfWords,SMFPGroup>, protected FEData
{
  Ctor* copy( ) const { return new ReadSMVectorOfWords; }    // overrides Ctor1
  SMObject* construct(SMObject* a1, Chars&, istream&) const; // overrides Ctor1
};


struct ReadSMMap : public Ctor1<SMMap,SMFPGroup>, protected FEData
{
  Ctor* copy( ) const { return new ReadSMMap; }              // overrides Ctor1
  SMObject* construct(SMObject* a1, Chars&, istream&) const; // overrides Ctor1
};


struct ReadSMMap2 : public Ctor2<SMMap,SMFPGroup,SMFPGroup>, protected FEData
{
  ReadSMMap2(bool transpose = false)
    : Ctor2<SMMap,SMFPGroup,SMFPGroup>( transpose ) { }
  Ctor* copy( ) const { return new ReadSMMap2( transposeARGS ); }
  // overrides Ctor1
  SMObject* construct(SMObject* a1, SMObject* a2, Chars&, istream&) const;
  // overrides Ctor1
};


struct ReadSymmetricGroup : public Ctor0<SMFPGroup>, protected FEData
{
  ReadSymmetricGroup() : Ctor0<SMFPGroup>( ) { }
  Ctor* copy( ) const { return new ReadSymmetricGroup(); }
  // overrides Ctor0
  SMObject* construct(Chars&, istream&) const;
  // overrides Ctor0
};

struct ReadSMMagnusBreakdown : 
  public Ctor1<SMMagnusBreakdown,SMFPGroup>, protected FEData
{
  Ctor* copy( ) const { return new ReadSMMagnusBreakdown; } // overrides Ctor1
  SMObject* construct(SMObject* a1, Chars&, istream&) const;
  // overrides Ctor1
};


// The following special case those menu items which require an
// additional int parameter.


struct ReadPowerOfMapItem : public Ctor1<PowerOfMap,SMMap>, protected FEData
{
  Ctor* copy( ) const { return new ReadPowerOfMapItem; }     // overrides Ctor1
  SMObject* construct(SMObject* a1, Chars&, istream&) const; // overrides Ctor1
};


struct ReadHomologyItem : 
  public Ctor1<HomologyProblem,SMFPGroup>, protected FEData
{
  Ctor* copy( ) const { return new ReadHomologyItem; }       // overrides Ctor1
  SMObject* construct(SMObject* a1, Chars&, istream&) const; // overrides Ctor1
};

struct ReadHomologyItem1 : 
  public Ctor1<AbelianIntegralHomologyProblem,SMFPGroup>, protected FEData
{
  Ctor* copy( ) const { return new ReadHomologyItem1; }       // overrides Ctor1
  SMObject* construct(SMObject* a1, Chars&, istream&) const; // overrides Ctor1
};

struct ReadAutEnumItem : 
  public Ctor1<AutEnumerator,SMFPGroup>, protected FEData
{
  Ctor* copy( ) const { return new ReadAutEnumItem; }        // overrides Ctor1
  SMObject* construct(SMObject* a1, Chars&, istream&) const; // overrides Ctor1
};


struct ReadFinAutEnumItem : 
  public Ctor1<AutEnumerator,SMFPGroup>, protected FEData
{
  Ctor* copy( ) const { return new ReadFinAutEnumItem; }     // overrides Ctor1
  SMObject* construct(SMObject* a1, Chars&, istream&) const; // overrides Ctor1
};


struct ReadInitialSegmentItem : 
  public Ctor1<InitialSegmentOfWord,SMWord>, protected FEData
{
  Ctor* copy( ) const { return new ReadInitialSegmentItem; } // overrides Ctor1
  SMObject* construct(SMObject* a1, Chars&, istream&) const; // overrides Ctor1
};

struct ReadPHeight : 
  public Ctor1<AbelianPHeightOfEltProblem,SMWord>, protected FEData
{
  Ctor* copy( ) const { return new ReadPHeight; } // overrides Ctor1
  SMObject* construct(SMObject* a1, Chars&, istream&) const; // overrides Ctor1
};

struct ReadTerminalSegmentItem : 
  public Ctor1<TerminalSegmentOfWord,SMWord>, protected FEData
{
  Ctor* copy( ) const { return new ReadTerminalSegmentItem; } // overrides Ctor1
  SMObject* construct(SMObject* a1, Chars&, istream&) const; // overrides Ctor1
};

struct ReadSegmentOfWordItem : 
  public Ctor1<SegmentOfWord,SMWord>, protected FEData
{
  Ctor* copy( ) const { return new ReadSegmentOfWordItem; } // overrides Ctor1
  SMObject* construct(SMObject* a1, Chars&, istream&) const; // overrides Ctor1
};


struct ReadFreeGetN_thElementItem : 
  public Ctor1<FreeGetN_thElement,SMFPGroup>, protected FEData
{
  Ctor* copy( ) const { return new ReadFreeGetN_thElementItem; } // overrides Ctor1
  SMObject* construct(SMObject* a1, Chars&, istream&) const; // overrides Ctor1
};

struct ReadFreeGetNextN_thElementItem : 
  public Ctor1<FreeGetNextN_thElement,SMWord>, protected FEData
{
  Ctor* copy( ) const { return new ReadFreeGetNextN_thElementItem; } // overrides Ctor1
  SMObject* construct(SMObject* a1, Chars&, istream&) const; // overrides Ctor1
};


struct ReadMakeNilpotentQuotientItem : 
    public Ctor1<MakeNilpotentQuotient,SMFPGroup>, protected FEData
{
  Ctor* copy( ) const { return new ReadMakeNilpotentQuotientItem; } 
  // overrides Ctor1
  
  SMObject* construct(SMObject* a1, Chars&, istream&) const; 
  // overrides Ctor1
};


struct ReadMakeQuotientItem : 
  public Ctor1<MakeQuotient, SMFPGroup>, protected FEData
{
  Ctor* copy( ) const { return new ReadMakeQuotientItem; } 
  // overrides Ctor1
  
  SMObject* construct(SMObject* a1, Chars&, istream&) const; 
  // overrides Ctor1
};


struct ReadIsNilpotentProblemItem : 
  public Ctor1<IsNilpotentProblem, SMFPGroup>, protected FEData
{
  Ctor* copy( ) const { return new ReadIsNilpotentProblemItem; } 
  // overrides Ctor1
  
  SMObject* construct(SMObject* a1, Chars&, istream&) const; 
  // overrides Ctor1
};

struct ReadIsSGNilpotentItem : 
  public Ctor1<IsSGNilpotent, SMSubgroup>, protected FEData
{
  Ctor* copy( ) const { return new ReadIsSGNilpotentItem; } 
  // overrides Ctor1
  
  SMObject* construct(SMObject* a1, Chars&, istream&) const; 
  // overrides Ctor1
};

struct ReadLCStermProblem : 
  public Ctor1<NGLCStermGensProblem, SMFPGroup>, protected FEData
{
  Ctor* copy( ) const { return new ReadLCStermProblem; } 
  // overrides Ctor1
  
  SMObject* construct(SMObject* a1, Chars&, istream&) const; 
  // overrides Ctor1
};

struct ReadNthPower : public Ctor1<IsWordNthPower, SMWord>, protected FEData
{
  Ctor* copy( ) const { return new ReadNthPower; } 
  
  SMObject* construct(SMObject* a1, Chars&, istream&) const; 
};

struct ReadNormalApproximation : 
  public Ctor1<NormalApproximationProblem, SMSubgroup>, protected FEData
{
  Ctor* copy( ) const { return new ReadNormalApproximation; } 
  
  SMObject* construct(SMObject* a1, Chars&, istream&) const; 
};


// --------- Package managment ------------------//

struct ReadPackage : 
  public Ctor0<AddPackage>, protected FEData
{
  Ctor* copy( ) const { return new ReadPackage; } 
  // overrides Ctor0
  
  SMObject* construct( Chars&, istream&) const; 
  // overrides Ctor0
};

struct ReadEditPackage : 
  public Ctor0<EditPackage>, protected FEData
{
  Ctor* copy( ) const { return new ReadEditPackage; } 
  // overrides Ctor0
  
  SMObject* construct( Chars&, istream&) const; 
  // overrides Ctor0
};

struct ReadGroupPackageID : 
  public Ctor1<RunPackage, SMFPGroup>, protected FEData
{
  Ctor* copy( ) const { return new ReadGroupPackageID; } 
  // overrides Ctor0
  
  SMObject* construct(SMObject* a1, Chars&, istream&) const; 
  // overrides Ctor0
};

struct ReadWordPackageID : 
  public Ctor1<RunPackage, SMWord>, protected FEData
{
  Ctor* copy( ) const { return new ReadWordPackageID; } 
  // overrides Ctor0
  
  SMObject* construct(SMObject* a1, Chars&, istream&) const; 
  // overrides Ctor0
};

struct ReadSubgroupPackageID : 
  public Ctor1<RunPackage, SMSubgroup>, protected FEData
{
  Ctor* copy( ) const { return new ReadSubgroupPackageID; } 
  // overrides Ctor0
  
  SMObject* construct(SMObject* a1, Chars&, istream&) const; 
  // overrides Ctor0
};

struct ReadMapPackageID : 
  public Ctor1<RunPackage, SMMap>, protected FEData
{
  Ctor* copy( ) const { return new ReadMapPackageID; } 
  // overrides Ctor0
  
  SMObject* construct(SMObject* a1, Chars&, istream&) const; 
  // overrides Ctor0
};

struct ReadHomoPackageID : 
  public Ctor1<RunPackage, SMHomomorphism>, protected FEData
{
  Ctor* copy( ) const { return new ReadHomoPackageID; } 
  // overrides Ctor0
  
  SMObject* construct(SMObject* a1, Chars&, istream&) const; 
  // overrides Ctor0
};

struct ReadWordWordPackageID : 
  public Ctor2<RunPackage,SMWord,SMWord>, protected FEData
{
  Ctor* copy( ) const { return new ReadWordWordPackageID; }
  // overrides Ctor1
  SMObject* construct(SMObject* a1, SMObject* a2, Chars&, istream&) const;
  // overrides Ctor1
};

struct ReadSubgroupSubgroupPackageID : 
  public Ctor2<RunPackage,SMSubgroup,SMSubgroup>, protected FEData
{
  Ctor* copy( ) const { return new ReadSubgroupSubgroupPackageID; }
  // overrides Ctor1
  SMObject* construct(SMObject* a1, SMObject* a2, Chars&, istream&) const;
  // overrides Ctor1
};

struct ReadSubgroupWordPackageID : 
  public Ctor2<RunPackage,SMSubgroup,SMWord>, protected FEData
{
  Ctor* copy( ) const { return new ReadSubgroupWordPackageID; }
  // overrides Ctor1
  SMObject* construct(SMObject* a1, SMObject* a2, Chars&, istream&) const;
  // overrides Ctor1
};


//---------------------------------------------------------------------------//
//------------------------------ Menu ---------------------------------------//
//---------------------------------------------------------------------------//


class Menu : public FEData
{
public:

  enum MenuKind { CHECKIN, TOOLS, MAKE, PACKAGES, DATABASE, TESTING };

  void startItemGroup( );
  void startItemGroup(const Expression& condition);

  void startCascade(const Text& text);
  void closeCascade( );

  void addDisabled(const Text& text);

  void separator( );

  void done( );

  //-------------------------------------------------------------------------//
  //---------------------------- Action -------------------------------------//
  //-------------------------------------------------------------------------//

  struct Action
  {
	 virtual void print(ostream& ostr) const { }
  };

  struct DefineFPGroup : public Action
  {
	 void print(ostream& ostr) const;      // overrides Action
  };

  struct DefineFreeGroup : public Action
  {
	 void print(ostream& ostr) const;      // overrides Action
  };

  struct DefineAbelianGroup : public Action
  {
	 void print(ostream& ostr) const;      // overrides Action
  };

  struct DefineNilpotentGroup : public Action
  {
	 void print(ostream& ostr) const;      // overrides Action
  };
  struct DefineFreeNilpotentGroup : public Action
  {
	 void print(ostream& ostr) const;      // overrides Action
  };

  struct DefineSmallCancGroup : public Action
  {
	 void print(ostream& ostr) const;      // overrides Action
  };

  struct DefineORGroup : public Action
  {
	 void print(ostream& ostr) const;      // overrides Action
  };

  struct DefinePermutation : public Action
  {
	 void print(ostream& ostr) const;      // overrides Action
  };

  struct DefineWord : public Action
  {
	 void print(ostream& ostr) const;      // overrides Action
  };

  struct DefineEquation : public Action
  {
	 void print(ostream& ostr) const;      // overrides Action
  };

  struct DefineEquation2 : public Action
  {
	 void print(ostream& ostr) const;      // overrides Action
  };

  struct DefineEqSystem : public Action
  {
         void print(ostream& ostr) const;      // overrides Action
  };

  struct DefineSubgroup : public Action
  {
	 void print(ostream& ostr) const;      // overrides Action
  };


  struct DefineSetOfWords : public Action
  {
	 void print(ostream& ostr) const;      // overrides Action
  };

  struct DefineVectorOfWords : public Action
  {
	 void print(ostream& ostr) const;      // overrides Action
  };

  
  struct DefineMap : public Action
  {
    void print(ostream& ostr) const;      // overrides Action

  };


  struct DefineMap2 : public Action
  {
	 void print(ostream& ostr) const;      // overrides Action
  };


  struct DefineInverseMap2 : public Action
  {
	 void print(ostream& ostr) const;      // overrides Action
  };


  struct DefineInt : public Action
  {
	 DefineInt(const char* p, int l)
	   : prompt(p), lower(l), suppliedUpper(false)
	 { }

	 DefineInt(const char* p, int l, int u)
	   : prompt(p), lower(l), upper(u), suppliedUpper(true)
	 { }

	 void print(ostream& ostr) const;      // overrides Action

  private:
	 const char* prompt;
	 int lower, upper;
	 bool suppliedUpper;
  };

  struct DefinePackage : public Action
  {
    DefinePackage() { }
    void print(ostream& ostr) const;      // overrides Action


  };

  struct DefineEditPackage : public Action
  {
    DefineEditPackage() { }
    void print(ostream& ostr) const;      // overrides Action


  };

 struct PackageID : public Action
  {
    PackageID( int id):theID( id ) { }
    void print(ostream& ostr) const;      // overrides Action
    private:
    int theID;

  };

  struct BoundedInteger 
  {
    BoundedInteger() 
      : lowerBound(0), upperBound(0),
	haveLowerBound(false), haveUpperBound(false)
    { }

    BoundedInteger(int lower) 
      : lowerBound(lower), upperBound(0), 
	haveLowerBound(true), haveUpperBound(false) 
    { }

    BoundedInteger(int lower, int upper) 
      : lowerBound(lower), upperBound(upper), 
	haveLowerBound(true), haveUpperBound(true) 
    { }

    int lowerBound, upperBound;
    bool haveLowerBound, haveUpperBound;
  };
  //@db need to change location of that class

  struct DefineInt2 : public Action
  {
    DefineInt2( const char* p0, const BoundedInteger& bi0,
	        const char* p1, const BoundedInteger& bi1 )
    {
      prompt[0] = p0;
      prompt[1] = p1;
      boundInt[0] = bi0;
      boundInt[1] = bi1;
    }
    
    void print(ostream& ostr) const;      // overrides Action

  private:	
    const char* prompt[2];
    BoundedInteger boundInt[2];
  };
  //@db temporary implementation, to eliminate in the future.

  struct DefineSetOfRelators : public Action
  {
    void print(ostream& ostr) const;      // overrides Action
  };

protected:

  Menu(MenuKind);

  void add(Ctor* ctor, const Text& text, const Action& action);

private:

  enum StateType { INIT, ITEM_GROUP, DONE };

  StateType state;
};


//@rn We inline all of the Menu<n> member functions, probably
//@rn inappropriately, to avoid having to do explicit instantiation.
//@rn This can be improved when gcc supports template repositories.


class Menu0 : public Menu
{
public:

  Menu0(MenuKind mk) : Menu( mk ) {
	 cout << "} itemGroups {";
  }

  void add(const CtorArgs0& ctor,
			  const Text& text,
			  const Action& action = Action()
			  )
  {
	 Menu::add( ctor.copy(), text, action );
  }

};


template <class A1>
class Menu1 : public Menu
{
public:

  Menu1(MenuKind mk) : Menu( mk ) {
	 cout << A1::type();
	 cout << "} itemGroups {";
  }

  void add(const CtorArgs1<A1>& ctor,
			  const Text& text,
			  const Action& action = Action()
			  )
  {
	 Menu::add( ctor.copy(), text, action );
  }

};


template <class A1, class A2>
class Menu2 : public Menu
{
public:

  Menu2(MenuKind mk) : Menu( mk ) {
	 cout << A1::type() << ' ' << A2::type();
	 cout << "} itemGroups {";
  }

  void add(const CtorArgs2<A1,A2>& ctor,
			  const Text& text,
			  const Action& action = Action()
			  )
  {
	 Menu::add( ctor.copy(), text, action );
  }

};


template <class A1, class A2, class A3>
class Menu3 : public Menu
{
public:

  Menu3(MenuKind mk) : Menu( mk )
  {
    cout << A1::type() << ' ' << A2::type() << ' ' << A3::type();
    cout << "} itemGroups {";
  }

  void add( const CtorArgs3<A1,A2,A3>& ctor,
	    const Text& text,
	    const Action& action = Action()
	    )
  {
    Menu::add( ctor.copy(), text, action );
  }

};


template <class A1, class A2, class A3, class A4>
class Menu4 : public Menu
{
public:

  Menu4(MenuKind mk) : Menu( mk )
  {
    cout << A1::type() << ' ' << A2::type() << ' ' << A3::type() << ' '
	 << A4::type();
    cout << "} itemGroups {";
  }

  void add( const CtorArgs4<A1,A2,A3,A4>& ctor,
	    const Text& text,
	    const Action& action = Action()
	    )
  {
    Menu::add( ctor.copy(), text, action );
  }

};


#endif
