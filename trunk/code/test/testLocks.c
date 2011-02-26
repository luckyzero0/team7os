/*Quick test program to check if lock related SYSCALLS are working.*/


#include "syscall.h"
#include "utils.h"

int main()
{
	int lockID, lockIDPlus1;
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
	printf("About to exit!\n");
	Exit(0);
	
}
