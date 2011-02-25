/*Quick test program to check if lock related SYSCALLS are working.*/


#include "syscall.h"
int lockID;

int main()
{
	Write("\n\nRunning TestLocks Program\n",31,ConsoleOutput);
	Write("Creating a lock\n",17,ConsoleOutput);
	lockID = CreateLock("Lock1",5);
	Write("Trying to acquire a lock that hasn't been created. This should not work.\n",74,ConsoleOutput);
	Acquire(lockID+1); 
	Write("Trying to acquire a lock that has been created. This should work.\n",67,ConsoleOutput);
	Acquire(lockID);
	Write("Trying to release a lock that hasn't been created. This should not work.\n",74,ConsoleOutput);
	Release(lockID+1); /*Should give error*/
	Write("Trying to release a lock that has been created. This should work.\n",67,ConsoleOutput);
	Release(lockID);
	Write("Trying to destroy a lock that hasn't been created. This should not work.\n",74,ConsoleOutput);
	DestroyLock(lockID+1); /*Should give error*/
	Write("Trying to destroy a lock that has been created. This should work.\n",67,ConsoleOutput);
	DestroyLock(lockID);
	Halt();
	
}
