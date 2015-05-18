// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: 
//           
//
// Principal Author: Alexei Myasnikov
//
// Status: in progress
//
// Revision History:
//


#include "ViewContents.h"
#include "conversions.h"
#include "OutMessages.h"
#include "WordEnumerator.h"
#include "EquationParser.h"

// ----------------------------- EditInteger  -----------------------------//
void EditInteger::submit(ostream& ostr)const {
  ostr << "{ " << type() << " " <<  name() << " \"" << theText << "\" " 
       << changeable << " " << defaultValue << " " << minValue << " " << maxValue << " }"; 
}

// ----------------------------- EditInteger  -----------------------------//
void EditBool::submit(ostream& ostr)const {
  ostr << "{ " << type() << " " <<  name() << " \"" << theText << "\" " 
       << changeable << " " << defaultValue << " }"; 
}

// ----------------------------- EditText  -----------------------------//
void EditText::submit(ostream& ostr)const {
  ostr << "{ " << type() << " " <<  name() << " \"" << theText << "\" " 
       << changeable << " \"" << defaultValue << "\" " << " }"; 
}

void  EditText::setValue( istream& is, Chars& errMsg, Chars name ) { 
  //Chars c;
    is > defaultValue;
    /*   defaultValue = "";
    char ch;
    for (int i = 0;i<c.length();i++) {
      ch = c[i];
      if ( ch == '\377' )
	defaultValue += ' ';
      else
	defaultValue += ch;
    }
    */
}
// ----------------------------- EditWord  -----------------------------//
void EditWord::setValue( istream& is, Chars& errMsg,  Chars name){
  EditText::setValue(is,name);
  
  //PatternParser p( defaultValue );
  //@njz
  //  istrstream istr( defaultValue );
  std::istrstream istr( defaultValue );
  //
  
  EquationParser p( istr );
  Chars m;
  VectorOf<Chars> newNames;
  theWord = p.parseEquation(  VectorOf<Chars>(),newNames,m );
  
  if (m.length() > 0) {
    bool start = false;
    errMsg = theText + Chars(": "); 
    for (int i=0;i<m.length();i++){
      if (m[i] == '}') break;
      if ( start ) errMsg += m[i];
      if (m[i] == '{') start = true;
    }
    
    theWord = Generator(1);
  }

  if (theWord.cyclicallyReduce().length() == 0)
    errMsg = theText + Chars(": the generic word can not be trivial "
			     "in the cyclically reduced form."); 
			     
}

// ----------------------------- GroupWindow  -----------------------------//

GroupWindow::GroupWindow(const GroupWindow& gw): 
  ViewContent(gw), children(NULL), last(NULL)        
{
  
  ViewContentCell* from = gw.children;
  while (from){
    ViewContentCell* tmpC = new ViewContentCell(from->content());
    if (children == NULL)
      last = children = tmpC;
    else {
      last->setNext(tmpC);
      last = last->next();
    }
    
    from = from->next();
  }
}
  


GroupWindow::~GroupWindow() {
  ViewContentCell* from = children;
  while (from){
    ViewContentCell* next = from->next();
    delete from;
    from = next;
  }    
}

void GroupWindow::submit(ostream& ostr) const {
  ostr << "{ " << type() << " " << name() << properties()  ;
  ViewContentCell* from = children;
  while (from){
    ostr << " ";
    from->content().submit(ostr);
    from = from->next();
  }
  ostr << " }" ;
}

void GroupWindow::add(const ViewContent& c){
  ViewContentCell* addNew = new ViewContentCell(c);
  if (children == NULL){
    last = children = addNew;
    return;
  }
  last->setNext(addNew);
  last = addNew;
}

const ViewContent* GroupWindow::contentOf( const Chars& n ) const
 
{
  if (name() == n){
    return this;
  }
  
  ViewContentCell* from = children;
  while (from){
    const ViewContent* tmp = from->content().contentOf(n);
      if ( tmp ){
      return tmp;
    }
    from = from->next();
  }    
  return NULL;
}

// ----------------------------- RadioButtonGroup  --------------------------//

void RadioButtonGroup::add(const ViewContent& c){
  if (strcmp(c.type(), "RadioButton") !=0 )
    error("Wrong type of parameter");
  if ( selButton.length() == 0)
    selButton = c.name();

  GroupWindow::add(c);
}




// ----------------------------- Subproblem  -----------------------------//
void Subproblem::submit(ostream& ostr)const {
  ostr << "{ " << type() << " " << name() << " "<< numOfARCs 
       << " {" << Text(description) << "} " <<  theASI << " helpID foo }"; 
}


// ----------------------------- ParameterStructure -------------------------//

void ParameterStructure::read( istream& istr )
{
  bool haveErrors = false;
  Chars type = "";
  Chars name;
  istr >> type;
  
  while ( type != Chars("{EOF_PARAM}")) { 
    
    istr >> name; 
    if (type == Chars("int") || type == Chars("RadioButtonGroup")
	|| type == Chars("text") || type == Chars("bool")) {
      Chars errMsg;
      theParameters.setValue(istr,errMsg, name);
      if (errMsg.length() > 0 ) {
	ParseParamErrorMessage(bossOID,name,type,errMsg).send();
	haveErrors = true;
      }
    } else
      error("void ParameterStructure::read( istream& istr ) : "
	    "unexpected type of a parameter");
    istr >> type;
  }
  
  if (!haveErrors)
    ParseParamOk(bossOID).send();
}

