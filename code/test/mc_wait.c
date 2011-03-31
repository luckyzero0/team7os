#include "syscall.h"
#include "utils.h"

int main() {
	LockID lock = CreateLock("lock", 4);
	ConditionID condition = CreateCondition("condition", 9);

	Acquire(lock);
	printf("Acquired lock in mc_wait.\n", 0, 0,0,"","");
	Wait(condition, lock);
	printf("Stopped waiting on condition in mc_wait.\n", 0, 0,0,"","");
	Release(lock);

	DestroyCondition(condition, lock);
	DestroyLock(lock);

	printf("Destroyed both in mc_wait.\n", 0, 0,0,"","");

	Exit(0);
}