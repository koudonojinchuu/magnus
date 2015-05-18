#include "crossTrainer.h"

#ifdef Frank_GenuineMac
#include "CrunPane.h"
#include "CStringArray.h"
#include "CpresText.h"
#endif

#include "FStringArray.h"
#include "FpLib.h"
#include "FpTree.h"
#include "FpLibInitInfo.h"
#include "FpTreeInitInfo.h"
#include "rsrcInterface.h"
#include "text_functions.h"
#include "FstrToWordNode.h"
#include "FTTpres.h"
#include "FTTpresNode.h"
#include "FfreeList.h"
#include "FTT2node.h"
#include "FTietzeNode.h"
#include "FTietzeDoc.h"
#include "FTTpresNode.h"
#include "FTTdriverNode.h"
#include "FstrToWordNode.h"
#include "nodeMsg.h"
#ifdef Frank_GenuineMac
#include "CNewDLOGsDialog.h"
#include "CTietzeDoc.h"
#endif
#include <stdio.h>

// The TietzeNode has to be aware that its supervisor is a
// TietzeDoc.  This is the only node that needs to know the 
// structure of its supervisor.  (It is bad for nodes to know 
// who is directing them, because this knowlege greatly limits
// reusability.)  However, the top node has to communicate with
// both SUN and MAC interfaces, and it is best to work this out
// at compile time.

#ifdef Frank_GenuineMac
FTietzeNode::FTietzeNode(CTietzeDoc *aTietzeDoc)
#else
FTietzeNode::FTietzeNode(FTietzeDoc *aTietzeDoc)
#endif
	:FGLnode(0){
	itsDoc=aTietzeDoc;
	itsStrings=itsDoc->itsStrings;
	itsWordToStringArrayInfo=0;
	letterToNumber=0;
	numberToLetter=0;
	pTree=0;
	itsTTpres=0;
	factorRank=-1;
// nodes
	itsStrToWordNode=0;
	itsTTpresNode=0;
	itsTTdriverNode=0;

	displayParentSerial=-1;
	displaySerial=-1;
	displayPres=0;
	displayType=-1;

			
	TTrels= new FfreeList(itsStrings->numItems);

// the following initialization is on behalf of CfreeWord
	FfreeWord::freeWordSize=FfreeWord::datumSize();
	

//  control
	func=TietzeFunc;
	mailPerson=inputMail;
	
}

FTietzeNode::~FTietzeNode(void){
	delete itsStrToWordNode;itsStrToWordNode=0;
	delete itsTTpresNode;itsTTpresNode=0;
	delete itsTTdriverNode;itsTTdriverNode=0;
	delete TTrels;TTrels=0;
	delete itsTTpres;itsTTpres=0;
#ifdef Frank_GenuineMac
	forgetHan((Handle)letterToNumber);
	forgetHan((Handle)numberToLetter);
#else
	Handle han=(Handle)letterToNumber;
	forgetHan(han);
	han=(Handle)numberToLetter;
	forgetHan(han);
#endif
	delete itsWordToStringArrayInfo; itsWordToStringArrayInfo=0;
	delete pTree; pTree=0;
	delete itsGLink; itsGLink=0;

#ifdef Frank_MemoryDebugging
	extern FGLink *pTreeLink;
	extern long pTreeTotal;
	pTreeLink=0;
	pTreeTotal=0;
#endif

}


Boolean  FTietzeNode::TietzeFunc(FGLnode *node){
	FTietzeNode *n=(FTietzeNode*)node;
	switch(n->mailPerson){
		case inputMail: return n->inputGo();
		case strToWordMail: return n->strToWordGo();
		case presMail: return n->presGo();
		case driverMail: return n->driverGo();
		default:n->nodeErr(6);
	}
}

Boolean  FTietzeNode::inputGo(void){
	if(doInputEntry()){
		mailPerson=strToWordMail;
		return TRUE;
	}
	else return FALSE;
}

Boolean  FTietzeNode::strToWordGo(void){
	if(!itsStrToWordNode){

		itsStrToWordNode=new FstrToWordNode(this,itsStrings, 
			lettersNo,letterToNumber,1,itsStrings->numItems);
	}
	if(!itsStrToWordNode->run()){
		delete itsStrToWordNode;itsStrToWordNode=0;
		mailPerson=presMail;
	}

	return TRUE;
}

Boolean  FTietzeNode::presGo(void){
	if(!itsTTpresNode){
		itsTTpres= new FTTpres((Boolean)FALSE);
		itsTTpres->f=TTrels;
		itsTTpres->globalSymbols=new FList(lettersNo);
		
		// initialize global symbols to trival lut
		for(long i=1;i<=lettersNo;++i){
			itsTTpres->globalSymbols->Append((FObject*)i);
		}
		
		itsTTpresNode=new FTTpresNode(this,itsTTpres);
	}
	if(!itsTTpresNode->run()){
		delete itsTTpresNode;itsTTpresNode=0;
		mailPerson=driverMail;

	}
	return TRUE;
}

Boolean  FTietzeNode::driverGo(void){
	if(!itsTTdriverNode){
		itsGLink=new FGLink;

#ifdef Frank_MemoryDebugging
		extern FGLink *pTreeLink;
		pTreeLink=itsGLink;
#endif

		FpTreeInitInfo *info= new FpTreeInitInfo;
		pTree= new FpTree(itsGLink,0,info);
		delete info;info=0;
		// we set up the pTree base vertex here so that the
		// driver node base class FGLwalkNode initializes correctly

			
		pTree->j=0;
		pTree->I();
		pTree->e=1;
		extern long presCount;
		pTree->V(++presCount);
		pTree->bv=pTree->cv;
		pTree->putPres(itsTTpres);

		displayParentSerial=-1;
		displaySerial=presCount;
		displayPres=itsTTpres;
		displayType=-1;
		send(displayPresMsg);
	
		itsTTdriverNode=new FTTdriverNode(this,itsTTpres,itsGLink,pTree);

// Conclude pTree initialization

		pTree->super=itsTTdriverNode;
		send(calibrateTT1msg);
		send(calibrateTT1pmsg);
		send(calibrateTT2msg);
		send(calibrateTT2pmsg);
		pTree->send(pVertexInitMsg);
		pTree->Put();
		++pTree->passNumber;

#ifdef Frank_MemoryDebugging
		extern long initialTotal,pTreeTotal,libTreeTotal;
		long permanentTotal=pTreeTotal+libTreeTotal;
#ifdef Frank_GenuineMac
		extern TEHandle debuggingTextHandle;
		if(debuggingTextHandle) 
			long textSize=GetHandleSize((**debuggingTextHandle).hText);
#else
			long textSize=0;
#endif

		initialTotal=FreeMem()+textSize+permanentTotal; 
				// because this text and data is debited
				// later when computing leaks, and so must be
				// added here to start with a zero balance
		tout <= "initial Total=" <= initialTotal <=" bytes\n";
#endif

	}
	if(!itsTTdriverNode->run()){
		delete itsTTdriverNode;itsTTdriverNode=0;
		delete pTree; pTree=0;
		delete itsGLink;itsGLink=0;
		return FALSE;
	}
	return TRUE;
}

Boolean FTietzeNode::doInputEntry(void){
	setTietzeMembers();
	lettersNoDisplay();
	itsWordToStringArrayInfo=new FwordToStringArrayInfo;
	itsWordToStringArrayInfo->displayWidth=TietzeScrollBasisWidth;
	itsWordToStringArrayInfo->strs=itsStrings;
	itsWordToStringArrayInfo->lettersPerLine=TietzeLettersPerLine;
#ifdef Frank_GenuineMac
	itsWordToStringArrayInfo->slack=presSlack;
#else
	itsWordToStringArrayInfo->slack=0;
#endif
	itsWordToStringArrayInfo->lettersNo=lettersNo;
	itsWordToStringArrayInfo->numberToLetter=numberToLetter;
	
	return(TRUE);
}

Boolean FTietzeNode::setTietzeMembers(void){
	itsElementSize=itsStrings->elementSize;
	lettersNo=0;
	relationsNo=0;
	letterToNumber=(long**)GLhandle(256*sizeof(long),
					"FTietzeNode::setTietzeMembers");
	numberToLetter=(unsigned char**)GLhandle(256*sizeof(char),
					"FTietzeNode::setTietzeMembers");
	for(long i=0;i<256;i++){
		(*letterToNumber)[i]=0;
		(*numberToLetter)[i]=0;
	}	
	long numItems=itsStrings->numItems;
	
	for(long index=1;index<=numItems;++index){
		Str255 str;
		Boolean scanning=TRUE;
		Boolean emptyWord=TRUE;
		itsStrings->GetItem(str,index);
		for(int i=1;i<=str[0] && scanning;++i){
			unsigned char c;
			short charType;
			c=str[i];
			charType=getCharType(c);
			switch(charType){
				case GLletter:
				emptyWord=FALSE;
				if((*letterToNumber)[c]==0){
					++lettersNo;
					(*letterToNumber)[c]=lettersNo;
					(*numberToLetter)[lettersNo]=c;
				}
				break;
				case GLnumber:
				case GLminus:
				case GLtilde:
				break;
				case GLspace:{
					//we get rid of the space
					long noRemaining;
					--str[0]; 
					noRemaining=itsElementSize-(i+1);
					BlockMove((Ptr)&str[i+1],(Ptr)&str[i],noRemaining);
					--i;
				}
				break;
				case GLcomment:scanning=FALSE;
				break;
				case GLother: 
				default: xErr(32);
			}
		}
		itsStrings->SetItem(str,index); //save all adjustments to str
	}
	if(lettersNo>0)return TRUE;
	return FALSE;
}  

// NB  We must alter the send routine because the supervisor
// is not a node but rather a TietzeDoc

void FTietzeNode::send(short aMessage){
	if(itsDoc){
		itsDoc->respond(aMessage);
	}
}
void FTietzeNode::respond(short aMessage){

	switch(aMessage){
		// the following interface messages are relayed to the
		// document
		case displayPresMsg:
			if(itsTTdriverNode){
				displayParentSerial=itsTTdriverNode->displayParentSerial;
				displaySerial=itsTTdriverNode->displaySerial;
				displayPres=itsTTdriverNode->displayPres;
				displayType=itsTTdriverNode->displayType;
				send(displayPresMsg);
			}
			return;
		case freeFactorMsg:
			factorRank=itsTTdriverNode->factorRank;
			send(aMessage);
			return;		
		case finiteGroupMsg:
			groupOrder=itsTTdriverNode->groupOrder;
			send(aMessage);
			return;		
		case cyclicGroupMsg:
		case trivialGroupMsg:
		case freeGroupMsg:   
			send(aMessage);
			return;
	}
	switch(mailPerson){
		case driverMail:
			switch(aMessage){
				case forgetPresMsg:
					delete itsTTpres;itsTTpres=0;
				break;
				default: send(aMessage);
			}
		break;
		case strToWordMail:
			switch (aMessage){
				break;	
				case removeBlanksMsg:
					// we don't need to do this since we removed the blanks
					// earlier when we were creating the character luts
				break;				
				case emptyWordStrMsg:
				break;				
				case appendWordMsg:
				case appendLastWordMsg:{
					if(!TTrels->isDuplicate(itsStrToWordNode->f)){
						TTrels->Append(itsStrToWordNode->f);
						++relationsNo;
					}
				}
				break;				
				case strToWordAuRevoirMsg:
				break;							
				case strToWordGoodbyeKissMsg:
					relationsNoDisplay();
				break;			
				default: send(aMessage);
			}
		break;
		case presMail:
			switch(aMessage){
				break;	
				case presAuRevoirMsg:
				case presGoodbyeKissMsg:
				break;
				default: send(aMessage);
			}
		break;
		default: send(aMessage);
	}
}
  
void FTietzeNode::emptyPresentationDisplay(void){
	//empty presentation has rank 0
	relationsNoDisplay();
	lettersNoDisplay();
}
 
void FTietzeNode::relationsNoDisplay(void){
/* COMMENTED OUT
	itsDoc->setDisplay(GLrelationsNo,relationsNo);
END COMMENTED OUT*/
} 
void FTietzeNode::lettersNoDisplay(void){
/* COMMENTED OUT
	itsDoc->setDisplay(GLlettersNo,lettersNo);
END COMMENTEND OUT*/
} 

