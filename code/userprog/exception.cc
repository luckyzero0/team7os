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

#define MAX_LOCKS 100
#define MAX_CONDITIONS 200

using namespace std;

struct LockEntry {
	Lock* lock;
	AddrSpace* space;
	bool needsToBeDeleted;
	int aboutToBeAcquired;
};

LockEntry locks[MAX_LOCKS];
Lock* locksLock = new Lock("locksLock"); //used to lock the locks array

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

	delete paddr;
	return len;
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

void Exit_Syscall() {
	// if this is NOT the last thread in the last process, Finish() currentThread
	if (scheduler->HasThreadsRemaining()) {
		DEBUG('a', "Threads remaining in the scheduler, so we only finish the current thread.\n");
		currentThread->Finish();
	} else { // else Halt() the machine
		DEBUG('a', "No more threads remaining, so we're going to halt the machine.\n");
		interrupt->Halt();
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

	//at this point buf is the valid name
	locksLock->Acquire();
	int index = getAvailableLockID();
	if (index == -1) {
		printf("No locks available!\n");
	} else {
		locks[index].lock = new Lock(buf);
		locks[index].space = currentThread->space;
	}
	locksLock->Release();
	printf("Returning lock index: %d\n", index); //DEBUG
	return index;
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

	locksLock->Acquire();
	if (locks[id].space != currentThread->space) {
		printf("LockID[%d] cannot be destroyed from a non-owning process!\n", id);
	} else {
		if (locks[id].lock->IsBusy() || locks[id].aboutToBeAcquired > 0) {
			locks[id].needsToBeDeleted = TRUE;
			printf("Lock[%d] will be deleted when possible.\n",id); //DEBUG
		} else {
			deleteLock(id);
			printf("Lock[%d] has been deleted.\n",id);//DEBUG
		}
	}
	locksLock->Release();
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
		if ( copyin(vaddr,len,buf) == -1 ) {
			printf("%s","Bad pointer passed to to write: data not written\n");
			delete[] buf;
			return -1;
		}
	}

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
}

void Acquire_Syscall(LockID id) {
	if (id < 0 || id >= MAX_LOCKS) {
		printf("LockID[%d] is out of range!\n", id);
		return;
	}

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
	printf("Lock [%d] has been acquired.\n", id); //DEBUG
}

void Release_Syscall(LockID id) {
	if (id < 0 || id >= MAX_LOCKS) {
		printf("LockID[%d] is out of range!\n", id);
		return;
	}

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

	printf("Releasing lock[%d].\n",id); //DEBUG
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
	conditions[conditionID].condition->Wait(locks[lockID].lock); //this might not quite work

	conditionsLock->Acquire();
	conditions[conditionID].aboutToBeWaited--;

	if (conditions[conditionID].needsToBeDeleted 
		&& !conditions[conditionID].condition->HasThreadsWaiting()
		&& conditions[conditionID].aboutToBeWaited == 0) {
			deleteCondition(conditionID);
	}
	conditionsLock->Release();
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
}

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
			////// START OUT ADDITIONS
		case SC_Yield:
			DEBUG('a', "Yield syscall.\n");
			currentThread->Yield();
			break;

		case SC_Exit:
			DEBUG('a', "Exit syscall.\n");
			Exit_Syscall();
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
		}

		// Put in the return value and increment the PC
		machine->WriteRegister(2,rv);
		machine->WriteRegister(PrevPCReg,machine->ReadRegister(PCReg));
		machine->WriteRegister(PCReg,machine->ReadRegister(NextPCReg));
		machine->WriteRegister(NextPCReg,machine->ReadRegister(PCReg)+4);
		return;
	} else {
		cout<<"Unexpected user mode exception - which:"<<which<<"  type:"<< type<<endl;
		interrupt->Halt();
	}
}
