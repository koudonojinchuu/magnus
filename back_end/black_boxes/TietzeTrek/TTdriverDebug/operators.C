#include "Ftypes.h" // a lot of code uses the definitions 
					// in this file

#include "FGLink.h"
#include "FBlockManager.h"
#include "FGLvarInterface.h"
#include "GLfunctions.h"
#include <stdio.h>
#include "FList.h"
#include "FResources.h"
#include "FtoolBox.h"
#include "GLfunctions.h"

#ifndef Frank_GenuineMac
#include <iostream.h>
#include <fstream.h>
extern ofstream fout;
#endif

extern ostream &gout;
extern ostream &memout;


// Memory Management operators.  These are important on the Mac.
// THEY ARE ALSO IMPORTANT ON THE SUN.  The reason is that
// SetHandleSizePatch (which calls realloc()) moves memory and 
// invalidates external references.  For SetHandleSizePatch to 
// know when it should fail rather than move memory, the handle
// locking mechanism MUST BE SUPPORTED.

void operator+(FGLvarInterface *v,const GLref& ref){
	 // CAUTION: each operator+ must be balanced with an operator+
	 v->itsGLink->itsBlockManager->LockBlock(ref);
}

void operator-(FGLvarInterface *v,const GLref& ref){
	 // CAUTION: each operator- must be balanced with an operator+ 
	 // or an operator&
	 v->itsGLink->itsBlockManager->RestoreBlock(ref);
}

Ptr operator&(FGLvarInterface *v,const GLref& ref){
	// CAUTION: each operator& must be balanced with an operator-
	v->itsGLink->itsBlockManager->LockBlock(ref);
	return v->xCheckPtr(ref);
}
 

#ifdef Frank_GenuineMac

#include "CLogPane.h"
#include "CTietzeTrekDoc.h"
#include "CTietzeTrekApp.h"
extern CTietzeTrekDoc *trekDoc;
extern CTietzeTrekApp *trekApp;

CLogPane * ostream::xgetDoc(void){
	if((trekDoc==NULL) || (trekDoc->itsMainPane==NULL)){
		return NULL;
	}
	return (CLogPane*)(trekDoc->itsMainPane);
}
#endif


ostream& operator<=(ostream& s,const char* str){
#ifdef Frank_GenuineMac
	CLogPane *p;
	if((p=s.xgetDoc())==NULL)return s;
	Str255 Pstr;
	char *strptr=(char*)str;
	unsigned char *Pstrptr=Pstr;
	unsigned char len=0;
	for(char c=*strptr;c!='\0' && len<256;c=*(++strptr)){
		++len;
		*(++Pstrptr)=c;
	}
	if(len>255)len=255;
	*Pstr=len;
	p->debugIns(Pstr);
	return s;
#else
	for(char *c=(char*)str;*c!=0;++c){
		if(*c=='\n'){
			fout<<endl;
			s<<endl;
		}
		else{
			fout<<*c;
			s<<*c;
		}
	}
	return s;
#endif
}

ostream& operator<=(ostream& s,const Str255 str){
#ifdef Frank_GenuineMac
	CLogPane *p;
	if((p=s.xgetDoc())==NULL)return s;
	p->debugIns(str);
	return s;
#else
	unsigned char len=str[0];
	unsigned char *c=(unsigned char *)str;
	for(long i=1;i<=len;++i){
		++c;
		if(*c=='\n'){
			fout<<endl;
			s<<endl;
		}
		else{
			fout<<*c;
			s<<*c;
		}
	}
	return s;
#endif
}



ostream& operator<=(ostream& s,const short num){
	Cstr str;
	sprintf(str,"%d",num);
	return s<=str;
}

ostream& operator<=(ostream& s,const int num){
	Cstr str;
	sprintf(str,"%d",num);
	return s<=str;
}
ostream& operator<=(ostream& s,const long num){
	Cstr str;
	sprintf(str,"%ld",num);
	return s<=str;
}


ostream& operator<=(ostream &s,const char aChar){
	Cstr str;
	str[0]=aChar;
	str[1]=0;
	return s<=str;
}

ostream& operator<=(ostream &s,const unsigned char aChar){
	Str255 str;
	str[0]=1;
	str[1]=aChar;
	return s<=str;
}

ostream& operator<=(ostream &s,const Handle han){

// NOTICE it is permissible to cast pointers to handles
// so that you can print out pointer addresses in a sensible format.
// If left uncast they will be interpreted as C strings,
// a highly confusing and somewhat dangerous situation.
// There is an exception to every rule...

	Cstr str;
	sprintf(str,"(address)%lx",han);
	return s <=str;
}

ostream& operator<=(ostream& s,const GLref& ref){
	return s <= "(" <=ref.index <="," <=ref.offset <=")";
}

