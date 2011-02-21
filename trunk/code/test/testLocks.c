//Quick test program to check if lock related SYSCALLS are working.


#include "syscall.h"
int lockID;

int main()
{
	lockID = CreateLock("Lock1",5);
	Acquire(lockID+1); //Should give error
	Acquire(lockID);
	Release(lockID+1); //Should give error
	Release(lockID);
	DestroyLock(lockID+1); //Should give error
	DestroyLock(lockID);
	Halt();

}