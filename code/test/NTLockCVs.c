/*NTLockCVs.c
 * A short suite (PUN!) of tests to verify
 * Lock related RPCs
 */
#include "utils.h"
#include "syscall.h"

int lockID;
int cvID;
int mvID;
int x;
	
int main(){	
	/*routines for testing lock creation and deletion*/
	printf("Creating a new lock on the network...\n",0,0,0,"","");	
	lockID = CreateLock("lock",4);
	printf("LockID = [%d]\n",lockID,0,0,"","");	
	
	printf("Creating a new CV on the network...\n",0,0,0,"","");
	cvID = CreateCondition("CV",2);
	printf("CVID = [%d]\n",mvID,0,0,"","");
	
	printf("Creating a new MV on the network...\n",0,0,0,"","");
	mvID = CreateMonitor("MV",2);
	
	printf("Acquiring Lock[%d].\n",lockID,0,0,"","");
	Acquire(lockID);	
	printf("Lock[%d] acquired.\n",lockID,0,0,"","");
	
	printf("Modifying MV[%d].\n",mvID,0,0,"","");
	for (x = 0; x < 20; x++;)
	{
		SetMonitor(x);
	}
	printf("Done modifying. MV[%d] = [%d]\n",mvID,GetMonitor(mvID),0,"","");
	
	printf("Signalling CVID[%d].\n",cvID,0,0,"","");
	Signal(cvID);
	printf("CVID[%d] signaled.\n",cvID,0,0,"","");
	
	printf("Releasing LockID[%d].\n"lockID,0,0,"","");
	Release(lockID);
	printf("LockID[%d] released.\n"lockID,0,0,"","");
	
	printf("Destroying LockID[%d]...\n",lockID,0,0,"","");
	DestroyLock(lockID);		
	printf("LockID[%d] destroyed.\n",lockID,0,0,"","");
	
	printf("Destroying CVID[%d]...\n",cvID,0,0,"","");
	DestroyCondition(cvID);
	printf("CVID[%d] destroyed.\n",cvID,0,0,"","");
	
	
	printf("Done.\n",0,0,"","");
	
	Exit(0);
}
