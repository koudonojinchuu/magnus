#include "crossTrainer.h"
#include "FfoldStepper.h"
#include "FGLvar.h"
#include "GLfunctions.h"
	
FfoldStepper::FfoldStepper(FGLvar *v,long edgeRefOffset,long orientedEdgesNo,
	              const GLref& startPacket){
	this->v=v;
	this->edgeRefOffset=edgeRefOffset;
	this->orientedEdgesNo=orientedEdgesNo;
	packetRef=startPacket;
}

Boolean FfoldStepper::firstEdge(void){
	edgeIndex=0;
	edgeSign=-1;
	count=-1;
	// place the edge ref sizeof(GLref) bytes BEFORE the first
	// slot, so that the call to nextEdge advances to the first slot
	edgeRef.index=packetRef.index;
	edgeRef.offset=packetRef.offset+edgeRefOffset-(long)sizeof(GLref);
	return(nextEdge());
}

Boolean FfoldStepper::nextEdge(void){
	// this routine only scans the packet at location packetRef
	// When the scan is done, loads the location of the next packet
	// at packetRef.  If there is no next packet, loads NA.
	// Its up to the caller to check for the next packet.  If it
	// exists, the next packet can then be scanned by calling firstEdge,
	// followed by calls to nextEdge
	
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
			GLref oldRef=packetRef;
			v->xGetPacketNext(packetRef,packetDEREF);
			v-oldRef;
			return(FALSE);  // all edges in packet have been 
							// processed
		}
	}
	v-packetRef;
	return(TRUE);  // an edge has been found
}
