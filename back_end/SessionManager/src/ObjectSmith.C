// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of class ObjectSmith
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//


#include "ObjectSmith.h"


//---------------------------------------------------------------------------//
//---------------------------- ObjectSmith ----------------------------------//
//---------------------------------------------------------------------------//


int ObjectSmith::registerCallBack(Ctor* ctor)
{
  if ( freeCallBack - theCallBacks >= theCallBacksLen ) {
         //@njz
         //	 Ctor** temp = new (Ctor*)[theCallBacksLen + jumpSize];
	 Ctor** temp = new Ctor*[theCallBacksLen + jumpSize];
	 //
	 for( int i = 0; i < theCallBacksLen; ++i )
		temp[i] = theCallBacks[i];
	 delete [] theCallBacks;
	 theCallBacks = temp;
	 freeCallBack = theCallBacks + theCallBacksLen;
	 theCallBacksLen += jumpSize;
  }

  *freeCallBack = ctor;
  return freeCallBack++ - theCallBacks;
}


void ObjectSmith::readMessage(istream& istr)
{
  int i;
  istr >> i;
  if ( i >= 0 && i < theCallBacksLen )
	 theCallBacks[i]->readFrom(istr);

  //@rn else perhaps warn
}


//---------------------------------------------------------------------------//
//---------------------------- ObjectSmith ----------------------------------//
//----------------------- static initializations ----------------------------//

int ObjectSmith::theCallBacksLen = 0;
Ctor** ObjectSmith::theCallBacks = 0;
Ctor** ObjectSmith::freeCallBack = 0;

// gcc 2.6.3 wets itself if I try anything more, so the rest of the
// init is done in ObjectSmith::registerCallBack.
