#include "Menu.h"
#include "fastProblems.h"
#include "SMFPGroup.h"
#include "PackagesData.h"
#include "PackagesObject.h"

#ifndef _SMAppsPackages_h_
#define _SMAppsPackages_h_


// ------------------------- PackageBlackBox --------------//

class PackageBlackBox {
public:

  PackageBlackBox( PObject* po, const Chars&, const ListOf<Chars>& pl ); 
  
  ~PackageBlackBox(){}
  /////////////////////////////////////////////////////////////////////// 
  //                                                                   //
  // Methods                                                           //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////

  bool start();

  bool finished();

  bool getLine( Chars& line );

private:

  friend class RunPackageARCer;

  BlackBox package;
  FPGroup theGroup;
  Chars initString;
  Chars theCommand;
  PObject* theObject;
  
  ListOf<Chars> paramList;
};

// -------------------------- AddPackage --------------------------- //


class AddPackage : public Supervisor
{

public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

 AddPackage(const Chars& n = Chars(), const Chars& c = Chars(), 
	    int cs = 0, Type os = Type(Type::notype()), const ListOf<ParamRecord>& pl = ListOf<ParamRecord>() );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

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

  void start( ) { }           // overrides ComputationManager

  void terminate( ) { }       // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

 // SMFPGroup& theGroup;

  Chars theName;
  Chars theCommand;
  int checkinSelect;
  Type objectSelect;
  ListOf<ParamRecord> paramList;


};

// -------------------------- EditPackage --------------------------- //


class EditPackage : public Supervisor
{

public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

 EditPackage( Type id = Type( Type::notype() ), int i = 0, Chars action = Chars() );
 
 ~EditPackage() { delete theObject; }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

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

  void start( ) { }           // overrides ComputationManager

  void terminate( ) { }       // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  PObject* theObject;
  int theIndex;
  Chars theAction;
};

// ------------------ RunPackageARCer ---------------------- //


class RunPackageARCer : public ARCer
{
public:
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
 /////////////////////////////////////////////////////////////////////////
 
  RunPackageARCer( ComputationManager& boss, PObject* po, const Chars& com, 
		   const ListOf<Chars>& pl ) : 
    ARCer( boss ),
    package(po,com, pl)
    {   
    } 

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  // Inherited from ARCer:
  // bool takeControl( );

  Chars getFileName() const { return file.getFileName();}

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC interaction members:                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void runComputation( );
 
  void writeResults( ostream& );

  void readResults( istream& );

  
private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  PackageBlackBox package;
  File file;
};



// -------------------------- RunPackage --------------------------- //


class RunPackage : public Supervisor
{

public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  RunPackage( SMFPGroup& G, int id = 0);
  
  RunPackage( SMWord& w, int id = 0);
  
  RunPackage( SMSubgroup& s, int  id = 0);
  
  RunPackage( SMMap& m, int id = 0);

  RunPackage( SMHomomorphism& m, int id = 0);

  RunPackage( SMWord& ,SMWord& , int id = 0);
  
  RunPackage( SMSubgroup& ,SMSubgroup& , int id = 0);

  RunPackage( SMSubgroup& ,SMWord& , int id = 0);

  void initialize();

  ~RunPackage() { delete arcer; delete packageObject; }
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
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

  void start( );           // overrides ComputationManager

  void terminate( ) { }       // overrides ComputationManager

  void readline(istream& in, char* line, int blength);

private:

  Chars paramToChars(Chars name, PValue value, ParamRecord::PARAM_TYPE t) const ;

  void trimall( char* s, int max = 1000)const;
  
  bool actionParser(char* ) const;

  void sendOutput();

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  RunPackageARCer* arcer;
  SMFPGroup& theGroup;


  int theID;
  Chars packageName;
  PackageRecord thePackageRecord;
  PObject* packageObject;
  ListOf<ParamRecord> params;
  bool started;

  bool useGlobalLink;
  streampos pos;

  // char tmpBuffer[1000];

  //  ifstream* inStream;
  File file;
};



#endif
