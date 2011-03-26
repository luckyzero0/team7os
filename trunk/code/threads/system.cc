// system.cc 
//	Nachos initialization and cleanup routines.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"

// This defines *all* of the global data structures used by Nachos.
// These are all initialized and de-allocated by this file.

Thread *currentThread;			// the thread we are running now
Thread *threadToBeDestroyed;  		// the thread that just finished
Scheduler *scheduler;			// the ready list
Interrupt *interrupt;			// interrupt status
Statistics *stats;			// performance metrics
Timer *timer;				// the hardware timer device,
					// for invoking context switches
BitMap *physPageBitMap;
AddrSpace *processTable[PROCESS_TABLE_SIZE];	
int threadCount;	

Lock *bigLock = new Lock("bigLock");

#ifdef FILESYS_NEEDED
FileSystem  *fileSystem;
#endif

#ifdef FILESYS
SynchDisk   *synchDisk;
#endif

#ifdef USER_PROGRAM	// requires either FILESYS or FILESYS_STUB
Machine *machine;	// user program memory and registers
#endif

#ifdef USE_TLB
IPTEntry ipt[NumPhysPages];
OpenFile* swapFile;
#endif

bool PRAND = false;
bool PFIFO = true;

#ifdef NETWORK
PostOffice *postOffice;
#endif

extern void initializeLocks();
extern void initializeConditions();

// External definition, to allow us to take a pointer to this function
extern void Cleanup();


//----------------------------------------------------------------------
// TimerInterruptHandler
// 	Interrupt handler for the timer device.  The timer device is
//	set up to interrupt the CPU periodically (once every TimerTicks).
//	This routine is called each time there is a timer interrupt,
//	with interrupts disabled.
//
//	Note that instead of calling Yield() directly (which would
//	suspend the interrupt handler, not the interrupted thread
//	which is what we wanted to context switch), we set a flag
//	so that once the interrupt handler is done, it will appear as 
//	if the interrupted thread called Yield at the point it is 
//	was interrupted.
//
//	"dummy" is because every interrupt handler takes one argument,
//		whether it needs it or not.
//----------------------------------------------------------------------
static void
TimerInterruptHandler(int dummy)
{
    if (interrupt->getStatus() != IdleMode)
	interrupt->YieldOnReturn();
}

//----------------------------------------------------------------------
// Initialize
// 	Initialize Nachos global data structures.  Interpret command
//	line arguments in order to determine flags for the initialization.  
// 
//	"argc" is the number of command line arguments (including the name
//		of the command) -- ex: "nachos -d +" -> argc = 3 
//	"argv" is an array of strings, one for each command line argument
//		ex: "nachos -d +" -> argv = {"nachos", "-d", "+"}
//----------------------------------------------------------------------
void
Initialize(int argc, char **argv)
{
    int argCount;
    char* debugArgs = "";
    bool randomYield = FALSE;

#ifdef USER_PROGRAM
    bool debugUserProg = FALSE;	// single step user program
#endif
#ifdef FILESYS_NEEDED
    bool format = FALSE;	// format disk
#endif
#ifdef NETWORK
    double rely = 1;		// network reliability
    int netname = 0;		// UNIX socket name
#endif
    
    for (argc--, argv++; argc > 0; argc -= argCount, argv += argCount) {
	argCount = 1;
	if (!strcmp(*argv, "-d")) {
	    if (argc == 1)
		debugArgs = "+";	// turn on all debug flags
	    else {
	    	debugArgs = *(argv + 1);
	    	argCount = 2;
	    }
	} else if (!strcmp(*argv, "-rs")) {
	    ASSERT(argc > 1);
	    RandomInit(atoi(*(argv + 1)));	// initialize pseudo-random
						// number generator
	    randomYield = TRUE;
	    argCount = 2;
	}
#ifdef USER_PROGRAM
	if (!strcmp(*argv, "-s"))
	    debugUserProg = TRUE;
#endif
#ifdef FILESYS_NEEDED
	if (!strcmp(*argv, "-f"))
	    format = TRUE;
#endif
#ifdef NETWORK
	if (!strcmp(*argv, "-l")) {
	    ASSERT(argc > 1);
	    rely = atof(*(argv + 1));
	    argCount = 2;
	} else if (!strcmp(*argv, "-m")) {
	    ASSERT(argc > 1);
	    netname = atoi(*(argv + 1));
	    argCount = 2;
	}
#endif
    }

	initializeLocks();
	initializeConditions();
    DebugInit(debugArgs);			// initialize DEBUG messages
    stats = new Statistics();			// collect statistics
    interrupt = new Interrupt;			// start up interrupt handling
    scheduler = new Scheduler();		// initialize the ready queue
    if (randomYield)				// start the timer (if needed)
	timer = new Timer(TimerInterruptHandler, 0, randomYield);

    threadToBeDestroyed = NULL;
	physPageBitMap = new BitMap(NumPhysPages);
	threadCount = 0;
	for(int x = 0; x < PROCESS_TABLE_SIZE; x++)
		processTable[x] = NULL;
    // We didn't explicitly allocate the current thread we are running in.
    // But if it ever tries to give up the CPU, we better have a Thread
    // object to save its state. 
    currentThread = new Thread("main");		
    currentThread->setStatus(RUNNING);

    interrupt->Enable();
    CallOnUserAbort(Cleanup);			// if user hits ctl-C
    
#ifdef USER_PROGRAM
    machine = new Machine(debugUserProg);	// this must come first
#endif

#ifdef FILESYS
    synchDisk = new SynchDisk("DISK");
#endif

#ifdef FILESYS_NEEDED
    fileSystem = new FileSystem(format);
#endif

#ifdef NETWORK
    postOffice = new PostOffice(netname, rely, 10);
#endif

#ifdef USE_TLB
	//tlb is initialized in machine.cc

	// initialize IPT
	for (int i = 0; i < NumPhysPages; i++) {
		ipt[i].physicalPage = i;
		ipt[i].virtualPage = -1;
		ipt[i].spaceID = -1;
		ipt[i].pageLocation = -1;
		ipt[i].pageType = -1;
		ipt[i].timestamp = -1;
		ipt[i].readOnly = false;
		ipt[i].use = false;
		ipt[i].valid = false;
		ipt[i].dirty = false;
		ipt[i].inUse = false;
	}

	fileSystem->Create("swapFile", 16000 * PageSize);
	swapFile = fileSystem->Open("swapFile");

#endif
}

int getPhysicalPage() {
#ifdef USE_TLB
	iptLock->Acquire();
	for (unsigned int i = 0; i < NumPhysPages; i++) {
		if (!ipt[i].valid && !ipt[i].inUse) {
			//check and writeback dirty
			ipt[i].valid = true;
			ipt[i].inUse = true;
			iptLock->Release();
			return i;
		}
	}
	iptLock->Release();
	return -1;
#else
	for (int i = 0; i < NumPhysPages; i++) {
		if (!physPageBitMap->Test(i)) { //if no one has set this page
			physPageBitMap->Mark(i);
			return i;
		}
	}
	return -1;
#endif
}

int getSpaceID(AddrSpace* space) {
	for (int i = 0; i < PROCESS_TABLE_SIZE; i++) {
		if (processTable[i] == space) {
			return i;
		}
	}
	return -1;
}

void giveUpPhysicalPage(int physPageNum) {
#ifdef USE_TLB
	ASSERT(ipt[physPageNum].spaceID == getSpaceID(currentThread->space));
	ipt[physPageNum].valid = false;
#else
	physPageBitMap->Clear(physPageNum);
#endif
}

int getContiguousPhysicalPages(int numPages) {
#ifndef USE_TLB
	int startPPN = -1;
	for (int i = 0; i < NumPhysPages - numPages; i++) {
		bool worked = true;
		for (int j = 0; j < numPages; j++) {
			if (physPageBitMap->Test(i + j)) {
				worked = false;
				break;
			}
		}
		if (worked) {
			startPPN = i;
			break;
		}
	}
	
	if (startPPN != -1) { // if we fail to find numPages contiguous free pages, return -1
		for (int i = 0; i < numPages; i++) { // claim those pages as our own, and return the start page
			physPageBitMap->Mark(startPPN + i);
		}
	}	
	
	return startPPN;
#else
	return -1;
#endif
}

//----------------------------------------------------------------------
// Cleanup
// 	Nachos is halting.  De-allocate global data structures.
//----------------------------------------------------------------------
void
Cleanup()
{
    printf("\nCleaning up...\n");
#ifdef NETWORK
    delete postOffice;
#endif
    
#ifdef USER_PROGRAM
    delete machine;
#endif

#ifdef FILESYS_NEEDED
    delete fileSystem;
#endif

#ifdef FILESYS
    delete synchDisk;
#endif
    
    delete timer;
    delete scheduler;
    delete interrupt;
    
    Exit(0);
}

