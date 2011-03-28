//Server.cc
/* The main routine for the server
 * Listens for msgs from clients
 */
 
#include "copyright.h"

#include "system.h"
#include "network.h"
#include "post.h"
#include "interrupt.h"

#define MAX_LOCKS 100 
#define MAX_CONDITIONS 200
#define MAX_MONITORS 200

extern "C" {
	int bcopy(char *, char *, int);
};

PacketHeader outPktHdr, inPktHdr;
MailHeader outMailHdr, inMailHdr;
char *data = "Hello there!";
char *ack = "Got it!";
char buffer[MaxMailSize];
bool success;


struct LockEntry {
	Lock* lock;
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
	
	initServerData();
	    
	printf("Server online...\n");
	printf("Listening for clients on the network...\n");

	while(true){
		
		postOffice->Receive(0, &inPktHdr, &inMailHdr, buffer);
		printf("Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.from);
    	fflush(stdout);
    	
    	outPktHdr.to = inPktHdr.from;
    	outMailHdr.to = inMailHdr.from;
    	outMailHdr.length = strlen(ack) + 1;
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
