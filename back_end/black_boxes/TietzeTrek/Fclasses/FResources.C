#include "crossTrainer.h"


#include "FResources.h"

// the genuine mac version only uses this class to get the
// STR# indices

#ifdef Frank_GenuineMac
#include "Packages.h"  
#endif


#ifndef Frank_GenuineMac

#include "FList.h"
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>


#define noSTRs 15 // UPDATE when adding new STR resources to this object 
#define MacResourceSize 32767  //Mac resource is 32k max
	
#endif


FResources::FResources(void){

#ifndef Frank_GenuineMac	
	STR= new FList(noSTRs);
	memout <= "loading string resources, please be patient...\n";
	for(long i=0;i<noSTRs;++i){
		long resIndex=i+xSTRoffset;
		Str255 SIZEstr;
		sprintf((char*)SIZEstr,"SIZE=");
		CtoPstr((char*)SIZEstr);
		Cstr fname;
		
// UNIX and Mac OS have different file naming conventions which
// we take into account here

#ifdef THINK_CPLUS
		sprintf((char*)fname,":Resources:STR%ld.derez",resIndex);
#else
		sprintf((char*)fname,"Resources/STR%ld.derez",resIndex);
#endif

		FILE *stream=fopen((char*)fname,"r");
		if(!stream){
			memout <= "can't open resource " <= resIndex <= ", goodbye\n";
			exit(0);
		}		
		Str255 str;
		if(fread(&str[1],1,5,stream)!=5){
			memout <= "can't read resouce " <= resIndex <= ", goodbye\n";
			exit(0);
		}
		str[0]=5;
		if(IUEqualString(str, SIZEstr)){
			memout <= "resource " <= resIndex 
				<= " doesn't begin with \"SIZE=\", gooodby\n";
			exit(0);
		}
		short size;
		if(fscanf(stream,"%hd",&size)!=1){
			memout <= "can't read SIZE of resource " <= resIndex 
				<= ", (syntax error?).  Goodbye\n";
			exit(0);
		}
		if(size<0 || size>MacResourceSize){
			memout <= "resource " <= resIndex <= " size is out of range, goodbye\n";
			exit(0);
		}
		loadResource(stream,i,resIndex,size);
		if(fclose(stream)){
			memout <= "can't close resource " <= resIndex <= ",goodbye\n";
			exit(0);
		}
	}
#endif
}

#ifndef Frank_GenuineMac

void FResources::loadResource(FILE *stream,long i,long resIndex,short size){
	// the resource file should parse as follows
	//  $"0031 1943 616E 2774 206F 7065 6E20 474C"
	// (byte aligned at the end)
	// This is standard SayDerez hex output format, so don't mess
	// with it and all will be well.  Everything else in the file is
	// ignored.  
	unsigned char *data= (unsigned char *)GLptr(size,"FResources::loadResource");
	unsigned char *currentPtr=data;
	if(sizeof(short)!=2){
		memout <= "loadResource is doomed because shorts are wrong size\n";
		exit(0);
	}
	STR->Append((FObject*)data);
	// we skip the first and second lines, which don't
	// contain any hex data
	char aChar;	
	Boolean seekingNewLine=TRUE;
	Boolean unexpectedEnd=FALSE;
	while(seekingNewLine&&!unexpectedEnd){
		unexpectedEnd=!fread(&aChar,1,1,stream);
		if(aChar=='\n')seekingNewLine=FALSE;
	}
	seekingNewLine=TRUE;
	short count=0;
	while(count<size&& !unexpectedEnd){
		short noShorts;
		while(seekingNewLine&&!unexpectedEnd){

			unexpectedEnd=!fread(&aChar,1,1,stream);
			if(aChar=='\n'){
				Boolean seekingDollar=TRUE;
				seekingNewLine=FALSE;
				while(seekingDollar&&!unexpectedEnd){
					unexpectedEnd=!fread(&aChar,1,1,stream);
					if(aChar=='$')seekingDollar=FALSE;
				}
			}
		}			
			
		//skip the quotation mark
		unexpectedEnd=!fread(&aChar,1,1,stream);

		noShorts=0;
		while(!seekingNewLine && count<size &&!unexpectedEnd){
			unsigned char datum[2];
			unexpectedEnd=fscanf(stream,"%hx",(unsigned short *) &datum)!=1;
			if(!unexpectedEnd){
				if(count+1 == size){
					// the last byte is contained in datum[1]
					*currentPtr=datum[1];
					count=count+1;
					++currentPtr;
				}
				else if(count<=size-2){
					// we load 2 bytes and increment counters
					BlockMove((Ptr)datum,(Ptr)currentPtr,2);
					count=count+2;
					currentPtr=&currentPtr[2];
					++noShorts;
					seekingNewLine= noShorts==8;
				}
				else unexpectedEnd=TRUE;
			}
		}
	}
	if(!unexpectedEnd){
		unexpectedEnd=!fread(&aChar,1,1,stream);
	}
	if(!unexpectedEnd){
		unexpectedEnd= aChar!=0x22; // aChar should be the final quote
	}	
	if(unexpectedEnd){
		memout <= "syntax error in resource " <= resIndex <= ", goodbye\n";
		exit(0);
	}
}

#endif

