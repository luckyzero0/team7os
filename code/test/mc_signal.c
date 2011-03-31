#include "syscall.h"
#include "utils.h"

int main() {
	LockID lock = CreateLock("lock", 4);
	ConditionID condition = CreateCondition("condition", 9);

	Acquire(lock);
	printf("Acquired lock in mc_signal.\n", 0, 0,0,"","");
	Signal(condition, lock);
	printf("Signaled condition in mc_signal.\n", 0, 0,0,"","");
	Release(lock);

	DestroyCondition(condition, lock);
	DestroyLock(lock);

	printf("Destroyed both in mc_signal.\n", 0, 0,0,"","");

	Exit(0);
}