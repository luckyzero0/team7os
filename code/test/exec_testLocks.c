#include "syscall.h"
#include "utils.h"

int main() {
	Exec("../test/testLocks", 17);
	Exit(0);
}