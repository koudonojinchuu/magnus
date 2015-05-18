#include "crossTrainer.h"
#include "debugGLink.h"
#include <stdio.h>
#include "FdebugNode.h"
#include "FfoldStepper.h"


ostream& repReg(ostream& s,FGLvarInterface *v){
	if(xDEBUG){
		cout <= "registers: bv=" <= v->bv <= ",cv=" <= v->cv
			<= ", e=" <= v->e <= ", j=" <= v->j <= "\n";
	}
	return s;
}

ostream& repE(ostream& s,FGLvarLow *v){
	if(xDEBUG){
		repbE(s,v);
		repcE(s,v);
	}
	return s;
}

ostream& repbE(ostream& s,FGLvarLow *v){
	if(xDEBUG){
		cout <="     base ";
		xrepE(s,v,v->bv);
		cout <="\n";
	}
	return s;
}


ostream& repcE(ostream& s,FGLvarLow *v){
	if(xDEBUG){
		cout <="     companion ";
		xrepE(s,v,v->cv);
		cout <="\n";
	}
	return s;
}

ostream& xrepE(ostream& s,FGLvarLow *v,GLref& ref){
	if(xDEBUG){
		Ptr DEREF=v&ref;
		if(v->xGetPacketisIdeal(DEREF)){
			cout <="is ideal, distinguished edge report not supported.";
			v-ref;
			return s;
		}
		GLref packetRef;
		v->xGetPacketRef(packetRef,GLr,DEREF);
		v-ref;
		cout <="packet ref:" <= packetRef;
		DEREF=v&packetRef;
		GLploc p;
		v->xIndexToGLploc(p,GLr,v->e);
		long offset=v->xmodIndexToOffset(GLr,p.modIndex);
		GLref oppVertex;
		BlockMove((Ptr)&DEREF[offset],(Ptr)&oppVertex,sizeof(GLref));
		v-packetRef;
		cout <=", opposite vertex:"<=oppVertex;
	}
	return s;
}

ostream& repBoolean(ostream& s,Boolean val){
	if(val){
		cout <="TRUE";
	}
	else{
		cout <="FALSE";
	}
	return s;
}


ostream& repFlagToType(ostream& s,short flag){
	switch(flag){
		case GLir:
		case GLr:  cout <="**real**";  /*real vertex*/
		break;
		case GLi:  cout <="**ideal**";  /*idea vertex*/
		break;
		default:   cout <="**BAD FLAG**";  /*bad type*/
	}
	return s;
}


ostream& repLink(ostream& s,short type,GLref ref,FGLvar *v){
	if(!xDEBUG) return s;;
	GLflag flag=v->xPacketTypeToFlag(type,ref);
	Ptr DEREF=v&ref;
	GLref refSave;
	v->xGetPacketRef(refSave,flag,DEREF);
	long eSave=v->e;
	v->e=0;				// first packet distinguished for forward search
	Boolean notEmpty;
	switch(flag){
		case GLr: 
			cout <="Real link: ";
			notEmpty=v->rN();
		break;
		case GLir:
		case GLi: cout <="Ideal link: ";	
			notEmpty=v->iN();
		break;
	}
	if(notEmpty){	
		s <= "\n";
		scanDoubleInfo d;
		d.userPtr= &s;
		d.flag=flag;
		v->xGetPacketRef(d.firstPacket,flag,DEREF);
		GLploc p;
		v->xIndexToGLploc(p,flag,v->e);
		d.modIndexStart=p.modIndex;
		d.goForward=TRUE;
		d.skipFirstEntry=FALSE;
		v->scanDoublePacket(&d,repScanEdge);
	}
	else{
		cout <="None." <= "\n";
	}	
	v->xPutPacketRef(flag,DEREF,refSave);
	v-ref;
	v->e=eSave;
	return s;
}


Boolean repScanEdge(scanDoubleInfo *info){

// info->userPtr has the ostream in it, but we don't
// use it because of compiler blues

	FGLvarLow *v=(FGLvarLow*)info->var;
	GLref oref;
	v->xEdgeEntryToOwner(oref,info->refDatum);
	long index=v->doubleEntryToIndex(info);
	
	cout <= info->newPacket <= "-----[" <= index <= "]-----";
	GLref opacketRef;
	opacketRef=info->refDatum;
	opacketRef.offset=opacketRef.offset-v->ownerOffset[info->flag];
	cout <= opacketRef;
	Ptr DEREF=v&oref;
	if(v->xGetPacketisIdeal(DEREF)){
		cout <="**ideal**";
	}
	else{
		cout <="**real**";
	}
	v-oref;
	cout <= oref <= "\n";
	return(TRUE);
}

void dumpGraph(ostream& s,FGLvar *v){
	GLref bvSave=v->bv;
	GLref cvSave=v->cv;
	long eSave=v->e;
	FdebugNode *d=new FdebugNode(s,v);
	Boolean debugSave=xDEBUG;
	xDEBUG=TRUE;
	while(d->run()){};
	if(d->corruptEdge){
		Str255 str;
		sprintf((char*) str,"dumpGraph: corrupt edge, goodbye");
		CtoPstr((char*)str);
		DebugStr(str);
		ExitToShell();
	}
	delete d;d=0;
	xDEBUG=debugSave;
	v->bv=bvSave;
	v->cv=cvSave;
	v->e=eSave;
}

Boolean repPackets(ostream& s,FGLvar *v,GLref& startRef){
	// CAUTION: DOES NOT PRESERVE v->e !!!

	if(!xDEBUG) return TRUE;

	// check startRef
	if(isNull(startRef)){
		cout <= "Error: bad startRef: " <= startRef <= "\n";
		return FALSE;
	}
	cout <= "first packet location: " <= startRef <= "\n";
	long orientedEdgesNo=2*v->noEntries[GLr];
	long edgeRefOffset=v->dataOffset[GLr];
	FfoldStepper *f
		= new FfoldStepper(v,edgeRefOffset,orientedEdgesNo,startRef);
	Boolean beginningPacketSearch=TRUE;
	Boolean noErrors=TRUE;
	while(!isNull(f->packetRef)){
		Boolean edgeOK;
		if(beginningPacketSearch){
			if(f->firstEdge()){
				edgeOK=repEdgeFromStepper(s,v,f);
				if(!edgeOK)noErrors=FALSE;
				beginningPacketSearch=FALSE;
			}
			else{
				cout <= "no edges in packet" <= "\n"
					<= "next packet location: "
					<= f->packetRef <= "\n";
			}
		}
		else{
			if(f->nextEdge()){
				edgeOK=repEdgeFromStepper(s,v,f);
				if(!edgeOK)noErrors=FALSE;
			}	
			else {	
				beginningPacketSearch=TRUE;
				cout <= "next packet location: " 
					<= f->packetRef <= "\n";
			}
		}	
	}

	delete f;f=0;
	return noErrors;
}

Boolean repEdgeFromStepper(ostream& s,FGLvar *v,FfoldStepper *f){
	GLref datum=f->refDatum;
	cout <= "edge " <= v->e <= ":\n";
	v+datum;
	Ptr DEREF=xPtr(v,datum);
	if(!DEREF){
		cout <= "     ERROR: corrupt initial edgeDatum\n";
		SysBeep(1);
		v-datum;
		return FALSE;
	}
	GLref overtex;
	BlockMove((Ptr)DEREF,(Ptr)&overtex,sizeof(GLref));
	v-datum;
	cout <= "     overtex is " <= overtex <= "\n";
	GLref opacket;
	opacket.index=datum.index;
	opacket.offset=datum.offset-v->ownerOffset[GLr];
	cout <= "p     opacket is " <= opacket <= "\n";
	DEREF=v&datum;
	DEREF= &DEREF[-v->ownerOffset[GLr]];
	DEREF=&DEREF[v->xmodIndexToOffset(GLr,-v->e)];
	GLref odatum;	
	BlockMove((Ptr)DEREF,(Ptr)&odatum,sizeof(GLref));
	v-datum;
	v+odatum;
	DEREF=xPtr(v,odatum);
	if(!DEREF){
		cout <= "     ERROR: corrupt or undefined terminal edgeDatum: "
			<= odatum <= "\n";
		SysBeep(1);
		v-odatum;
		return FALSE;
	}
	DEREF= &DEREF[-v->ownerOffset[GLr]];
	DEREF=&DEREF[v->xmodIndexToOffset(GLr,v->e)];

	BlockMove((Ptr)DEREF,(Ptr)&datum,sizeof(GLref));
	v-odatum;
	Boolean corruptEdge= !SameAddr(datum,f->refDatum);
	if(corruptEdge){
		cout <= "     ERROR: inconsistent terminal edgeDatum\n";
		SysBeep(1);
		return FALSE;
	}
	return TRUE;
}

Boolean checkPackets(ostream& s,FGLvar *v,GLref& startRef){
	// CAUTION: DOES NOT PRESERVE v->e !!!

	if(!xDEBUG) return TRUE;

	// check startRef
	if(isNull(startRef)){
		cout <= "Error: bad startRef: " <=startRef <= "\n";
		return FALSE;
	}
	long orientedEdgesNo=2*v->noEntries[GLr];
	long edgeRefOffset=v->dataOffset[GLr];
	FfoldStepper *f
		=new FfoldStepper(v,edgeRefOffset,orientedEdgesNo,startRef);
	Boolean beginningPacketSearch=TRUE;
	Boolean noErrors=TRUE;
	while(!isNull(f->packetRef)){
		Boolean edgeOK;
		if(beginningPacketSearch){
			if(f->firstEdge()){
				edgeOK=checkEdgeFromStepper(s,v,f);
				if(!edgeOK)noErrors=FALSE;
				beginningPacketSearch=FALSE;
			}
		}
		else{
			if(f->nextEdge()){
				edgeOK=checkEdgeFromStepper(s,v,f);
				if(!edgeOK)noErrors=FALSE;
			}	
			else {
				beginningPacketSearch=TRUE;
			}
		}	
	}

	delete f;f=0;
	return noErrors;
}

Boolean checkEdgeFromStepper(ostream& s,FGLvar *v,FfoldStepper *f){
	GLref datum=f->refDatum;
	v+datum;
	Ptr DEREF=xPtr(v,datum);
	if(!DEREF){
		cout <= "ERROR: corrupt initial edgeDatum\n";
		SysBeep(1);
		v-datum;
		return FALSE;
	}
	DEREF= &DEREF[-v->ownerOffset[GLr]];
	DEREF=&DEREF[v->xmodIndexToOffset(GLr,-v->e)];
	GLref odatum;	
	BlockMove((Ptr)DEREF,(Ptr)&odatum,sizeof(GLref));
	v-datum;
	v+odatum;
	DEREF=xPtr(v,odatum);
	if(!DEREF){
		cout <= "ERROR: corrupt or undefined terminal edgeDatum: "
			<= odatum <= "\n";
		SysBeep(1);
		v-odatum;
		return FALSE;
	}
	DEREF= &DEREF[-v->ownerOffset[GLr]];
	DEREF=&DEREF[v->xmodIndexToOffset(GLr,v->e)];

	BlockMove((Ptr)DEREF,(Ptr)&datum,sizeof(GLref));
	v-odatum;
	Boolean corruptEdge= !SameAddr(datum,f->refDatum);
	if(corruptEdge){
		cout <= "ERROR: inconsistent terminal edgeDatum\n";
		SysBeep(1);
		return FALSE;
	}
	return TRUE;
}

