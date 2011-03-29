//Server.cc
/* The main routine for the server
 * Listens for msgs from clients
 */
 
#include "copyright.h"

#include "system.h"
#include "network.h"
#include "post.h"
#include "interrupt.h"
#include "syscall.h"

#define MAX_LOCKS 100 
#define MAX_CONDITIONS 200
#define MAX_MONITORS 200

extern "C" {
	int bcopy(char *, char *, int);
};


struct LockEntry {
	Lock* lock; //Changed from Lock to ServerLock
	AddrSpace* space;
	bool needsToBeDeleted;
	int aboutToBeAcquired;
};
LockEntry serverLocks[MAX_LOCKS];

struct ConditionEntry {
	Condition* condition;
	AddrSpace* space;
	bool needsToBeDeleted;
	int aboutToBeWaited;
};
ConditionEntry serverCVs[MAX_CONDITIONS];

typedef int Monitor;
struct MonitorEntry {
	Monitor monitor;
	AddrSpace* space;
	bool needsToBeDeleted;
	int aboutToBeWaited;
};
MonitorEntry serverMVs[MAX_MONITORS];


void initServerData();

void RunServer(void){
	printf("Server coming online...\n");
	
	char *data = "Hello there!";
	char *ack = "Got it!";
	char buffer[MaxMailSize];
	bool success;
	PacketHeader outPktHdr, inPktHdr;
	MailHeader outMailHdr, inMailHdr;
	initServerData();
	    
	printf("Server online...\n");
	printf("Listening for clients on the network...\n");

	while(true){		
		
		
		postOffice->Receive(0, &inPktHdr, &inMailHdr, buffer);
		printf("Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.from);
    	fflush(stdout);
    	
    	//parse that shit
    	int fnCall = 0;
    	switch(fnCall){
	    	
	    	case SC_CreateLock:
	    		//ack = itoa(CreateLock_Server_Syscall("namefrombuffer"));    		
	    	break;
    	}
    	
    	outPktHdr.to = inPktHdr.from;
    	outMailHdr.to = inMailHdr.from;
    	outMailHdr.length = strlen(ack) + 1;
		outMailHdr.from = 0;
    	success = postOffice->Send(outPktHdr, outMailHdr, ack); 		
    	
    	if ( !success ) {
      		printf("The postOffice Send failed.\n");
      	interrupt->Halt();      	      	       	      	 
    	}    			    	    
	}
	
	interrupt->Halt();
}

void initServerData(){
	
	//make and/or initialize server data
	//initialize locks
	for (int i = 0; i < MAX_LOCKS; i++) {
		serverLocks[i].lock = NULL;
		serverLocks[i].space = NULL;
		serverLocks[i].needsToBeDeleted = FALSE;
		serverLocks[i].aboutToBeAcquired = 0;
	}

	//initialize conditions
	for (int i = 0; i < MAX_CONDITIONS; i++) {
		serverCVs[i].condition = NULL;
		serverCVs[i].space = NULL;
		serverCVs[i].needsToBeDeleted = FALSE;
		serverCVs[i].aboutToBeWaited = 0;
	}

	//initialize monitorvars		
	for (int i = 0; i < MAX_CONDITIONS; i++) {
		serverMVs[i].monitor = NULL;
		serverMVs[i].space = NULL;
		serverMVs[i].needsToBeDeleted = FALSE;
		serverMVs[i].aboutToBeWaited = 0;
	}
}

// Helper functions for "Server Syscalls" ==============================================
//		Put by Mihir
void deleteServerCondition(int id) {
	delete serverCVs[id].condition;
	serverCVs[id].condition = NULL;
	serverCVs[id].space = NULL;
	serverCVs[id].needsToBeDeleted = FALSE;
	serverCVs[id].aboutToBeWaited = 0;
}

int getAvailableServerConditionID() {
	int index = -1;
	for (int i = 0; i < MAX_CONDITIONS; i++) {
		if (serverCVs[i].condition == NULL) {
			index = i;
			break;
		}
	}
	return index;
}

void deleteServerLock(int id) {
	delete serverLocks[id].lock;
	serverLocks[id].lock = NULL;
	serverLocks[id].space = NULL;
	serverLocks[id].needsToBeDeleted = FALSE;
	serverLocks[id].aboutToBeAcquired = 0;
}

int getAvailableServerLockID() {
	int index = -1;
	for (int i = 0; i < MAX_LOCKS; i++) {
		if (serverLocks[i].lock == NULL) {
			index = i;
			break;
		}
	}
	return index;
}

// "Server" Syscalls. Client's syscall should send a message to server,
// telling it to do one of these things. Feel free to get rid of "Syscall"
// if the names are too long
//	Put by Mihir

LockID CreateLock_Syscall_Server(char* name){
		//locksLock->Acquire();
		int index = getAvailableServerLockID();
		if (index == -1) {
			printf("No locks available!\n");
		} else {
			serverLocks[index].lock = new Lock(name);
			serverLocks[index].space = currentThread->space;
		}
		//locksLock->Release();
		DEBUG('a', "Returning lock index: %d\n", index); //DEBUG
		return index;
}

void Acquire_Syscall_Server(LockID id){
}

void Release_Syscall_Server(LockID id){
}

void DestroyLock_Syscall_Server(LockID id){
}

ConditionID CreateCondition_Syscall_Server(char* name){
	return 0;
}

void Signal_Syscall_Server(ConditionID conditionID, LockID lockID){
}

void Wait_Syscall_Server(ConditionID conditionID, LockID lockID){
}

void Broadcast_Syscall_Server(ConditionID conditionID, LockID lockID){
}

void DestroyCondition_Syscall_Server(ConditionID conditionID){
}

MonitorID CreateMonitor_Syscall_Server(char* name){
	return 0;
}

int GetMonitor_Syscall_Server(MonitorID monitorID){
	return 0;
}

void SetMonitor_Syscall_Server(MonitorID monitorID, int value){
}