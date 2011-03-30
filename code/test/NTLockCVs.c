/*NTLockCVs.c
 * A short suite (PUN!) of tests to verify
 * Lock related RPCs
 */
#include "utils.h"
#include "syscall.h"

int lockID1, lockID2;
int x;
	
int main(){
	
	/*routines for testing lock creation and deletion*/
	printf("Creating a new locks on the network...\n",0,0,0,"","");	
	lockID1 = CreateLock("MYLOCK",6);
	lockID2 = CreateLock("MYOTHERLOCK",11);
	
	printf("LockID1 = [%d]\n",lockID1,0,0,"","");
	printf("LockID2 = [%d]\n",lockID2,0,0,"","");
	
	
	printf("Attempting to destroy LockID1...\n",0,0,0,"","");
	DestroyLock(lockID1);
	printf("Attempting to destroy a lock that doesn't actually exist...\n",0,0,0,"","");
	DestroyLock(97);
	printf("Trying to destroy LockID1 again...\n",0,0,0,"","");
	DestroyLock(lockID1);
	printf("Acquiring lockID2...\n",0,0,0,"","");
	Acquire(lockID2);
	printf("Trying to destroy LockID2 while acquired...\n",0,0,0,"","");
	DestroyLock(lockID2);
	printf("Releasing lockID2...\n",0,0,0,"","");
	Release(lockID2);
	
	
			
	
	/*for(x = 0; x < 200; x++) /*200 is twice our max number of locks
	{
		lockID1 = CreateLock("Reused lock",11); 
	}*/
	
	
	Exit(0);
}
