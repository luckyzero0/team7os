// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "synch.h"
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stdio.h>

//Networking
#ifdef NETWORK
#include "network.h"
#include "post.h"
#include "interrupt.h"
#endif
//

#define MAX_LOCKS 100
#define MAX_CONDITIONS 200
#define MAX_MONITORS 200
#define MAX_MONITOR_ARRAYS 50
#define MAX_MONITOR_ARRAY_VALUES 50
#define MAX_TIMER_DATAS 50
#define MAX_THREADS 1000
#define SERVER_ID 0

extern "C" { int bzero(char *, int); };
extern "C" {
	int bcopy(char *, char *, int);
};

using namespace std;

int threadArgs[MAX_THREADS];

//Networking
#ifdef NETWORK
PacketHeader outPktHdr, inPktHdr;
MailHeader outMailHdr, inMailHdr;
char buffer[MaxMailSize]; //Data sent in message
bool success; //where send was successful
#endif

//For TimedSetMonitorArrayValue
struct TimerData{
	TimerData():monitorArrayID(),index(),value(),numYields(),isTaken(){}
	MonitorArrayID monitorArrayID;
	int index, value, numYields;
	bool isTaken;
};

//TimerData timerDatas[MAX_TIMER_INFOS];
TimerData timerDatas[MAX_TIMER_DATAS];
Lock* timerLock = new Lock("timerLock");
int getAvailableTimerData(){
	for(int i=0; i<MAX_TIMER_DATAS; i++){
		if (timerDatas[i].isTaken == false){
			return i;
		}
	}
	return -1;
}
/*void initializeTimerDatas(){
	for(int i=0; i<MAX_TIMER_DATA; i++){
		timerDatas[i].monitorArrayID = 0;
		timerDatas[i].index = 0;
		timerDatas[i].value = 0;
		timerDatas[i].numYields = 0;
	}
}*/



struct LockEntry {
	Lock* lock;
	AddrSpace* space;
	bool needsToBeDeleted;
	int aboutToBeAcquired;
};

LockEntry locks[MAX_LOCKS];
Lock* locksLock = new Lock("locksLock"); //used to lock the locks array
Lock* forkLock = new Lock("forkLock"); 

void initializeLocks() {
	for (int i = 0; i < MAX_LOCKS; i++) {
		locks[i].lock = NULL;
		locks[i].space = NULL;
		locks[i].needsToBeDeleted = FALSE;
		locks[i].aboutToBeAcquired = 0;
	}
}

struct ConditionEntry {
	Condition* condition;
	AddrSpace* space;
	bool needsToBeDeleted;
	int aboutToBeWaited;
};

ConditionEntry conditions[MAX_CONDITIONS];
Lock* conditionsLock = new Lock("conditionsLock"); //used to lock the conditions array

void initializeConditions() {
	for (int i = 0; i < MAX_CONDITIONS; i++) {
		conditions[i].condition = NULL;
		conditions[i].space = NULL;
		conditions[i].needsToBeDeleted = FALSE;
		conditions[i].aboutToBeWaited = 0;
	}
}

int copyin(unsigned int vaddr, int len, char *buf) {
	// Copy len bytes from the current thread's virtual address vaddr.
	// Return the number of bytes so read, or -1 if an error occors.
	// Errors can generally mean a bad virtual address was passed in.
	bool result;
	int n=0;			// The number of bytes copied in
	int *paddr = new int;

	while ( n >= 0 && n < len) {
		result = machine->ReadMem( vaddr, 1, paddr );
		while(!result) // FALL 09 CHANGES
		{
			result = machine->ReadMem( vaddr, 1, paddr ); // FALL 09 CHANGES: TO HANDLE PAGE FAULT IN THE ReadMem SYS CALL
		}	

		buf[n++] = *paddr;

		if ( !result ) {
			//translation failed
			return -1;
		}

		vaddr++;
	}

	buf[n] = '\0';

	delete paddr;
	return n;
}

int copyout(unsigned int vaddr, int len, char *buf) {
	// Copy len bytes to the current thread's virtual address vaddr.
	// Return the number of bytes so written, or -1 if an error
	// occors.  Errors can generally mean a bad virtual address was
	// passed in.
	bool result;
	int n=0;			// The number of bytes copied in

	while ( n >= 0 && n < len) {
		// Note that we check every byte's address
		result = machine->WriteMem( vaddr, 1, (int)(buf[n++]) );

		if ( !result ) {
			//translation failed
			return -1;
		}
		vaddr++;
	}

	return n;
}

// Dear Grader, 
// This is for your enjoyment during these troubling times.

//   =]

/*I fed my goat vintage whiskey through a funnel while listening to this Nantes.

As my tears fell and enveloped his rough hide, he sang me﻿ to sleep.

He told me of his childhood in India and played the bagpipes.

The goat spoke a ukranian proverb in arabic and wove hemp into a colorful blanket which he draped over my tall shoulders.

I marveled as the goat flew me over the ocean and pointed out landmarks as we darted between the clouds.

He brought me home to beirut.*/


void exec_thread(int dontUse){
	bigLock->Acquire();
	DEBUG('a', "Exec thread is executing with spaceID[%d].\n", getSpaceID(currentThread->space));
	currentThread->space->InitRegisters();		// set the initial register values
	currentThread->space->RestoreState();		// load page table register
	bigLock->Release();
	machine->Run();
}

SpaceID Exec_Syscall(unsigned int vaddr, int len){
	bigLock->Acquire();
	static int called = 0; 

	DEBUG('e', "In EXEC for the %d time.\n", called++);

	OpenFile *f;			// The new open file
	char *buf = new char[len+1];
	if (!buf){
		printf("%s","Can't allocate kernel buffer in Open\n");
		bigLock->Release();
		return -1;
	}
	if( copyin(vaddr,len,buf) == -1 ) {
		printf("%s","Bad pointer passed to Exec\n");
		delete buf;
		bigLock->Release();
		return -1;
	}

	DEBUG('e', "We are trying to open the file: %s\n", buf);

	f = fileSystem->Open(buf);
	delete[] buf;

	if ( f ) {
		SpaceID spaceID = -1;
		//Update the process table and related data structures
		for (int i = 0; i<PROCESS_TABLE_SIZE; i++){
			if (processTable[i] == NULL){
				spaceID = i;
				break;
			}
		}

		if (spaceID == -1){
			printf("%s","No space on the process table for this new process!\n");
			bigLock->Release();
			return spaceID;
		}

		AddrSpace* addrSpace = new AddrSpace(f, spaceID);
		if (!addrSpace->didConstructSuccessfully()) {
			printf("Error in AddrSpace constructor, aborting EXEC call.\n");
			return -1;
		}

		processTable[spaceID] = addrSpace;

		//For right now we assume physical pages were handed out successfully, because we were told we have infinite space for this assignment.
		DEBUG('e', "Current thread in EXEC has %d numPages.\n", currentThread->space->getNumPages());

		Thread* t = new Thread("dammitmihir");
		t->space = addrSpace;
		t->startVPN = t->space->getMainThreadStartVPN();



		DEBUG('e', "Made a new process at SpaceID[%d], forking the exec_thread.\n", spaceID);

		t->Fork(exec_thread, 0);
		bigLock->Release();

		return spaceID;
	} else {
		printf("Couldn't open the specified file in EXEC!\n");
		bigLock->Release();
		return -1;
	}
}


void Create_Syscall(unsigned int vaddr, int len) {
	// Create the file with the name in the user buffer pointed to by
	// vaddr.  The file name is at most MAXFILENAME chars long.  No
	// way to return errors, though...
	char *buf = new char[len+1];	// Kernel buffer to put the name in

	if (!buf) return;

	if( copyin(vaddr,len,buf) == -1 ) {
		printf("%s","Bad pointer passed to Create\n");
		delete buf;
		return;
	}

	buf[len]='\0';

	fileSystem->Create(buf,0);
	delete[] buf;
	return;
}

int Open_Syscall(unsigned int vaddr, int len) {
	// Open the file with the name in the user buffer pointed to by
	// vaddr.  The file name is at most MAXFILENAME chars long.  If
	// the file is opened successfully, it is put in the address
	// space's file table and an id returned that can find the file
	// later.  If there are any errors, -1 is returned.
	char *buf = new char[len+1];	// Kernel buffer to put the name in
	OpenFile *f;			// The new open file
	int id;				// The openfile id

	if (!buf) {
		printf("%s","Can't allocate kernel buffer in Open\n");
		return -1;
	}

	if( copyin(vaddr,len,buf) == -1 ) {
		printf("%s","Bad pointer passed to Open\n");
		delete[] buf;
		return -1;
	}

	buf[len]='\0';

	f = fileSystem->Open(buf);
	delete[] buf;

	if ( f ) {
		if ((id = currentThread->space->fileTable.Put(f)) == -1 )
			delete f;
		return id;
	}
	else
		return -1;
}


void Write_Syscall(unsigned int vaddr, int len, int id) {
	// Write the buffer to the given disk file.  If ConsoleOutput is
	// the fileID, data goes to the synchronized console instead.  If
	// a Write arrives for the synchronized Console, and no such
	// console exists, create one. For disk files, the file is looked
	// up in the current address space's open file table and used as
	// the target of the write.

	char *buf;		// Kernel buffer for output
	OpenFile *f;	// Open file for output


	if ( id == ConsoleInput) return;

	if ( !(buf = new char[len]) ) {
		printf("%s","Error allocating kernel buffer for write!\n");
		return;
	} else {
		if ( copyin(vaddr,len,buf) == -1 ) {
			printf("%s","Bad pointer passed to to write: data not written\n");
			delete[] buf;
			return;
		}
	}

	if ( id == ConsoleOutput) {
		for (int ii=0; ii<len; ii++) {
			printf("%c",buf[ii]);
		}

	} else {
		if ( (f = (OpenFile *) currentThread->space->fileTable.Get(id)) ) {
			f->Write(buf, len);
		} else {
			printf("%s","Bad OpenFileId passed to Write\n");
			len = -1;
		}
	}

	delete[] buf;
}

int Read_Syscall(unsigned int vaddr, int len, int id) {
	// Write the buffer to the given disk file.  If ConsoleOutput is
	// the fileID, data goes to the synchronized console instead.  If
	// a Write arrives for the synchronized Console, and no such
	// console exists, create one.    We reuse len as the number of bytes
	// read, which is an unnessecary savings of space.
	char *buf;		// Kernel buffer for input
	OpenFile *f;	// Open file for output

	if ( id == ConsoleOutput) return -1;

	if ( !(buf = new char[len]) ) {
		printf("%s","Error allocating kernel buffer in Read\n");
		return -1;
	}

	if ( id == ConsoleInput) {
		//Reading from the keyboard
		scanf("%s", buf);

		if ( copyout(vaddr, len, buf) == -1 ) {
			printf("%s","Bad pointer passed to Read: data not copied\n");
		}
	} else {
		if ( (f = (OpenFile *) currentThread->space->fileTable.Get(id)) ) {
			len = f->Read(buf, len);
			if ( len > 0 ) {
				//Read something from the file. Put into user's address space
				if ( copyout(vaddr, len, buf) == -1 ) {
					printf("%s","Bad pointer passed to Read: data not copied\n");
				}
			}
		} else {
			printf("%s","Bad OpenFileId passed to Read\n");
			len = -1;
		}
	}

	delete[] buf;
	return len;
}

void Close_Syscall(int fd) {
	// Close the file associated with id fd.  No error reporting.
	OpenFile *f = (OpenFile *) currentThread->space->fileTable.Remove(fd);

	if ( f ) {
		delete f;
	} else {
		printf("%s","Tried to close an unopen file\n");
	}
}

int getNumProcesses() {
	int numProcesses = 0;
	for (int i = 0; i < PROCESS_TABLE_SIZE; i++) {
		if (processTable[i] != NULL) {
			numProcesses++;
		}
	}
	return numProcesses;
}



void Exit_Syscall(int status) {
	bigLock->Acquire();
	int numProcesses = getNumProcesses();

	DEBUG('p', "In exit!, with %d processes currently active.  Current process has id %d with %d active threads.\n", numProcesses,
		getSpaceID(currentThread->space), currentThread->space->numThreads);

	if (numProcesses == 1 && currentThread->space->numThreads == 0) { //we are the final thread remaining
		printf("Exiting final process with return value: %d.\n", status);
		interrupt->Halt();
	} else if (currentThread->space->numThreads == 0) { //kill the process and free the address space and stuff
		SpaceID spaceID = getSpaceID(currentThread->space);
		printf("Exiting non-final process %d with return value: %d.\n", spaceID, status);

		delete currentThread->space;
		processTable[spaceID] = NULL;
		bigLock->Release();
		DEBUG('c', "In KILL PROCESS block of exit for SpaceID[%d].\n", spaceID);
		currentThread->Finish();
	} else { //we are not the last thread in a process, so just kill the thread
		DEBUG('p', "Giving up a non-final thread in a process.\n");
		printf("Exiting thread with return value: %d\n", status);
		currentThread->space->RemoveCurrentThread();
		bigLock->Release();
		currentThread->Finish();
	}

}


int getAvailableLockID() {
	int index = -1;
	for (int i = 0; i < MAX_LOCKS; i++) {
		if (locks[i].lock == NULL) {
			index = i;
			break;
		}
	}
	return index;
}

LockID CreateLock_Syscall(unsigned int vaddr, int len) {	
	char* buf;
	if ( !(buf = new char[len]) ) {
		printf("%s","Error allocating kernel buffer for write!\n");
		return -1;
	} else {
		if ( copyin(vaddr,len,buf) == -1 ) {
			printf("%s","Bad pointer passed to to write: data not written\n");
			delete[] buf;
			return -1;
		}
	}

	// If Networking is enabled, send message to server asking it to create
	// a lock. The LockID will be returned to the client.
#ifdef NETWORK
	char msg[MaxMailSize] = {""};	
	sprintf(msg,"%d,%s,*",SC_CreateLock,buf);	//Message is in the form [<RequestType><data><ThreadID>]	
	
	outPktHdr.to = rand()%NUM_SERVERS;
	outMailHdr.to = 0;
	outMailHdr.from = currentThread->ID;
	outMailHdr.length = strlen(msg) + 1;
	bool success = postOffice->Send(outPktHdr, outMailHdr, msg);
	if ( !success ) {
		printf("The postOffice Send to server failed.\n");
		interrupt->Halt();      	      	       	      	 
	} 
	postOffice->Receive(currentThread->ID, &inPktHdr, &inMailHdr, buffer); //Check my mailbox, which corresponds to my threadID
	DEBUG('a',"Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.from);
	fflush(stdout);
	LockID lockID = atoi(buffer);
	return lockID;

#else
	locksLock->Acquire();
	int index = getAvailableLockID();
	if (index == -1) {
		printf("No locks available!\n");
	} else {
		locks[index].lock = new Lock(buf);
		locks[index].space = currentThread->space;
		printf("Created lock with name: %s\n", locks[index].lock->getName());
	}
	locksLock->Release();
	DEBUG('a', "Returning lock index: %d\n", index); //DEBUG
	return index;
#endif


}

void deleteLock(int id) {
	delete locks[id].lock;
	locks[id].lock = NULL;
	locks[id].space = NULL;
	locks[id].needsToBeDeleted = FALSE;
	locks[id].aboutToBeAcquired = 0;
}

void DestroyLock_Syscall(LockID id) {
	if (id < 0 || id >= MAX_LOCKS) {
		printf("LockID[%d] is out of range!\n", id);
		return;
	}

	
	// If Networking is enabled, send message to server asking it to destroy
	// a lock.

#ifdef NETWORK
	char msg[MaxMailSize] = {""};
	sprintf(msg,"%d,%d,*",SC_DestroyLock,id); //Message is in the form [<RequestType><data><ThreadID>]

	outPktHdr.to = rand()%NUM_SERVERS;
	outMailHdr.to = 0;
	outMailHdr.from = currentThread->ID;
	outMailHdr.length = strlen(msg) + 1;
	bool success = postOffice->Send(outPktHdr, outMailHdr, msg);
	if ( !success ) {
		printf("The postOffice Send to Server failed.\n");
		interrupt->Halt();      	      	       	      	 
	} 
	postOffice->Receive(currentThread->ID, &inPktHdr, &inMailHdr, buffer); //Check my mailbox, which corresponds to my threadID
	DEBUG('a',"Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.from); 
	fflush(stdout);
	
#else
	locksLock->Acquire();
	if (locks[id].space != currentThread->space) {
		printf("LockID[%d] cannot be destroyed from a non-owning process!\n", id);
	} else {
		if (locks[id].lock->IsBusy() || locks[id].aboutToBeAcquired > 0) {
			locks[id].needsToBeDeleted = TRUE;
			DEBUG('a', "Lock[%d] will be deleted when possible.\n",id); //DEBUG
		} else {
			deleteLock(id);
			DEBUG('a', "Lock[%d] has been deleted.\n",id);//DEBUG
		}
	}
	locksLock->Release();
#endif
}

int getAvailableConditionID() {
	int index = -1;
	for (int i = 0; i < MAX_CONDITIONS; i++) {
		if (conditions[i].condition == NULL) {
			index = i;
			break;
		}
	}
	return index;
}


ConditionID CreateCondition_Syscall(unsigned int vaddr, int len) {
	char* buf;
	if ( !(buf = new char[len]) ) {
		printf("%s","Error allocating kernel buffer for write!\n");
		return -1;
	} else {		
		if( copyin(vaddr,len,buf) != (signed int)strlen(buf) ) {
			printf("%s","Bad string length, data not written.\n");
			delete[] buf;
			return -1;
		}		
		if ( copyin(vaddr,len,buf) == -1 ) {
			printf("%s","Bad pointer passed to to write: data not written\n");
			delete[] buf;
			return -1;
		}				
	}

	// If Networking is enabled, send message to server asking it to create
	// a condition variable. ConditionID will be returned to the client

#ifdef NETWORK
	char msg[MaxMailSize];
	sprintf(msg,"%d,%s,*",SC_CreateCondition,buf); //Message is in the form [<RequestType><data><ThreadID>]

	outPktHdr.to = rand()%NUM_SERVERS;
	outMailHdr.to = 0;
	outMailHdr.from = currentThread->ID;
	outMailHdr.length = strlen(msg) + 1;
	bool success = postOffice->Send(outPktHdr, outMailHdr, msg);
	if ( !success ) {
		printf("The postOffice Send to Server failed.\n");
		interrupt->Halt();      	      	       	      	 
	} 


	postOffice->Receive(currentThread->ID, &inPktHdr, &inMailHdr, buffer); //Check my mailbox, which corresponds to my threadID
	DEBUG('a',"Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.from);
	fflush(stdout);
	ConditionID conditionID = atoi(buffer);
	return conditionID;
#else
	conditionsLock->Acquire();
	int index = getAvailableConditionID();
	if (index == -1) {
		printf("No conditions available!\n");
	} else {
		conditions[index].condition = new Condition(buf);
		conditions[index].space = currentThread->space;
	}
	conditionsLock->Release();

	return index;
#endif
}

void deleteCondition(int id) {
	delete conditions[id].condition;
	conditions[id].condition = NULL;
	conditions[id].space = NULL;
	conditions[id].needsToBeDeleted = FALSE;
	conditions[id].aboutToBeWaited = 0;
}

void DestroyCondition_Syscall(ConditionID id) {
	if (id < 0 || id >= MAX_CONDITIONS) {
		printf("ConditionID[%d] is out of range!\n", id);
		return;
	}

	// If Networking is enabled, send message to server asking it to destroy
	// a condition variable. 

#ifdef NETWORK
	char msg[MaxMailSize];
	sprintf(msg,"%d,%d,*",SC_DestroyCondition,id); //Message is in the form [<RequestType><data><ThreadID>]

	outPktHdr.to = rand()%NUM_SERVERS;
	outMailHdr.to = 0;
	outMailHdr.from = currentThread->ID;
	outMailHdr.length = strlen(msg) + 1;
	bool success = postOffice->Send(outPktHdr, outMailHdr, msg);
	if ( !success ) {
		printf("The postOffice Send to Server failed.\n");
		interrupt->Halt();      	      	       	      	 
	} 
	postOffice->Receive(currentThread->ID, &inPktHdr, &inMailHdr, buffer); //Check my mailbox, which corresponds to my threadID
	DEBUG('a',"Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.from);
	fflush(stdout);
#else
	conditionsLock->Acquire();
	if (conditions[id].space != currentThread->space) {
		printf("ConditionID[%d] cannot be destroyed from a non-owning process!\n", id);
	} else {
		if (conditions[id].condition->HasThreadsWaiting() || conditions[id].aboutToBeWaited > 0) {
			conditions[id].needsToBeDeleted = TRUE;
		} else {
			deleteCondition(id);
		}
	}
	conditionsLock->Release();
#endif
}

void Acquire_Syscall(LockID id) {
	if (id < 0 || id >= MAX_LOCKS) {
		printf("LockID[%d] is out of range!\n", id);
		return;
	}

	// If Networking is enabled, send message to server asking it to acquire
	// a certain lock. Msg will only be sent back when lock is available

#ifdef NETWORK
	char msg[MaxMailSize];
	sprintf(msg,"%d,%d,*",SC_Acquire,id,currentThread->ID); //Message is in the form [<RequestType><data><ThreadID>]

	outPktHdr.to = rand()%NUM_SERVERS;
	outMailHdr.to = 0;
	outMailHdr.from = currentThread->ID;
	outMailHdr.length = strlen(msg) + 1;
	bool success = postOffice->Send(outPktHdr, outMailHdr, msg);
	if ( !success ) {
		printf("The postOffice Send to Server failed.\n");
		interrupt->Halt();      	      	       	      	 
	} 
	postOffice->Receive(currentThread->ID, &inPktHdr, &inMailHdr, buffer); //Check my mailbox, which corresponds to my threadID
	DEBUG('a',"Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.from);
	fflush(stdout);
#else

	locksLock->Acquire();
	if (locks[id].space != currentThread->space) {
		printf("LockID[%d] cannot be acquired from a non-owning process!\n", id);
		locksLock->Release();
		return;
	} 

	locks[id].aboutToBeAcquired++;
	locksLock->Release();
	locks[id].lock->Acquire();
	locks[id].aboutToBeAcquired--;
	DEBUG('a', "Lock [%d] has been acquired.\n", id); //DEBUG
#endif
}


void Release_Syscall(LockID id) {
	if (id < 0 || id >= MAX_LOCKS) {
		printf("LockID[%d] is out of range!\n", id);
		return;
	}

	// If Networking is enabled, send message to server asking it to release
	// a certain lock. 

#ifdef NETWORK
	char msg[MaxMailSize];
	sprintf(msg,"%d,%d,*",SC_Release,id,currentThread->ID); //Message is in the form [<RequestType><data><ThreadID>]

	outPktHdr.to = rand()%NUM_SERVERS;
	outMailHdr.to = 0;
	outMailHdr.from = currentThread->ID;
	outMailHdr.length = strlen(msg) + 1;
	bool success = postOffice->Send(outPktHdr, outMailHdr, msg);
	if ( !success ) {
		printf("The postOffice Send to Server failed.\n");
		interrupt->Halt();      	      	       	      	 
	} 
	postOffice->Receive(currentThread->ID, &inPktHdr, &inMailHdr, buffer); //Check my mailbox, which corresponds to my threadID
	DEBUG('a',"Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.from);
	fflush(stdout);
#else

	locksLock->Acquire();
	if (locks[id].space != currentThread->space) {
		printf("LockID[%d] cannot be released from a non-owning process!\n", id);
		locksLock->Release();
		return;
	}

	locks[id].lock->Release();
	if (locks[id].needsToBeDeleted && !locks[id].lock->IsBusy() 
		&& locks[id].aboutToBeAcquired == 0) {
			deleteLock(id);
	}
	locksLock->Release();

	DEBUG('a', "Releasing lock[%d].\n",id); //DEBUG
#endif
}

void Signal_Syscall(ConditionID conditionID, LockID lockID) {
	if (conditionID < 0 || conditionID >= MAX_CONDITIONS) {
		printf("ConditionID[%d] is out of range!\n", conditionID);
		return;
	}

	if (lockID < 0 || lockID >= MAX_CONDITIONS) {
		printf("LockID[%d] is out of range!\n", lockID);
		return;
	}

	// If Networking is enabled, send message to server asking it to signal
	// a lock from a CV. 

#ifdef NETWORK
	char msg[MaxMailSize];
	sprintf(msg,"%d,%d,%d,*",SC_Signal,conditionID, lockID,currentThread->ID); //Message is in the form [<RequestType><data><ThreadID>]

	outPktHdr.to = rand()%NUM_SERVERS;
	outMailHdr.to = 0;
	outMailHdr.from = currentThread->ID;
	outMailHdr.length = strlen(msg) + 1;
	bool success = postOffice->Send(outPktHdr, outMailHdr, msg);
	if ( !success ) {
		printf("The postOffice Send to Server failed.\n");
		interrupt->Halt();      	      	       	      	 
	} 
	postOffice->Receive(currentThread->ID, &inPktHdr, &inMailHdr, buffer); //Check my mailbox, which corresponds to my threadID
	DEBUG('a',"Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.from);
	fflush(stdout);
#else

	conditionsLock->Acquire();
	locksLock->Acquire();
	if (conditions[conditionID].space != currentThread->space) {
		printf("ConditionID[%d] cannot be waited from a non-owning process!\n", conditionID);
		locksLock->Release();
		conditionsLock->Release();
		return;
	} 

	if (locks[lockID].space != currentThread->space) {
		printf("LockID[%d] cannot be passed to Wait from a non-owning process!\n", lockID);
		locksLock->Release();
		conditionsLock->Release();
		return;
	}

	conditions[conditionID].condition->Signal(locks[lockID].lock);

	if (conditions[conditionID].needsToBeDeleted 
		&& !conditions[conditionID].condition->HasThreadsWaiting()
		&& conditions[conditionID].aboutToBeWaited == 0) {
			deleteCondition(conditionID);
	}
	locksLock->Release();
	conditionsLock->Release();
#endif
}

void Wait_Syscall(ConditionID conditionID, LockID lockID) {
	if (conditionID < 0 || conditionID >= MAX_CONDITIONS) {
		printf("ConditionID[%d] is out of range!\n", conditionID);
		return;
	}

	if (lockID < 0 || lockID >= MAX_CONDITIONS) {
		printf("LockID[%d] is out of range!\n", lockID);
		return;
	}

	// If Networking is enabled, send message to server asking it to make a CV wait on a lock

#ifdef NETWORK
	char msg[MaxMailSize];
	sprintf(msg,"%d,%d,%d,*",SC_Wait,conditionID, lockID,currentThread->ID); //Message is in the form [<RequestType><data><ThreadID>]

	outPktHdr.to = rand()%NUM_SERVERS;
	outMailHdr.to = 0;
	outMailHdr.from = currentThread->ID;
	outMailHdr.length = strlen(msg) + 1;
	bool success = postOffice->Send(outPktHdr, outMailHdr, msg);
	if ( !success ) {
		printf("The postOffice Send to Server failed.\n");
		interrupt->Halt();      	      	       	      	 
	} 
	postOffice->Receive(currentThread->ID, &inPktHdr, &inMailHdr, buffer); //Check my mailbox, which corresponds to my threadID
	DEBUG('a',"Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.from);
	fflush(stdout);
#else

	conditionsLock->Acquire();
	locksLock->Acquire();
	if (conditions[conditionID].space != currentThread->space) {
		printf("ConditionID[%d] cannot be waited from a non-owning process!\n", conditionID);
		locksLock->Release();
		conditionsLock->Release();
		return;
	} 

	if (locks[lockID].space != currentThread->space) {
		printf("LockID[%d] cannot be passed to Wait from a non-owning process!\n", lockID);
		locksLock->Release();
		conditionsLock->Release();
		return;
	}

	conditions[conditionID].aboutToBeWaited++;
	locksLock->Release();
	conditionsLock->Release();
	conditions[conditionID].condition->Wait(locks[lockID].lock); 

	conditionsLock->Acquire();
	conditions[conditionID].aboutToBeWaited--;

	if (conditions[conditionID].needsToBeDeleted 
		&& !conditions[conditionID].condition->HasThreadsWaiting()
		&& conditions[conditionID].aboutToBeWaited == 0) {
			deleteCondition(conditionID);
	}
	conditionsLock->Release();
#endif
}

void Broadcast_Syscall(ConditionID conditionID, LockID lockID) {
	if (conditionID < 0 || conditionID >= MAX_CONDITIONS) {
		printf("ConditionID[%d] is out of range!\n", conditionID);
		return;
	}

	if (lockID < 0 || lockID >= MAX_CONDITIONS) {
		printf("LockID[%d] is out of range!\n", lockID);
		return;
	}

	// If Networking is enabled, send message to server asking it to make a CV broadcast to a lock

#ifdef NETWORK
	char msg[MaxMailSize];
	sprintf(msg,"%d,%d,%d,*",SC_Broadcast,conditionID, lockID,currentThread->ID); //Message is in the form [<RequestType><data><ThreadID>]

	outPktHdr.to = rand()%NUM_SERVERS;
	outMailHdr.to = 0;
	outMailHdr.from = currentThread->ID;
	outMailHdr.length = strlen(msg) + 1;
	bool success = postOffice->Send(outPktHdr, outMailHdr, msg);
	if ( !success ) {
		printf("The postOffice Send to Server failed.\n");
		interrupt->Halt();      	      	       	      	 
	} 
	postOffice->Receive(currentThread->ID, &inPktHdr, &inMailHdr, buffer); //Check my mailbox, which corresponds to my threadID
	DEBUG('a',"Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.from);
	fflush(stdout);
#else

	conditionsLock->Acquire();
	locksLock->Acquire();
	if (conditions[conditionID].space != currentThread->space) {
		printf("ConditionID[%d] cannot be waited from a non-owning process!\n", conditionID);
		locksLock->Release();
		conditionsLock->Release();
		return;
	} 

	if (locks[lockID].space != currentThread->space) {
		printf("LockID[%d] cannot be passed to Wait from a non-owning process!\n", lockID);
		locksLock->Release();
		conditionsLock->Release();
		return;
	}

	conditions[conditionID].condition->Broadcast(locks[lockID].lock);

	if (conditions[conditionID].needsToBeDeleted 
		&& !conditions[conditionID].condition->HasThreadsWaiting()
		&& conditions[conditionID].aboutToBeWaited == 0) {
			deleteCondition(conditionID);
	}

	locksLock->Release();
	conditionsLock->Release();
#endif
}

//===============================================================================================
//					NETWORKING - Monitor Variable Syscalls
//===============================================================================================

#ifdef NETWORK
MonitorID CreateMonitor_Syscall(unsigned int vaddr, int len){

	//Validating the desired name

	char* buf;
	if ( !(buf = new char[len]) ) {
		printf("%s","Error allocating kernel buffer for write!\n");
		return -1;
	} else {		
		if( copyin(vaddr,len,buf) != (signed int)strlen(buf) ) {
			printf("%s","Bad string length, data not written.\n");
			delete[] buf;
			return -1;
		}		
		if ( copyin(vaddr,len,buf) == -1 ) {
			printf("%s","Bad pointer passed to to write: data not written\n");
			delete[] buf;
			return -1;
		}				
	}

	char msg[MaxMailSize];
	sprintf(msg,"%d,%s,*",SC_CreateMonitor,buf,currentThread->ID); //Message is in the form [<RequestType><data><ThreadID>]

	outPktHdr.to = rand()%NUM_SERVERS;
	outMailHdr.to = 0;
	outMailHdr.from = currentThread->ID;
	outMailHdr.length = strlen(msg) + 1;
	bool success = postOffice->Send(outPktHdr, outMailHdr, msg);
	if ( !success ) {
		printf("The postOffice Send to Server failed.\n");
		interrupt->Halt();      	      	       	      	 
	} 


	postOffice->Receive(currentThread->ID, &inPktHdr, &inMailHdr, buffer); //Check my mailbox, which corresponds to my threadID
	DEBUG('a',"Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.from);
	fflush(stdout);
	MonitorID id = atoi(buffer);
	return id;
}


int GetMonitor_Syscall(MonitorID monitorID){

	//Validating the desired monitorID
	if (monitorID < 0 || monitorID >= MAX_MONITORS) {
		printf("MonitorID[%d] is out of range!\n", monitorID);
		return -1;
	}
	char msg[MaxMailSize];
	sprintf(msg,"%d,%d,*",SC_GetMonitor,monitorID,currentThread->ID); //Message is in the form [<RequestType><data><ThreadID>]

	outPktHdr.to = rand()%NUM_SERVERS;
	outMailHdr.to = 0;
	outMailHdr.from = currentThread->ID;
	outMailHdr.length = strlen(msg) + 1;
	bool success = postOffice->Send(outPktHdr, outMailHdr, msg);
	if ( !success ) {
		printf("The postOffice Send to Server failed.\n");
		interrupt->Halt();      	      	       	      	 
	} 


	postOffice->Receive(currentThread->ID, &inPktHdr, &inMailHdr, buffer); //Check my mailbox, which corresponds to my threadID
	DEBUG('a',"Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.from);
	fflush(stdout);
	int value = atoi(buffer);
	return value;
}


void SetMonitor_Syscall(MonitorID monitorID, int value){

	//Validating the desired monitorID
	if (monitorID < 0 || monitorID >= MAX_MONITORS) {
		printf("MonitorID[%d] is out of range!\n", monitorID);
		return;
	}

	char msg[MaxMailSize];
	sprintf(msg,"%d,%d,%d,*", SC_SetMonitor, monitorID, value, currentThread->ID); //Message is in the form [<RequestType><data><ThreadID>]

	outPktHdr.to = rand()%NUM_SERVERS;
	outMailHdr.to = 0;
	outMailHdr.from = currentThread->ID;
	outMailHdr.length = strlen(msg) + 1;
	bool success = postOffice->Send(outPktHdr, outMailHdr, msg);
	if ( !success ) {
		printf("The postOffice Send to Server failed.\n");
		interrupt->Halt();      	      	       	      	 
	} 


	postOffice->Receive(currentThread->ID, &inPktHdr, &inMailHdr, buffer); //Check my mailbox, which corresponds to my threadID
	DEBUG('a',"Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.from);
	fflush(stdout);

}

MonitorArrayID CreateMonitorArray_Syscall(unsigned int vaddr, int len, int arrayLength, int initialValue) {
	//Validating the desired name

	// TODO: deal with sending initialValue as well on the front and back end
	char* buf;
	if ( !(buf = new char[len]) ) {
		printf("%s","Error allocating kernel buffer for write!\n");
		return -1;
	} else {		
		if( copyin(vaddr,len,buf) != (signed int)strlen(buf) ) {
			printf("%s","Bad string length, data not written.\n");
			delete[] buf;
			return -1;
		}		
		if ( copyin(vaddr,len,buf) == -1 ) {
			printf("%s","Bad pointer passed to to write: data not written\n");
			delete[] buf;
			return -1;
		}				
	}
	if (arrayLength > MAX_MONITOR_ARRAY_VALUES || arrayLength < 0){
		printf("Invalid array length!\n");
		delete[] buf;
		return -1;
	}

	char msg[MaxMailSize];
	sprintf(msg,"%d,%s,%d,%d,*",SC_CreateMonitorArray,buf,arrayLength, initialValue, currentThread->ID); //Message is in the form [<RequestType><data><ThreadID>]

	outPktHdr.to = rand()%NUM_SERVERS;
	outMailHdr.to = 0;
	outMailHdr.from = currentThread->ID;
	outMailHdr.length = strlen(msg) + 1;
	bool success = postOffice->Send(outPktHdr, outMailHdr, msg);
	if ( !success ) {
		printf("The postOffice Send to Server failed.\n");
		interrupt->Halt();      	      	       	      	 
	} 


	postOffice->Receive(currentThread->ID, &inPktHdr, &inMailHdr, buffer); //Check my mailbox, which corresponds to my threadID
	DEBUG('a',"Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.from);
	fflush(stdout);
	MonitorArrayID id = atoi(buffer);
	return id;

}

int GetMonitorArrayValue_Syscall(MonitorArrayID monitorArrayID, int index) {
	//Validating the desired monitorID
	if (monitorArrayID < 0 || monitorArrayID >= MAX_MONITOR_ARRAYS) {
		printf("MonitorArrayID[%d] is out of range!\n", monitorArrayID);
		return -1;
	}

	char msg[MaxMailSize];
	sprintf(msg,"%d,%d,%d,*", SC_GetMonitorArrayValue, monitorArrayID, index, currentThread->ID); //Message is in the form [<RequestType><data><ThreadID>]

	outPktHdr.to = rand()%NUM_SERVERS;
	outMailHdr.to = 0;
	outMailHdr.from = currentThread->ID;
	outMailHdr.length = strlen(msg) + 1;
	bool success = postOffice->Send(outPktHdr, outMailHdr, msg);
	if ( !success ) {
		printf("The postOffice Send to Server failed.\n");
		interrupt->Halt();      	      	       	      	 
	} 


	postOffice->Receive(currentThread->ID, &inPktHdr, &inMailHdr, buffer); //Check my mailbox, which corresponds to my threadID
	DEBUG('a',"Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.from);
	fflush(stdout);
	int value = atoi(buffer);
	return value;
}


void SetMonitorArrayValue_Syscall(MonitorArrayID monitorArrayID, int index, int value) {
	//Validating the desired monitorarrayID
	if (monitorArrayID < 0 || monitorArrayID >= MAX_MONITOR_ARRAYS) {
		printf("MonitorArrayID[%d] is out of range!\n", monitorArrayID);
		return;
	}

	char msg[MaxMailSize];
	sprintf(msg,"%d,%d,%d,%d,*", SC_SetMonitorArrayValue, monitorArrayID, index, value, currentThread->ID); //Message is in the form [<RequestType><data><ThreadID>]

	outPktHdr.to = rand()%NUM_SERVERS;
	outMailHdr.to = 0;
	outMailHdr.from = currentThread->ID;
	outMailHdr.length = strlen(msg) + 1;
	bool success = postOffice->Send(outPktHdr, outMailHdr, msg);
	if ( !success ) {
		printf("The postOffice Send to Server failed.\n");
		interrupt->Halt();      	      	       	      	 
	} 


	postOffice->Receive(currentThread->ID, &inPktHdr, &inMailHdr, buffer); //Check my mailbox, which corresponds to my threadID
	DEBUG('a',"Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.from);
	fflush(stdout);
}

void DestroyMonitorArray_Syscall(MonitorArrayID monitorArrayID) {
	if (monitorArrayID < 0 || monitorArrayID >= MAX_MONITOR_ARRAYS) {
		printf("MonitorArrayID[%d] is out of range!\n", monitorArrayID);
		return;
	}

	// If Networking is enabled, send message to server asking it to destroy
	// a MonitorArray. 

	char msg[MaxMailSize];
	sprintf(msg,"%d,%d,*",SC_DestroyMonitorArray,monitorArrayID,currentThread->ID); //Message is in the form [<RequestType><data><ThreadID>]

	outPktHdr.to = rand()%NUM_SERVERS;
	outMailHdr.to = 0;
	outMailHdr.from = currentThread->ID;
	outMailHdr.length = strlen(msg) + 1;
	bool success = postOffice->Send(outPktHdr, outMailHdr, msg);
	if ( !success ) {
		printf("The postOffice Send to Server failed.\n");
		interrupt->Halt();      	      	       	      	 
	} 
	postOffice->Receive(currentThread->ID, &inPktHdr, &inMailHdr, buffer); //Check my mailbox, which corresponds to my threadID
	DEBUG('a',"Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.from);
	fflush(stdout);

}



void HandleTimer(int timerIndex){
	printf("In HandleTimer\n");
	TimerData* timerData = &timerDatas[timerIndex];
	MonitorArrayID monitorArrayID = timerData->monitorArrayID;
    int index = timerData->index;
	int value = timerData->value;
	int numYields = timerData->numYields;
	printf("DEBUG Index[%d] -- Value[%d] -- numYields[%d]\n",index,value,numYields); 

	printf("DEBUG: Acquiring lock in HandleTimer\n");
	timerLock->Acquire();
	printf("DEBUG: Lock acquired in HandleTimer\n");
	timerData->isTaken = false;
	timerLock->Release();
	printf("DEBUG: Lock released in HandleTimer\n");
	
	printf("DEBUG: Yielding for [%d] yields.\n",numYields);
	for (int i=0; i<numYields; i++){
		printf("Yielding: [%d/%d]\n",i,numYields);
		currentThread->Yield();
	}
	printf("Calling SetMonitorArrayValue syscall!\n");
	SetMonitorArrayValue_Syscall(monitorArrayID, index, value);
}

void TimedSetMonitorArrayValue_Syscall(MonitorArrayID monitorArrayID, int index, int value, int numYields)
{
	printf("Entered the TimedSetMonitorArrayValue syscall, trying to acquire timerLock..\n");
	timerLock->Acquire();
	printf("timerLock has been acquired\n");
	int timerIndex = getAvailableTimerData();
	if (timerIndex < 0){
		printf("Error: No available Timer Datas\n");
		return;
	}
	TimerData* timerData = &timerDatas[timerIndex];
	timerData->isTaken = true;
	timerLock->Release();
	timerData->monitorArrayID = monitorArrayID;
	timerData->index = index;
	timerData->value= value;
	timerData->numYields = numYields;
	printf("Creating new timer thread\n");
	Thread* thread = new Thread("TimerThread");
	//Not sure if I should do all this
	//printf("Setting timerThread's space to currentThread's space\n");
	//thread->space = currentThread->space; //put it in the same addrspace

	//allocate space for new thread	
	thread->ID = threadCount++;
	//threadArgs[thread->ID] = arg;
	//printf("Adding this thread to addrspace's list of threads\n");
	//thread->space->AddNewThread(thread);

	//thread->space->RestoreState();

	//fork the thread, somehow
	printf("Forking the timerThread\n");
	thread->Fork(HandleTimer, timerIndex);


}


#endif
/*===========================================================================================================================
==============================================================================================================================*/




void kernel_thread(int virtualAddr)
{
	bigLock->Acquire();
	DEBUG('a', "Starting up the new kernel thread!\n");
	//mod the PC to begin execution at the new thread
	machine->WriteRegister(PCReg,virtualAddr);
	machine->WriteRegister(NextPCReg,virtualAddr+4);	

	//restoreState?
	currentThread->space->RestoreState();

	//mod the stack
	int newStackReg = (currentThread->startVPN) * PageSize + UserStackSize - 16;
	DEBUG('a', "Thread[%d] getting startVPN = %d.\n", currentThread->ID, currentThread->startVPN);
	DEBUG('a', "Thread[%d] setting stack reg to %d.\n", currentThread->ID, newStackReg);
	machine->WriteRegister(StackReg, newStackReg);
	DEBUG('a', "Wrote the stack reg.\n");
	bigLock->Release();
	machine->Run();	
}

void Fork_Syscall(unsigned int funcAddr, int arg) //func = virtualaddr of function
{	
	bigLock->Acquire();	
	if(funcAddr >= (unsigned int)currentThread->space->getCodeSize() || funcAddr < 0)
	{
		printf("Cannot Fork FunctionPointer[0x%x]. Out of bounds.\n", funcAddr);
		bigLock->Release();
		return;
	}	
	if(funcAddr == 0)
	{
		printf("Cannot Fork FunctionPointer[0x%x]. Forking would result in non-terminal execution.\n", funcAddr); 
		bigLock->Release();
		return;
	}
	if(funcAddr%4 != 0)
	{
		printf("Cannot Fork FunctionPointer[0x%x]. Address not aligned.\n", funcAddr);
		bigLock->Release();
		return;
	}

	//create the new thread
	Thread* thread = new Thread("kernelthread");	
	thread->space = currentThread->space; //put it in the same addrspace

	//allocate space for new thread	
	thread->ID = threadCount++;
	threadArgs[thread->ID] = arg;
	thread->space->AddNewThread(thread);

	thread->space->RestoreState();

	//fork the thread, somehow
	thread->Fork(kernel_thread,funcAddr);
	DEBUG('a', "Forked the thread.\n");

	bigLock->Release();
}

int Rand_Syscall()
{
	return rand();	
}

int GetForkArg_Syscall() {
	return threadArgs[currentThread->ID];
}

int GetThreadID_Syscall() {
	return currentThread->ID;
}

#ifdef USE_TLB
unsigned int fullMemPPN = 0;
BitMap* swapFileBitMap = new BitMap(16000);
int tlbIndex = -1;

void RemovePageFromTLB(int ppn) {
	int spaceID = getSpaceID(currentThread->space);

	IntStatus oldLevel = interrupt->SetLevel(IntOff);
	for (unsigned int i = 0; i < TLBSize; i++) {
		// if the page we are evicting has the same VPN and spaceID as a page in the TLB, invalidate the TLB page
		// spaceID for the TLB is the current spaceID, because all TLB entries correspond to only the current thread
		if (machine->tlb[i].virtualPage == ipt[ppn].virtualPage && spaceID == ipt[ppn].spaceID
			&& machine->tlb[i].valid) {
				machine->tlb[i].valid = false;
				if (machine->tlb[i].dirty) {
					ipt[ppn].dirty = true;
				}
		}
	}
	interrupt->SetLevel(oldLevel);
}

void UpdateIPT(int vpn, int ppn){
	ipt[ppn].virtualPage = vpn;
	ipt[ppn].spaceID = getSpaceID(currentThread->space);
	ipt[ppn].physicalPage = ppn; // becase it was possibly set to -1
	ipt[ppn].valid = true;
	ipt[ppn].dirty = false;

	ipt[ppn].use = false;
	ipt[ppn].readOnly = currentThread->space->pageTable[vpn].readOnly;
	ipt[ppn].pageType = currentThread->space->pageTable[vpn].pageType;
	ipt[ppn].pageLocation = currentThread->space->pageTable[vpn].pageLocation;
	ipt[ppn].byteOffset = currentThread->space->pageTable[vpn].byteOffset;
	ipt[ppn].byteSize = currentThread->space->pageTable[vpn].byteSize;
	// inUse is already true
}

void UpdateTLB(int vpn, int ppn) {
	IntStatus oldLevel = interrupt->SetLevel(IntOff);
	tlbIndex = (tlbIndex + 1) % TLBSize;

	//propage the dirty bit to IPT if the TLB Entry we were replacing was dirty
	if (machine->tlb[tlbIndex].valid && machine->tlb[tlbIndex].dirty) {
		int oldTLBPPN = machine->tlb[tlbIndex].physicalPage;
		ipt[oldTLBPPN].dirty = true;
	}

	//copy from the IPT to the TLB
	machine->tlb[tlbIndex].virtualPage = vpn;
	machine->tlb[tlbIndex].physicalPage = ppn;
	machine->tlb[tlbIndex].dirty = false;
	machine->tlb[tlbIndex].readOnly = currentThread->space->pageTable[vpn].readOnly;
	machine->tlb[tlbIndex].use = false;
	machine->tlb[tlbIndex].valid = true;

	for (int i = 0; i < TLBSize; i++) {
		DEBUG('z', "e spaceID:%d tlb[%d] vpn:%d ppn:%d dirty:%d valid:%d.\n", getSpaceID(currentThread->space), i, machine->tlb[i].virtualPage, machine->tlb[i].physicalPage, machine->tlb[i].dirty, machine->tlb[i].valid);
	}

	interrupt->SetLevel(oldLevel);
}

int HandleFullMemory(int vpn) {
	int ppn = -1;
	numMemoryFull++;

	// lock the ipt to find an acceptable ipt entry, then set its inUse and release the iptLock
	iptLock->Acquire();
	do {
		if (PRAND) {
			ppn = rand() % NumPhysPages;
		} else if (PFIFO) {
			ppn = (++fullMemPPN) % NumPhysPages;
		}
	} while (!ipt[ppn].valid || ipt[ppn].inUse);

	ipt[ppn].inUse = true;
	AddrSpace* owningSpace = processTable[ipt[ppn].spaceID];

	// this shouldn't happen
	if (owningSpace == NULL) {
		for (int i = 0; i < NumPhysPages; i++) {
			printf("OWNING SPACE NULL IN HANDLE FULL MEMORY ipt[%d] vpn:%d dirty:%d inUse:%d valid:%d spaceID:%d\n", ipt[i].physicalPage, ipt[i].virtualPage, ipt[i].dirty, ipt[i].inUse, ipt[i].valid, ipt[i].spaceID);
		}
		return ppn;
	}

	iptLock->Release();

	RemovePageFromTLB(ppn); // make sure we invalidate any TLB entry that matches the physical page we are evicting

	DEBUG('z', "About to get the owning space lock to evict a page.\n");
	owningSpace->pageTableLock->Acquire();
	DEBUG('z', "Got the owning space lock from %d to evict a page.\n", getSpaceID(owningSpace));

	DEBUG('b', "In HandleFullMemory() with vpn = %d and selected ppn to evict: %d.\n", vpn, ppn);

	if (ipt[ppn].dirty) { // if it's dirty, write it to the swap file

		numSwapFileWrites++;
		ASSERT(!ipt[ppn].readOnly);
		// write back to swapfile
		DEBUG('b', "Flushing a dirty page = %d to the swapfile.\n", ppn);

		// if we haven't written it to the swap file before, find a place for it and record this in the pageTable
		if (ipt[ppn].pageLocation != PageLocationSwapFile) { //we don't already have a space for this page in the swapfile, get it one
			int swapFileIndex = swapFileBitMap->Find();
			if (swapFileIndex == -1) {
				printf("We ran out of space in the swap file, need to increase the bitmap.\n");
				exit(1);
			}
			owningSpace->pageTable[ipt[ppn].virtualPage].pageLocation = PageLocationSwapFile;
			owningSpace->pageTable[ipt[ppn].virtualPage].byteOffset = swapFileIndex * PageSize;
			owningSpace->pageTable[ipt[ppn].virtualPage].byteSize = PageSize;
		}

		// actually write to the physical swapFile
		swapFile->WriteAt(&(machine->mainMemory[ppn * PageSize]), PageSize, owningSpace->pageTable[ipt[ppn].virtualPage].byteOffset);
		DEBUG('b', "Wrote the dirty vpn = %d, ppn = %d to the swapfile at swapFileIndex: %d. numThreads = %d\n", ipt[ppn].virtualPage, ppn, owningSpace->pageTable[ipt[ppn].virtualPage].byteOffset / PageSize, currentThread->space->numThreads);

		//update the page table to reflect that we kicked out the page
		owningSpace->pageTable[ipt[ppn].virtualPage].physicalPage = -1;
		owningSpace->pageTable[ipt[ppn].virtualPage].valid = true; // setting this to invalid would mean the VPN was invalid (given up by thread), but we don't want that
		owningSpace->pageTable[ipt[ppn].virtualPage].dirty = false;
		owningSpace->pageTable[ipt[ppn].virtualPage].use = false;
		//	owningSpace->pageTable[ipt[ppn].virtualPage].inUse = false; // when we copy from pageTable to IPT later, this should be true
		DEBUG('p', "Copied the ipt entry back to the owningSpace page table. numThreads = %d\n", currentThread->space->numThreads);
	}

	owningSpace->pageTable[ipt[ppn].virtualPage].physicalPage = -1; // set physicalPage to -1 regardless of whether the thing we evicted was dirty or not, because it's not at a physical page

	owningSpace->pageTableLock->Release();

	return ppn;
}

void HandleIPTMiss(int vpn) {
	DEBUG('b', "In HandleIPTMiss() for vpn = %d.\n", vpn);
	for (int i = 0; i < currentThread->space->numPages; i++) {
		DEBUG('b', "pt[%d] ppn:%d dirty:%d readOnly:%d inUse:%d valid:%d spaceID:%d pageLocation:%d swapFileIndex:%d\n", currentThread->space->pageTable[i].virtualPage, currentThread->space->pageTable[i].physicalPage, currentThread->space->pageTable[i].dirty, currentThread->space->pageTable[i].readOnly, currentThread->space->pageTable[i].inUse, currentThread->space->pageTable[i].valid, currentThread->space->pageTable[i].spaceID, currentThread->space->pageTable[i].pageLocation, currentThread->space->pageTable[i].byteOffset / PageSize);
	}
	for (int i = 0; i < NumPhysPages; i++) {
		DEBUG('b', "ipt[%d] vpn:%d dirty:%d readOnly:%d inUse:%d valid:%d spaceID:%d\n", ipt[i].physicalPage, ipt[i].virtualPage, ipt[i].dirty, ipt[i].readOnly, ipt[i].inUse, ipt[i].valid, ipt[i].spaceID);
	}

	numIPTMisses++;
	iptLock->Acquire();
	int ppn = getPhysicalPage(); // sets inUse to true on ipt[ppn]
	iptLock->Release();
	if (ppn == -1) {
		// do some crazy swapfile black magic		
		ppn = HandleFullMemory(vpn);
	}

	currentThread->space->pageTableLock->Acquire();

	// ppn's inUse is set
	DEBUG('p', "About to read, numThreads = %d.\n", currentThread->space->numThreads);

	// here we are reading the page from its location (Executable, SwapFile, or nowhere) into main memory
	if (currentThread->space->pageTable[vpn].pageLocation == PageLocationExecutable) {
		DEBUG('d', "Reading from the executable, with byteSize = %d.\n", currentThread->space->pageTable[vpn].byteSize);
		currentThread->space->executable->ReadAt(&(machine->mainMemory[ppn * PageSize]), currentThread->space->pageTable[vpn].byteSize, currentThread->space->pageTable[vpn].byteOffset);
		ASSERT(currentThread->space->pageTable[vpn].byteSize <= PageSize);
		if (currentThread->space->pageTable[vpn].byteSize != PageSize) {
			DEBUG('d', "Zeroing out the remainder not read from code.\n");
			// the page had some uninitialize data on it that we need to zero out.
			bzero(&(machine->mainMemory[ppn * PageSize + currentThread->space->pageTable[vpn].byteSize]), PageSize - currentThread->space->pageTable[vpn].byteSize);
		}
	} else if (currentThread->space->pageTable[vpn].pageLocation == PageLocationNotOnDisk ) {
		bzero(&(machine->mainMemory[ppn * PageSize]), PageSize); // zero the whole page
	} else if (currentThread->space->pageTable[vpn].pageLocation == PageLocationSwapFile) { // it's on the swap file, we have work to do
		DEBUG('d', "Reading from the swapfile.\n");
		ASSERT(currentThread->space->pageTable[vpn].byteOffset % PageSize == 0); // the byte offset should be the very beginning of a page-sized amount of bytes, since we always read/write whole pages to the swapfiel
		swapFile->ReadAt(&(machine->mainMemory[ppn * PageSize]), PageSize, currentThread->space->pageTable[vpn].byteOffset);
		DEBUG('d', "Read in vpn: %d from the swapFile at swapFileIndex: %d.\n", vpn, currentThread->space->pageTable[vpn].byteOffset / PageSize);
	} else {
		ASSERT(false);
	}

	currentThread->space->pageTable[vpn].physicalPage = ppn;


	DEBUG('p', "Read vpn = %d into memory. numThreads = %d\n", vpn, currentThread->space->numThreads);

	//	iptLock->Acquire();
	UpdateIPT(vpn, ppn);
	UpdateTLB(vpn, ppn);
	ipt[ppn].inUse = false;
	currentThread->space->pageTable[vpn].inUse = false;
	//	iptLock->Release();

	currentThread->space->pageTableLock->Release();
}

void HandlePageFault() {
	int badVAddr = machine->ReadRegister(BadVAddrReg);
	DEBUG('p', "In HandlePageFault() for badVAddr = %d.\n", badVAddr);
	int badVPN = badVAddr / PageSize;

	for (int i = 0; i < TLBSize; i++) {
		DEBUG('z', "b spaceID:%d tlb[%d] vpn:%d ppn:%d dirty:%d valid:%d.\n", getSpaceID(currentThread->space), i, machine->tlb[i].virtualPage, machine->tlb[i].physicalPage, machine->tlb[i].dirty, machine->tlb[i].valid);
	}

	AddrSpace* space = currentThread->space;

	if (badVPN >= space->numPages) {
		printf("VPN does not exist for the given AddrSpace!\n");
		return;
	}

	space->pageTableLock->Acquire();
	DEBUG('z', "Grabbed page table lock.\n");

	if (space->pageTable[badVPN].inUse) { // another thread is already page faulting to bring this into the ipt/tlb, so just get out and wait a sec
		DEBUG('b', "VPN %d inUse set, returning from PageFault in tid:%d without doing anything.\n", badVPN, currentThread->ID);
		space->pageTableLock->Release();
		currentThread->Yield(); // not really necessary, but might help a bit, just get out of the way so the other thread can finish
		return;
	}

	space->pageTable[badVPN].inUse = true; // lock this virtual page
	space->pageTableLock->Release();

	int ppn = -1; //check IPT for physical page
	iptLock->Acquire();
	DEBUG('z', "Grabbed iptLock.\n");
	for (unsigned int i = 0; i < NumPhysPages; i++) {
		int spaceID = getSpaceID(currentThread->space);
		if (!ipt[i].inUse && ipt[i].valid && ipt[i].virtualPage == badVPN && ipt[i].spaceID == spaceID) { //if valid, matching process / vpn
			UpdateTLB(badVPN, i); // found the page in the IPT, just copy it to the TLB and we're done
			space->pageTable[badVPN].inUse = false;
			iptLock->Release();
			return;
		}
	}
	iptLock->Release();

	// couldn't find an empty ppn
	HandleIPTMiss(badVPN);

	//	DEBUG('p', "Finished HandlePageFault() for badVAddr = %d, it now uses ppn = %d.  numThreads = %d\n", badVAddr, ppn, currentThread->space->numThreads);
}

#endif


Lock* pageFaultTESTLock = new Lock("pageFaultTESTLock");

void ExceptionHandler(ExceptionType which) {
	int type = machine->ReadRegister(2); // Which syscall?
	int rv=0; 	// the return value from a syscall

	if ( which == SyscallException ) {
		switch (type) {
		default:
			DEBUG('a', "Unknown syscall - shutting down.\n");
		case SC_Halt:
			DEBUG('a', "Shutdown, initiated by user program.\n");
			interrupt->Halt();
			break;
		case SC_Create:
			DEBUG('a', "Create syscall.\n");
			Create_Syscall(machine->ReadRegister(4), machine->ReadRegister(5));
			break;
		case SC_Open:
			DEBUG('a', "Open syscall.\n");
			rv = Open_Syscall(machine->ReadRegister(4), machine->ReadRegister(5));
			break;
		case SC_Write:
			DEBUG('a', "Write syscall.\n");
			Write_Syscall(machine->ReadRegister(4),
				machine->ReadRegister(5),
				machine->ReadRegister(6));
			break;
		case SC_Read:
			DEBUG('a', "Read syscall.\n");
			rv = Read_Syscall(machine->ReadRegister(4),
				machine->ReadRegister(5),
				machine->ReadRegister(6));
			break;
		case SC_Close:
			DEBUG('a', "Close syscall.\n");
			Close_Syscall(machine->ReadRegister(4));
			break;
			////// START OUR ADDITIONS
		case SC_Yield:
			DEBUG('a', "Yield syscall.\n");
			currentThread->Yield();
			break;

		case SC_Exit:
			DEBUG('a', "Exit syscall.\n");
			Exit_Syscall(machine->ReadRegister(4));
			break;

		case SC_CreateLock:
			DEBUG('a', "CreateLock syscall.\n");
			rv = CreateLock_Syscall(machine->ReadRegister(4),
				machine->ReadRegister(5));
			break;

		case SC_DestroyLock:
			DEBUG('a', "DestroyLock syscall.\n");
			DestroyLock_Syscall(machine->ReadRegister(4));
			break;

		case SC_CreateCondition:
			DEBUG('a', "CreateCondition syscall.\n");
			rv = CreateCondition_Syscall(machine->ReadRegister(4),
				machine->ReadRegister(5));
			break;

		case SC_DestroyCondition:
			DEBUG('a', "DestroyCondition syscall.\n");
			DestroyCondition_Syscall(machine->ReadRegister(4));
			break;

		case SC_Acquire:
			DEBUG('a', "Acquire syscall.\n");
			Acquire_Syscall(machine->ReadRegister(4));
			break;

		case SC_Release:
			DEBUG('a', "Release syscall.\n");
			Release_Syscall(machine->ReadRegister(4));
			break;

		case SC_Signal:
			DEBUG('a', "Signal syscall.\n");
			Signal_Syscall(machine->ReadRegister(4),
				machine->ReadRegister(5));
			break;

		case SC_Wait:
			DEBUG('a', "Wait syscall.\n");
			Wait_Syscall(machine->ReadRegister(4),
				machine->ReadRegister(5));
			break;

		case SC_Broadcast:
			DEBUG('a', "Broadcast syscall.\n");
			Broadcast_Syscall(machine->ReadRegister(4),
				machine->ReadRegister(5));							
			break;
#ifdef NETWORK
		case SC_CreateMonitor:
			DEBUG('a', "CreateMonitor syscall.\n");
			rv = CreateMonitor_Syscall(machine->ReadRegister(4),
				machine->ReadRegister(5));							
			break;		

		case SC_GetMonitor:
			DEBUG('a', "GetMonitor syscall.\n");
			rv = GetMonitor_Syscall(machine->ReadRegister(4));							
			break;
			
		case SC_SetMonitor:
			DEBUG('a', "SetMonitor syscall.\n");
			SetMonitor_Syscall(machine->ReadRegister(4),
				machine->ReadRegister(5));							
			break;

		case SC_CreateMonitorArray:
			DEBUG('a', "CreateMonitorArray syscall.\n");
			rv = CreateMonitorArray_Syscall(machine->ReadRegister(4),
				machine->ReadRegister(5), machine->ReadRegister(6),
				machine->ReadRegister(7));
			break;

		case SC_GetMonitorArrayValue:
			DEBUG('a', "GetMonitorArrayValue syscall.\n");
			rv = GetMonitorArrayValue_Syscall(machine->ReadRegister(4),
				machine->ReadRegister(5));
			break;

		case SC_SetMonitorArrayValue:
			DEBUG('a', "SetMonitorArrayValue syscall.\n");
			SetMonitorArrayValue_Syscall(machine->ReadRegister(4),
				machine->ReadRegister(5), machine->ReadRegister(6));
			break;

		case SC_DestroyMonitorArray:
			DEBUG('a', "DestroyMonitorArray syscall.\n");
			DestroyMonitorArray_Syscall(machine->ReadRegister(4));
			break;

		case SC_TimedSetMonitorArrayValue:
			DEBUG('a',"TimedSetMonitorArrayValue syscall\n");
			TimedSetMonitorArrayValue_Syscall(machine->ReadRegister(4),
			machine->ReadRegister(5),machine->ReadRegister(6),
			machine->ReadRegister(7));
			break;
		
#endif
		case SC_Fork:
			DEBUG('a', "Fork syscall.\n");
			Fork_Syscall(machine->ReadRegister(4), -2);		
			break;

		case SC_Exec:
			DEBUG('a', "Exec syscall.\n");
			rv = Exec_Syscall(machine->ReadRegister(4),
				machine->ReadRegister(5));
			break;

		case SC_Rand:
			DEBUG('a', "Rand syscall.\n");
			rv = Rand_Syscall();
			break;

		case SC_ForkWithArg:
			DEBUG('a', "ForkWithArg syscall.\n");
			Fork_Syscall(machine->ReadRegister(4), machine->ReadRegister(5));
			break;

		case SC_GetForkArg:
			DEBUG('a', "GetForkArg syscall.\n");
			rv = GetForkArg_Syscall();
			break;

		case SC_GetThreadID:
			DEBUG('a', "GetThreadID syscall.\n");
			rv = GetThreadID_Syscall();
			break;

		case SC_USleep:
			DEBUG('a', "USleep syscall.\n");
			usleep(machine->ReadRegister(4));
			break;
	}

	

		// Put in the return value and increment the PC
		machine->WriteRegister(2,rv);
		machine->WriteRegister(PrevPCReg,machine->ReadRegister(PCReg));
		machine->WriteRegister(PCReg,machine->ReadRegister(NextPCReg));
		machine->WriteRegister(NextPCReg,machine->ReadRegister(PCReg)+4);
		return;
	} else if ( which == PageFaultException ) {
		stats->numPageFaults++;
#ifdef USE_TLB
		//	pageFaultTESTLock->Acquire(); // HACK
		//IntStatus oldLevel = interrupt->SetLevel(IntOff); // HACK

		HandlePageFault();
		//interrupt->SetLevel(oldLevel); // HACK
		//	pageFaultTESTLock->Release(); // HACK
#endif
	} else {
#ifdef USE_TLB
		for (int i = 0; i < currentThread->space->numPages; i++) {
			printf("ueme pt[%d] ppn:%d dirty:%d readOnly:%d inUse:%d valid:%d spaceID:%d pageLocation:%d swapFileIndex:%d\n", currentThread->space->pageTable[i].virtualPage, currentThread->space->pageTable[i].physicalPage, currentThread->space->pageTable[i].dirty, currentThread->space->pageTable[i].readOnly, currentThread->space->pageTable[i].inUse, currentThread->space->pageTable[i].valid, currentThread->space->pageTable[i].spaceID, currentThread->space->pageTable[i].pageLocation, currentThread->space->pageTable[i].byteOffset / PageSize);
		}
		for (int i = 0; i < NumPhysPages; i++) {
			printf("ueme ipt[%d] vpn:%d dirty:%d readOnly:%d inUse:%d valid:%d spaceID:%d\n", ipt[i].physicalPage, ipt[i].virtualPage, ipt[i].dirty, ipt[i].readOnly, ipt[i].inUse, ipt[i].valid, ipt[i].spaceID);
		}
#endif
		cout<<"Unexpected user mode exception - which:"<<which<<"  type:"<< type<<endl;
		interrupt->Halt();
	}
}
