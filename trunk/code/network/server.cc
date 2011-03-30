//Server.cc
/* The main rserverOutine for the server
 * Listens for msgs from clients
 */
 
#include "copyright.h"

#include "system.h"
#include "network.h"
#include "post.h"
#include "interrupt.h"
#include "syscall.h"
#include "list.h"

#define MAX_LOCKS 100 
#define MAX_CONDITIONS 200
#define MAX_MONITORS 200
//#define ServerLock Lock

extern "C" {
	int bcopy(char *, char *, int);
};

struct WaitListEntry{
	int clientID;
	int threadID;
	char* msg;
};

struct LockEntry {
	ServerLock* lock; //Changed from Lock to ServerLock
	int clientID;
	int threadID;
	bool needsToBeDeleted;
	int aboutToBeAcquired;	
};
LockEntry serverLocks[MAX_LOCKS];

struct ConditionEntry {
	ServerCondition* condition;
	int clientID;
	int threadID;
	bool needsToBeDeleted;
	int aboutToBeWaited;
};
ConditionEntry serverCVs[MAX_CONDITIONS];

typedef int Monitor;
struct MonitorEntry {
	Monitor monitor;
	int clientID;
	int threadID;
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
void handleIncomingRequests();
void handleQueuedRequests();


char *ack = "";
bool serverSuccess;
PacketHeader serverOutPktHdr, serverInPktHdr;
MailHeader serverOutMailHdr, serverInMailHdr;
int fnCall = 0;
int numLocks = 0;
string args[4];
int sender;
int threadBox;
bool xferLock = FALSE;
List* waitList;
char serverBuffer[MaxMailSize];
bool requestCompleted;
Lock* RequestLock = new Lock("requestLock");


void RunServer(void){
	printf("Server coming online...\n");	
	

	waitList = new List();
	initServerData();
	    
	printf("Server online...\n");	
	
	handleIncomingRequests();	
	
	
}

void handleIncomingRequests(){
	printf("Listening for clients on the network...\n");
	//requestLock->Acquire();
	while(true){						
		postOffice->Receive(0, &serverInPktHdr, &serverInMailHdr, serverBuffer);
		printf("Got \"%s\" from %d, box %d\n",serverBuffer,serverInPktHdr.from,serverInMailHdr.from);
		sender = serverInPktHdr.from;
	    fflush(stdout);    	    	
	    parsePacket(serverBuffer);            		    
    	
    	fnCall = atoi(args[0].c_str());
    	printf("FnCall = [%d]\n",fnCall);
    	switch(fnCall){
	    	
	    	case SC_CreateLock:
	    		printf("Request from Client[%d], ThreadID[%s]. Creating a new ServerLock.\n", serverInPktHdr.from, args[2].c_str());	    			    		    		
	    		sprintf(ack,"%d",CreateLock_Syscall_Server(const_cast<char *>(args[1].c_str())));    	
	    		threadBox = atoi(args[2].c_str());
	    	break;
	    	
	    	case SC_Acquire:
	    		printf("Request from Client[%d], ThreadID[%s]. Acquiring ServerLock[%d]\n",serverInPktHdr.from,args[2].c_str(), atoi(args[1].c_str()));
	    		Acquire_Syscall_Server(atoi(args[1].c_str()));	    
	    		threadBox = atoi(args[2].c_str());		
	    	break;
	    	
	    	case SC_Release:	  
	    		printf("Request from Client[%d], ThreadID[%s]. Releasing ServerLock[%d]\n",serverInPktHdr.from,args[2].c_str(), atoi(args[1].c_str()));
	    		Release_Syscall_Server(atoi(args[1].c_str()));	    	
	    		threadBox = atoi(args[2].c_str());
	    	break;
	    	
	    	case SC_DestroyLock:
	    		printf("Request from Client[%d], ThreadID[%s]. Destroying ServerLock[%d]\n",serverInPktHdr.from,args[2].c_str(), atoi(args[1].c_str()));
	    		DestroyLock_Syscall_Server(atoi(args[1].c_str())); 
	    		threadBox = atoi(args[2].c_str());	
	    	break;
	    	
	    	case SC_CreateCondition:
	    		printf("Request from Client[%d], ThreadID[%s]. Creating a new ServerCV.\n", serverInPktHdr.from, args[2].c_str());
	    		sprintf(ack,"%d",CreateCondition_Syscall_Server(const_cast<char *>(args[1].c_str())));	    	
	    		threadBox = atoi(args[2].c_str());
	    	break;
	    	
	    	case SC_Signal:	   
	    		printf("Request from Client[%d], ThreadID[%s]. Signaling ServerCV[%d] with ServerLock[%d]\n",serverInPktHdr.from, args[3].c_str(), atoi(args[1].c_str()),atoi(args[2].c_str()));
	    		Signal_Syscall_Server(atoi(args[1].c_str()),atoi(args[2].c_str()));	 
	    		threadBox = atoi(args[3].c_str());
	    	break;
	    	
	    	case SC_Wait:	    
	    		printf("Request from Client[%d], ThreadID[%s]. Waiting on ServerCV[%d] with ServerLock[%d]\n",serverInPktHdr.from, args[3].c_str(), atoi(args[1].c_str()),atoi(args[2].c_str()));
	    		Wait_Syscall_Server(atoi(args[1].c_str()),atoi(args[2].c_str()));	
	    		threadBox = atoi(args[3].c_str());
	    	break;
	    	
	    	case SC_Broadcast:	  
	    		printf("Request from Client[%d], ThreadID[%s]. Broadcasting ServerCV[%d] with ServerLock[%d]\n",serverInPktHdr.from, args[3].c_str(), atoi(args[1].c_str()),atoi(args[2].c_str()));
	    		Broadcast_Syscall_Server(atoi(args[1].c_str()),atoi(args[2].c_str()));  	
	    		threadBox = atoi(args[3].c_str());
	    	break;
	    	
	    	case SC_DestroyCondition:
	    		printf("Request from Client[%d], ThreadID[%s]. Destroying ServerCV[%d]\n",serverInPktHdr.from,args[2].c_str(), atoi(args[1].c_str()));
	    		DestroyCondition_Syscall_Server(atoi(args[1].c_str())); 
	    		threadBox = atoi(args[3].c_str());
	    	break;
	    		
    	}
    	if(!requestCompleted)	    	   
    		continue;    	
    	
    	
    	
    	serverOutPktHdr.to = sender;
    	serverOutMailHdr.to = threadBox;
    	serverOutMailHdr.length = strlen(ack) + 1;
		serverOutMailHdr.from = 0;
		printf("Sending reply to Client[%d], Box[%d]\n",sender, threadBox);
    	serverSuccess = postOffice->Send(serverOutPktHdr, serverOutMailHdr, ack); 	
    	   	    
	    	
    	
    	ack = "";
    	for(int x = 0; x < 4; x++)
    		args[x] = "";
    	fflush(stdout);
    	//requestLock->Release(); 
    	if ( !serverSuccess ) {
      		printf("The postOffice Send failed.\n");
      	interrupt->Halt();      	      	       	      	 
    	}    			    	   
    	
	}
	
	interrupt->Halt();
}


//parses the char[] from the packet
void parsePacket(char* serverBuffer){
	int arg = 0;
	int i = 0;
	int j = 0;
	int readback = 0;
	char temp[100];
	
	//printf("Parsing packet.\n");
	while(serverBuffer[i] != '*') //we chose * as our terminating character
	{
		//printf("Packet[%d] = [%c]\n",i,serverBuffer[i]);		
		if(serverBuffer[i] == ',')
		{
			//printf("Found new argument. Parsing.\n");
			i -= readback;
			readback = -1;
			while(serverBuffer[i] != ',')
			{
				//printf("ParsedArg[%d] = [%c]\n", i, serverBuffer[i]);
				temp[j] = serverBuffer[i];
				j++;
				i++;
			}
			temp[j] = '\0';
			j=0;			
			args[arg] = temp;			
			//printf("Args[%d] = [%s]\n", arg, args[arg].c_str());
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
		serverLocks[i].clientID = -1;
		serverLocks[i].threadID = -1;
		serverLocks[i].needsToBeDeleted = FALSE;
		serverLocks[i].aboutToBeAcquired = 0;				
	}

	//initialize conditions
	for (int i = 0; i < MAX_CONDITIONS; i++) {
		serverCVs[i].condition = NULL;
		serverCVs[i].clientID = -1;
		serverCVs[i].threadID = -1;
		serverCVs[i].needsToBeDeleted = FALSE;
		serverCVs[i].aboutToBeWaited = 0;
	}

	//initialize monitorvars		
	for (int i = 0; i < MAX_CONDITIONS; i++) {
		serverMVs[i].monitor = NULL;
		serverMVs[i].clientID = -1;
		serverMVs[i].threadID = -1;
		serverMVs[i].needsToBeDeleted = FALSE;
		serverMVs[i].aboutToBeWaited = 0;
	}
}

// Helper functions for "Server Syscalls" ==============================================
//		Put by Mihir
void deleteServerCondition(int id) {
	delete serverCVs[id].condition;
	serverCVs[id].condition = NULL;
	serverCVs[id].clientID = -1;
	serverCVs[id].threadID = -1;
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
	serverLocks[id].clientID = -1;
	serverLocks[id].threadID = -1;
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
			serverLocks[index].lock = new ServerLock(name);
			serverLocks[index].clientID = serverInPktHdr.from;
			serverLocks[index].threadID = atoi(args[2].c_str());
			numLocks++;
		}
		//locksLock->Release();
		printf("Returning lock index: %d\n", index); //DEBUG	
		requestCompleted = true;	
		return index;
}

void Acquire_Syscall_Server(LockID id){
	//locksLock->Acquire();
	if (serverLocks[id].clientID != serverInPktHdr.from) {
		printf("LockID[%d] cannot be acquired from a non-owning client!\n", id);
		//locksLock->Release();
		ack = "Acquire failed.";
		requestCompleted = true;
		return;
	} 
	if(serverLocks[id].lock == NULL){
		ack = "Lock does not exist.";
		requestCompleted = true;
		return;
	}
	/*if(serverLocks[id].lock->IsBusy())
	{
		printf("LockID[%d] is busy, unable to complete request. Queuing client.\n",id);
		WaitListEntry* wl = new WaitListEntry;		
		wl->threadID = atoi(args[2].c_str());				
		wl->clientID = sender;
		wl->msg = serverBuffer;
		printf("WaitListEntry[%d][%d]\n",wl->clientID,wl->threadID);
		waitList->Append(wl);
		printf("Appended to waitqueue.\n");		
		requestCompleted = false;
		return;
	}*/
	
	serverLocks[id].aboutToBeAcquired++;
	//serverLocksLock->Release();
	if(serverLocks[id].lock->IsBusy())
		requestCompleted = false;
	else
		requestCompleted = true;
		
	serverLocks[id].lock->Acquire(serverLocks[id].clientID, atoi(args[2].c_str()));
	serverLocks[id].aboutToBeAcquired--;
	DEBUG('a', "Lock [%d] has been acquired.\n", id); //DEBUG
	sprintf(ack,"Lock [%d] has been acquired.", id); //DEBUG	
}

void Release_Syscall_Server(LockID id){
	//locksLock->Acquire();
	if (serverLocks[id].clientID != serverInPktHdr.from) {
		printf("LockID[%d] cannot be released from a non-owning client!\n", id);
		ack = "LockID cannot be released from a non-owning client.";
		//locksLock->Release();
		requestCompleted = true;
		return;
	}
	if(serverLocks[id].lock == NULL){
		requestCompleted = true;
		ack = "Lock does not exist.";
		return;
	}


	serverLocks[id].lock->Release(serverLocks[id].clientID);	
	sprintf(ack, "Lock[%d] has been released.",id);	
	if (serverLocks[id].needsToBeDeleted && !serverLocks[id].lock->IsBusy() 
		&& serverLocks[id].aboutToBeAcquired == 0) {			
			deleteServerLock(id);
			ack = "Lock released and deleted.";
	}
	//locksLock->Release();
	requestCompleted = true;
	printf("Lock[%d] has been released.\n",id);
} 

void DestroyLock_Syscall_Server(LockID id){
	//locksLock->Acquire();	
	if (serverLocks[id].clientID != serverInPktHdr.from) {		
		printf("LockID[%d] cannot be destroyed from a non-owning process!\n", id);
		ack = "LockID cannot be destroyed from non-owner.";
		requestCompleted = true;
	} else {	
		if(serverLocks[id].lock == NULL){
			ack = "LockID does not exist.";
			requestCompleted = true;
		}
		else if (serverLocks[id].lock->IsBusy() || serverLocks[id].aboutToBeAcquired > 0) {
			serverLocks[id].needsToBeDeleted = TRUE;
			ack = "Lock will be deleted when possible.";
			DEBUG('a', "Lock[%d] will be deleted when possible.\n",id); //DEBUG
			requestCompleted = true;
		} else {
			deleteServerLock(id);			
			DEBUG('a', "Lock[%d] has been deleted.\n",id);//DEBUG
			sprintf(ack,"Lock [%d] has been deleted.", id); //DEBUG
			requestCompleted = true;
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
		serverCVs[index].condition = new ServerCondition(name);
		serverCVs[index].clientID = serverInPktHdr.from;		
		serverLocks[index].threadID = atoi(args[2].c_str());
	}
	//conditionsLock->Release();	
	requestCompleted = true;
	return index;
}

void Signal_Syscall_Server(ConditionID conditionID, LockID lockID){
	//conditionsLock->Acquire();
	//locksLock->Acquire();
	if (serverCVs[conditionID].clientID != serverInPktHdr.from) {
		printf("ConditionID[%d] cannot be waited from a non-owning process!\n", conditionID);
		sprintf(ack,"ConditionID[%d] cannot be waited from a non-owning process!", conditionID);
		//locksLock->Release();
		//conditionsLock->Release();
		requestCompleted = true;
		return;
	} 

	if (serverLocks[lockID].clientID != serverInPktHdr.from) {
		printf("LockID[%d] cannot be passed to Wait from a non-owning process!\n", lockID);
		sprintf(ack,"LockID[%d] cannot be passed to Wait from a non-owning process!", lockID);
		//locksLock->Release();
		//conditionsLock->Release();
		requestCompleted = true;
		return;
	}

	serverCVs[conditionID].condition->Signal(serverLocks[lockID].lock);
	printf("CV[%d] signaled with Lock[%d] successfully.\n",conditionID,lockID);
	sprintf(ack, "CV[%d] signaled with Lock[%d] successfully.",conditionID,lockID);
	if (serverCVs[conditionID].needsToBeDeleted 
		&& !serverCVs[conditionID].condition->HasThreadsWaiting()
		&& serverCVs[conditionID].aboutToBeWaited == 0) {
			deleteServerCondition(conditionID);
			sprintf(ack, "CV[%d] signaled with Lock[%d] successfully and deleted.",conditionID,lockID);
	}
	//locksLock->Release(); 
	//conditionsLock->Release();
	requestCompleted = true;
}

void Wait_Syscall_Server(ConditionID conditionID, LockID lockID){
	//conditionsLock->Acquire();
	//locksLock->Acquire();
	if (serverCVs[conditionID].clientID != serverInPktHdr.from) {
		printf("ConditionID[%d] cannot be waited from a non-owning process!\n", conditionID);
		//locksLock->Release();
		//conditionsLock->Release();
		requestCompleted = true;
		return;
	} 

	if (serverLocks[lockID].clientID != serverInPktHdr.from) {
		printf("LockID[%d] cannot be passed to Wait from a non-owning process!\n", lockID);
		//locksLock->Release();
		//conditionsLock->Release();
		requestCompleted = true;
		return;
	}

	serverCVs[conditionID].aboutToBeWaited++;
	//locksLock->Release();
	//conditionsLock->Release();
	serverCVs[conditionID].condition->Wait(serverLocks[lockID].lock); //this might not quite work

	//conditionsLock->Acquire();
	serverCVs[conditionID].aboutToBeWaited--;	
	//conditionsLock->Release();
	requestCompleted = false;
}

void Broadcast_Syscall_Server(ConditionID conditionID, LockID lockID){
	//conditionsLock->Acquire();
	//locksLock->Acquire();
	if (serverCVs[conditionID].clientID != serverInPktHdr.from) {
		printf("ConditionID[%d] cannot be waited from a non-owning process!\n", conditionID);
		sprintf(ack,"ConditionID[%d] cannot be waited from a non-owning process!", conditionID);
		//locksLock->Release();
		//conditionsLock->Release();
		requestCompleted = true;
		return;
	} 

	if (serverLocks[lockID].clientID != serverInPktHdr.from) {
		printf("LockID[%d] cannot be passed to Wait from a non-owning process!\n", lockID);
		sprintf(ack, "LockID[%d] cannot be passed to Wait from a non-owning process!", lockID);
		//locksLock->Release();
		//conditionsLock->Release();
		requestCompleted = true;
		return;
	}

	serverCVs[conditionID].condition->Broadcast(serverLocks[lockID].lock);
	sprintf(ack, "Condition[%d] successfully broadcasted.",conditionID);
	if (serverCVs[conditionID].needsToBeDeleted 
		&& !serverCVs[conditionID].condition->HasThreadsWaiting()
		&& serverCVs[conditionID].aboutToBeWaited == 0) {
			deleteServerCondition(conditionID);
			sprintf(ack, "Condition[%d] successfully broadcasted and deleted.",conditionID);
	}

	//locksLock->Release();
	//conditionsLock->Release();
	requestCompleted = true;
}

void DestroyCondition_Syscall_Server(ConditionID id){
	//conditionsLock->Acquire();
	if (serverCVs[id].clientID != serverInPktHdr.from) {
		printf("ConditionID[%d] cannot be destroyed from a non-owning process!\n", id);
	} else {
		if (serverCVs[id].condition->HasThreadsWaiting() || serverCVs[id].aboutToBeWaited > 0) {
			serverCVs[id].needsToBeDeleted = TRUE;
		} else {
			deleteServerCondition(id);
		}
	}
	requestCompleted = true;
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