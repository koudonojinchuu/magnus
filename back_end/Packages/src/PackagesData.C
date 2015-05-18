
#include "PackagesData.h"
#include "MagnusHome.h"
#include "List.h"
#include "SMFPGroup.h"
#include "FEData.h"



void Record::setID(int i) const
{
  Record* This = (Record*)this;
  This->id = i;
}

// -------------------------- PackageRecord ----------------------- //

//@njz
void PackageRecord::readFrom( istream& in)
//void PackageRecord::readFrom( std::istream& in)
//
{
  if (in.eof())
    return;
  in.read(name,21);
  name[20] = '\0';
  in.read(comm, 129);
  comm[128] = '\0';

  //@njz
  //  in.read(&cs, sizeof(int) );
  //  in.read(&os, sizeof(int) );
  //  int ID;
  //  in.read(&ID, sizeof(int) );
  //  setID( ID );
  in >> cs;
  in >> os;
  int ID;
  in >> ID;
  setID( ID );
  //
}

void PackageRecord::writeTo ( ostream& out ) const
{
  out.write(name,21);

  out.write(comm,129);

  //@njz
  //  out.write(&cs, sizeof(int) );
  //  out.write(&os, sizeof(int) );
  //  int ID = getID();
  //  out.write(&ID, sizeof(int) );
  //  out.flush();
  out << cs;
  out << os;
  int ID = getID();
  out << ID;
  out.flush();
  //
}


// -------------------------- ParamRecord ----------------------- //

void ParamRecord::readFrom( istream& in)
{
  if (in.eof())
    return;
  in.read(name,81);
  name[80] = '\0';

  //@njz
  //  in.read(&packID, sizeof(int) );
  //  int t;
  //  in.read(&t, sizeof(int) );
  in >> packID;
  int t;
  in >> t;
  //

  if (t == int(INT))
    pType = INT;
  else if(t == int(BOOL))
    pType = BOOL;
  else if(t == int(STRING))
    pType = STRING;
  else
    error(" ParamRecord::readFrom( istream& in) : can't recognize parameter type");
}

void ParamRecord::writeTo ( ostream& out ) const
{
  out.write(name,81);
  
  if (packID < 0 )
    error("Invalid package ID");
  
  //@njz
  //  out.write(&packID, sizeof(int) );
  out << packID;
  //

  int t = int(pType);
  //@njz
  //  out.write(&t, sizeof(int) );
  out << t;
  //
  out.flush();
}


// -------------------------- DatabaseOf ----------------------- //

template <class T>
DatabaseOf<T>::DatabaseOf(const Chars& f_name) : fileName(f_name), lastIDNumber( 1 )
{
  //  Chars dbPath = Chars(MagnusHome::magnusHome()) + Chars("/packages.dat");
  int l = fileName.length();
  
  // check extention 
  if ( fileName[l-1] == 't' &&  fileName[l-2] == 'a' && 
       fileName[l-3] == 'd'  && fileName[l-4] == '.') {
    Chars cfgFileName( fileName);
    cfgFileName[l-3]='c';cfgFileName[l-2]='f';cfgFileName[l-1]='g';
    
    cfgFile.open(cfgFileName,  ios::in | ios::out);
    if (!cfgFile)
      error("Can't open the database configuration file");

    if (!cfgFile.eof())
      //@njz
      //      cfgFile.read(&lastIDNumber, sizeof(int) );
      cfgFile >> lastIDNumber;
      //
    
  } else
    error("Does not look like a database file.");
  
  dataFile.open(f_name, ios::in | ios::out);
  if (!dataFile)
    error("Can't open the database");
}

template <class T>
DatabaseOf<T>::~DatabaseOf() 
{
  cfgFile.close();
  dataFile.close();
}

template <class T>
void DatabaseOf<T>::goFirst() 
{
  dataFile.clear();
  dataFile.seekg(0);
}

template <class T>
bool DatabaseOf<T>::goTo( int i)
{
  if ( i < 0)
    return false;

  dataFile.clear();
  
  int ofset = i*T::size(); 
  dataFile.seekg(ofset, ios::beg);
}

template <class T>
void DatabaseOf<T>::skip() 
{
  if (!dataFile.eof())
    dataFile.seekg(T::size(),ios::cur);
}

template <class T>
bool DatabaseOf<T>::done() const 
{
  DatabaseOf<T>* This = (DatabaseOf<T>* )this;
  This->dataFile.peek();
  return This->dataFile.eof();
}



template <class T>
T DatabaseOf<T>::getRecord() 
{
  T r;
  if (!dataFile.eof())
    r.readFrom(dataFile);
  return r;
}  


template <class T>
void  DatabaseOf<T>::append( const T& r )
{
  dataFile.clear();
  dataFile.seekp(0,ios::end);

  lastIDNumber++;
  r.setID(lastIDNumber);
  r.writeTo(dataFile);

  cfgFile.clear();
  cfgFile.seekg(0);
  //@njz
  //  cfgFile.write(&lastIDNumber, sizeof(int) );
  cfgFile << lastIDNumber;
  //
  cfgFile.flush();
}

template <class T>
void  DatabaseOf<T>::change( int i,const T& r )
{
  dataFile.clear();
  dataFile.seekp(i*T::size(),ios::beg);
  r.writeTo(dataFile);
}

template <class T>
void  DatabaseOf<T>::del( int i )
{
  int n =  nOfRecords()-1;
  T sr[n];
  
  goFirst();
  int c = 0;
  while (!done()){
    if ( c!=i ){
      if ( c >= i )
	sr[c-1] = getRecord();
      else
	sr[c] = getRecord();
    } else
      T r = getRecord();
    c++;
  }
  
  dataFile.close();
  ofstream tmp_out(fileName);
  for (int k=0;k<n;k++){
    sr[k].writeTo(tmp_out);
  }
  tmp_out.close();
  
  dataFile.open(fileName, ios::in | ios::out);
  if (!dataFile)
    error("Can't open the database");
}


template <class T>
int  DatabaseOf<T>::nOfRecords() const
{
  DatabaseOf<T>* pd = (DatabaseOf<T>*)this;
  streampos p = pd->dataFile.tellp();
  pd->dataFile.clear();
  pd->dataFile.seekp(0,ios::end);
  int n = pd->dataFile.tellp() / T::size();
  pd->dataFile.seekp(p,ios::beg);
  return n;

}
// -------------------------- Packages ----------------------- //

Chars Packages::getPackagePath(PObject* o)
{  
  Chars uniqPart = o->getDBName();

  Chars dbPath = Chars(MagnusHome::magnusHome()) + 
    Chars("/packages/") + uniqPart + Chars(".dat");
  
  return dbPath;
}

Chars Packages::getParamPath(PObject* o)
{  
  Chars uniqPart = o->getDBName();

  Chars dbPath = Chars(MagnusHome::magnusHome()) + 
    Chars("/packages/") + uniqPart + Chars("_param.dat");
  
  return dbPath;
}

void Packages::addPackage( const Chars& theName, const Chars& theCommand,
			   Type o, int t, const ListOf<ParamRecord>& params)
{
  PObject* po = getPObjectFromType( o );
  Chars dbPath = getPackagePath(po);
  DatabaseOf<PackageRecord> db(dbPath);
  //@njz
  //  iostream tmp;
  //  int io;
  //  tmp << o;
  //  tmp >> io;
  std::stringstream tmp;
  int io;
  tmp << o;
  tmp >> io;
  //
  PackageRecord r( theName, theCommand, t, io);
  db.append(r);


  DatabaseOf<ParamRecord> p_db(getParamPath(po));
  
  ListIterator<ListOf<ParamRecord> > I(params);
  for ( I.reset();!I.done();I.next() ){
    ParamRecord param = I.value();
    param.setPID(r.getID());
    p_db.append(param);
  }

  delete po;
}

PackageRecord Packages::getPackage( PObject* o,int id, ListOf<ParamRecord>& params)
{
  Chars dbPath = getPackagePath(o);
  DatabaseOf<PackageRecord> db(dbPath);

  db.goTo(id);

  PackageRecord pr =  db.getRecord();
  
  DatabaseOf<ParamRecord> p_db(getParamPath(o));
  ParamRecord param;
  for ( p_db.goFirst();!p_db.done(); ){
    ParamRecord paramr = p_db.getRecord();
    if (paramr.getPackageID() == pr.getID())
      params.append(paramr);
  }
 

  return pr;
}

void Packages::deletePackage(  PObject* o,int i)
{
  Chars dbPath = getPackagePath(o);
  DatabaseOf<PackageRecord> db(dbPath);

  db.goTo(i);
  PackageRecord pr = db.getRecord();

  DatabaseOf<ParamRecord> p_db(getParamPath(o));
  ParamRecord param;
  ListOf<int> delList;
  int ii=0;
  for ( p_db.goFirst();!p_db.done(); ii++){
    ParamRecord paramr = p_db.getRecord();
    if (paramr.getPackageID() == pr.getID()){
      delList.append(ii);
      ii--;
    }
  }
  
  ListIterator<ListOf<int> > I(delList);
  for (I.reset();!I.done();I.next())
    p_db.del(I.value());

  db.del(i);

  
}

int Packages::getNOfPackage( PObject* o)
{
  Chars dbPath = getPackagePath(o);
  DatabaseOf<PackageRecord> db(dbPath);

  return  db.nOfRecords();
}


void Packages::addPackagesToMenu(  )
{
  
  PGroup().addPackagesToMenu();

  PWord().addPackagesToMenu();
 
  PSubgroup().addPackagesToMenu();

  PMap().addPackagesToMenu();

  PHomo().addPackagesToMenu();

  PWordWord().addPackagesToMenu();

  PSubgroupSubgroup().addPackagesToMenu();

  PSubgroupWord().addPackagesToMenu();
}


void Packages::printCheckinTypes( ostream& out )
{
  out << "{ "
      << "{ {Abelian} " << (int)SMFPGroup::ABELIAN << " } "
       << "{ {Finitely Presented} " <<  (int)SMFPGroup::FP << " } "
       << "{ {Free} " <<  (int)SMFPGroup::FREE << " } "
       << "{ {Nilpotent} " <<  (int)SMFPGroup::NILPOTENT << " } "
       << "{ {Free Nilpotent} " <<  (int)SMFPGroup::FREE_NILPOTENT << " } "
       << "{ {One Relator} " <<  (int)SMFPGroup::ONE_RELATOR << " } "
       << "{ {Small Cancellation} " <<  (int)SMFPGroup::SMALL_CANCELLATION << " } "
       << "} ";
	
}


void Packages::printObjectNames( ostream& out )
{
  out << "{ ";
  PGroup().printObjectName( out );
  PWord().printObjectName( out );
  PSubgroup().printObjectName( out );
  PMap().printObjectName( out );
  PHomo().printObjectName( out );
  PWordWord().printObjectName( out );
  PSubgroupSubgroup().printObjectName( out );
  PSubgroupWord().printObjectName( out );
  out  << "} ";
}

void Packages::printObjectFileNames( ostream& out )
{
  out << "{ "
      << "{ " << PGroup().ipcFileName( ) << "} "
      << "{ " << PWord().ipcFileName(  )<< "} "
      << "{ " << PSubgroup().ipcFileName(  )<< "} "
      << "{ " << PMap().ipcFileName( )<< "} "
      << "{ " << PHomo().ipcFileName( )<< "} "
      << "{ " << PWordWord().ipcFileName(  )<< "} "
      << "{ " << PSubgroupSubgroup().ipcFileName(  )<< "} "
      << "{ " << PSubgroupWord().ipcFileName(  )<< "} "
      << "} ";
}

SMFPGroup::Checkin_Type Packages::getCheckinType( int n )
{
 if ((int)SMFPGroup::ABELIAN == n)
    return SMFPGroup::ABELIAN;
  else if((int)SMFPGroup::FP == n)
    return SMFPGroup::FP;
  else if((int)SMFPGroup::FREE == n)
    return SMFPGroup::FREE;
  else if((int)SMFPGroup::NILPOTENT == n)
    return SMFPGroup::NILPOTENT;
  else if((int)SMFPGroup::FREE_NILPOTENT == n)
    return SMFPGroup::FREE_NILPOTENT;
  else if((int)SMFPGroup::ONE_RELATOR == n)
    return SMFPGroup::ONE_RELATOR;
  else if((int)SMFPGroup::SMALL_CANCELLATION == n)
    return SMFPGroup::SMALL_CANCELLATION;
  else
    error("SMFPGroup::Checkin_Type Packages::getCheckinType( int n ) :" 
	  " can't recognize checkin type");
}

Chars Packages::getCheckinTypeName( int n )
{
 if ((int)SMFPGroup::ABELIAN == n)
    return Chars("Abelian");
  else if((int)SMFPGroup::FP == n)
    return Chars("Finitely presented");
  else if((int)SMFPGroup::FREE == n)
    return Chars("Free");
  else if((int)SMFPGroup::NILPOTENT == n)
    return Chars("Nilpotent");
  else if((int)SMFPGroup::FREE_NILPOTENT == n)
    return Chars("Free nilpotent");
  else if((int)SMFPGroup::ONE_RELATOR == n)
    return Chars("One relator");
  else if((int)SMFPGroup::SMALL_CANCELLATION == n)
    return Chars("Small cancellation");
  else
    error("SMFPGroup::Checkin_Type Packages::getCheckinType( int n ) :" 
	  " can't recognize checkin type");
}
