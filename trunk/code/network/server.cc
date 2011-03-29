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
void parsePacket(char*);

LockID CreateLock_Syscall_Server(char* name);
void DestroyLock_Syscall_Server(LockID id);
ConditionID CreateCondition_Syscall_Server(char* name);
void DestroyCondition_Syscall_Server(ConditionID id);
void Acquire_Syscall_Server(LockID id);
void Release_Syscall_Server(LockID id);
void Signal_Syscall_Server(ConditionID conditionID, LockID lockID);
void Wait_Syscall_Server(ConditionID conditionID, LockID lockID);
void Broadcast_Syscall_Server(ConditionID conditionID, LockID lockID);



int fnCall = 0;
string args[4];

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
    	parsePacket(buffer);            	
    	fnCall = atoi(args[0].c_str());
    	printf("FnCall = [%d]\n",fnCall);
    	switch(fnCall){
	    	
	    	case SC_CreateLock:
	    		printf("Creating a new ServerLock.\n");	    			    		    		
	    		sprintf(ack,"%d",CreateLock_Syscall_Server(const_cast<char *>(args[1].c_str())));    		
	    	break;
	    	
	    	case SC_Acquire:	    		
	    	break;
	    	
	    	case SC_Release:	    	
	    	break;
	    	
	    	case SC_DestroyLock:
	    		printf("Destroying ServerLock[%d]\n",atoi(args[1].c_str()));
	    		DestroyLock_Syscall_Server(atoi(args[1].c_str())); 	
	    	break;
	    	
	    	case SC_CreateCondition:
	    		printf("Creating a new ServerCV.\n");
	    		sprintf(ack,"%d",CreateCondition_Syscall_Server(const_cast<char *>(args[1].c_str())));	    	
	    	break;
	    	
	    	case SC_Signal:	    	
	    	break;
	    	
	    	case SC_Wait:	    	
	    	break;
	    	
	    	case SC_Broadcast:	    	
	    	break;
	    	
	    	case SC_DestroyCondition:
	    		printf("Destroying ServerCV[%d]\n",atoi(args[1].c_str()));
	    		DestroyCondition_Syscall_Server(atoi(args[1].c_str())); 
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

//parses the char[] from the packet
void parsePacket(char* buffer){
	int arg = 0;
	int i = 0;
	int j = 0;
	int readback = 0;
	char temp[100];
	
	printf("Parsing packet.\n");
	while(buffer[i] != '*') //we chose * as our terminating character
	{
		//printf("Packet[%d] = [%c]\n",i,buffer[i]);		
		if(buffer[i] == ',')
		{
			//printf("Found new argument. Parsing.\n");
			i -= readback;
			readback = -1;
			while(buffer[i] != ',')
			{
				//printf("ParsedArg[%d] = [%c]\n", i, buffer[i]);
				temp[j] = buffer[i];
				j++;
				i++;
			}
			temp[j] = '\0';
			j=0;			
			args[arg] = temp;			
			printf("Args[%d] = [%s]\n", arg, args[arg].c_str());
			arg++;
		}
		readback++;
		i++;
	}
	
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
	//locksLock->Acquire();
	if (serverLocks[id].space != currentThread->space) {
		printf("LockID[%d] cannot be acquired from a non-owning process!\n", id);
		//locksLock->Release();
		return;
	} 

	serverLocks[id].aboutToBeAcquired++;
	//serverLocksLock->Release();
	serverLocks[id].lock->Acquire();
	serverLocks[id].aboutToBeAcquired--;
	DEBUG('a', "Lock [%d] has been acquired.\n", id); //DEBUG
}

void Release_Syscall_Server(LockID id){
	//locksLock->Acquire();
	if (serverLocks[id].space != currentThread->space) {
		printf("LockID[%d] cannot be released from a non-owning process!\n", id);
		//locksLock->Release();
		return;
	}

	serverLocks[id].lock->Release();
	if (serverLocks[id].needsToBeDeleted && !serverLocks[id].lock->IsBusy() 
		&& serverLocks[id].aboutToBeAcquired == 0) {
			deleteServerLock(id);
	}
	//locksLock->Release();

	DEBUG('a', "Releasing lock[%d].\n",id); //DEBUG
}

void DestroyLock_Syscall_Server(LockID id){
	//locksLock->Acquire();
	printf("INZE DESTRUCTION!\n");
	if (serverLocks[id].space != currentThread->space) {
		printf("LockID[%d] cannot be destroyed from a non-owning process!\n", id);
	} else {
		if (serverLocks[id].lock->IsBusy() || serverLocks[id].aboutToBeAcquired > 0) {
			serverLocks[id].needsToBeDeleted = TRUE;
			DEBUG('a', "Lock[%d] will be deleted when possible.\n",id); //DEBUG
		} else {
			deleteServerLock(id);
			DEBUG('a', "Lock[%d] has been deleted.\n",id);//DEBUG
		}
	}
	//locksLock->Release();
}


ConditionID CreateCondition_Syscall_Server(char* name){
	//conditionsLock->Acquire();
	int index = getAvailableServerConditionID();
	if (index == -1) {
		printf("No conditions available!\n");
	} else {
		serverCVs[index].condition = new Condition(name);
		serverCVs[index].space = currentThread->space;
	}
	//conditionsLock->Release();
	return index;
}

void Signal_Syscall_Server(ConditionID conditionID, LockID lockID){
	//conditionsLock->Acquire();
	//locksLock->Acquire();
	if (serverCVs[conditionID].space != currentThread->space) {
		printf("ConditionID[%d] cannot be waited from a non-owning process!\n", conditionID);
		//locksLock->Release();
		//conditionsLock->Release();
		return;
	} 

	if (serverLocks[lockID].space != currentThread->space) {
		printf("LockID[%d] cannot be passed to Wait from a non-owning process!\n", lockID);
		//locksLock->Release();
		//conditionsLock->Release();
		return;
	}

	serverCVs[conditionID].condition->Signal(serverLocks[lockID].lock);

	if (serverCVs[conditionID].needsToBeDeleted 
		&& !serverCVs[conditionID].condition->HasThreadsWaiting()
		&& serverCVs[conditionID].aboutToBeWaited == 0) {
			deleteServerCondition(conditionID);
	}
	//locksLock->Release();
	//conditionsLock->Release();
}

void Wait_Syscall_Server(ConditionID conditionID, LockID lockID){
	//conditionsLock->Acquire();
	//locksLock->Acquire();
	if (serverCVs[conditionID].space != currentThread->space) {
		printf("ConditionID[%d] cannot be waited from a non-owning process!\n", conditionID);
		//locksLock->Release();
		//conditionsLock->Release();
		return;
	} 

	if (serverLocks[lockID].space != currentThread->space) {
		printf("LockID[%d] cannot be passed to Wait from a non-owning process!\n", lockID);
		//locksLock->Release();
		//conditionsLock->Release();
		return;
	}

	serverCVs[conditionID].aboutToBeWaited++;
	//locksLock->Release();
	//conditionsLock->Release();
	serverCVs[conditionID].condition->Wait(serverLocks[lockID].lock); //this might not quite work

	//conditionsLock->Acquire();
	serverCVs[conditionID].aboutToBeWaited--;

	if (serverCVs[conditionID].needsToBeDeleted 
		&& !serverCVs[conditionID].condition->HasThreadsWaiting()
		&& serverCVs[conditionID].aboutToBeWaited == 0) {
			deleteServerCondition(conditionID);
	}
	//conditionsLock->Release();
}

void Broadcast_Syscall_Server(ConditionID conditionID, LockID lockID){
}

void DestroyCondition_Syscall_Server(ConditionID id){
	//conditionsLock->Acquire();
	if (serverCVs[id].space != currentThread->space) {
		printf("ConditionID[%d] cannot be destroyed from a non-owning process!\n", id);
	} else {
		if (serverCVs[id].condition->HasThreadsWaiting() || serverCVs[id].aboutToBeWaited > 0) {
			serverCVs[id].needsToBeDeleted = TRUE;
		} else {
			deleteServerCondition(id);
		}
	}
	//conditionsLock->Release();
}

MonitorID CreateMonitor_Syscall_Server(char* name){
	return 0;
}

int GetMonitor_Syscall_Server(MonitorID monitorID){
	return 0;
}

void SetMonitor_Syscall_Server(MonitorID monitorID, int value){
}