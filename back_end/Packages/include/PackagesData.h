#include <fstream.h>
#include "Chars.h"
#include "Menu.h"
#include "SMFPGroup.h"
#include "PackagesObject.h"
#include "Type.h" 

#ifndef _PackagesData_h_
#define _PackagesData_h_


class Record
{
public:

  Record(): id(0) { }
  
  //@njz
  virtual void readFrom( istream& in) = 0;
  virtual void writeTo ( ostream& out ) const = 0;
  //  virtual void readFrom( std::istream& in) = 0;
  //  virtual void writeTo ( std::ostream& out ) const = 0;
  //
  
  int getID() const { 
    
    if (id) return id;
    else error("PackageRecord::getID() : ID hasn't been initialized yet.");
  }
  
 
  // this is done from DatabaseOf only where record is usually const. parameter
  // that why need pseudo const. function
  void setID(int i) const;
  
private:
  int id;
 };


struct PackageRecord : public Record{
  PackageRecord(): Record(), cs( 0 ), os(0) { }
  PackageRecord(const Chars& n, const Chars& c, int csel, int osel ): Record(), cs(csel), os(osel){
    strncpy(name,n,20);  name[20] = '\0';
    strncpy(comm,c,128);  comm[128] = '\0';
  }
     

  //@njz  
  void readFrom( istream& in);
  void writeTo ( ostream& out ) const;
  //  void readFrom( std::istream& in);
  //  void writeTo ( std::ostream& out ) const;
  //
  
  Chars getName() const { return Chars(name); }
  Chars getCommand() const { return Chars(comm); }
  int getCheckinSelection() const { return cs; }
  int getObjectSelection() const { return os; }
  
   static int size()  { return 162; } // 3*4 - 3 integers + 21 + 129 
private:

  char name[21];
  char comm[129];
  int cs;
  int os;
 };


struct ParamRecord : public Record{
  enum PARAM_TYPE { INT, BOOL, STRING };
  ParamRecord(): Record(), packID( 0 ) { }
  ParamRecord(const Chars& n, PARAM_TYPE pt ): Record(), packID(-1), pType(pt){
    strncpy(name,n,80);  name[80] = '\0';
  }
     
  
  void readFrom( istream& in);
  void writeTo ( ostream& out ) const;
  
  Chars getName() const { return Chars(name); }
  int getPackageID() const { return packID; }
  ParamRecord::PARAM_TYPE getType() const { return pType; }
  
  void setPID(int i) { packID = i; }

  static int size()  { return 89; } // 2*4 - 2 integers + 81
private:


  char name[81];
  PARAM_TYPE pType;
  int packID;
 };

// -------------------------- PackageDatabase ----------------------- //

template <class T> class DatabaseOf
{
 public:
 

  // Constructors/destructor
  DatabaseOf(const Chars& );
  ~DatabaseOf();
  
  // iteration
  void goFirst();
  bool goTo( int i);
  // moves to ith position
  
  void skip();
  // moves current position one record ahead
  
  bool done() const;
  
  // accessors / modifiers
  T getRecord();
  // reads and returns record from current position (position is moved to
  // the end of the record)

  // @am  Most functions are implemented in a naive way. Need some optimization
  void append( const T& );
  void del( int i );
  void change( int i,const T& r );
  
  
    int nOfRecords() const;

 private:
  Chars fileName;
  fstream dataFile;
  fstream cfgFile;
  int lastIDNumber;

};


// @am some declaration problem !
template <class A1> class Menu1;

// -------------------------- Packages ----------------------- //

class Packages : protected FEData
{

public:
   enum PObjects { GROUP, WORD, SUBGROUP, MAP, HOMO, SUBGROUP_SUBGROUP, 
 		  WORD_WORD, SUBGROUP_WORD };

  Packages() { }
  void addPackage( const Chars& theName, const Chars& theCommand, Type o, int t, 
		   const ListOf<ParamRecord>& params);
  void deletePackage( PObject* o, int i);
  PackageRecord getPackage( PObject*,int, ListOf<ParamRecord>&);
  int getNOfPackage( PObject* o);


  static Chars getPackagePath(PObject* o);
  static Chars getParamPath(PObject* o);


  void addPackagesToMenu(  );
  static void printObjectNames( ostream& );
  static void printObjectFileNames( ostream& out );
  static void printCheckinTypes( ostream& );
  static SMFPGroup::Checkin_Type getCheckinType( int n );
  static Chars getCheckinTypeName( int n );

};

#endif
