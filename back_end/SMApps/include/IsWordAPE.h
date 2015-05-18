// Contents: Declaration of classes IsWordAPE, IsWordAPEProblem, IsWordAPEARCer
//
// Principal Author: Alexander Ushakov
//
// Status: in progress
//
// Revision History:
//


#ifndef _IS_WORD_APE_H_
#define _IS_WORD_APE_H_


#include "TurnerProperSubgroupEnumerator.h"
#include "FreeGroup.h"
#include "Supervisor.h"
#include "RankOfSubgroup.h"


// ------------------------  IsWordAPEARCer ------------------------------ //


class IsWordAPEARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  IsWordAPEARCer( ComputationManager& boss, 
		  const class FreeGroup& group, 
		  const Word& word );

  ~IsWordAPEARCer();

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC interaction members:                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void runComputation( );
 
  void writeResults( ostream& );

  void readResults( istream& );
  
  Chars getFileName( ) const { return PSE->getFileName(); }

  Trichotomy getAnswer() const { return answer;}
  bool HaveSG() const { return haveSG; } 

  bool getSG(SGofFreeGroup& sg) {
    if (answer!=no) return true;
    sg = *theSG;
    return true;
  }

  //@njz: moved from private to be public to avoid complain
  //
  // ------------------------  pairSG_bool ------------------------------ //
  
  class pairSG_bool
    {
    public:
      
      pairSG_bool(const SGofFreeGroup& sg) : theSG(sg) , checked(false) { }
      pairSG_bool(const SGofFreeGroup& sg, bool b) : theSG(sg), checked(b) { }
      
      inline bool operator == (const pairSG_bool& sg) const { 
	return (theSG==sg.theSG);
      }
      
      inline int hash() const {
	return theSG.hash();
      }
      
      void setChecked( ) { checked=true; }
      bool getChecked( ) const { return checked; }
      
      SGofFreeGroup getSG() const { return theSG; }
      
    private:
      SGofFreeGroup theSG;
      bool checked;
      
    };
   //

private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const class FreeGroup theGroup;
  // given free group
  const Word theWord;
  // given word

  const static int period = 1000;

  Trichotomy answer;
  SGofFreeGroup* theSG;
  bool haveSG;
  // answer

  ProperSubgroupEnumerator* PSE;
  // Subgroup enumerator 

  void lookingup();
  // enumerate subgroup
  bool checkForPrimitive(SGofFreeGroup sg);
  // check word for primitive in subgroup sg
  void printSG( ostream& file, const SGofFreeGroup& subgroup );

  //@njz: pairSG_bool moved to be public
  //

};


// ------------------------  IsWordAPE ------------------------------ //


class IsWordAPE : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  IsWordAPE(class IsWordAPEProblem& problemObject );

  ~IsWordAPE( );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Trichotomy getAnswer() const { 
       return theARCer.getAnswer();
  }

  bool haveSG() const { 
       return theARCer.HaveSG();
  }

  bool getSG(SGofFreeGroup& sg) {
       return theARCer.getSG(sg);
  }

  Chars getFileName( ) { return theARCer.getFileName(); }
  // Returns name of the file with computation details


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Control:                                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void takeControl( );         // overrides ComputationManager

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // State Transitions:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void start( ) { };              // overrides ComputationManager

  void terminate( ) { };          // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const class SMWord& theWord;

  FreeGroup theGroup;

  IsWordAPEARCer theARCer;

};


// -------------------- IsWordAPEProblem --------------------------- //


class IsWordAPEProblem : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  IsWordAPEProblem(const class SMWord& w);


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const class SMWord& getWord( ) const { return theWord; }


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Front End Interfacing:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void viewStructure(ostream& ostr) const;  // overrides SMObject


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Control:                                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void takeControl( );         // overrides ComputationManager


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // State Transitions:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void start( ) { };          // overrides ComputationManager

  void terminate( ) { };      // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const SMWord& theWord;
 
  bool linkHasBeenSent;
  

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Subordinate Computation Managers:                                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Subordinate<IsWordAPEProblem, IsWordAPE> isWordAPE;
 
};

#endif
