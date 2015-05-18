#ifndef _MARGIN_H_
#define _MARGIN_H_

#include "Chars.h"
#include "error.h"
//#include "OneRelatorGroup.h"
//#include "SubgroupOfOneRelatorGroup.h"
//#include "HNNExtOfORGroup.h"

#define DEBUG_RAW 1

#ifdef DEBUG_RAW
#  define DEBUG_PRINT(ostr,object)			{ ::debugPrint( (ostr), (object) ); }
#  define DEBUG_PRINT_WORD(ostr,group,word)		{ (ostr) << (word); }
#  define DEBUG_PRINT_VECTOR_WORDS(ostr,group,vector)	{ (ostr) << (vector); }
#else
#  define DEBUG_PRINT(ostr,object)			{ (ostr) << (object); }
#  define DEBUG_PRINT_WORD(ostr,group,word)		{ (group).printWord( (ostr), (word) ); }
#  define DEBUG_PRINT_VECTOR_WORDS(ostr,group,vector)	{ (group).printVectorOfWords( (ostr), (vector) ); }
#endif

void debugPrint( ostream& ostr, const class SubgroupOfOneRelatorGroup& S );
void debugPrint( ostream& ostr, const class OneRelatorGroup& G );
void debugPrint( ostream& ostr, const class OneRelatorGroupRep& G );
void debugPrint( ostream& ostr, const class HNNExtOfORGroupGeneric& H );

class Margin : public Chars 
{
public:

  Margin( ) : Chars(), ptr(0) { }

  void set( Margin& t ); 

  ~Margin() { 
    if( ptr ) 
      *(Chars *)ptr = (Chars)(*this); 
  }

  Margin *ptr;

};

extern Margin globalMargin;
extern int  rightMargin;

#endif
