/* MultiClientTest.c
 * This test ensures that the server can handle multiple clients
 * connecting to the server and that the server can handle the 
 * stress of such a hostile work environment.
 */
#include "utils.h"
#include "syscall.h"

int lockID;
int cvID;
int mvID;
int x;
	
int main(){	
	/*routines for testing lock creation and deletion*/
	printf("Beginning multiClient test. For best results, run with -a debug arguments to see server replies.\n",0,0,0,"","");	
	printf("Creating a new lock on the network...\n",0,0,0,"","");	
	lockID = CreateLock("lock",4);
	printf("LockID = [%d]\n",lockID,0,0,"","");	
	
	printf("Creating a new CV on the network...\n",0,0,0,"","");
	cvID = CreateCondition("CV",2);
	printf("CVID = [%d]\n",cvID,0,0,"","");
	
	printf("Creating a new MV on the network...\n",0,0,0,"","");
	mvID = CreateMonitor("MV",2);
	printf("MVID = [%d]\n",mvID,0,0,"","");
	
	printf("Acquiring Lock[%d].\n",lockID,0,0,"","");
	Acquire(lockID);	
	printf("Lock[%d] acquired.\n",lockID,0,0,"","");
	
	printf("Modifying MV[%d].\n",mvID,0,0,"","");
	for (x = 0; x < 20; x++)
	{
		SetMonitor(mvID,x);
	}
	printf("Done modifying. MV[%d] = [%d]\n",mvID,GetMonitor(mvID),0,"","");
	
	printf("Signalling CVID[%d].\n",cvID,0,0,"","");
	Signal(cvID,lockID);
	printf("CVID[%d] signaled.\n",cvID,0,0,"","");
	
	
	/*printf("Destroying CVID[%d]...\n",cvID,0,0,"","");
	DestroyCondition(cvID);
	printf("CVID[%d] destroyed, verifying with a signal.\n",cvID,0,0,"","");
	Signal(cvID,lockID);
	*/
	printf("Releasing LockID[%d].\n",lockID,0,0,"","");
	Release(lockID);
	printf("LockID[%d] released",lockID,0,0,"","");
		
	/*
	printf("Destroying LockID[%d]...\n",lockID,0,0,"","");
	DestroyLock(lockID);		
	printf("LockID[%d] destroyed, verifying with an acquire.\n",lockID,0,0,"","");
	Acquire(lockID);
	*/
		
	printf("Done.\n",0,0,0,"","");
	
	Exit(0);
}