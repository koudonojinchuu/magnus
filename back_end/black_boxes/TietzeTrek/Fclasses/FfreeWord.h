#ifndef Frank_FfreeWord
#define Frank_FfreeWord



#include "FObject.h"

class freeMultiplyInfo{public: long x,xexp,y,yexp,z,zexp;
				Boolean consolidation;};

class FfreeWord:public FObject{
public:
	long **wordHandle;
	long wordLength,noEntries,wordSize;
	Boolean leftJustified;
	long blockSize,handleSize,nextLeft,nextRight;		
	static long freeWordSize;
	
	FfreeWord(long blockSize);
	FfreeWord(Ptr& DEREF);
	FfreeWord(const FfreeWord* copyMe);
	~FfreeWord(void);
	virtual void leftJustify(void);
	virtual Boolean leftMultiply(long x, long xexp);
	virtual Boolean rightMultiply(long y, long yexp);
	static long datumSize(void);
	virtual long totalSize(void);
	virtual void storeData(Ptr& DEREF);

protected:
	virtual Boolean multiply(freeMultiplyInfo& info);
	virtual void putLeft(long x,long xexp);
	virtual void putRight(long y,long yexp);
	virtual void makeSpaceAvailable(void);
	virtual void shift(long startAt);
};

ostream& operator<=(ostream& s,FfreeWord *f);
#endif

