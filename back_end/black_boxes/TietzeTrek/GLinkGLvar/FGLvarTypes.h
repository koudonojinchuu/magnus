/*******************

Written by Frank Rimlinger, August 1992

*******************/
#ifndef Frank_FGLvarTypes
#define Frank_FGLvarTypes
#include "GLref.h"

class FGLvarInterface;

typedef short GLflag;
#define GLflags 6
enum {	GLr,		// real link of a real vertex 
		GLir,		// ideal link of a real vertex 
		GLi,		// ideal link of an ideal vertex 
		GLlut,		// ideal vertex look-up table.
		GLrv, 		// real vertex, used internally
		GLiv		// ideal vertex, used internally
	 };
enum {noIDslotAvailable=-1};
		
// all of the rest of the typedefs support internal data
#define resourceBasedInfo 6
class GLploc{public: long base; long modIndex;};
class breadCrumb{public: GLref ref;long e;};
enum {GLreal,GLideal};						// packet or vertex types
enum{  GLdefined,GLunusedOrNew};			// write request types
enum{  GLdefinedOrBest,GLundefinedOrBest};  // read request types
	
enum{	GLcreateInitialPacket,
		GLuseExistingPacket,
		GLprependPacket,
		GLinsertPacket,
		GLappendPacket}; // status types
		
class writePacketInfo{public: GLflag flag;			//supplied by caller
				short request;			//supplied by caller
				long index;				//supplied by caller
				GLref refDatum;			//supplied by caller
				GLref ownerVertexRef;	//supplied by caller
				GLref entryPacketRef;	//supplied by caller
				long base;				//supplied by packetWrite
				long offset;			//supplied by packetWrite
				short status;			//returned to packetWrite
				GLref smallRef;			//returned to packetWrite
				GLref bigRef;			//returned to packetWrite
				GLref existingRef;		//returned to packetWrite
				GLref exitPacketRef;	//returned to caller
			  };
			  
class readPacketInfo{ public:GLflag flag;			//supplied by caller
				short request;			//supplied by caller
				long index;				//supplied by caller
				GLref entryPacketRef;	//supplied by caller
				long base;				//supplied by packetRead
				long offset;			//supplied by packetRead
				GLref refDatum;			//returned to caller
				GLref exitPacketRef;	//returned to caller
			  };
			  
//CAUTION: don't forget to support InUse when defining a scanning function			  
	
class scanSingleInfo{public: GLflag flag;		//supplied at initialization
				void *userPtr;			//supplied at initialization
				GLref firstPacket;		//supplied at initialization
				long modIndexStart;		//supplied at initialization
				Boolean goForward;		//supplied at initialization
				Boolean skipFirstEntry;	//supplied at initialization	
				FGLvarInterface *var;		//supplied by scanning method
				GLref newPacket;		//updated by scanning method
				long base;				//updated by scanning method
				Ptr DEREF;				//initialized and updated by 
										//scanning method
				GLref refDatum;			//returned to scanning method
				Ptr DEREFcurrent;		//returned to scanning method
				Ptr DEREFlast;			//returned to scanning method
				long modIndex;			//returned to scanning method
			  };

class scanDoubleInfo{public: GLflag flag;			//supplied at initialization
				void *userPtr;			//supplied at initialization
				GLref firstPacket;		//supplied at initialization
				long modIndexStart;		//supplied at initialization
				Boolean goForward;		//supplied at initialization
				Boolean skipFirstEntry;	//supplied at initialization	
				FGLvarInterface *var;	//supplied by scanning method
				GLref newPacket;		//updated by scanning method
				long base;				//updated by scanning method
				Ptr DEREF;				//initialized and updated by 
										//scanning method
				GLref refDatum;			//returned to scanning method
				Ptr DEREFcurrent;		//returned to scanning method
				Ptr DEREFlast;			//returned to scanning method
				long plusModIndex;		//returned to scanning method
				long minusModIndex;		//returned to scanning method
				long sign;				//returned to scanning method
			  };			  
typedef Boolean (*scanSingleFunc)(scanSingleInfo *info);
typedef Boolean (*scanDoubleFunc)(scanDoubleInfo *info);

#endif

