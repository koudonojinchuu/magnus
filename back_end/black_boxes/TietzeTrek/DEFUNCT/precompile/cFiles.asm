// this file must be loaded when running the SUN version on the Mac.
// These utilities are used by the oopsDebugFar++ debugger.
// The "real" versions create an exception which in turn
// looks up a message.  To bypass this, we just hope that
// stdio is still functioning at this point and print a
// message on the screen.

#include <stdio.h>
	void __noObject(void){
		printf("__noObject: Called by oopsDebug: message sent\n");
		printf("to a bad object pointer/handler\n");
		exit(0);
	}
	void __noMethod(void){
		printf( "__noMethod: Called by oopsDebug: message lookup failed.\n");
		exit(0);
	}
