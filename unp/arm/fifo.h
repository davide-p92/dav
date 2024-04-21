#define AddPointerFifo(NAME,SIZE,TYPE,SUCCESS,FAIL)\
	TYPE volatile *NAME ## PutPt;\
	TYPE volatile *NAME ## GetPt;\
	TYPE static NAME ## Fifo[SIZE];\
	void NAME ## Fifo_Init(void){\
		NAME ## PutPt = NAME ## GetPt = &NAME ## Fifo[0];\
	}\
	int NAME ## Fifo_Put(TYPE data){\
		TYPE volatile *nextPutPt;\
		nextPutPt = NAME ## PutPt + 1;\
		if(nextPutPt == &NAME ## Fifo[SIZE]){\
			nextPutPt = &NAME ## Fifo[0];\
		}\
		if(nextPutPt == NAME ## GetPt){\
			return(FAIL);\
		}\
		else{\
			*(NAME ## PutPt) = data;\
			NAME ## PutPt = nextPutPt;\
			return(SUCCESS);\
		}\
	}\
	int NAME ## Fifo_Get(TYPE *datapt){\
		if(NAME ## PutPt == NAME ## GetPt){\
			return(FAIL);\
		}\
		*datapt = *(NAME ## GetPt ## ++);\
		if(NAME ## GetPt == &NAME ## Fifo[SIZE]){\
			NAME ## GetPt = &NAME ## Fifo[0];\
		}\
		
		return(SUCCESS);\
	}
