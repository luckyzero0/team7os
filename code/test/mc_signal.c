#include "syscall.h"
#include "utils.h"

int main() {
	LockID lock = CreateLock("lock", 4);
	ConditionID condition = CreateCondition("condition", 9);

	Acquire(lock);
	printf("Acquired lock[%d] in mc_signal.\n", lock, 0,0,"","");
	Signal(condition, lock);
	printf("Signaled condition[%d] with lock[%d] in mc_signal.\n", lock, condition,0,"","");	

	/*DestroyCondition(condition);
	DestroyLock(lock);*/

	Release(lock);

	printf("Destroyed both in mc_signal.\n", 0, 0,0,"","");

	Exit(0);
}