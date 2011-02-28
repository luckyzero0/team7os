/*Quick test program to check if lock related SYSCALLS are working.
* Performs a few tests to check bad construction data, as well as
* Passing bad lock IDs to demonstrate attempting to acquire non-owned
* locks.
* The final test demonstrates a small mutex scenario, in which the final
* result of raceCondition should should be 0, 5, or 10, depending on which
* thread runs first, but never any value in between.
*/


#include "syscall.h"
#include "utils.h"

void t1();
void t2();
int raceCondition = 0;
int i;
int lockID, lockIDPlus1;
int main()
{
	
	printf("\n\nRunning TestLocks Program\n",0,0,0,"","");
	printf("Creating a lock\n",0,0,0,"","");
	lockID = CreateLock("Lock1",5);
	lockIDPlus1 = lockID + 1;
	printf("Trying to acquire a lock that hasn't been created. This should not work.\n",0,0,0,"","");
	Acquire(lockIDPlus1); 
	printf("Trying to acquire a lock that has been created. This should work.\n",0,0,0,"","");
	Acquire(lockID);
	printf("Trying to release a lock that hasn't been created. This should not work.\n",0,0,0,"","");
	Release(lockIDPlus1); /*Should give error*/
	printf("Trying to release a lock that has been created. This should work.\n",00,0,0,"","");
	Release(lockID);
	printf("Trying to destroy a lock that hasn't been created. This should not work.\n",0,0,0,"","");
	DestroyLock(lockIDPlus1); /*Should give error*/
	printf("Trying to destroy a lock that has been created. This should work.\n",0,0,0,"","");
	DestroyLock(lockID);
	
	printf("Simulating some potential race conditions...\n",0,0,0,"","");
	printf("Creating a new lock\n",0,0,0,"","");
	lockID = CreateLock("RaceLock",8);
	printf("Forking t1 and t2. If locks are successful, they should run 1 at a time.\n",0,0,0,"","");
	Fork(t1);		
	Fork(t2);
		
	printf("About to exit!\n",0,0,0,"","");
	Exit(0);
	
}

void t1(){	
	Acquire(lockID);	
	printf("Thread1 forked and has the lock.\n",0,0,0,"","");	
	printf("Thread1 executing.\n",0,0,0,"","");
	for(i = 0; i < 10; i ++)
	{
		printf("Thread1 modifying raceCondition = [%d]\n",raceCondition,0,0,"","");
		raceCondition += 1;
	}
	printf("Thread1 done. raceCondition = [%d] == (10)  Releasing the lock.\n",raceCondition,0,0,"","");
	Release(lockID);
	Exit(0);
}

void t2(){
	Acquire(lockID);	
	printf("Thread2 forked and has the lock.\n",0,0,0,"","");	
	printf("Thread2 executing.\n",0,0,0,"","");
	for(i = 0; i < 1; i ++)
	{
		printf("Thread2 modifying raceCondition = [%d]\n",raceCondition,0,0,"","");
		raceCondition /= 2;
	}
	printf("Thread2 done. raceCondition = [%d] == (0 OR 5) Releasing the lock.\n",raceCondition,0,0,"","");
	Release(lockID);
	Exit(0);
}