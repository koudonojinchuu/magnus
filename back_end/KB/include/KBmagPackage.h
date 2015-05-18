// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of KBmagPackage class.
//
// Principal Author: Sarah Rees
//
// Status: in progress
//
// Revision History:
//
// * 06/24/95 Roger moved the ctors to .C, and made the static
//            const Chars cdbin
//            into a class data member, to both work around a compiler
//            bug, and to avoid problems with the order of static init.
//
// Next implementation steps:
//
// * Deal with the many @rn, @rn:@sr, @sr comments here and in KBmagPackage.C


#ifndef _KBmagPackage_H_
#define _KBmagPackage_H_

#include <iomanip.h>

#include "Vector.h"
#include "Chars.h"
#include "Set.h"
#include "Word.h"
#include "DFSA.h"
#include "DiffMachine.h"
#include "GenMult.h"
#include "WordOrder.h"

extern "C" {
  char* tempnam(const char*,const char *); 
         // Lib functions SunOS doesn't declare.
}
class KBmagPackage {
public:
 
  // KBmagPackage is a wrapper for Derek Holt's implementation of the automatic
  // group procedures which attempt to prove a group automatic by constructing
  // and verifying a shortlex automatic structure.
  // Each of the relevant programs corresponds to a method in this class,
  // which carries the name of Holt's program.
  // Various automata are computed by these programs, and are stored in
  // temporary files, but may be accessed by methods in this class.
  //
  // The method autgroup() runs the whole procedure. It returns YES if the
  // procedure completes and proves the group automatic, NO if the procedure
  // completes but fails to prove the group automatic, DONTKNOW if the procedure  // is unable to complete (probably through lack of memory). For a group which
  // is not automatic, autgroup() could run forever, so it is unwise to call it.  // It is wiser to run its constituent parts individually.
  //
  // These are as follows:-
  //
  // The method kbprog() runs a Knuth-Bendix procedure for a finite amount
  // of time, and, given enough resources, must eventually terminate,
  // when certain parameter bounds are met.
  // When it terminates a difference machine is computed.
  // YES is returned on successful termination, DONTKNOW if the
  // program cannot terminate through lack of resources.
  //
  // Once a difference machine has been supplied, the method gpwa() computes
  // a word acceptor from it. The construction is a finite logical operation,
  // and, given enough resources, must terminate.
  // Once a difference machine and word acceptor have been supplied, the
  // method gpgenmult() computes a general multiplier from it. Given
  // enough resources, this mult also terminate.
  // For both gpwa() and gpgenmult(),
  // YES is returned on successful termination, DONTKNOW if the
  // program cannot terminate through lack of resources.
  //
  // gpcheckmult() performs preliminary checks on the word acceptor and
  // general multipliers, and returns NO if those checks fails, YES on
  // successful termination and DONTKNOW if the program cannot terminate through  
  // lack of resources. In the case of a NO answer, additional word
  // differences are found and the difference machine is accordingly modified.
  // A new word acceptor and new general multiplier may be computed from
  // the modified difference machine, using gpwa() and gpgenmult()
  // and the preliminary checks run again.
  // The loop may be run until a YES answer is obtained from gpcheckmult()
  //
  // gpaxioms() performs further checks on the word acceptor and general
  // multipliers, and returns YES if those check verify that the automata
  // form an automatic structure, NO if the checks fail and DONTKNOW
  // if the checks fail to complete due to lack of resources.
  //
  // The sensible way to test whether or not a group is automatic is thus
  // to run kbprog(), then the sequence gpwa(), gpgenmult(), gpcheckmult()
  // repeatedly until a YES answer is obtained from gpcheckmult(),
  // then finally gpaxioms(). autgroup() runs the full suite of
  // programs as above, and gpmakefsa() runs the loop gpwa(), gpcheckmult(),
  // gpcheckmult(), stopping only when a YES answer is obtained.
  // Hence both of autgroup() and gpmakefsa() should be used only with extreme
  // caution.
  //
  // The automata may be recovered from the package using the
  // methods wordAcceptor(), differenceMachine() and generalMultiplier().
  // These automata may also be set, in which case any previously calculated
  // by the package will be overwritten using the methods setWordAcceptor(),
  // setDifferenceMachine() and setGeneralMultiplier(). Thus for instance
  // the method gpwa() may be used to compute a word acceptor from any
  // difference machine, and gpgenmult() may be used to compute the general
  // multiplier based on any difference machine and word acceptor.
  // In fact the automatic groups procedure builds in general two difference
  // machines, the first of which accepts a sublanguage of the second.
  //
  // The integer argument of the methods differenceMachine and
  // setDifferenceMachine() differentiates between those two automata. At the
  // present the second is always used by the methods gpwa() and gpgenmult() -
  // the Holt programs may be run to use the first as a space-saving device,
  // but currently this option is not available within this class.
  //
 
  // The Package is initialised with the list of generators and relators
  // of the groups,
  // and (optionally)
  // an integer array with specifies the order of the generators
  // and their inverses used in the shortlex ordering.
  // and an integer tidyint, which is a parameter for the KnuthBendix procedure. 




  KBmagPackage(const VectorOf<Chars>& genNames, const SetOf<Word>& rels,
          const WordOrder & word_order,int tidyint) ; 

  KBmagPackage(const VectorOf<Chars>& genNames, const SetOf<Word>& rels,
          const WordOrder & word_order) ; 

  KBmagPackage(const VectorOf<Chars>& genNames, const SetOf<Word>& rels);

  ~KBmagPackage();
  // Takes care of shutting down the programs, and unlinking temp files/pipes.
  Bool sanityCheck() const { return !error; }

// In each of the below, YES is returned if the program completes successfully
// having verified all checks which need to be made, and DONTKNOW is returned
// if for some reason the program cannot complete (usually because of lack of
// memory). NO is returned if the program completes and some checks have failed.
// In fact a NO answer is not possible from gpgenmult, gpmakefsa, gpwa or
// kbprog.
  Trichotomy autgroup();
  Trichotomy gpaxioms();
  Trichotomy gpcheckmult();
  Trichotomy gpgenmult(Bool eqcheck=YES);
  Trichotomy gpmakefsa(Bool eqcheck=YES);
  Trichotomy gpwa();
  Trichotomy kbprog (int haltingfactor=100,int tidyint=20,int maxeqns=200,
         int maxstates=1000);

  Chars getName() const { Chars s = problemName; return s;}

  Bool isProvedAutomatic( ) const { return provedAutomatic; }
  Bool isProvedConfluent( ) const { return provedConfluent; }


  Chars getGeneratorName(Generator g) const {
     int i = ord(g);
     if (i>0) return generatorNames[i-1];
     else return(generatorNames[-i-1]+"^-1");
  }

  GroupDFSA wordAcceptor();
  GenMult generalMultiplier();
  DiffMachine differenceMachine(int i);

  void setWordAcceptor(const GroupDFSA & WA);
  void setGeneralMultiplier(const GenMult & GM);
  void setDifferenceMachine(const DiffMachine & Diff,int i);

// and now versions of the above which use DFSARep's rather than DFSA's
// Maybe these should be private.
// They are necessary for use in code which needs to do serious manipulation
// with the automata, e.g. hyperbolicity testing.

  GroupDFSARep wordAcceptorRep();
  GenMultRep generalMultiplierRep();
  DiffMachineRep differenceMachineRep(int i);

  void setWordAcceptor(const GroupDFSARep & WARep);
  void setGeneralMultiplier(const GenMultRep & GMRep);
  void setDifferenceMachine(const DiffMachineRep & DiffRep,int i);
  Bool minimize(DFSA & D);
  Bool minimize(GroupDFSA & D);
  Bool minimize(DFSARep & D);
  Bool minimize(GroupDFSARep & D);
  Bool gpcomp(GroupDFSA & D1,GroupDFSA & D2,GroupDFSA & D3);
  Bool gpcomp(GroupDFSARep & D1,GroupDFSARep & D2,GroupDFSARep & D3);

private:

  int numOfSymbols;
  int tidyInterval;

  Chars problemName;     // Problem name supplied to KBmagPackage

  VectorOf<Chars> generatorNames;
  SetOf<Word> relators;
  WordOrder order;
  
  Bool error;
  Bool provedAutomatic;
  Bool provedConfluent;

  Chars cdbin;  //@rn Should be const, but then trouble initing.


  // Methods:

  Bool findInputFile(const Chars & fname);
  void createRWSFile();
  Word readWord(istream& istr);
  void printWord(ostream& str,const Word& w);
/*
  void restartProcess();
  void haltProcess();
*/
};

#endif  
