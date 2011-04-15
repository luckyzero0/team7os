/*testConditions.c*/
/*
 * Tests CVs. Attempt to pass bad construction data,
 * as well as access CVs with bad parameters.
 * If successfully, bad CVs are not created, and
 * t2 will run before t1.
 * Finally, tests broadcast. If successful, all threads will
 * exit.
 * NOTE: This test is designed to run without RSing. If you RS,
 * the test creates it's own race conditions that are beyond
 * the scope of this program.
 */
#include "syscall.h"
#include "utils.h"

void t1();
void t2();
void broadcastTest();
int raceCondition = 0;
int i = 0;
int lockID;
int runLockCV;
int forkLock;
int cvID;

int main(){	
	printf("Beggining CV Testing suite...\n",0,0,0,"","");
	printf("Trying to create bad CVs...\n",0,0,0,"","");
		
	/*printf("Attempting to create CV with bad nameLength (too long)...\n",0,0,0,"","");
	cvID = CreateCondition("RaceCV",21);*/
	
	printf("Creating a good CV...\n",0,0,0,"","");
	cvID = CreateCondition("RaceCV",6);
	printf("Creating a lock for the CV...\n",0,0,0,"","");
	lockID = CreateLock("RaceLock",8);	
	
	printf("Attempting to signal with a bad lock...\n",0,0,0,"","");
	Signal(cvID, lockID+1);
	printf("Attempting to Wait with a bad lock...\n",0,0,0,"","");
	Wait(cvID,lockID+1);
	printf("Attempting to Broadcast with a bad lock...\n",0,0,0,"","");
	Broadcast(cvID,lockID+1);
	
	printf("Attempting to signal a non-owned CV...\n",0,0,0,"","");
	Signal(cvID+1,lockID);
	printf("Attempting to wait on a non-owned CV...\n",0,0,0,"","");
	Wait(cvID+1,lockID);
	printf("Attempting to broadcast on a non-owned CV...\n",0,0,0,"","");
	Broadcast(cvID+1,lockID);
			
	printf("Creating threads to *actually* test CV functionallity...\n",0,0,0,"","");
	runLockCV = CreateCondition("runCV",5);
	Acquire(lockID);
	Fork(t1);
	Fork(t2);	
	Wait(runLockCV,lockID);
	printf("RaceCondition = [%d], and should = 80 if t2 ran before t1.\n",raceCondition,0,0,"","");	
	printf("Setting up broadcast test. Forking a lot of threads.\n",0,0,0,"","");	
	for(i = 0; i < 8; i++)
	{		
		ForkWithArg(broadcastTest,i);				
	}	
	Wait(runLockCV,lockID);
	printf("Acquiring lock and broadcasting on the CV.\n",0,0,0,"","");	
	Broadcast(cvID,lockID);
	Release(lockID);
	Exit(0);
}


void t1(){
	printf("Thread[%d] forked.\n",GetThreadID(),0,0,"","");
	printf("Thread[%d] waiting on CV for t2 to signal...\n",GetThreadID(),0,0,"","");
	Acquire(lockID);
	printf("Thread[%d] acquired the execution lock...\n",GetThreadID(),0,0,"","");
	Wait(cvID,lockID);
	printf("Thread1 has been signaled by T2. Entering CritSection\n",0,0,0,"","");
	for(i = 0; i < 3; i++)
		raceCondition *= 2;
	printf("Thread1 done working in critSection. Signaling main thread and Exiting.\n",0,0,0,"","");
	Signal(runLockCV,lockID);
	Release(lockID);
	Exit(0);
}

void t2(){	
	printf("Thread[%d] forked.\n",GetThreadID(),0,0,"","");
	Acquire(lockID);	
	printf("Thread[%d] operating in CritSection.\n",GetThreadID(),0,0,"","");
	for(i = 0; i < 10; i++)
		raceCondition += 1;
	printf("Thread2 done modifying raceCondition, value = [%d]\n",raceCondition,0,0,"","");
	printf("Thread2 signaling t1...and exiting.\n",0,0,0,"","");
	Signal(cvID,lockID);
	Release(lockID);
	Exit(0); 
}

void broadcastTest(){	
	int myArg;
	myArg = GetForkArg();
	printf("Thread[%d] Forked. Waiting on CV\n",myArg,0,0,"","");	
	Acquire(lockID);
	if(myArg == 7) /*this is the last thread*/
	{
		printf("Thread[%d] is the final thread. Signalling the CV.\n",myArg,0,0,"","");		
		Signal(runLockCV,lockID);
	}
	Wait(cvID,lockID);	
	printf("Broadcast received. Thread[%d] exiting.\n",myArg,0,0,"","");
	Release(lockID);
	Exit(0);
}
