// synch.cc 
//	Routines for synchronizing threads.  Three kinds of
//	synchronization routines are defined here: semaphores, locks 
//   	and condition variables (the implementation of the last two
//	are left to the reader).
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation.  We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts.  While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synch.h"
#include "system.h"

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	Initialize a semaphore, so that it can be used for synchronization.
//
//	"debugName" is an arbitrary name, useful for debugging.
//	"initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

Semaphore::Semaphore(char* debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new List;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	De-allocate semaphore, when no longer needed.  Assume no one
//	is still waiting on the semaphore!
//----------------------------------------------------------------------

Semaphore::~Semaphore()
{
    delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
// 	Wait until semaphore value > 0, then decrement.  Checking the
//	value and decrementing must be done atomically, so we
//	need to disable interrupts before checking the value.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
//----------------------------------------------------------------------

void
Semaphore::P()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
    
    while (value == 0) { 			// semaphore not available
	queue->Append((void *)currentThread);	// so go to sleep
	currentThread->Sleep();
    } 
    value--; 					// semaphore available, 
						// consume its value
    
    (void) interrupt->SetLevel(oldLevel);	// re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::V
// 	Increment semaphore value, waking up a waiter if necessary.
//	As with P(), this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that threads
//	are disabled when it is called.
//----------------------------------------------------------------------

void
Semaphore::V()
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    thread = (Thread *)queue->Remove();
    if (thread != NULL)	   // make thread ready, consuming the V immediately
	scheduler->ReadyToRun(thread);
    value++;
    (void) interrupt->SetLevel(oldLevel);
}

// Dummy functions -- so we can compile our later assignments 
// Note -- without a correct implementation of Condition::Wait(), 
// the test case in the network assignment won't work!

Lock::Lock(char* debugName) {
	name = debugName;
	owner = NULL;
	state = FREE;
	waitQueue = new List;
}

Lock::~Lock() {
	delete waitQueue;
}

void Lock::Acquire() {
	IntStatus oldLevel = interrupt->SetLevel(IntOff);
	if (this->owner == currentThread) {
		interrupt->SetLevel(oldLevel);
		return;
	}
	else if (this->state == FREE) {
		this->state = BUSY;
		this->owner = currentThread;
	}
	else {
		this->waitQueue->Append(currentThread);
		currentThread->Sleep();
	}
	interrupt->SetLevel(oldLevel);
}




void Lock::Release() {
	IntStatus oldLevel = interrupt->SetLevel(IntOff);
	if (this->owner != currentThread) {
	  if (this->owner == NULL) {
	    printf("Lock %s: has a NULL owner!\n", this->getName());
	  }
	  printf("Cannot release lock from a non-owning thread! id = %d\n", currentThread->ID);
		interrupt->SetLevel(oldLevel);
		return;
	}
	else if (!this->waitQueue->IsEmpty() ){
		Thread* nextThread = (Thread *) this->waitQueue->Remove();
		nextThread->setStatus(READY);
		scheduler->ReadyToRun(nextThread);
		this->owner = nextThread;
	}
	else {
		this->state = FREE;
		this->owner = NULL;
	}
	
	interrupt->SetLevel(oldLevel);
}

bool Lock::IsHeldByCurrentThread() {
	IntStatus oldLevel = interrupt->SetLevel(IntOff);
	bool result = (this->owner == currentThread);
	interrupt->SetLevel(oldLevel);
	return result;
}

bool Lock::IsBusy() {
	IntStatus oldLevel = interrupt->SetLevel(IntOff);
	bool result = (this->state == BUSY);
	interrupt->SetLevel(oldLevel);
	return result;
}

//========================================================================
//					NETWORKING - IMPLEMENTING SERVERLOCK FUNCTIONS
//========================================================================
#ifdef NETWORK
ServerLock::ServerLock(char* debugName) {
	name = debugName;
	client = -1; //default to -1 = invalid
	thread = -1; //default to -1 = invalid
	state = FREE;
	waitQueue = new List;
}

ServerLock::~ServerLock() {
	delete waitQueue;
}

bool ServerLock::Acquire(int clientID, int threadID, int requestServer) { //Bool indicates whether lock has been acquired instantly or not
	PacketHeader lockOutPktHdr; 
	MailHeader lockOutMailHdr;
	IntStatus oldLevel = interrupt->SetLevel(IntOff);
	if (this->client == clientID && this->thread == threadID) {	//check IDs instead of actual threads	
		
		lockOutPktHdr.to = clientID;
    	lockOutMailHdr.to =threadID;
    	lockOutMailHdr.length = strlen(svrMsg) + 1;    	
		lockOutMailHdr.from = 0;
		sprintf(svrMsg,"Lock already owned.\n");
    	DEBUG('a',"Client[%d]->Thread[%d]. already owns this lock.\n",lockOutPktHdr.to,lockOutMailHdr.to);    		    	
    	postOffice->Send(lockOutPktHdr, lockOutMailHdr, svrMsg);    	
		
		interrupt->SetLevel(oldLevel);
		return true;
	}
	else if (this->state == FREE) {		
		this->state = BUSY;
		this->client = clientID;
		this->thread = threadID;
	}
	else {	
		printf("Lock is busy. [%d][%d]Waiting...\n",clientID,threadID);
		ClientThreadPair* ctp = new ClientThreadPair(clientID,threadID,requestServer);
		this->waitQueue->Append(ctp); //instead of appending currentThread, append the CTP of currentThread
		//currentThread->Sleep();   Server thread should NOT go to sleep, just dont message client
		return false;
	}
	interrupt->SetLevel(oldLevel);
	return true;
}

void ServerLock::Release(int clientID, int threadID) {
	PacketHeader lockOutPktHdr; 
	MailHeader lockOutMailHdr;
	IntStatus oldLevel = interrupt->SetLevel(IntOff);
	if (this->client != clientID || this->thread != threadID) { //check IDs instead of actual threads	
	  if (this->client == -1) {
	    printf("ServerLock %s: has a NULL owner!\n", this->getName());
	  }
	  printf("Cannot release lock from a non-owning thread! Client[%d]\n", clientID);
		interrupt->SetLevel(oldLevel);
		return;
	}
	else if (!this->waitQueue->IsEmpty() ){	
		DEBUG('a',"Transferring ownership of lock\n.");
		ClientThreadPair* ctp = (ClientThreadPair*) waitQueue->Remove(); 
		
		lockOutPktHdr.to = ctp->clientID;	//get clientID from CTP, and set that as the client to message
    	lockOutMailHdr.to = ctp->threadID;   //get threadID from CTP, and set that as the destination mailbox    		
    	lockOutMailHdr.length = strlen(svrMsg) + 1;    	
		lockOutMailHdr.from = 0;
		sprintf(svrMsg,"Xfrng to Cl[%d]->TID[%d].\n",lockOutPktHdr.to,lockOutMailHdr.to);    
    	DEBUG('a',"ServerLock was released. Transferring ownership to Client[%d]->Thread[%d].\n",lockOutPktHdr.to,lockOutMailHdr.to);    		
    	this->client = lockOutPktHdr.to; //transfering ownership to waiting thread
    	this->thread = lockOutMailHdr.to; //transfering ownership to waiting thread
		if (ctp->requestServerID == postOffice->getNetAddr()) {
    		postOffice->Send(lockOutPktHdr, lockOutMailHdr, svrMsg); 
		}
	}
	else {	
		DEBUG('a',"The lock is free now.\n");
		this->state = FREE;
		this->client = -1;		
	}	
	interrupt->SetLevel(oldLevel);	
}

bool ServerLock::IsHeldByCurrentThread(int clientID, int threadID) {
	IntStatus oldLevel = interrupt->SetLevel(IntOff);
	bool result = (this->client == clientID && this->thread == threadID); //Check IDs instead of threads
	interrupt->SetLevel(oldLevel);
	return result;
}

bool ServerLock::IsBusy() {
	IntStatus oldLevel = interrupt->SetLevel(IntOff);
	bool result = (this->state == BUSY);
	interrupt->SetLevel(oldLevel);
	return result;
}
#endif
//////////-----------------------------------------------------------*/

Condition::Condition(char* debugName) {
	name = debugName;
	waitingLock = NULL;
	waitQueue = new List;
}

Condition::~Condition() {
	delete waitQueue;
}

void Condition::Wait(Lock* conditionLock) { 
	IntStatus oldLevel = interrupt->SetLevel(IntOff);
	if (conditionLock == NULL) {
		printf("Wait called on a condition with a NULL conditionLock!\n");
		interrupt->SetLevel(oldLevel);
		return;
	}
	if (this->waitingLock == NULL) {
		this->waitingLock = conditionLock;
	}
	if (this->waitingLock != conditionLock) {
		printf("Wait passed a lock != to its lock!\n");
		interrupt->SetLevel(oldLevel);
		return;
	}
	// okay to wait on CV
	// Add thread to CV wait queue
	currentThread->setStatus(BLOCKED);
	this->waitQueue->Append(currentThread);
	conditionLock->Release();
	currentThread->Sleep();
	conditionLock->Acquire();
	interrupt->SetLevel(oldLevel);
}

void Condition::Signal(Lock* conditionLock) {
	IntStatus oldLevel = interrupt->SetLevel(IntOff);
	
	if (this->waitQueue->IsEmpty() ) {
		interrupt->SetLevel(oldLevel);
		return;
	}
	
	if (this->waitingLock != conditionLock) {
		printf("Signal passed a lock != to its lock!\n");
		interrupt->SetLevel(oldLevel);
		return;
	}
	
	Thread* signalledThread = (Thread *) this->waitQueue->Remove();
	signalledThread->setStatus(READY);
	scheduler->ReadyToRun(signalledThread);
	
	if (this->waitQueue->IsEmpty() ) {
		waitingLock = NULL;
	}
	
	interrupt->SetLevel(oldLevel);
}

void Condition::Broadcast(Lock* conditionLock) {
  while (!this->waitQueue->IsEmpty() ) {
    this->Signal(conditionLock);
  }
}

bool Condition::HasThreadsWaiting() {
	IntStatus oldLevel = interrupt->SetLevel(IntOff);
	bool result = !this->waitQueue->IsEmpty();
	interrupt->SetLevel(oldLevel);
	return result;
}

//======================================================================
//				NETWORK - ServerCondition function implementations
//======================================================================
#ifdef NETWORK

ServerCondition::ServerCondition(char* debugName) {
	name = debugName;
	waitingLock = NULL;
	waitQueue = new List;
}

ServerCondition::~ServerCondition() {
	delete waitQueue;
}

void ServerCondition::Wait(ServerLock* conditionServerLock) { 
	IntStatus oldLevel = interrupt->SetLevel(IntOff);
	if (conditionServerLock == NULL) {
		printf("Wait called on a condition with a NULL conditionServerLock!\n");
		interrupt->SetLevel(oldLevel);
		return;
	}
	if (this->waitingLock == NULL) {
		this->waitingLock = conditionServerLock;
	}
	if (this->waitingLock != conditionServerLock) {
		printf("Wait passed a lock != to its lock!\n");
		interrupt->SetLevel(oldLevel);
		return;
	}
	// okay to wait on CV
	// Add client ID to CV wait queue
	//currentThread->setStatus(BLOCKED);
	ClientThreadPair* ctp = new ClientThreadPair(conditionServerLock->client,conditionServerLock->thread);
	this->waitQueue->Append(ctp); //Add client and thread info to wait queue
	DEBUG('a',"Appended Client[%d]Thread[%d] to CV WaitQ.\n",conditionServerLock->client,conditionServerLock->thread);
	conditionServerLock->Release(conditionServerLock->client, conditionServerLock->thread); //Release the lock
	//currentThread->Sleep();  //Do not go to sleep, instead just don't message the client back
	interrupt->SetLevel(oldLevel);
}

void ServerCondition::Signal(ServerLock* conditionServerLock) {
	IntStatus oldLevel = interrupt->SetLevel(IntOff);
	
	if (this->waitQueue->IsEmpty() ) {
		printf("The waitQ was empty.\n");
		interrupt->SetLevel(oldLevel);
		return;
	}
	
	if (this->waitingLock != conditionServerLock) {
		printf("Signal passed a lock != to its lock!\n");
		interrupt->SetLevel(oldLevel);
		return;
	}
	
	ClientThreadPair* ctp = (ClientThreadPair*) this->waitQueue->Remove();
	printf("Signal received. Signalling [%d][%d]\n",ctp->clientID,ctp->threadID);
	conditionServerLock->Acquire(ctp->clientID, ctp->threadID); //Try to acquire the lock using the IDs from the next CTP in the waiting queue
	//conditionServerLock->Release(conditionServerLock->client,conditionServerLock->thread);//we need to give up the lock so it can be acquired
	/*signalledThread->setStatus(READY);
	scheduler->ReadyToRun(signalledThread);*/
	
	if (this->waitQueue->IsEmpty() ) {
		waitingLock = NULL;
	}
	
	interrupt->SetLevel(oldLevel);
}

void ServerCondition::Broadcast(ServerLock* conditionServerLock) {
  //int oClient = conditionServerLock->client;
  //int oThread = conditionServerLock->thread;
  while (!this->waitQueue->IsEmpty() ) {	
    this->Signal(conditionServerLock);    
  }
  //conditionServerLock->Acquire(oClient,oThread);
}

bool ServerCondition::HasThreadsWaiting() {
	IntStatus oldLevel = interrupt->SetLevel(IntOff);
	bool result = !this->waitQueue->IsEmpty();
	interrupt->SetLevel(oldLevel);
	return result;
}
#endif

