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


struct LockEntry {
        ServerLock* lock; //Changed from Lock to ServerLock
        int clientID; //keep track of client and thread instead of addrSpace
        int threadID;
        bool needsToBeDeleted;
        int aboutToBeAcquired;  
};
LockEntry serverLocks[MAX_LOCKS];

struct ConditionEntry {
        ServerCondition* condition; //same as above
        int clientID;
        int threadID;
        bool needsToBeDeleted;
        int aboutToBeWaited;
};
ConditionEntry serverCVs[MAX_CONDITIONS];

typedef int Monitor; //monitors are just ints.
struct MonitorEntry {
        Monitor monitor;
        int clientID;
        int threadID;
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
MonitorID CreateMonitor_Syscall_Server(char* name);
int GetMonitor_Syscall_Server(MonitorID monitorID);
void SetMonitor_Syscall_Server(MonitorID monitorID, int value);


void handleIncomingRequests();


char *ack = "";
bool serverSuccess;
PacketHeader serverOutPktHdr, serverInPktHdr;
MailHeader serverOutMailHdr, serverInMailHdr;
int fnCall = 0;
int numLocks = 0;
string args[4];
int sender; //the clientID of received msg
int threadBox; //the ThreadID making the syscall within the client. ThreadID = MailBox#
char serverBuffer[MaxMailSize];
bool requestCompleted; //determines whether a message is sent back immediately


void RunServer(void){
        printf("Server coming online...\n");            
        initServerData();           
        printf("Server online...\n");           
        handleIncomingRequests();                       
}

/* Main loop for handling incoming requests
 * We receive a packet/request to mailbox 0,
 * and parse the packet to determine the 
 * request:
 * A packet sent from exception.cc will contain
 *      1.) syscall enum
 *      2.) parameters required to complete the syscall
 * the packet is parsed and then stored and then processed.
 * If the request completes, a msg is sent back to the client.
 * Otherwise, we listen for the next request.
 */

void handleIncomingRequests(){
        printf("Listening for clients on the network...\n");
        //requestLock->Acquire();
        while(true){                                            
                postOffice->Receive(0, &serverInPktHdr, &serverInMailHdr, serverBuffer);
                printf("Got \"%s\" from %d, box %d\n",serverBuffer,serverInPktHdr.from,serverInMailHdr.from);
                sender = serverInPktHdr.from; //store this to be used outside of the main loop
            fflush(stdout);             
            parsePacket(serverBuffer);                              
        
        fnCall = atoi(args[0].c_str()); //the Syscall_Enum is the first argument parsed
        printf("FnCall = [%d]\n",fnCall);
        switch(fnCall){
                
                case SC_CreateLock:
                        printf("Request from Client[%d], ThreadID[%s]. Creating a new ServerLock.\n", serverInPktHdr.from, args[2].c_str());                                                            
                        sprintf(ack,"%d",CreateLock_Syscall_Server(const_cast<char *>(args[1].c_str()))); //create response to client from return value         
                        threadBox = atoi(args[2].c_str()); //ensure that we send our reply to the proper client mailbox/threadID
                break;
                
                case SC_Acquire:
                        printf("Request from Client[%d], ThreadID[%s]. Acquiring ServerLock[%d]\n",serverInPktHdr.from,args[2].c_str(), atoi(args[1].c_str()));
                        Acquire_Syscall_Server(atoi(args[1].c_str())); //value of ack is set inside the function, as it does not return a value
                        threadBox = atoi(args[2].c_str());              
                break;
                
                case SC_Release:          
                        printf("Request from Client[%d], ThreadID[%s]. Releasing ServerLock[%d]\n",serverInPktHdr.from, args[2].c_str(), atoi(args[1].c_str()));
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
                
                case SC_CreateMonitor:
                        printf("Request from Client[%d], ThreadID[%s]. Creating a new Monitor.\n", serverInPktHdr.from, args[2].c_str());                                                               
                        sprintf(ack,"%d",CreateMonitor_Syscall_Server(const_cast<char *>(args[1].c_str())));            
                        threadBox = atoi(args[2].c_str());
                break;
                
                case SC_SetMonitor:
                        printf("Request from Client[%d], ThreadID[%s]. Creating a new Monitor.\n", serverInPktHdr.from, args[3].c_str());                                                               
                        SetMonitor_Syscall_Server(atoi(args[1].c_str()),atoi(args[2].c_str()));
                        threadBox = atoi(args[3].c_str());
                break;
                
                case SC_GetMonitor:
                        printf("Request from Client[%d], ThreadID[%s]. Creating a new Monitor.\n", serverInPktHdr.from, args[2].c_str());                                                               
                        sprintf(ack,"%d",GetMonitor_Syscall_Server(atoi(args[1].c_str())));     
                        threadBox = atoi(args[2].c_str());
                break;
                        
        }
        /* If a request could not be completed, as in the case of
         * attempting to acquire a busy lock, we do not message the
         * client back right away, as this would allow them to bypass
         * the waitqueue of the lock.
         */
        if(!requestCompleted)   
                continue;       
        
        
        //prepare the messsage and send it out.
        serverOutPktHdr.to = sender;
        serverOutMailHdr.to = threadBox;
        serverOutMailHdr.length = strlen(ack) + 1;
                serverOutMailHdr.from = 0;
                printf("Sending reply to Client[%d], Box[%d] MSG = [%s]\n",sender, threadBox, ack);
        serverSuccess = postOffice->Send(serverOutPktHdr, serverOutMailHdr, ack);       
                    
                
        
        //clear everything out for the next message
        ack = "";
        for(int x = 0; x < 4; x++)
                args[x] = "";
        fflush(stdout);         
        if ( !serverSuccess ) {
                printf("The postOffice Send failed.\n");
                break;           
        }                                  
        
        }
        
        interrupt->Halt();
}


//parses the char* from the packet
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
        }
}

// Helper functions for "Server Syscalls" ==============================================
//              Put by Mihir
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
// Except this one. I had to write this one. ========================================
//             -J
int getAvailableServerMonitorID() {
        int index = -1;
        for (int i = 0; i < MAX_MONITORS; i++) {
                if (serverMVs[i].monitor == NULL) {
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
// telling it to do one of these things.
// Once the server receives the message and interprets the request,
// it will call the appropriate syscall which has the same basic
// functionality as those in exception.cc, except that we check
// client/threadID instead of address space.
//======================================================================
LockID CreateLock_Syscall_Server(char* name){           
                //locksLock->Acquire(); //the server is a single thread, so these locks aren't needed
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
                ack = "Acquire failed."; //msg created to send to client. View these with DEBUG -a 
                requestCompleted = true;
                return;
        } 
        if(serverLocks[id].lock == NULL){
                sprintf(ack,"Acquire failed Lock[%d]=null.",id);
                requestCompleted = true;
                return;
        }
        
        serverLocks[id].aboutToBeAcquired++;
        //serverLocksLock->Release();
        if(serverLocks[id].lock->IsBusy())
        {                               
                        printf("Lock[%d] is busy. Request Failed.\n",id);
                        requestCompleted = false;
        }
        else
        {
                printf("Lock[%d] is free. Request Succeeded.\n",id);
                requestCompleted = true;
        }
                
        //server lock Acquire takes the clientID and threadID. This has to do with
        //how lock ownership is transferred on the server, as the server cannot
        //acquire the lock itself, as trying to acquire a busy lock would cause
        //the server to lock up.        
        serverLocks[id].lock->Acquire(serverLocks[id].clientID, atoi(args[2].c_str()));
        serverLocks[id].aboutToBeAcquired--;    
        sprintf(ack,"Lock [%d] acquired", id);
}

void Release_Syscall_Server(LockID id){
        //locksLock->Acquire();
        if (serverLocks[id].clientID != serverInPktHdr.from) {
                printf("LockID[%d] cannot be released from a non-owning client!\n", id);
                ack = "Release failed.";
                //locksLock->Release();
                requestCompleted = true;
                return;
        }
        if(serverLocks[id].lock == NULL){
                requestCompleted = true;
                ack = "Lock = NULL.";
                return;
        }

        //serverLocks take clientID in Release for the same reasons as
        //above (the way we handle transferring lock ownership requires
        // it).
        serverLocks[id].lock->Release(serverLocks[id].clientID, atoi(args[2].c_str())); 
        sprintf(ack, "Lock[%d] released.",id);
        if (serverLocks[id].needsToBeDeleted && !serverLocks[id].lock->IsBusy() 
                && serverLocks[id].aboutToBeAcquired == 0) {                    
                        deleteServerLock(id);
                        ack = "Lock released and deleted.";
        }
        //locksLock->Release();
        requestCompleted = true;
        printf("Lock[%d] released.\n",id);
} 

void DestroyLock_Syscall_Server(LockID id){
        //locksLock->Acquire(); 
        if (serverLocks[id].clientID != serverInPktHdr.from) {          
                printf("LockID[%d] cannot be destroyed from a non-owning process!\n", id);
                ack = "Destroy failed.";
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
                sprintf(ack,"ConditionID[%d] wait failed. Bad owner!", conditionID);
                //locksLock->Release();
                //conditionsLock->Release();
                requestCompleted = true;
                return;
        } 

        if (serverLocks[lockID].clientID != serverInPktHdr.from) {
                printf("LockID[%d] cannot be passed to Wait from a non-owning process!\n", lockID);
                sprintf(ack,"LockID[%d] not owned by client.", lockID);
                //locksLock->Release();
                //conditionsLock->Release();
                requestCompleted = true;
                return;
        }

        //serverCVs take a serverLock. Once again, this has to do with how
        //lock ownership is transferred.
        serverCVs[conditionID].condition->Signal(serverLocks[lockID].lock);
        printf("CV[%d] signaled with Lock[%d] successfully.\n",conditionID,lockID);
        sprintf(ack, "CV[%d] signaled successfully.",conditionID,lockID);
        if (serverCVs[conditionID].needsToBeDeleted 
                && !serverCVs[conditionID].condition->HasThreadsWaiting()
                && serverCVs[conditionID].aboutToBeWaited == 0) {
                        deleteServerCondition(conditionID);
                        sprintf(ack, "CV[%d] signaled successfully and deleted.",conditionID,lockID);
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
                sprintf(ack, "CV[%d] wait failed.",conditionID);
                requestCompleted = true;
                return;
        } 

        if (serverLocks[lockID].clientID != serverInPktHdr.from) {
                printf("LockID[%d] cannot be passed to Wait from a non-owning process!\n", lockID);
                sprintf(ack, "Wait failed.\n", lockID);
                //locksLock->Release();
                //conditionsLock->Release();
                requestCompleted = true;
                return;
        }

        serverCVs[conditionID].aboutToBeWaited++;
        //locksLock->Release();
        //conditionsLock->Release();
        
        //same as before, we take a serverLock to handle ownership 
        //issues
        serverCVs[conditionID].condition->Wait(serverLocks[lockID].lock);

        //conditionsLock->Acquire();
        serverCVs[conditionID].aboutToBeWaited--;       
        //conditionsLock->Release();
        requestCompleted = false; //client needs to wait, so don't msg them back
}

void Broadcast_Syscall_Server(ConditionID conditionID, LockID lockID){
        //conditionsLock->Acquire();
        //locksLock->Acquire();
        if (serverCVs[conditionID].clientID != serverInPktHdr.from) {
                printf("ConditionID[%d] cannot be broadcasted from a non-owning process!\n", conditionID);
                sprintf(ack,"Broadcast failed..!", conditionID);
                //locksLock->Release();
                //conditionsLock->Release();
                requestCompleted = true;
                return;
        } 

        if (serverLocks[lockID].clientID != serverInPktHdr.from) {
                printf("LockID[%d] cannot be passed to Broadcast from a non-owning process!\n", lockID);
                sprintf(ack, "Broadcast failed.", lockID);
                //locksLock->Release();
                //conditionsLock->Release();
                requestCompleted = true;
                return;
        }

        serverCVs[conditionID].condition->Broadcast(serverLocks[lockID].lock);
        sprintf(ack, "CV[%d] broadcasted.",conditionID);
        if (serverCVs[conditionID].needsToBeDeleted 
                && !serverCVs[conditionID].condition->HasThreadsWaiting()
                && serverCVs[conditionID].aboutToBeWaited == 0) {
                        deleteServerCondition(conditionID);
                        sprintf(ack, "CV[%d] broadcasted and deleted.",conditionID);
        }

        //locksLock->Release();
        //conditionsLock->Release();
        requestCompleted = true;
}

void DestroyCondition_Syscall_Server(ConditionID id){
        //conditionsLock->Acquire();
        if (serverCVs[id].clientID != serverInPktHdr.from) {
                printf("CVID[%d] cannot be destroyed from a non-owning process!\n", id);
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
        //locksLock->Acquire();
                int index = getAvailableServerMonitorID();
                if (index == -1) {
                        printf("No locks available!\n");
                } else {
                        serverMVs[index].monitor = -1;
                        serverMVs[index].clientID = serverInPktHdr.from;
                        serverMVs[index].threadID = atoi(args[2].c_str());                      
                }
                //locksLock->Release();
                printf("Returning monitor index: %d\n", index); //DEBUG 
                requestCompleted = true;        
                return index;
}

int GetMonitor_Syscall_Server(MonitorID monitorID){
        return serverMVs[monitorID].monitor;
}


void SetMonitor_Syscall_Server(MonitorID monitorID, int value){
        serverMVs[monitorID].monitor = value;
        sprintf(ack,"MVar[%d]set.",monitorID);
}