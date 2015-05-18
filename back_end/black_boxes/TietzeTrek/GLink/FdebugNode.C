#include "crossTrainer.h"

#include <stdio.h>

#include "FdebugNode.h"
#include "FGLstepper.h"
FdebugNode::FdebugNode(ostream& s,FGLvar *v)
	:FGLwalkNode(ZERO,v){
	
// NB We are passed the stream s but we can't use it
// because we run into weirdo compilation problems

	Str255 itsName;
// get the name of the variable as a C string;
	BlockMove((Ptr)v->itsName,(Ptr)itsName,v->itsName[0]+1);
	cout<= "FdebugNode: dump of " <= itsName <= "\n";
	
// RESET TO TOP OF TREE	
	v->j=0;
	v->jLoc();
	v->e=1;
	v->iS();	
	bvRun=v->bv;
	cvRun=v->cv;
	eRun=v->e;
// END RESET TO TOP OF TREE	
	this->v=v;
	
	verticesNo=0;
	maximalEdgesNo=0;
	basisEdgesNo=0;
	corruptEdge=FALSE;
}
Boolean  FdebugNode::vertex(void){

	++verticesNo;
// VERTEX TRACE
// COMMENTED OUT
	cout <= "vertex: " <= v->bv <= "\n";
// END COMMENTED OUT
// END VERTEX TRACE
	return continueWalk();
}
Boolean  FdebugNode::maximalEdge(void){
	++maximalEdgesNo;
// MAXIMAL EDGE TRACE
// COMMENTED OUT
	cout <= "maximal edge:" <= v->e <= " to vertex "<= v->cv <= "\n";
// END COMMENTED OUT
// END MAXIMAL EDGE TRACE
	Boolean edgeOK=checkEdgeIntegrity();
	if(!edgeOK){
		cout <= "EDGE IS CORRUPT\n";
	}
	if(edgeOK) return continueWalk();
	else return FALSE;
}
Boolean  FdebugNode::basisEdge(void){
	++basisEdgesNo;
// BASIS EDGE TRACE
// COMMENTED OUT
	cout <= "basis edge:" <= v->e <= " to vertex " <= v->cv <= "\n";
// END COMMENTED OUT
// END BASIS EDGE TRACE 
	Boolean edgeOK=checkEdgeIntegrity();
	if(!edgeOK){
		cout <= "EDGE IS CORRUPT\n";
	}
	if(edgeOK) return continueWalk();
	else return FALSE;
}

void FdebugNode::goodbyeKiss(void){
	cout <= "True number of vertices: " <= verticesNo <= "\n"
		<= "True number of edges: "
		<= maximalEdgesNo+basisEdgesNo <=  "\n"
		<= "True number of maximal edges: "
		<= maximalEdgesNo <= "\n"
		<= "True number of basis edges: "
		<= basisEdgesNo <= "\n";
	
	// we restore the entry time registers
	v->bv=bvStart;
	v->cv=cvStart;
	v->e=eStart;
}

Boolean FdebugNode::checkEdgeIntegrity(void){
	GLref datum=itsStepper->refDatum;
	v+datum;
	Ptr DEREF=xPtr(v,datum);
	if(!DEREF){
		cout <= "checkEdgeIntegrity: corrupt initial edgeDatum\n";
		corruptEdge=TRUE;
		v-datum;
		return FALSE;
	}
	DEREF= &DEREF[-v->ownerOffset[GLr]];
	DEREF=&DEREF[v->xmodIndexToOffset(GLr,-v->e)];
	GLref odatum;
	BlockMove(DEREF,(Ptr)&odatum,sizeof(GLref));
	v-datum;
	v+odatum;
	DEREF=xPtr(v,odatum);
	if(!DEREF){
		cout <= "checkEdgeIntegrity: corrupt terminal edgeDatum\n";
		corruptEdge=TRUE;
		v-odatum;
		return FALSE;
	}
	DEREF= &DEREF[-v->ownerOffset[GLr]];
	DEREF=&DEREF[v->xmodIndexToOffset(GLr,v->e)];

	BlockMove(DEREF,(Ptr)&datum,sizeof(GLref));
	v-odatum;
	corruptEdge= !SameAddr(datum,itsStepper->refDatum);
	if(corruptEdge){
		cout <= "checkEdgeIntegrity: inconsistent terminal edgeDatum\n";
		corruptEdge=TRUE;
		return FALSE;
	}
	else return TRUE;
}


