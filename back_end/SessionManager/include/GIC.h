// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Declaration of class GIC (Group Information Center)
//
// Principal Author: Roger Needham, Dmitry Bormotov
//
// Status: this can never be complete
//
// Usage:
//
// For a given piece of info, there are up to four kinds of names needed:
// 1) The data member
// 2) The member function through which it is reported
// 3) The member function via which its existence is queried
// 4) The member function via which it is accessed
// 
// In an attempt at coherence, we try to use these conventions for a
// Trichotomy (resp. bool) <I>
// e.g., I == abelian, (resp. I == solvedWordProblem):
// 
// 1) <I>
// 2) putIs<I>  resp. putHave<I>
// 3) is<I>     resp. have<I>
// 
// and in other cases (e.g., I == confluentKBMachine):
// 
// 1) <I>
// 2) putHave<I>
// 3) have<I>
// 4) get<I>
//
//
// Next Implementation Steps:
//
// * Better exception instrumentation, e.g., conflicting reports of order
//
// * It is not clear how much the explanations will need to be accessed,
//   since they are sent to the FE when the explained info is reported.
//   Put accessors in as needed.
//
//
// Revision History:
//



#ifndef _GIC_H_
#define _GIC_H_


//#include <Integer.h>
//#include <DArray.h>
//#include "Associations.h"
//#include "Trichotomy.h"
//#include "Chars.h"

#include "FEData.h"
#include "BaseProperties.h"
#include "InformationCenter.h"
#include "OID.h"

// Types stored by the GIC:

//#include "KBMachine.h"
//#include "Map.h"
//#include "DFSA.h"
//#include "DiffMachine.h"
//#include "AbelianGroup.h"
//#include "FreeByCyclic.h"
//#include "MSCGroup.h"
//#include "NilpotentGroup.h"
//#include "SubgroupGraph.h"
//#include "CosetEnumerator.h"
//#include "APwithOneRelator.h"


//---------------------------------------------------------------------------//
//--------------------- GIC PROPERTIES --------------------------------------//
//---------------------------------------------------------------------------//

class GroupOrderProperty : public IntegerProperty {
public:
  GroupOrderProperty(const Integer& data, const Chars& descr) 
    : IntegerProperty(data, descr) {}
  PropertyType actualType  () const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "GroupOrder";   }
protected:
  GenericProperty* clone() const { return new GroupOrderProperty(*this); }
private:
  static const PropertyType theTypeID;
};

/*
class GroupOrderUpperBoundProperty : public IntegerProperty {
public:
  GroupOrderUpperBoundProperty(const Integer& data, const Chars& descr) 
    : IntegerProperty(data, descr) {}
  PropertyType actualType  () const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "GroupOrderUpperBound"; }
private:
  static const PropertyType theTypeID;
};
*/

class SolvedWordProblemProperty : public NoDataProperty {
public:
  SolvedWordProblemProperty(const Chars& descr) : NoDataProperty(descr) {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "SolvedWordProblem"; }
protected:
  GenericProperty* clone() const {return new SolvedWordProblemProperty(*this);}
private:
  static const PropertyType theTypeID;
};

class FastWordProblemProperty : public NoDataProperty {
public:
  FastWordProblemProperty(const Chars& descr) : NoDataProperty(descr) {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "FastWordProblem"; }
protected:
  GenericProperty* clone() const { return new FastWordProblemProperty(*this); }
private:
  static const PropertyType theTypeID;
};

class CompleteCayleyGraphProperty : public SubgroupGraphProperty {
public:
  CompleteCayleyGraphProperty(const SubgroupGraph& graph) 
    : SubgroupGraphProperty(graph) {}
  PropertyType actualType  () const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "CompleteCayleyGraph"; }
protected:
  GenericProperty* clone() const { 
    return new CompleteCayleyGraphProperty(*this); 
  }
private:
  static const PropertyType theTypeID;
};

class ConfluentKBMachineProperty : public KBMachineProperty {
public:
  ConfluentKBMachineProperty(const KBMachine& data) : KBMachineProperty(data){}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "ConfluentKBMachine"; }
protected:
  GenericProperty* clone() const { return new ConfluentKBMachineProperty(*this); }
private:
  static const PropertyType theTypeID;
};


class IsAutomaticProperty : public NoDataProperty {
public:
  IsAutomaticProperty(const Chars& descr) : NoDataProperty(descr) {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "IsAutomatic"; }
protected:
  GenericProperty* clone() const { return new IsAutomaticProperty(*this); }
private:
  static const PropertyType theTypeID;
};

class Automatic_GroupDFSAProperty : public GroupDFSAProperty {
public:
  Automatic_GroupDFSAProperty(const GroupDFSA& data) 
    : GroupDFSAProperty(data) {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "Automatic_GroupDFSA"; }
protected:
  GenericProperty* clone() const { return new Automatic_GroupDFSAProperty(*this); }
private:
  static const PropertyType theTypeID;
};

class Automatic_DiffMachineProperty : public DiffMachineProperty {
public:
  Automatic_DiffMachineProperty(const DiffMachine& data) 
    : DiffMachineProperty(data) {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "Automatic_DiffMachine"; }
protected:
  GenericProperty* clone() const { return new Automatic_DiffMachineProperty(*this); }
private:
  static const PropertyType theTypeID;
};

class OneRelatorProperty : public WordProperty {
public:
  OneRelatorProperty(const Word& data) : WordProperty(data) {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "OneRelator"; }
protected:
  GenericProperty* clone() const { return new OneRelatorProperty(*this); }
private:
  static const PropertyType theTypeID;
};

class OneRelatorWithTorsionProperty : public NoDataProperty {
public:
  OneRelatorWithTorsionProperty() : NoDataProperty() {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "OneRelatorWithTorsion"; }
protected:
  GenericProperty* clone() const { return new OneRelatorWithTorsionProperty(*this); }
private:
  static const PropertyType theTypeID;
};

class AbelianPresentationProperty : public AbelianGroupProperty {
public:
  AbelianPresentationProperty(const AbelianGroup& data, 
			      const Chars descr = Chars() ) 
    : AbelianGroupProperty(data,descr) {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "AbelianPresentation"; }
protected:
  GenericProperty* clone() const { return new AbelianPresentationProperty(*this); }
private:
  static const PropertyType theTypeID;
};

/*

class CyclicDecompositionProperty : public AbelianGroupProperty {
public:
  CyclicDecompositionProperty(const AbelianGroup& data) 
    : AbelianGroupProperty(data) {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "CyclicDecomposition"; }
private:
  static const PropertyType theTypeID;
};

class PrimaryDecompositionProperty : public AbelianGroupProperty {
public:
  PrimaryDecompositionProperty(const AbelianGroup& data) 
    : AbelianGroupProperty(data) {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "PrimaryDecomposition"; }
private:
  static const PropertyType theTypeID;
};

class CanonicalSmithDecompositionProperty : public AbelianGroupProperty {
public:
  CanonicalSmithDecompositionProperty(const AbelianGroup& data) 
    : AbelianGroupProperty(data) {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "CanonicalSmithDecomposition"; }
private:
  static const PropertyType theTypeID;
};

*/

class IsAbelianProperty : public NoDataProperty {
public:
  IsAbelianProperty() : NoDataProperty() {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "IsAbelian"; }
protected:
  GenericProperty* clone() const { return new IsAbelianProperty(*this); }
private:
  static const PropertyType theTypeID;
};

class IsFreeProperty : public NoDataProperty {
public:
  IsFreeProperty(const Chars& descr) : NoDataProperty(descr) {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "IsFree"; }
protected:
  GenericProperty* clone() const { return new IsFreeProperty(*this); }
private:
  static const PropertyType theTypeID;
};

class IsFiniteProperty : public NoDataProperty {
public:
  IsFiniteProperty(const Chars& descr) : NoDataProperty(descr) {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "IsFinite"; }
protected:
  GenericProperty* clone() const { return new IsFiniteProperty(*this); }
private:
  static const PropertyType theTypeID;
};

class NilpotencyClassProperty : public IntProperty {
public:
  NilpotencyClassProperty(const int& data) : IntProperty(data) {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "NilpotencyClass"; }
protected:
  GenericProperty* clone() const { return new NilpotencyClassProperty(*this); }
private:
  static const PropertyType theTypeID;
};

class NilpotentQuotientsProperty : public NilpGroupAssocProperty {
public:
  NilpotentQuotientsProperty(const AssociationsOf<int,NilpotentGroup*>& data) 
    : NilpGroupAssocProperty(data) {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "NilpotentQuotients"; }
protected:
  GenericProperty* clone() const { return new NilpotentQuotientsProperty(*this); }
private:
  static const PropertyType theTypeID;
};

class IsFreeNilpotentProperty : public NoDataProperty {
public:
  IsFreeNilpotentProperty() : NoDataProperty() {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "IsFreeNilpotent"; }
protected:
  GenericProperty* clone() const { return new IsFreeNilpotentProperty(*this); }
private:
  static const PropertyType theTypeID;
};

class ActualNilpotencyClassProperty : public IntProperty {
public:
  ActualNilpotencyClassProperty(const int& data) : IntProperty(data) {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "ActualNilpotentClass"; }
protected:
  GenericProperty* clone() const { return new ActualNilpotencyClassProperty(*this); }
private:
  static const PropertyType theTypeID;
};


class IsFreeByCyclicProperty : public FreeByCyclicProperty {
public:
  IsFreeByCyclicProperty(const FreeByCyclic& data):FreeByCyclicProperty(data){}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "IsFreeByCyclic"; }
protected:
  GenericProperty* clone() const { return new IsFreeByCyclicProperty(*this); }
private:
  static const PropertyType theTypeID;
};

/*

class IsFreeByCyclic_MapProperty : public MapProperty {
public:
  IsFreeByCyclic_MapProperty(const Map& data) : MapProperty(data) {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "IsFreeByCyclic_Map"; }
private:
  static const PropertyType theTypeID;
};

*/

class MSCProperty : public MSCGroupProperty {
public:
  MSCProperty(const FPGroup& G, int lambda) : MSCGroupProperty(G,lambda) {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "MSC"; }
protected:
  GenericProperty* clone() const { return new MSCProperty(*this); }
private:
  static const PropertyType theTypeID;
};

class MSCLambdaProperty : public IntProperty {
public:
  MSCLambdaProperty(const int& data, const Chars& descr = Chars()) 
    : IntProperty(data,descr) {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "MSCLambda"; }
protected:
  GenericProperty* clone() const { return new MSCLambdaProperty(*this); }
private:
  static const PropertyType theTypeID;
};

class APofFreeProperty : public AmalgProductOfFreeGroupsProperty {
public:
  APofFreeProperty(const AmalgProductOfFreeGroups& data) 
    : AmalgProductOfFreeGroupsProperty(data) {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "APofFree"; }
protected:
  GenericProperty* clone() const { return new APofFreeProperty(*this); }
private:
  static const PropertyType theTypeID;
};

class HNNofFreeProperty : public HNNExtOfFreeGroupProperty {
public:
  HNNofFreeProperty(const HNNExtOfFreeGroup& data) 
    : HNNExtOfFreeGroupProperty(data) {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "HNNofFree"; }
protected:
  GenericProperty* clone() const { return new HNNofFreeProperty(*this); }
private:
  static const PropertyType theTypeID;
};

class SchreierTransversalProperty : public PermutationRepresentationProperty {
public:
  SchreierTransversalProperty(const PermutationRepresentation& data) 
    : PermutationRepresentationProperty(data) {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "SchreierTransversal"; }
protected:
  GenericProperty* clone() const { return new SchreierTransversalProperty(*this); }
private:
  static const PropertyType theTypeID;
};

class WordDecomposerProperty : public DecomposeInSubgroupOfFPGroupProperty {
public:
  WordDecomposerProperty(const DecomposeInSubgroupOfFPGroup& data) 
    : DecomposeInSubgroupOfFPGroupProperty(data) {}
  PropertyType actualType()   const { return type(); }
  static PropertyType type() { return theTypeID; }
  Chars propertyName() const { return "WordDecomposer"; }
protected:
  GenericProperty* clone() const { return new WordDecomposerProperty(*this); }
private:
  static const PropertyType theTypeID;
};


// By its nature as a `knowledge representer', which no one knows how to
// do, this has evolved in odd and sometimes conflicting ways as
// requirements have become apparent.  It will need combing and
// rethinking periodically.


//---------------------------------------------------------------------------//
//-------------------------------- GIC --------------------------------------//
//---------------------------------------------------------------------------//


class GIC : public InformationCenter, protected FEData
{  
public:

// ----------------- Flags for solutions of word problem ------------------ //

 enum AlgorithmID { NONE, AB_INV, NORM_CLOSURE, REWR_SYSTEM, 
		     AUT_STRUCTURE, GENETIC, FREE };

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  GIC(OID group_oid);

  ~GIC( );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Reporting Information:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void putHaveOrder( Integer the_order, Chars explanation = "", 
		     bool showExplanation = true );

  //void putHaveOrderUpperBound(Integer the_bound, Chars explanation);

  void putHaveSolvedWordProblem( Chars explanation, 
				 bool bShowExplanation = true );

  void putHaveFastWordProblem( Chars explanation,
			       bool bShowExplanation = true );

  void putHaveCompleteCayleyGraph(SubgroupGraph CG);

  void putHaveConfluentKBMachine(KBMachine kbm);

  void putIsAutomatic( Trichotomy IsAutomatic );

  void putHaveAutomatic(GroupDFSA wa, DiffMachine dm);

  void putIsOneRelator(const Word& relator);

  void putIsOneRelatorWithTorsion(const Word& relator);

  void putHaveCyclicDecomposition( const AbelianGroup& );

  void putHavePrimaryDecomposition(const AbelianGroup& );

  void putHaveCanonicalSmithPresentation( const AbelianGroup& );

  void putIsAbelian( );

  void putIsFree( bool IsFree = true, Chars explanation = "" );

  void putIsFinite( bool IsFinite = true, Chars explanation = "",
		    bool showExplanation = true );

  void putHaveNilpotentQuotInited(const NilpotentGroup& theQuot, int theClass);
  void putHaveNilpotentGroupInited(const NilpotentGroup& theGroup, 
                                   Chars filename);
  void putIsFreeNilpotent(const FreeGroup& F, int theClass);
  void putIsNilpotent(int theClass);


  void putIsFreeByCyclic(const FreeGroup& F, const Map& aut);

  void putHaveMSC(FPGroup G, int lambda);

  void putHaveMSCLambda( int lambda );

  void putHaveAPOfFree( const AmalgProductOfFreeGroups& );

  void putHaveHNNofFree( const class HNNExtOfFreeGroup& );

  void putHaveSchreierTransvl(const PermutationRepresentation&);

  void putHaveWordDecomposer( const class DecomposeInSubgroupOfFPGroup& );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Reporting Secondary Information:                                    //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void postSecondaryLogMessage(Chars message);
  // This may not be such a good place for this, but we at least have
  // a central location for reporting secondary chatter about what, e.g.,
  // a computation manager is doing to the log of the group.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessing Information:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool haveOrder( ) const;
  const Integer& getOrder( ) const;
  const Chars    getOrderExplanation( ) const;
  const Chars    getOrderMessage( ) const; 

  //@dp: const Integer& getOrderUpperBound( ) const; 
  //@dp: const Integer& getOrderLowerBound( ) const;

  bool haveSolvedWordProblem( ) const;
  bool haveFastWordProblem( ) const;

  bool haveCompleteCayleyGraph( ) const;
  SubgroupGraph getCompleteCayleyGraph( ) const; 

  bool haveConfluentKBMachine( ) const;
  const KBMachine& getConfluentKBMachine( ) const;

  Trichotomy isAutomatic( ) const;
  bool haveAutomatic( ) const;
  const Chars getAutomaticMessage( ) const;
  const GroupDFSA& getWordAcceptor( ) const;
  const DiffMachine& getDiffMachine( ) const;

  bool isOneRelatorWithTorsion( ) const;
  bool isOneRelator( ) const;
  Word getOneRelator( ) const;
  // When the group is one relator (but not necessarily the given
  // presentation), this returns the relator.

  bool haveCyclicDecomposition( ) const;
  bool havePrimaryDecomposition( ) const;
  const AbelianGroup& getCyclicDecomposition( ) const; 
  const Chars getCyclicDecompositionMessage( ) const;

  Trichotomy  isAbelian( ) const;
  Trichotomy  isFree( ) const;
  Trichotomy  isFinite( ) const;
  const Chars getFiniteExplanation( ) const;
  const Chars getFiniteMessage( ) const;

  bool haveNilpotentQuotInited (int theClass) const;
  bool haveNilpotentGroupInited(            ) const;

  //@dp: bool haveLCSQuotients() const;

  const NilpotentGroup& getNilpotentQuotInited (int theClass) const;
  const NilpotentGroup& getNilpotentGroupInited(            ) const;
  const Chars& getBasisFileName() const { return BCfileName; }

  Trichotomy isFreeNilpotent( ) const;
  Trichotomy isNilpotent( ) const;
  int getNilpotentcyClass( ) const;

  void putActualNilpotencyClass(int c );
  bool haveActualNilpotencyClass( ) const;
  int  getActualNilpotencyClass ( ) const;


  bool isFreeByCyclic( ) const;
  const FreeByCyclic& getFreeByCyclic( ) const;

  bool haveMSC( ) const;
  const MSCGroup& getMSC( ) const;

  bool haveMSCLambda( ) const;
  int  getMSCLambda( ) const;
  Chars getMSCMessage() const;

  bool haveAPOfFree( ) const;
  const AmalgProductOfFreeGroups& getAPOfFree( ) const;

  bool haveHNNofFree( ) const;
  const class HNNExtOfFreeGroup& getHNNofFree() const;
  
  bool haveSchreierTransvl() const;
  const PermutationRepresentation& getSchreierTransvl() const;

  bool haveWordDecomposer( ) const;
  const class DecomposeInSubgroupOfFPGroup& getWordDecomposer( ) const;

private:

  GIC(const GIC&); // Disable copying


  AssociationsOf<int,NilpotentGroup*>& getNilpotentQuotients() const;
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  OID groupOID;
  // Need this for composing messages about what has been found.
  
  //  Trichotomy abelian;
  //  Trichotomy nilpotent;
  //  Trichotomy freeNilpotent;

  //  Trichotomy free;
  //  Chars freeExplanation;

  //  Trichotomy finite;
  //  Chars finiteExplanation;
  //  Chars finiteMessage;

  //  Integer order;
  //  Chars orderExplanation;
  //  Chars orderMessage;
  //  Integer orderUpperBound;
  //  Chars explainOrderUpperBound;
  //  Integer orderLowerBound;

  //  bool solvedWordProblem;
  //  bool fastWordProblem;
  //  Chars explainSolvedWordProblem;

  //  SubgroupGraph* theCayleyGraph;
  //  // Non-Null only when the group is finite, and we have built a
  //  // complete Cayley graph.

  //  bool KBMachineIsConfluent;
  //  KBMachine confluentKBMachine;
  //  // By default, machine accepting empty set

  //  Trichotomy IsAutomatic;
  //  Chars automaticMessage;
  //  GroupDFSA wordAcceptor;
  //  DiffMachine diffMachine;

  //  bool ORWithTorsion, OR;
  //  Word theOneRelator;
  //  // Is the relator iff the group is one relator.

  //  AbelianGroup* theAbelianGroup;
  //  // We may not have this yet. It is the structure of the group's
  //  // abelianization, which is the group itself when it is abelian.
  //  Chars theCyclicDecompositionMessage;
  //  FreeByCyclic* theFreeByCyclic;

  //  MSCGroup* theMSCGroup;
  //  // We may not have this yet. It is the MSC presentation in the
  //  // generators given by the end user.

  //  int MSCLambda;
  //  Chars MSCMessage;

  //  int nilpotencyClass;
  // Stores the class of nilpotent group. 

  //  int actualNilpotencyClass;
  // Computed class of nilpotent group.

  //AssociationsOf<int, NilpotentGroup*> theNilpotentQuotients;

  //  AmalgProductOfFreeGroups* theAPOfFree;
  //  class HNNExtOfFreeGroup* theHNNofFree;
  Chars BCfileName;
  //  PermutationRepresentation* permutationRepresentation;
  //  class DecomposeInSubgroupOfFPGroup *theWordDecomposer;
};


#endif
