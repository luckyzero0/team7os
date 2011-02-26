#include "syscall.h"
#include "utils.h"

int main() {
	char* name = "../test/testLocks";
	Exec(name, strlen(name));
	Exit(0);
}