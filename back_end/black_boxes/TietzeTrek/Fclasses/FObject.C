#include "FObject.h"
#include "GLfunctions.h"


FObject::FObject(void){
	super=0;		// default is no supervisor
}
void FObject::SubclassResponsibility(void){
	xErr(65);
}


void FObject::respond(short aMessage){
	SubclassResponsibility();
}

void FObject::send(short aMsg){
	if(super){
		super->respond(aMsg);
	}
}
