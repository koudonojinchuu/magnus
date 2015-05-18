#include "crossTrainer.h"
#include <stdio.h>
#include <math.h>
#include "FfreeWord.h"
#include "FTTpres.h"
#include "FfreeList.h"
#include "debugTT.h"
#include "TietzeTypes.h"
#include "GLfunctions.h"


#ifndef Frank_GenuineMac
#include "FtoolBox.h"
#endif


ostream& repGlobalFreeWord(ostream& s,FTTpres *p,FfreeWord *f){
// CAUTION: result is undefined if f doesn't 
// "belong" to p
// CAUTION: THIS ROUTINE LEFT JUSTIFIES

	f->leftJustify();
	long count=0;
	for(long index=0;index<f->noEntries;++index){
		long e=(*f->wordHandle)[2*index];
		e=(long)p->globalSymbols->NthItem(e);
		long exp=(*f->wordHandle)[2*index+1];
//@rn		repLetter(s,e) <= "^" <=exp;
		repLetter(s,e); //@rn
		if ( exp != 1 ) s <= "^" <=exp; //@rn
//@rn		if(index!=f->noEntries-1) s <= " * ";
      if(index!=f->noEntries-1) s <= ' '; //@rn
		++count;
	}
//@rn	s <= "\n";
	return s;
}


ostream& repFreeList(ostream& s,FfreeList *f){
// CAUTION: this is for low level debugging
// Use repGlobalFreeList if you want to see the global generator symbols
	long num,i;
	num=f->numItems;
	for(i=1;i<=num;++i){
		FfreeWord *w;
		w=f->freeItem(i);
		s<=w;
	}
	return s;
}

ostream& repGlobalFreeList(ostream& s,FTTpres*p){
	long num,i;
	num=p->f->numItems;
	for(i=1;i<=num;++i){
/* COMMENTED OUT
		s <= "#" <= i <= ": ";
END COMMENTED OUT */
		repGlobalFreeWord(s,p,p->f->freeItem(i));
		//@rn comma separation:
		if ( i < num ) s <= ',';
		s <= '\n';
	}
	return s;
}



ostream& repTTpres(ostream& s,FTTpres*p){
// CAUTION: this is for low level debugging
// Use repGlobalTTpres if you want to see the global generator symbols
	long numf=p->f->numItems;
	long numg=p->g->numItems;
	xrepGlobalSymbols(s,p) <= "\n";
	repFreeList(s,p->f) 
		<= "The (generator,number of occurrences) pairs are\n";
	long count=0;
	for(long index=1;index<=numg;++index){
		long e=(long)p->g->NthItem(index);
		long eTotal=(long)p->gCount->NthItem(index);
		s <= "(" <= e <= "," <= eTotal <= ") ";
		++count;
	}
	s <= "\n";
	return s;
}

ostream& repGlobalSymbols(ostream& s,FTTpres*p){
//@rn	s <= "{ ";
   s <= '<'; //@rn
	long num=p->globalSymbols->numItems;
	long i;
	for(i=1;i<num;++i){
		repLetter(s,(long)p->globalSymbols->NthItem(i))
			<= ',';
	}
	if(num>0){
		repLetter(s,(long)p->globalSymbols->NthItem(i));
	}
//@rn	s <= " }";
   s <= ';'; //@rn
	return s;
}

ostream& xrepGlobalSymbols(ostream& s,FTTpres*p){
	s <= "{ ";
	long num=p->globalSymbols->numItems;
	for(long i=1;i<num;++i){
		s <= "(" <= i <= ","
			<= (long)p->globalSymbols->NthItem(i)
			<= "), ";
	}
	if(num>0){
		s <= "(" <= num <= ","
			<= (long)p->globalSymbols->NthItem(num)
			<= ")";
	}
	s <= " }";
	return s;
}

ostream& repGlobalTTpres(ostream& s,FTTpres*p){
	long numf=p->f->numItems;
	long numg=p->g->numItems;
	repGlobalSymbols(s,p) <= "\n";
	repGlobalFreeList(s,p);
   s <= ">\n"; //@rn
/* COMMENTED OUT
	s <= "The (generator,number of occurrences) pairs are\n";
	long count=0;
	for(long index=1;index<=numg;++index){
		long e=(long)p->g->NthItem(index);
		long eTotal=(long)p->gCount->NthItem(index);
		s <= "(" <= e <= "," <= eTotal <= ") ";
		++count;
	}
	s <= "\n";
END COMMENTED OUT */
	return s;
}
ostream& repAnnotatedTTpres(ostream& s,FTTpres*p,
		FGLvar *v,GLref& loadRel){
	long numf=p->f->numItems;
	long numg=p->g->numItems;
	repGlobalSymbols(s,p) <= "\n";
	long num=p->f->numItems;
	for(long i=1;i<=num;++i){
		if(!isNull(loadRel)){
			Ptr DEREF=v&loadRel;
			if(!DEREF[i-1])s <= ">>";
			else s <= "  ";
			v-loadRel;
		}
		s <= "#" <= i <= ": "; 
		repGlobalFreeWord(s,p,p->f->freeItem(i));
	}
	return s;
}

ostream& repTime(ostream& s,double value){
	Cstr str;
	value=value/ONE_SECOND;
	sprintf(str,"%7.2fs.",value);
	s <= str;
	return s;
}

ostream& repLetter(ostream& s,long e){
	// translates to alphabetic character for better 
	// readability.  Observe this lettering scheme has
	// nothing to do with the input lettering scheme
	Boolean isCharacter=TRUE;
	if(0<e && e<27) e=e+0x60;
	else if(e>26 && e<53) e=e-26+0x40;
	else isCharacter=FALSE;
	if(isCharacter){
		s <= (char)e;
	}
//@rn	else s <= e;
	else s <= 'X' <= e; //@rn
	return s;
}

ostream& repTTtype(ostream& s,short currentType){
	switch(currentType){
		case t1:
			s <="TT1";
		break;
		case t1p:
			s <="TT1p";
		break;
		case t2:
			s <="TT2";
		break;
		case t2p:
			s <="TT2p";
		break;
		default: xErr(58);
	}
	return s;
}


FTTpres* randomRelation(FfreeList* f,long noLetters){
// this routine creates a new presentation from a given
// list of relators by adding a new random relator from
// the symbols 1 to noLetters.

// CAUTION: caller is responsible for insuring that noLetters
// does not exceed the number of local symbols in f, which must
// support a valid local numbering.  
// CAUTION: caller is responsible for completing construction
// by passing output TTpres to a TTpresNode and supplying a
// global symbol table.

	FfreeWord *w=new FfreeWord(128);
	long i=randBnd(1,10);
	for(long j=0;j<i;++j){
		long sign=randBnd(0,1);
		if(!sign)sign=-1;
		w->leftMultiply(randBnd(1,noLetters),sign*randBnd(1,3));
	}
	FTTpres* newPres= new FTTpres(TRUE);
	newPres->f= new FfreeList(f);
	newPres->f->Append(w);
	return newPres;
}
