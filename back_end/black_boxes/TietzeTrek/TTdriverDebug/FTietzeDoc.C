// this file only applies to the SUN version.  The analgous file for
// Macintosh is called CTietzeDoc

#ifndef Frank_GenuineMac

#include "rsrcInterface.h"
#include "FtoolBox.h"
#include "crossTrainer.h"
#include "FTietzeDoc.h"
#include "FStringArray.h"
#include "FTietzeNode.h"
#include "text_functions.h"
#include "nodeMsg.h"
#include "FpresText.h"
#include "Ftime.h"
#include "FTTdriverNode.h"
#include "debugTT.h"
#include "FTTpres.h"

// DEBUG
#include "limits.h"
// END DEBUG

#include <stdio.h>

FTietzeDoc::FTietzeDoc(void){
	itsTietzeNode=0;
	itsStrings=0;
	itsEditItem=0;
	
/********************************************************/
// Tietze Transfomation control data
/********************************************************/

// This data controls memory and time
// constraints on the TT algorithms, as well
// as certain required and optional features.
// The data is initialized here, but
// may be freely adjusted while the program
// is running by directly altering the appropriate 
// datum.  Changes take effect for all future
// transformations, but not the one currently in
// progress (if it exists).


// TminRequired is the minimum amount of time a generated
// presentation must accumulate for transformation T 
// to operate on it.  This value is dynamically adusted for 
// each operator on each generated transformation, but the 
// global value establishes a global minimum requirement. 

	TT1minRequired= 10*ONE_SECOND;		
	TT1pminRequired= 10*ONE_SECOND;
	TT2minRequired= ONE_SECOND;
	TT2pminRequired= 10*ONE_SECOND;

// TPayCheck is the amount of time that is deposited in the
// account of operation T for a given persentation each time
// that presentation is visited.  This value is dynamically 
// adusted for each operator on each generated transformation, 
// but the global value establishes a global minimum requirement.

	TT1PayCheck=10*ONE_SECOND;
	TT1pPayCheck= 10*ONE_SECOND;
	TT2PayCheck= ONE_SECOND;
	TT2pPayCheck= 10*ONE_SECOND;

// TRepeatMax is the maximum number of times operation T 
// should be immediately repeated on a given presentation.
// In no event is an operation repeated if there is mathematically
// no possibility of a successful outcome or if the previous 
// immediate operation timed out before succeeding.  

	TT1RepeatMax=1;
	TT1pRepeatMax=AUTO_SET;
	TT2RepeatMax=1;
	TT2pRepeatMax=AUTO_SET;
	
	
	TT2len=30;   // max reduced length of word chosen by TT2

// Library timeout for SUN.  This is a kluge to get
// around the fact that there is no error recovery
// on the SUN.  When error recovery becomes available
// this should be phased out.

	LibTimeLimit=60*ONE_SECOND;

// CAUTION: When debugging, don't set the paychecks
// too close to LONG_MAX.  If the high bit is set,
// time "goes negative", which is not the desired effect.
/* DEBUG
	TT1PayCheck=.01*LONG_MAX;
END DEBUG */
/* DEBUG
	TT1pPayCheck=.01*LONG_MAX;
	TT2PayCheck=.01*LONG_MAX;
	TT2pPayCheck=.01*LONG_MAX;
END DEBUG */

	OpenStdin();
	if(!itsEditItem || !itsStrings){
		memout<="Initialization failure, goodbye\n";
		ExitToShell();
	}
	for(long i=1;i<=itsStrings->numItems;++i){
		Str255 str;
		itsStrings->GetItem( str, i);
		if(!itsEditItem->ValidateString(str)){		
			memout<="Initialization failure, goodbye\n";
			ExitToShell();
		}
	}
	itsTietzeNode=new FTietzeNode(this);
}

FTietzeDoc::~FTietzeDoc(void){
	delete itsStrings;itsStrings=0;
	delete itsEditItem;itsEditItem=0;
}

#define INPUT_FILE_SIZE 32767
void FTietzeDoc::OpenStdin(void){
	Handle	theData;
	int count;
// we assume the input file is less than 32k

	theData=(Handle)GLhandle(INPUT_FILE_SIZE,"FTietzeDoc::OpenStdin");
	HLock(theData);
	gout << "instructions? (y for yes)\n";
	
	cin>>**theData;

	if(**theData=='y'){
		gout << "Please enter relators, separated by commas.\n"
			<< "N.B.  Carriage returns are ignored.\n"
			<< "Use letters for basis elements.\n"
			<< "Use postfixed signed integers for exponents.\n"
			<< "When done, enter ! followed by a carriage return.\n";
	}

	{
		Ptr dataPtr=*theData;
		cin>> *dataPtr;
		while(*dataPtr!= '!'){
			++dataPtr;
			cin>>*dataPtr;
		}
	}
	itsStrings=new FStringArray(sizeof(Str255));
	itsEditItem=new FpresText(this);
	if(theData) stringParse(itsStrings,theData);
	forgetHan(theData); // we don't need the raw data anymore
}
	
// these currently are not very useful on the SUN but could
// eventually be worked into some interface

void FTietzeDoc::setDisplay(short itemNo,long value){
	switch(itemNo){
		case GLrelationsNo: cout<= value <= " relations.\n";
		break;
		case GLlettersNo: cout<= value <= " letters.\n";
		break;
	}
}


void FTietzeDoc::setDisplayFromID(short itemNo, short strID){
	// gets text from static text #STR resource
	Str255 resStr;
	int intStrID=(int)strID;
	GetIndString(resStr,FResources::xSTRstatic,intStrID);
	gout <= "FTietzeDoc: itemNo=" <= itemNo <= "message is: " <= resStr;
}

void FTietzeDoc::respond(short aMessage){
	switch(aMessage){
		case calibrateTT1msg:{
			FTTdriverNode *n=itsTietzeNode->itsTTdriverNode;
			n->TT1minRequired=TT1minRequired;		
			n->TT1PayCheck=TT1PayCheck;
			n->TT1RepeatMax=TT1RepeatMax;
		}
		break;
		case calibrateTT1pmsg:{
			FTTdriverNode *n=itsTietzeNode->itsTTdriverNode;
			n->TT1pminRequired=TT1pminRequired;
			n->TT1pPayCheck=TT1pPayCheck;
			n->TT1pRepeatMax=TT1pRepeatMax;
		}
		break;
		case calibrateTT2msg:{
			FTTdriverNode *n=itsTietzeNode->itsTTdriverNode;
			n->TT2minRequired=TT2minRequired;
			n->TT2PayCheck=TT2PayCheck;
			n->TT2RepeatMax=TT2RepeatMax;
			n->TT2len=TT2len;   
		}
		break;
		case calibrateTT2pmsg:{
			FTTdriverNode *n=itsTietzeNode->itsTTdriverNode;
			n->TT2pminRequired=TT2pminRequired;
			n->TT2pPayCheck=TT2pPayCheck;
			n->TT2pRepeatMax=TT2pRepeatMax;
		}
		break;
		case calibrateLibmsg:{
			FTTdriverNode *n=itsTietzeNode->itsTTdriverNode;
			n->LibTimeLimit=LibTimeLimit;
		}
		break;
		case displayPresMsg:{
			FTietzeNode *n=itsTietzeNode;
			if(n->displayParentSerial>0){
				// child presentation display
				tout <= "#" <= n->displaySerial <= " from #" 
					<= n->displayParentSerial <= " by ";
				repTTtype(tout,n->displayType) <= "\n";
				repGlobalTTpres(tout,n->displayPres);
			}
			else{
				// initial presentation display
				tout <="#"<= n->displaySerial <= " Input presentation:" <= "\n";
				repGlobalTTpres(tout,n->displayPres);
			}
		}
		break;
		case trivialGroupMsg:
			SysBeep(1);
			tout <= "*** TRIVIAL ***\n";

		break;
		case cyclicGroupMsg:
			SysBeep(1);
			tout <= "*** CYCLIC ***\n";
		break;
		case freeFactorMsg:
			SysBeep(1);
			tout <= "*** NON-TRIVIAL FREE FACTOR ***\n";
		break;
		case finiteGroupMsg:
			SysBeep(1);
			tout <= "*** FINITE group of order "<=itsTietzeNode->groupOrder<=" ***\n";
		break;
		case freeGroupMsg:  
			SysBeep(1);
			tout <= "*** FREE of rank " 
				<= itsTietzeNode->factorRank 
				<= " ***\n";

		break; 
	}
}



#endif
