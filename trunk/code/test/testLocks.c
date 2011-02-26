/*Quick test program to check if lock related SYSCALLS are working.*/


#include "syscall.h"
#include "utils.h"
int lockID;

int main()
{
	printf("\n\nRunning TestLocks Program\n",0,0,0,"","");
	printf("Creating a lock\n",17,0,0,"","");
	lockID = CreateLock("Lock1",5);
	printf("Trying to acquire a lock that hasn't been created. This should not work.\n",74,0,0,"","");
	Acquire(lockID+1); 
	printf("Trying to acquire a lock that has been created. This should work.\n",67,0,0,"","");
	Acquire(lockID);
	printf("Trying to release a lock that hasn't been created. This should not work.\n",74,0,0,"","");
	Release(lockID+1); /*Should give error*/
	printf("Trying to release a lock that has been created. This should work.\n",67,0,0,"","");
	Release(lockID);
	printf("Trying to destroy a lock that hasn't been created. This should not work.\n",74,0,0,"","");
	DestroyLock(lockID+1); /*Should give error*/
	printf("Trying to destroy a lock that has been created. This should work.\n",67,0,0,"","");
	DestroyLock(lockID);
	Exit(0);
	
}
