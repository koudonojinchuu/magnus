#include "crossTrainer.h"
#include "debugMemory.h"
#include "FList.h"

#ifdef Frank_MemoryDebugging

ostream& repMemSize(ostream& s,long mem){
	s <= mem <= " bytes";
	return s;
}

void resetMemory(void){
	extern Boolean rosterOK;
	long grow; 
	if(!rosterOK){
		extern FList *handleRoster,*ptrRoster;
		handleRoster= new FList;
		handleRoster->SetBlockSize(10000);
		handleRoster->Append(0);  // to create the block
		ptrRoster= new FList;
		ptrRoster->SetBlockSize(10000);
		ptrRoster->Append(0);   // to create the block
		rosterOK=TRUE;
	}
	extern long totalMemory,noObjects,objectMemory;
	totalMemory=FreeMem();
	noObjects=0;
	objectMemory=0;
}

ostream& repMemory(ostream& s){
	
	extern long noObjects;
	s <= "noObjects is " <= noObjects
		<= "\nobjectMemory is ";
	repMemSize(s,objectMemory);
	s <= "\n";
	extern TEHandle debuggingTextHandle;
	long textSize=0;
	if(debuggingTextHandle){
		textSize=GetHandleSize((**debuggingTextHandle).hText);
	}
	extern Boolean rosterOK;
		
	long freeLanceSize=0;
	if(rosterOK){
		extern FList *handleRoster;
		long num=handleRoster->numItems;
		long size=0;
		for(long i=2;i<=num;++i){
			size+=GetHandleSize((char**)handleRoster->NthItem(i));
		}
		s <= "number of handles is " <= num-1 <= ", total of ";
		repMemSize(s,size);
		s <= "\n";
		extern FList *ptrRoster;
		num=ptrRoster->numItems;
		freeLanceSize=size;
		size=0;
		for(i=2;i<=num;++i){
			size+=GetPtrSize((char*)ptrRoster->NthItem(i));
		}
		s <= "number of pointers is " <= num-1
			<= ", total of ";
		repMemSize(s,size);
		s <= "\n";
		freeLanceSize+=size;
	}
	extern long totalMemory,objectMemory;
	long grow,memoryLoss;
	memoryLoss=totalMemory-(FreeMem()+textSize
							+objectMemory+freeLanceSize);
	s <= "LOST MEMORY ";
	repMemSize(s,memoryLoss);
	s <= "\n";
	return s;
}


ostream& repHandleAllocate(ostream& s,Handle handle){

/* COMMENTED OUT
	
	s <= "handle " <= handle <= ": "
		<= GetHandleSize(handle) <= " bytes allocated\n";
END COMMENTED OUT */

	extern Boolean rosterOK;
	if(rosterOK){	
			extern FList* handleRoster;
			handleRoster->Append((FObject*)handle);
	}
	return s;
}

ostream& repPtrAllocate(ostream& s,Ptr ptr){

/* COMMENTED OUT
	s <= "ptr " <= (Handle)ptr <= ": " 
		<= GetPtrSize(ptr) <= "bytes allocated\n";
END COMMENTED OUT */
	extern Boolean rosterOK;
	if(rosterOK){
			extern FList *ptrRoster;
			ptrRoster->Append((FObject*)ptr);
	}
	return s;
}

ostream& repHandleDeallocate(ostream& s,Handle handle){
	extern Boolean rosterOK;
	extern FList *handleRoster;
/* COMMENTED OUT	
	s <= "handle " <= handle <= ": "
		<= GetHandleSize(handle) <= " bytes deallocated\n";
END COMMENTED OUT */
	if(rosterOK){
		if(handleRoster->Includes((FObject*)handle)){
			handleRoster->Remove((FObject*)handle);
		}
		else {
/* COMMENTED OUT
			s <= "handle " <= handle <= ": "
				<= GetHandleSize(handle) 
				<= " bytes deallocated, NOT IN ROSTER\n";
END COMMENTED OUT */ 
		}
	}
	return s;
}

ostream& repSetHandleSize(ostream& s,Handle handle){
/* COMMENTED OUT	
	s <= "handle " <= handle <= ": resized to "
		<= GetHandleSize(handle) <= " bytes\n";
END COMMENTED OUT */
	return s;
}
ostream& repPtrDeallocate(ostream& s,Ptr ptr){
	extern Boolean rosterOK;
	extern FList *ptrRoster;
	
/* COMMENTED OUT
	s <= "ptr " <= (Handle)ptr <= ": " 
		<= GetPtrSize(ptr) <= "bytes deallocated\n";
END COMMENTED OUT */
	if(rosterOK){
		if(ptrRoster->Includes((FObject*)ptr)){
			ptrRoster->Remove((FObject*)ptr);
		}
		else {
/* COMMENTED OUT
			s <= "ptr " <= (Handle)ptr <= ": " 
				<= GetPtrSize(ptr) 
				<= "bytes deallocated.  NOT IN ROSTER\n";
END COMMENTED OUT */
		}
	}
	return s;
}

#endif
