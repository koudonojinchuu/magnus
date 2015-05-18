#include "crossTrainer.h"
#include "FGLrecycleStack.h"
#include "GLfunctions.h"
#include "FGLink.h"

	
FGLrecycleStack::FGLrecycleStack(GLflag flag,short aBlockSize)
	:FGLstack(aBlockSize){
	this->flag=flag;
}				
						
Boolean FGLrecycleStack::recyclePush(FGLvarInterface *v,const GLref& addr){
//checks to see if this packet is already dead
	Ptr DEREF=v&addr;
	if(DEREF==ZERO) xErr(24);
	GLref ref;
	GLref DEADVERTEX=FGLink::DEADVERTEX;
	switch(flag){
		case GLr:
		case GLir:
		case GLi:
		case GLlut:
			BlockMove(&DEREF[sizeof(long)],(Ptr)&ref,sizeof(GLref));	
			if(ref.index==DEADVERTEX.index)xErr(25);
			BlockMove((Ptr)&DEADVERTEX,&DEREF[sizeof(long)],sizeof(GLref));
			break;
		case GLrv:
		case GLiv:
			BlockMove(&DEREF[2*sizeof(Boolean)],(Ptr)&ref,sizeof(GLref));	
			if(ref.index==DEADVERTEX.index)xErr(25);
			BlockMove((Ptr)&DEADVERTEX,&DEREF[2*sizeof(Boolean)],sizeof(GLref));
			break;
	}
	v-addr;
	FGLstack::Push(addr);
	return TRUE;
}						
			
void FGLrecycleStack::recyclePop(GLref& addr,FGLvarInterface *v){
// bring this packet back to life, but first make sure it is dead
	FGLstack::Pop(addr);
	if(isNull(addr))return; //nothing on stack, don't worry about it
	Ptr DEREF=v&addr;
	if(DEREF==ZERO)xErr(26);
	GLref ref;
	GLref NA=FGLink::NA;
	GLref DEADVERTEX=FGLink::DEADVERTEX;
	switch(flag){
		case GLr:
		case GLir:
		case GLi:
		case GLlut:
			BlockMove(&DEREF[sizeof(long)],(Ptr)&ref,sizeof(GLref));	
			if(ref.index!=DEADVERTEX.index)xErr(27);
			BlockMove((Ptr)&NA,&DEREF[sizeof(long)],sizeof(GLref));
			break;
		case GLrv:
		case GLiv:
			BlockMove(&DEREF[2*sizeof(Boolean)],(Ptr)&ref,sizeof(GLref));	
			if(ref.index!=DEADVERTEX.index)xErr(27);
			BlockMove((Ptr)&NA,&DEREF[2*sizeof(Boolean)],sizeof(GLref));
			break;
	}
	v-addr;
}
