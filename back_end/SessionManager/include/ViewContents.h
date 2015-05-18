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

#ifndef _VIEWCONTENTS_H_
#define _VIEWCONTENTS_H_

#include "global.h"

//#ifndef BSD
//#include "values.h"
//#endif
//

#include "Chars.h"
#include "Word.h"
#include "iostream.h"
#include "ARCSlotID.h"
#include "FreeGroup.h"
#include "OID.h"
#include "FEData.h"

class PValue
{
public:
  PValue(int p): ivalue(p), theType(INT) { }
  PValue(Chars p): cvalue(p), theType(CHARS) { }
  PValue(Word p): wvalue(p), theType(WORD) { }
  PValue(const PValue& p): 
    ivalue(p.ivalue),
    cvalue(p.cvalue),
    wvalue(p.wvalue),
    theType(p.theType) { }
  
  operator int( ) { 
    if ( theType != INT )
      error("PValue::operator int( ): value is not integer");
    return ivalue;
  }
  operator Chars( ) { 
    if ( theType != CHARS )
      error("PValue::operator Chars( ): value is not Chars");
    return cvalue;
  }
  operator Word( ) { 
    if ( theType != WORD )
      error("PValue::operator Word( ): value is not Word");
    return wvalue;
  }
private:
  enum VType { INT, CHARS, WORD };
  int ivalue;
  Chars cvalue;
  Word wvalue;
  VType theType;
};

//------------------------ ViewContent -------------------- //

class ViewContent
{
public:

  ViewContent(const Chars& name): theName(name) {}

  ViewContent(const ViewContent& v):  theName(v.theName) {}
  
  virtual const char* type( ) const = 0;

  virtual ViewContent* clone( ) const = 0;

  const Chars& name()const { return theName; }

  virtual void submit(ostream& ostr)const=0;

  virtual PValue getValue(Chars name ) const = 0;

  virtual void setValue( istream& is,Chars& errMsg,Chars name) = 0;
  
  virtual const ViewContent* contentOf( const Chars& name )const = 0;

private:
  
  Chars theName;

};
// ----------------------------- ViewContentCell -----------------------------//
struct ViewContentCell
{
  ViewContentCell( const ViewContent& content): c(content.clone()), n(NULL) { }
  const ViewContent& content() const { return *c; }
  ViewContentCell* next() {return n; }
  void setNext(ViewContentCell* next) { n = next; }
  ViewContent* pcontent() { return c; }
private:
  ViewContent* c;
  ViewContentCell* n;
};

// ----------------------------- EditInteger  -----------------------------//

class EditInteger : public  ViewContent
{
public:
  EditInteger( const Chars& name,const Chars& text,int change = 0,int def = 0,
	       int minV = 0,int maxV = MAXINT):
    ViewContent(name),
    theText( text ),
    changeable( change ),
    defaultValue( def ),
    minValue(minV),
    maxValue(maxV) { }

  EditInteger(const EditInteger& i):
    ViewContent(i),
    theText( i.theText ),
    changeable( i.changeable ),
    defaultValue( i.defaultValue ),
    minValue(i.minValue),
    maxValue(i.maxValue) { }

  EditInteger* clone() const { return new EditInteger(*this); }

  const char* type( ) const { return "int"; }

  PValue getValue(Chars name = Chars() ) const { return PValue( defaultValue ); }
 
  void setValue( istream& is,Chars& errMsg,Chars name = Chars()) { 
    int tmpI;
    is >> tmpI;
    if (tmpI < minValue) {
      errMsg = theText+Chars(": this parameter can't be less than ")
	+ Chars( minValue );
      return;
    }
    if (tmpI > maxValue) {
      errMsg = theText+Chars(": this parameter can't be greater than ")
	+ Chars( maxValue );
      return;
    }
    defaultValue = tmpI; 
  }

  const ViewContent* contentOf( const Chars& n ) const{
    if (name() == n ){
      return this;
    }
    return NULL;
  }

  void submit(ostream& ostr)const;

private:
  int changeable;
  int defaultValue;
  int minValue;
  int maxValue;
  Chars theText;
};

// ----------------------------- EditBool  -----------------------------//

class EditBool : public  ViewContent
{
public:
  EditBool( const Chars& name,const Chars& text,int change = 0,int def = 0):
    ViewContent(name),
    theText( text ),
    changeable( change ),
    defaultValue( def ) { }

  EditBool(const EditBool& i):
    ViewContent(i),
    theText( i.theText ),
    changeable( i.changeable ),
    defaultValue( i.defaultValue ) { }

  EditBool* clone() const { return new EditBool(*this); }

  const char* type( ) const { return "bool"; }

  PValue getValue(Chars name = Chars() ) const { return PValue( defaultValue ); }
 
  void setValue( istream& is,Chars& errMsg,Chars name = Chars()) { 
    int tmpI;
    is >> tmpI;
    defaultValue = tmpI; 
  }

  const ViewContent* contentOf( const Chars& n ) const{
    if (name() == n ){
      return this;
    }
    return NULL;
  }

  void submit(ostream& ostr)const;

private:
  int changeable;
  int defaultValue;
  Chars theText;
};

// ----------------------------- EditText  -----------------------------//

class EditText : public  ViewContent
{
public:
  EditText( const Chars& name,const Chars& text,int change = 0,Chars def = ""):
    ViewContent(name),
    theText( text ),
    changeable( change ),
    defaultValue( def ) { }
    

  EditText(const EditText& c):
    ViewContent(c),
    theText( c.theText ),
    changeable( c.changeable ),
    defaultValue( c.defaultValue ) { }

  virtual EditText* clone() const { return new EditText(*this); }

  const char* type( ) const { return "text"; }

  virtual PValue getValue(Chars name = Chars() ) const { 
    return PValue( defaultValue ); 
  }
 
  virtual void setValue( istream& is, Chars& errMsg, Chars name = Chars());

  const ViewContent* contentOf( const Chars& n ) const{
    if (name() == n ){
      return this;
    }
    return NULL;
  }

  void submit(ostream& ostr)const;
protected:

   Chars defaultValue;
   Chars theText;

private:
  int changeable;
};

// ----------------------------- EditWord  -----------------------------//

class EditWord : public  EditText
{
public:
  
  //@db 2.91 : temporary put default instantiation for 'f' and 'w' below,
  //           we likely need to remove instantiation for 'change' instead 
  
  EditWord( const Chars& name,const Chars& text,int change = 0,
	    const FreeGroup& f = FreeGroup(), const Word& w = Word() ):
    EditText( name, text, change),
      theGroup( f ),
      theWord( w )
    {
      //@njz
      //      ostrstream os;
      std::ostrstream os;
      //
      theGroup.printWord(os,theWord);
      os << '\0';
      defaultValue = os.str();
    }
  

  EditWord(const EditWord& c):
    EditText(c),
    theGroup( c.theGroup ),
    theWord( c.theWord ) { }

  EditWord* clone() const { return new EditWord(*this); }

  PValue getValue(Chars name = Chars() ) const { 
    return PValue( theWord ); 
  }
 
  void setValue( istream& is, Chars& errMsg, Chars name = Chars());

private:
 

  Word theWord;
  FreeGroup theGroup;
  
};

// ----------------------------- GroupWindow  -----------------------------//

class GroupWindow : public  ViewContent
{
public:
  GroupWindow(const Chars& name):
    ViewContent(name),
    children(NULL),
    last(NULL) { }
    
  GroupWindow(const GroupWindow& gw);

  ~GroupWindow();

  GroupWindow* clone() const { return new GroupWindow( *this ); }

  virtual const char* type( ) const { return "GroupWindow"; }

  virtual Chars properties() const { return ""; }

  void submit(ostream& ostr) const;

  PValue getValue(Chars name) const { 
    const ViewContent* tmp = contentOf( name ); 
    if ( !tmp )
      error("PValue getValue(Chars name) const : couldn't find a parameter ");
    if ( tmp == this )
      error("PValue getValue(Chars name) const :"
	    " GroupWindow doesn't have value ");
    return tmp->getValue( name ); 
  }

  void setValue(istream& is, Chars& errMsg,Chars name) {
    const ViewContent* consttmp = contentOf( name ); 
    if ( ! consttmp )
      error(" void setValue( PValue p) : couldn't find a parameter ");
    if ( consttmp == this )
      error("PValue getValue(Chars name) const : "
	    "GroupWindow doesn't have value ");
    
     ViewContent* tmp = (ViewContent* )consttmp;
     tmp->setValue( is , errMsg, name );     
  }

  void add(const ViewContent& c);

  const ViewContent* contentOf( const Chars& name ) const;

protected:
  
  ViewContentCell* children;
  ViewContentCell* last;
};

// ----------------------------- RadioButtonGroup  --------------------------//

class RadioButtonGroup :  public GroupWindow
{
public:
  RadioButtonGroup(const Chars& name): GroupWindow( name ) { }
     
  RadioButtonGroup(const RadioButtonGroup& bg): 
    GroupWindow( bg ), 
    selButton( bg.selButton){ }

  RadioButtonGroup* clone() const { return new RadioButtonGroup( *this ); }
 
  const char* type( ) const { return "RadioButtonGroup"; }

  void add(const ViewContent& c);

  PValue getValue(Chars name) const { 
    const ViewContent* tmp = contentOf( name ); 
    if ( !tmp )
      error("PValue getValue(Chars name) const : couldn't find a parameter ");
    
    if ( tmp == this )
      return selButton;
    
    return tmp->getValue( name ); 
  }
  
  
  void setValue( istream& is, Chars& errMsg, Chars name) {
    const ViewContent* consttmp = contentOf( name ); 
    if ( ! consttmp )
      error(" void setValue( PValue p) : couldn't find a parameter ");
    if ( consttmp == this ){
      is >> selButton;
    } else {
      ViewContent* tmp = (ViewContent* )consttmp;
      tmp->setValue(  is, errMsg, name );
    }     
  }

private:
  Chars selButton;

};

// ----------------------------- RadioButton  --------------------------//

class RadioButton :  public GroupWindow
{
public:
  RadioButton(const Chars& name, const Chars& text, int change = 0)
    : GroupWindow( name ), theText(text),changeable( change ) { }
     
  RadioButton(const RadioButton& bg): 
    GroupWindow( bg ), 
    theText(bg.theText),
    changeable( bg.changeable ){ }

  RadioButton* clone() const { return new RadioButton( *this ); }
 
  const char* type( ) const { return "RadioButton"; }

  Chars properties() const { 
    return  Chars(" \"") + theText + Chars("\" ") + Chars(changeable) + Chars(" "); 
  }
  
private:
  int changeable;
  Chars theText;
};

// ----------------------------- Subproblem  -----------------------------//

class Subproblem : public ViewContent, public FEData
{
public:

  Subproblem(const Chars& name, ARCSlotID asi, const Text& d, int value = 0)
    : ViewContent( name ),
      numOfARCs( value ),
      description( d ),
      theASI( asi ){ }

  Subproblem(const Subproblem& s)
    : ViewContent(s),
      numOfARCs( s.numOfARCs),
      description( s.description ),
      theASI( s.theASI ){ }

  
  const char* type( ) const { return "Subproblem"; }
  
  Subproblem* clone( ) const { return new Subproblem(*this);}
  
  PValue getValue(class Chars) const { }
  void setValue(istream& is, Chars& errMsg, class Chars) { }

  void submit(ostream& ostr)const;

  const ViewContent* contentOf(const class Chars & )const { }

private:
  
  ARCSlotID theASI;
  int numOfARCs;
  Chars description;
};
// ----------------------------- ParameterStructure -------------------------//


class ParameterStructure
{
public:
  ParameterStructure(OID o,const GroupWindow& p)
    : theParameters( p ), bossOID( o ) { }
  
  ParameterStructure(const ParameterStructure& p): 
    theParameters( p.theParameters ), bossOID( p.bossOID ) { }

  void read( istream& istr );

  PValue getValue(const Chars& name)const {
    return theParameters.getValue( name );
  }

private:
  GroupWindow theParameters;
  OID bossOID;
};



#endif
