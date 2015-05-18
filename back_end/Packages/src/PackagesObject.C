#include "PackagesObject.h"
#include "PackagesData.h"
#include "Menu.h"
#include "SMFPGroup.h"
#include "SMHomomorphism.h"

// -------------------------- PGroup ----------------------- //

void PGroup::printObjectName( ostream& o ) const {
  o << "{ {Group} " << getObjectType() << " } ";
}
 
Chars PGroup::getObjectName(  ) const {
  return Chars("group");
}

void PGroup::addPackagesToMenu(  )
{
  
  ofstream ipc_out(ipcFileName());
  ipc_out << getObjectType( ) << endl;

  Chars dbPath = Packages::getPackagePath(this);
  DatabaseOf<PackageRecord> db(dbPath);
  
  Menu1<SMFPGroup> M( Menu::PACKAGES );
  M.startItemGroup( );

  bool started = false;
  int packageID = 0;
  
  while (!db.done()){
    
    PackageRecord r = db.getRecord();
    if (r.getName() != Chars()){
      M.startItemGroup( CheckinType( 1 ) == 
			Packages::getCheckinType(r.getCheckinSelection()) );
      
      M.add( ReadGroupPackageID(), Text(r.getName()), Menu::PackageID(packageID));
      ipc_out << "Name: \"" << r.getName()  
	      << "\"    Object: Group   " <<   r.getCheckinSelection() << endl;
      packageID++;
    }
  }

  M.done();

  ipc_out.close();
}

const Type PGroup::thePGroupType =  Type(Type::unique() );


// -------------------------- PWord ----------------------- //

void PWord::printObjectName( ostream& o ) const {
  o << "{ {Word} " << getObjectType() << " } ";
}

Chars PWord::getObjectName(  ) const {
  return Chars("word");
}

void PWord::writeInitString(ostream& o)const
{
  theGroup.writeInitString( o );
  theGroup.getFPGroup().printWord( o,theWord );
  o << ";"  << "  (" << oidW << ")" << endl<< flush;
}

void PWord::addPackagesToMenu(  )
{
  
  ofstream ipc_out(ipcFileName());
  ipc_out << getObjectType( ) << endl;

  Chars dbPath = Packages::getPackagePath(this);
  DatabaseOf<PackageRecord> db(dbPath);
  
  Menu1<SMWord> M( Menu::PACKAGES );
  M.startItemGroup( );

  bool started = false;
  int packageID = 0;
  while (!db.done()){
       
    PackageRecord r = db.getRecord();
    if (r.getName() != Chars()){
      M.startItemGroup( CheckinType( Parent( 1 ) ) == 
			 Packages::getCheckinType(r.getCheckinSelection()) );
      
      M.add( ReadWordPackageID(), Text(r.getName()), Menu::PackageID(packageID));
      ipc_out << "Name: \"" << r.getName() << "\"  "  << "Object: Word   " <<   r.getCheckinSelection() << endl;
      packageID++;
    }
  }
  
  M.done();

  ipc_out.close();
}

const Type PWord::thePGroupType =  Type(Type::unique() );


// -------------------------- PSubgroup ----------------------- //

void PSubgroup::printObjectName( ostream& o ) const {
  o << "{ {Subgroup} " << getObjectType() << " } ";
}

Chars PSubgroup::getObjectName(  ) const {
  return Chars("subgroup");
}


void PSubgroup::writeInitString(ostream& o)const
{
  theGroup.writeInitString( o );
  theGroup.getFPGroup().printVectorOfWords( o,theSubgroup );
  o << ";"  << "  (" << oidS << ")" << endl<< flush;
}

void PSubgroup::addPackagesToMenu(  )
{
  
  ofstream ipc_out(ipcFileName());
  ipc_out << getObjectType( ) << endl;

  Chars dbPath = Packages::getPackagePath(this);
  DatabaseOf<PackageRecord> db(dbPath);
  
  Menu1<SMSubgroup> M( Menu::PACKAGES );
  M.startItemGroup( );

  bool started = false;
  int packageID = 0;
  while (!db.done()){
    
    PackageRecord r = db.getRecord();
    if (r.getName() != Chars()){
      M.startItemGroup( CheckinType( Parent( 1 ) ) == 
			 Packages::getCheckinType(r.getCheckinSelection()) );
      
      M.add( ReadSubgroupPackageID(), Text(r.getName()), Menu::PackageID(packageID));
      ipc_out << "Name: \"" <<r.getName() << "\"  "  << "Object: Subgroup   " <<   r.getCheckinSelection() << endl;
      packageID++;
    }
  }
  
  M.done();

  ipc_out.close();
}

const Type PSubgroup::thePGroupType =  Type(Type::unique() );


// -------------------------- PMap ----------------------- //

void PMap::printObjectName( ostream& o ) const {
  o << "{ {Map} " << getObjectType() << " } ";
}

Chars PMap::getObjectName(  ) const {
  return Chars("map");
}

void PMap::writeInitString(ostream& o)const
{
  theDomain.writeInitString( o );
  theRange.writeInitString( o );
  o << '{' << theMap << '}'  << "  (" << oidM << ")" << endl<< flush;
}

void PMap::addPackagesToMenu(  )
{
  
  ofstream ipc_out(ipcFileName());
  ipc_out << getObjectType( ) << endl;

  Chars dbPath = Packages::getPackagePath(this);
  DatabaseOf<PackageRecord> db(dbPath);

  Menu1<SMMap> M( Menu::PACKAGES );
  M.startItemGroup( );

  bool started = false;
  int packageID = 0;
  while (!db.done()){
     
     PackageRecord r = db.getRecord();
     if (r.getName() != Chars()){
       M.startItemGroup( CheckinType( Domain(1) ) == 
			  Packages::getCheckinType(r.getCheckinSelection()) &&
			  CheckinType( Range(1) ) == 
			  Packages::getCheckinType(r.getCheckinSelection()) );
       
       M.add( ReadMapPackageID(), Text(r.getName()), Menu::PackageID(packageID));
       ipc_out << "Name: \"" << r.getName() << "\"  "  << "Object: Map   " <<   r.getCheckinSelection() << endl;
      packageID++;
     }
  }
  
  M.done();

  ipc_out.close();
}

const Type PMap::thePGroupType =  Type(Type::unique() );



// -------------------------- PHomo ----------------------- //

void PHomo::printObjectName( ostream& o ) const {
  o << "{ {Homomorphism} " << getObjectType() << " } ";
}

Chars PHomo::getObjectName(  ) const {
  return Chars("homomorphism");
}

/*
void PHomo::writeInitString(ostream& o)const
{
  o << theMap.domain()  << "  (" << oidD << ")" << '\n' 
    << theMap.domain()  << "  (" << oidR << ")" << '\n' 
    << '{' << theMap << '}'  << "  (" << oidM << ")" << endl<< flush;
}
*/

void PHomo::addPackagesToMenu(  )
{
  
  ofstream ipc_out(ipcFileName());
  ipc_out << getObjectType( ) << endl;

  Chars dbPath = Packages::getPackagePath(this);
  DatabaseOf<PackageRecord> db(dbPath);

  Menu1<SMHomomorphism> M( Menu::PACKAGES );
  M.startItemGroup( );

  bool started = false;
  int packageID = 0;
  while (!db.done()){
     
     PackageRecord r = db.getRecord();
     if (r.getName() != Chars()){
       M.startItemGroup( CheckinType( Domain(1) ) == 
			  Packages::getCheckinType(r.getCheckinSelection()) &&
			  CheckinType( Range(1) ) == 
			  Packages::getCheckinType(r.getCheckinSelection()) );
       
       M.add( ReadHomoPackageID(), Text(r.getName()), Menu::PackageID(packageID));
       ipc_out << "Name: \"" << r.getName() << "\"  "  << "Object: Homomorphism   " <<   r.getCheckinSelection() << endl;
      packageID++;
     }
  }
  
  M.done();

  ipc_out.close();
}

const Type PHomo::thePGroupType =  Type(Type::unique() );


// -------------------------- PWordWord ----------------------- //

void PWordWord::printObjectName( ostream& o ) const {
  o << "{ {Two words} " << getObjectType() << " } ";
}

Chars PWordWord::getObjectName(  ) const {
  return Chars("two words");
}

void PWordWord::writeInitString(ostream& o)const
{
  theGroup.writeInitString( o );
  theGroup.getFPGroup().printWord( o,theWord1 );
  o << ";"  << "  (" << oidW1 << ")" << endl;
  theGroup.getFPGroup().printWord( o,theWord2 );
  o << ";"  << "  (" << oidW2 << ")" << endl<< flush;
}

void PWordWord::addPackagesToMenu(  )
{
  
  ofstream ipc_out(ipcFileName());
  ipc_out << getObjectType( ) << endl;

  Chars dbPath = Packages::getPackagePath(this);
  DatabaseOf<PackageRecord> db(dbPath);
  
  Menu2<SMWord,SMWord> M( Menu::PACKAGES );
  M.startItemGroup( );
  
  bool started = false;
  int packageID = 0;
  while (!db.done()){

    PackageRecord r = db.getRecord();
    if (r.getName() != Chars()){
      M.startItemGroup( Parent( 1 ) == Parent( 2 ) &&
 			 CheckinType( Parent(1) ) == 
 			 Packages::getCheckinType(r.getCheckinSelection()) );
      
      M.add( ReadWordWordPackageID(), Text(r.getName()), Menu::PackageID(packageID));
      ipc_out << "Name: \"" << r.getName() << "\"  "  << "Objects: Two words   " <<   r.getCheckinSelection() << endl;
      packageID++;
    }
  }
  
  M.done();

  ipc_out.close();
}

const Type PWordWord::thePGroupType =  Type(Type::unique() );


// -------------------------- PSubgroupSubgroup ----------------------- //

void PSubgroupSubgroup::printObjectName( ostream& o ) const {
  o << "{ {Two subgroups} " << getObjectType() << " } ";
}

Chars PSubgroupSubgroup::getObjectName(  ) const {
  return Chars("two subgroups");
}

void PSubgroupSubgroup::writeInitString(ostream& o)const
{
  theGroup.writeInitString( o );
  theGroup.getFPGroup().printVectorOfWords( o,theSubgroup1 );
  o  << "  (" << oidS1 << ")" << endl;
  theGroup.getFPGroup().printVectorOfWords( o,theSubgroup2 );
  o  << "  (" << oidS2 << ")" << endl<< flush;
}

void PSubgroupSubgroup::addPackagesToMenu(  )
{
  
  ofstream ipc_out(ipcFileName());
  ipc_out << getObjectType( ) << endl;

  Chars dbPath = Packages::getPackagePath(this);
  DatabaseOf<PackageRecord> db(dbPath);
  
  Menu2<SMSubgroup,SMSubgroup> M( Menu::PACKAGES );
  M.startItemGroup( );
  
  bool started = false;
  int packageID = 0;
  while (!db.done()){
    
    PackageRecord r = db.getRecord();
    if (r.getName() != Chars()){
      M.startItemGroup( Parent( 1 ) == Parent( 2 ) &&
 			 CheckinType( Parent(1) ) == 
 			 Packages::getCheckinType(r.getCheckinSelection()) );
      
      ipc_out << "Name: \"" << r.getName() << "\"  "  << "Objects: Two subgroups   " <<   r.getCheckinSelection() << endl;
      M.add( ReadSubgroupSubgroupPackageID(), Text(r.getName()), Menu::PackageID(packageID));
      packageID++;
    }
  }
  
  M.done();

  ipc_out.close();
}

const Type PSubgroupSubgroup::thePGroupType =  Type(Type::unique() );


// -------------------------- PSubgroupWord ----------------------- //

void PSubgroupWord::printObjectName( ostream& o ) const {
  o << "{ {Subgroup and word} " << getObjectType() << " } ";
}

Chars PSubgroupWord::getObjectName(  ) const {
  return Chars("subgroup and word");
}

void PSubgroupWord::writeInitString(ostream& o)const
{
  theGroup.writeInitString( o );

  theGroup.getFPGroup().printVectorOfWords( o,theSubgroup );
  o   << "  (" << oidS << ")" << endl;
  theGroup.getFPGroup().printWord( o,theWord );
  o << ";"  << "  (" << oidW << ")" << endl<< flush;
}

void PSubgroupWord::addPackagesToMenu(  )
{
  
  ofstream ipc_out(ipcFileName());
  ipc_out << getObjectType( ) << endl;
  
  Chars dbPath = Packages::getPackagePath(this);
  DatabaseOf<PackageRecord> db(dbPath);
  
  Menu2<SMSubgroup,SMWord> M( Menu::PACKAGES );
  M.startItemGroup( );
  
  bool started = false;
  int packageID = 0;
  while (!db.done()){
    
    PackageRecord r = db.getRecord();
    if (r.getName() != Chars()){
      M.startItemGroup( Parent( 1 ) == Parent( 2 ) &&
 			 CheckinType( Parent(1) ) == 
 			 Packages::getCheckinType(r.getCheckinSelection()) );
      
      M.add( ReadSubgroupWordPackageID(), Text(r.getName()), Menu::PackageID(packageID));
      ipc_out << "Name: \"" << r.getName() << "\"  "  << "Objects: Subgroup and word   " <<   r.getCheckinSelection() << endl;
      packageID++;
    }
  }
  
  M.done();

  ipc_out.close();
}

const Type PSubgroupWord::thePGroupType =  Type(Type::unique() );

// --------------------------------------------------------- //
PObject* getPObjectFromType( Type t)
{
  if (t==PGroup::type())
    return new PGroup();
  else if (t==PWord::type())
    return new PWord();
  else if (t==PSubgroup::type())
    return new PSubgroup();
  else if (t==PMap::type())
    return new PMap();
  else if (t==PWordWord::type())
    return new PWordWord();
  else if (t==PSubgroupSubgroup::type())
    return new PSubgroupSubgroup();
  else if (t==PSubgroupWord::type())
    return new PSubgroupWord();
  else if (t==PHomo::type())
    return new PHomo();
  else
    error("Can't recognize object");
}

PObject* getPObjectFromInt( int i)
{
  //@njz
  //  strstream io;
  std::strstream io;
  //
  io << i << flush;
  
  Type::notype n;
  Type t(n);
  io >> t;
  
  return getPObjectFromType(t);
}
