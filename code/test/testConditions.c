/*testConditions.c*/
#include "syscall.h"
#include "utils.h"

void t1();
void t2();
int raceCondition = 0;
int i = 0;
int lockID;
int runLockCV;
int cvID;

int main(){	
	printf("Beggining CV Testing suite...\n",0,0,0,"","");
	printf("Trying to create bad CVs...\n",0,0,0,"","");
	
	printf("Attempting to create CV with bad nameLength (too short)...\n",0,0,0,"","");
	cvID = CreateCondition("RaceCV",2);
	printf("Attempting to create CV with bad nameLength (too long)...\n",0,0,0,"","");
	cvID = CreateCondition("RaceCV",21);
	
	printf("Creating a good CV...\n",0,0,0,"","");
	cvID = CreateCondition("RaceCV",6);
	printf("Creating a lock for the CV...\n",0,0,0,"","");
	lockID = CreateLock("RaceLock",8);
	
	printf("Attempting to signal with a bad lock...\n",0,0,0,"","");
	Signal(cvID, lockID+1);
	printf("Attempting to Wait with a bad lock...\n",0,0,0,"","");
	Wait(cvID,lockID+1);
	
	printf("Attempting to signal a non-owned CV...\n",0,0,0,"","");
	Signal(cvID+1,lockID);
	printf("Attempting to wait on a non-owned CV...\n",0,0,0,"","");
	Wait(cvID+1,lockID);
				
	printf("Creating threads to *actually* test CV functionallity...\n",0,0,0,"","");
	runLockCV = CreateCondition("runCV",5);
	Acquire(lockID);
	Fork(t1);
	Fork(t2);	
	Wait(runLockCV,lockID);
	printf("RaceCondition = [%d], and should = 80 if t2 ran before t1.\n",raceCondition,0,0,"","");
	
	Exit(0);
}


void t1(){
	printf("Thread1 forked.\n",0,0,0,"","");
	printf("Thread1 waiting on CV for t2 to signal...\n",0,0,0,"","");
	Acquire(lockID);
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
	printf("Thread2 forked.\n",0,0,0,"","");
	Acquire(lockID);	
	printf("Thread2 operating in CritSection.\n",0,0,0,"","");
	for(i = 0; i < 10; i++)
		raceCondition += 1;
	printf("Thread2 done modifying raceCondition, value = [%d]\n",raceCondition,0,0,"","");
	printf("Thread2 signaling t1...and exiting.\n",0,0,0,"","");
	Signal(cvID,lockID);
	Release(lockID);
	Exit(0); 
}

