
//#include "pps.h"
#include "PackagesData.h"
#include "Vector.h"
#include "ObjectSmith.h"
#include "OutMessages.h"
#include "IsNilpotentProblem.h"
#include "SMHomomorphism.h"

// -------------------------- EditPackage --------------------------- //

EditPackage::EditPackage( Type id, int i, Chars c  )
  : Supervisor( true ),
    theObject( getPObjectFromType( id ) ),
    theIndex( i ),
    theAction( c )

{
  resultIsFast();
}


void EditPackage::takeControl( )
{
  Packages ps;
  ListOf<ParamRecord> params;
  PackageRecord r = ps.getPackage(theObject, theIndex,params);

  ListIterator<ListOf<ParamRecord> > I(params);
  for (I.reset();!I.done();I.next()) {
   LogMessage msg;
   msg <<    "Parameters :  " << I.value().getName() << ", type " << int(I.value().getType());
   msg.send();
  }

  if ( theAction == Chars("delete") ) {
    ps.deletePackage( theObject, theIndex );
    theObject->addPackagesToMenu();
    
    LogMessage msg1;
    msg1 <<  "Package \\\"" << r.getName() <<"\\\" was deleted.";
    msg1.send();
  } else {
    Chars obj_name = theObject->getObjectName();
    PackageInfoMessage  msg(r.getName(),r.getCommand(),
			    Packages::getCheckinTypeName(r.getCheckinSelection()), 
			    obj_name);
    msg.send();
  }
}


void EditPackage::viewStructure(ostream& ostr) const
{
}

// -------------------------- AddPackage --------------------------- //

AddPackage::AddPackage(const Chars& n, const Chars& c, int cs, Type os,
		       const ListOf<ParamRecord>& pl)
  : Supervisor( true/*! G.gic.haveFastWordProblem()*/ ),
    //    theGroup( G ),
    theName( n ),
    theCommand( c ),
    checkinSelect( cs ),
    objectSelect( os ),
    paramList( pl )
{
  resultIsFast();
}


void AddPackage::takeControl( )
{

  Packages ps;
  ps.addPackage( theName, theCommand, objectSelect, checkinSelect, paramList);
  PObject* po = getPObjectFromType( objectSelect );
  po->addPackagesToMenu();
  delete po;
  
  LogMessage msg;
  msg << "Package \\\"" << theName << "\\\" was added.";
  msg.send();

}


void AddPackage::viewStructure(ostream& ostr) const
{
}


// ------------------- RunPackageARCer --------------------- //


void RunPackageARCer::runComputation( )
{
  
  if ( package.start())
    file << "Text { Package started. } \n" << flush;
  else {
    file << "Text { Couldn't start the package } \n" << flush;
    return;
  }

  //  Timer t(delay()*1000);
  while ( !package.finished() ) {
    Chars line;
    if (package.getLine(line)){
      file << line << endl <<  flush;
      //file.sync();
    }
    
  }
  
  file << "Text { Package finished. } \n" <<  flush;
}

void RunPackageARCer::writeResults( ostream& out ) 
{

}

void RunPackageARCer::readResults( istream& in )
{

}


// -------------------------- RunPackage --------------------------- //



RunPackage::RunPackage( SMFPGroup& G,  int id )
  : Supervisor( true ),
    theGroup( G ),
    theID( id ),
    started( false ),
    useGlobalLink( false ),
    pos(0),
    packageObject(new PGroup( G.getFPGroup() , G.oid(), G.gic.getNilpotentcyClass() ) )
    //  inStream( NULL )
  //arcer(*this, G.getFPGroup())
{
  initialize();
}


RunPackage::RunPackage( SMWord& w,  int id )
  : Supervisor( true ),
    theGroup( w.getParent() ),
    theID( id ),
    started( false ),
    useGlobalLink( false ),
    pos(0)
{
  PGroup G(w.getParent().getFPGroup() ,
	   w.getParent().oid(), 
	   w.getParent().gic.getNilpotentcyClass() );
  
  packageObject = new PWord( G, w.getWord(), w.oid() );
  initialize();
}

RunPackage::RunPackage( SMSubgroup& s,  int id )
  : Supervisor( true ),
    theGroup( s.getParent() ),
    theID( id ),
    started( false ),
    useGlobalLink( false ),
    pos(0)
{
  PGroup G(s.getParent().getFPGroup() ,
	   s.getParent().oid(), 
	   s.getParent().gic.getNilpotentcyClass() );
  
  
  packageObject =  new PSubgroup( G, s.getSubgroup().generators(), s.oid() );
  initialize();
}

RunPackage::RunPackage( SMMap& m,  int id )
  : Supervisor( true ),
    theGroup( m.getDomain() ),
    theID( id ),
    started( false ),
    useGlobalLink( false ),
    pos(0)
{
  PGroup D(m.getDomain().getFPGroup() ,
	   m.getDomain().oid(), 
	   m.getDomain().gic.getNilpotentcyClass() );
  
  PGroup R(m.getRange().getFPGroup() ,
	   m.getRange().oid(), 
	   m.getRange().gic.getNilpotentcyClass() );
  

  packageObject = new PMap( m.getMap(), D, R, m.oid() );

  initialize();
}


RunPackage::RunPackage( SMHomomorphism& m,  int id )
  : Supervisor( true ),
    theGroup( m.getDomain() ),
    theID( id ),
    started( false ),
    useGlobalLink( false ),
    pos(0)
{
  
  PGroup D(m.getDomain().getFPGroup() ,
	   m.getDomain().oid(), 
	   m.getDomain().gic.getNilpotentcyClass() );
  
  PGroup R(m.getRange().getFPGroup() ,
	   m.getRange().oid(), 
	   m.getRange().gic.getNilpotentcyClass() );
  

  packageObject = new PHomo( m.getMap(), D, R, m.oid() );
 
  initialize();
}

RunPackage::RunPackage( SMWord& w1, SMWord& w2, int id )
  : Supervisor( true ),
    theGroup( w1.getParent() ),
    theID( id ),
    started( false ),
    useGlobalLink( false ),
    pos(0)
    
{  
  
  PGroup G(w1.getParent().getFPGroup() ,
	   w1.getParent().oid(), 
	   w1.getParent().gic.getNilpotentcyClass() );
  
  packageObject = new PWordWord(G, w1.getWord(), 
			       w2.getWord() ,
			       w1.oid(), w2.oid() );

  initialize();
}

RunPackage::RunPackage( SMSubgroup& s1, SMSubgroup& s2, int id )
  : Supervisor( true ),
    theGroup( s1.getParent() ),
    theID( id ),
    started( false ),
    useGlobalLink( false ),
    pos(0)
{
  PGroup G(s1.getParent().getFPGroup() ,
	   s1.getParent().oid(), 
	   s1.getParent().gic.getNilpotentcyClass() );
  
  
  packageObject  = new PSubgroupSubgroup(G, 
					 s1.getSubgroup().generators(), 
					 s2.getSubgroup().generators(),
					 s1.oid(), s2.oid());
  
  initialize();
}

RunPackage::RunPackage( SMSubgroup& s, SMWord& w, int id )
  : Supervisor( true ),
    theGroup( s.getParent() ),
    theID( id ),
    started( false ),
    useGlobalLink( false ),
    pos(0)
{
  PGroup G(s.getParent().getFPGroup() ,
	   s.getParent().oid(), 
	   s.getParent().gic.getNilpotentcyClass() );

  
  packageObject = new PSubgroupWord(G, 
				   s.getSubgroup().generators(), 
				    w.getWord(), 
				    s.oid(), w.oid() );
  
  initialize();
}


void RunPackage::initialize(  )
{
  Packages ps;
  thePackageRecord = ps.getPackage(packageObject, theID, params);
  
  packageName =  thePackageRecord.getName();
}


void RunPackage::start() {
  

  ListOf<Chars> paramList;

   
  ListIterator<ListOf<ParamRecord> > I(params);
  int pname = 0;
  for (I.reset();!I.done();I.next(), pname++) {
    Chars pstring;
    pstring  = paramToChars(I.value().getName(),  
			    getParameters().getValue(Chars(pname)), 
			    I.value().getType());
    paramList.append( pstring );
  }

   arcer = new RunPackageARCer( *this, packageObject, thePackageRecord.getCommand(), paramList);


}

void RunPackage::takeControl( )
{
  if ( freeARCs() > 0 ){

    bool arcerStatus =  true;

    bool finished = arcer->takeControl();

    sendOutput();

    if( finished ) {
      
      adminTerminate();
      return;
    }
    else
      usedOneARC();
  }
}


void RunPackage::readline(istream& in, char* line, int blength)
{
  int p=-1;
  while (!in.eof()){
    p++;
    if (p >= blength)
      error("void RunPackage::readline(istream& , char*, int ) : line length over the limit. ");
    
    in.read(&line[p],1);
    
    if (line[p] == '\n')
      break;
  }
  line[p]='\0';
}

Chars RunPackage::paramToChars(Chars name, PValue value, ParamRecord::PARAM_TYPE t) const 
{
  Chars pstring;
  Chars tmpV;
  if (t == ParamRecord::INT){
    pstring = Chars("Integer");
    int i = (int)value;
    tmpV = Chars(i);
  } else if (t == ParamRecord::BOOL){
    pstring = Chars("Boolean");
    int i = (int)value;
    tmpV = Chars(i);
  } else if ( t == ParamRecord::STRING){
    pstring = Chars("String");
    tmpV = value;
  }else
    error("Type is not supported yet");
  return pstring + Chars(" ( ") + name + Chars(") (  ") + tmpV + ") ";
}

void RunPackage::sendOutput()
{

  ifstream in(arcer->getFileName());
  in.seekg(pos);

  char theline[1000]; 
  while (!in.eof()){
    readline(in,theline,999);
    pos = in.tellg();

    
    
    if (strlen(theline) > 0 ){
      char* tok = strtok(theline, "{");
      char trimtok[1000];
      strcpy(trimtok, tok);
      trimall(trimtok);
      
      if ( !actionParser(trimtok) )  {

	//  all output goes into a file link

	if (!started){
	  
	  LogMessage msg1;
	  msg1 << Link(Chars( "Click here to see the details from ") + packageName,
		       "Package",
		       file.getFileName(), true );
	  msg1.send();
	  started = true;
	  useGlobalLink = true;
	}
	file << theline << endl;
	
      }
    }
  }
  in.close();
}

bool RunPackage::actionParser(char* s) const
{
  // send plain text to the log
  if ( strcmp(s, "Text") == 0){

    char* tok = strtok(NULL, "}");
    LogMessage msg;
    msg << tok;
    msg.send();
   
    return true;
  }
   
  // create a file link 
  if (strcmp(s, "Link") == 0){
    
    char* text = strtok(NULL, ",");
    char* filename = strtok(NULL, "}");
    
    LogMessage msg1;
    msg1 << Link(Chars( text ),
		 "Package",Chars(filename), true );
    msg1.send();
    
    ofstream colorf(Chars(filename)+Chars(".color"),ios::app);
    colorf << "#0000000%000@" << flush;
    colorf.close();
  
    return true;
  }



  
  // Create an object in the workspace
  if (strncmp("Create", s, strlen("Create")) == 0 ){
    
    // get object description and parent
    char* description = strtok(NULL, "|");
    
    Chars errMsg;

    // create a group
    if (strcmp(s, "CreateGroup")==0){
      FPGroup G;
      //@njz
      //      strstream ss;
      std::strstream ss;
      //
      ss << description << endl;
      ss >> G;
      
      SMFPGroup* smo =
	new SMFPGroup( G,Text("The group created from a package") );
      
      
      ListOf<OID> dependencies;
      CheckinMessage( *smo, "", dependencies ).send();
      return true;
    } else { // if not a group than ...
      
      char* soid = strtok(NULL, "|" );
      
      int oid = atoi( soid );
      OID parentOID( oid );
      
      // @am just for debug purposes
      LogMessage msg;
      msg << s << ":  " << description  <<  " for " << oid << ".";
      msg.send();   
      
      
      SMObject* pGroup = TheObjects::get( parentOID );
      
      if ( pGroup == NULL ){
	LogMessage msg;
	msg << "Couldn`t create an object from a package: cannot find parent's information.";
	msg.send();
      } else {
	
	//@njz
	//	strstream ss;
	std::strstream ss;
	//
        
	// create a word in a workspace
	if (strcmp(s, "CreateWord") == 0) {
	  ss << description << endl;
	  
	  Word w = ((SMFPGroup*)pGroup)->getFPGroup().readWord(ss,errMsg);
	  if (errMsg.length()==0){
	    SMObject* smo =
	      new SMWord( *((SMFPGroup*)pGroup),w,
			  Text("The word created by a package" )
			  );
	    
	    ListOf<OID> dependencies(*((SMFPGroup*)pGroup) );
	    CheckinMessage( *smo, "", dependencies ).send();
	  } else {
	    LogMessage msg;
	    msg << "Couldn`t create a word from a package: " << errMsg;
	    msg.send();
	  }
	  return true;
	}
	
	// create a subgroup in a workspace
	if (strcmp(s, "CreateSubgroup") == 0) {
	  ss << "{ " << description << " }" << endl;
	  VectorOf<Word> sg = ((SMFPGroup*)pGroup)->getFPGroup().readVectorOfWords(ss,errMsg);
	  
	  if (errMsg.length()==0){
	    
	    SGofFreeGroup sgtmp(((SMFPGroup*)pGroup)->getFreePreimage() , sg );
	    
	    SMObject* smo =
	      new SMSubgroup( *((SMFPGroup*)pGroup),sgtmp,
			      Text("The subgroup created by a package" )
			      );
	    
	    ListOf<OID> dependencies( *((SMFPGroup*)pGroup) );
	    CheckinMessage( *smo, "", dependencies ).send();
	  } else {
	    LogMessage msg;
	    msg << "Couldn`t create a subgroup from a package: " << errMsg;
	    msg.send();
	  }
	  return true;
	}
	
	// create a map in a workspace
	if (strcmp(s, "CreateMap") == 0) {
	  
	  char* soid2 = strtok(NULL, "|" );
	  
	  int oid2 = atoi( soid2 );
	  OID parentOID2( oid2 );
	  
	  // read range
	  SMObject* pGroup2 = TheObjects::get( parentOID2 );
	  
	  Map tmpmap(((SMFPGroup*)pGroup)->getFPGroup(),((SMFPGroup*)pGroup2)->getFPGroup());
	  
	  ss << "{ " << description << " }" << endl;
	  ss >> tmpmap;
	  
	  //	if (errMsg.length()==0){
	  
	  SMObject* smo =
	    new SMMap( *((SMFPGroup*)pGroup), *((SMFPGroup*)pGroup2),tmpmap,
		       Text("The map created by a package" )
		       );
	  
	  ListOf<OID> dependencies( *((SMFPGroup*)pGroup) );
	  dependencies.append( *((SMFPGroup*)pGroup2) );
	  CheckinMessage( *smo, "", dependencies ).send();
	  // 	} else {
	  // 	  LogMessage msg;
	  // 	  msg << "Couldn`t create a subgroup from a package: " << errMsg;
	  // 	  msg.send();
	  // 	}
	  return true;
	}
      }
    }
  }
  
  return false;
}


void RunPackage::viewStructure(ostream& ostr) const
{
  
  
  EnumertatorProblemView pv(oid(),Text(packageName),Text(packageName),
			    helpID("RunPackage",theGroup),
			    Text(packageName)
			    );


  ListIterator<ListOf<ParamRecord> > I(params);
  int pname = 0;
  for (I.reset();!I.done();I.next(), pname++) {
    if (I.value().getType() == ParamRecord::INT)
      pv.addParameter(EditInteger( Chars(pname), I.value().getName(),1,0,-MAXINT,MAXINT) );
    else if ( I.value().getType() == ParamRecord::STRING)
      pv.addParameter(  EditText( Chars(pname),I.value().getName(),1,"") );
    else if ( I.value().getType() == ParamRecord::BOOL)
      pv.addParameter(  EditBool( Chars(pname),I.value().getName(),1,0) );
    else
      error("Type is not supported yet");

  }
  /*
  pv.addParameter(EditBool( "bool", "Some bool parameter",1,0) );
  
  RadioButton rRB1("RB1","Use Some button parameter");
  RadioButton rRB2("RB2","Do not use some button parameter");
  
  RadioButtonGroup RBG("RBG");
  RBG.add(rRB1);
  RBG.add(rRB2);
  pv.addParameter(RBG);
  */
   pv.addProblem( Subproblem("P1",ThisARCSlotID(),
			     Text("For the use of this package"),100));
   
  
   submit(ostr, pv);
}

void RunPackage::trimall( char* s, int max)const { 
  int shift = 0;
  int i=0;
  for(i=0;i<max && s[i]!='\0';i++){
    s[i-shift] = s[i];
    if (s[i] == ' ') shift++;
  }
  s[i-shift] = '\0';
}


// ------------- ObjectSmith::outputPackageMenuDefns ---------------------- //


void ObjectSmith::outputPackagesMenuDefns(ostream& ostr)
{
  {
    Menu0 M( Menu::PACKAGES );
    M.startItemGroup();
    
    M.add( ReadPackage(), Text("Add a package"), 
	   DefinePackage() );

    M.add( ReadEditPackage(), Text("Edit the list of packages"), 
	   DefineEditPackage() );

    M.done();
  }
  
  Packages().addPackagesToMenu(  );

}
