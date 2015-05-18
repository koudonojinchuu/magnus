#include "crossTrainer.h"
#include "FGLstepper.h"
#include "FGLvar.h"
#include "GLfunctions.h"


	
FGLstepper::FGLstepper(FGLvar *v,long edgeRefOffset,long orientedEdgesNo){
	this->v=v;
	this->edgeRefOffset=edgeRefOffset;
	this->orientedEdgesNo=orientedEdgesNo;
}

Boolean FGLstepper::firstEdge(const GLref& entryPacketRef){
	packetRef=entryPacketRef;
	edgeIndex=0;
	edgeSign=-1;
	count=-1;
	// place the edge ref sizeof(GLref) bytes BEFORE the first
	// slot, so that the call to nextEdge advances to the first slot
	edgeRef.index=packetRef.index;
	edgeRef.offset=packetRef.offset+edgeRefOffset-(long)sizeof(GLref);
	return(nextEdge());
}

Boolean FGLstepper::nextEdge(void){
	Ptr packetDEREF=v&packetRef;
	Ptr DEREF=v->xCheckPtr(edgeRef);  // this is a counter inside packetRef
	DEREF=&DEREF[sizeof(GLref)]; // advance to next entry
	if(edgeSign<0)++edgeIndex;
	edgeSign=-edgeSign;
	++count;
	Boolean seeking=TRUE;
	while(seeking){
		while(seeking && count<orientedEdgesNo){
			BlockMove(DEREF,(Ptr)&refDatum,sizeof(GLref));
			if(!(seeking=isNull(refDatum))){
				edgeRef.index=packetRef.index;
				edgeRef.offset=packetRef.offset
								  +edgeRefOffset+count*sizeof(GLref);
				v->e=edgeIndex*edgeSign;
			}
			else {
				DEREF=&DEREF[sizeof(GLref)]; // advance to next entry
				if(edgeSign<0)++edgeIndex;
				edgeSign=-edgeSign;
				++count;
			}
		}
		if(seeking){
			GLref oldPacket=packetRef;
			v->xGetPacketNext(packetRef,packetDEREF);  
			v-oldPacket;
			if(isNull(packetRef)){			// all edges in link have been 
				return(FALSE);  			// processed
			}
			DEREF=packetDEREF=v&packetRef; // restart for search
			DEREF=&DEREF[edgeRefOffset];	// of next packet
	 		edgeIndex=1;   
			edgeSign=1;
			count=0;
		}
	}
	v-packetRef;
	return(TRUE);  // an edge has been found
}
