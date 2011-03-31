#include "syscall.h"
#include "utils.h"

int main() {
	LockID lock = CreateLock("lock");
	ConditionID condition = CreateCondition("condition");

	Acquire(lock);
	printf("Acquired lock in mc_signal.\n");
	Signal(condition, lock);
	printf("Signaled condition in mc_signal.\n");
	Release(lock);

	DestroyCondition(condition, lock);
	DestroyLock(lock);

	printf("Destroyed both in mc_signal.\n");

	Exit(0);
}