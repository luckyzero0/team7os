// ipt.h
#ifndef IPT_H
#define IPT_H

#include "translate.h"

#define PageTypeCode 0
#define PageTypeData 1
#define PageTypeMixed 2

#define PageLocationNotOnDisk 0
#define PageLocationExecutable 1
#define PageLocationSwapFile 2

class IPTEntry: public TranslationEntry {
	public:
		int pageType;
		int pageLocation;
		int spaceID;
		int timestamp;
		int byteOffset;
		int byteSize;
};

#endif
