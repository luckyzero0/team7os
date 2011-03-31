#include "syscall.h"
#include "utils.h"

int main() {
	LockID lock = CreateLock("lock");
	ConditionID condition = CreateCondition("condition");

	Acquire(lock);
	printf("Acquired lock in mc_wait.\n");
	Wait(condition, lock);
	printf("Stopped waiting on condition in mc_wait.\n");
	Release(lock);

	DestroyCondition(condition, lock);
	DestroyLock(lock);

	printf("Destroyed both in mc_wait.\n");

	Exit(0);
}