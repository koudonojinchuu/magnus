#ifndef Frank_TietzeTypes
#define Frank_TietzeTypes

#include "Ftime.h"
class FrandomRef;
class FuTree;

enum TietzeType{
	t1,
	t1p,
	t2,
	t2p
};



class TT1info{public: Boolean memoryOK,done; 
				FrandomRef *buffer;
				FuTree *uTree;};
class TT1pinfo{public: Boolean memoryOK,done;
				GLref loadRel;};
class TT2info{public: Boolean memoryOK,done;};
class TT2pinfo{public: Boolean memoryOK,done;
				long targetNumber;};
class TTinfo{public: TT1info TT1; TT1pinfo TT1p;
				TT2info TT2; TT2pinfo TT2p;} ;
class bankAccount{public: clock_t balance,minRequired,payCheck;};
class TTbank{public: bankAccount TT1account,TT1paccount,TT2account,
				TT2paccount;};
class compareLink{public: GLref pdata,next,prev;};

#endif

