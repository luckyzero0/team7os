#include "syscall.h"
#include "utils.h"

int main() {
	int i;
	char* msg;
	msg = "../test/testLocks";
	for (i = 0; i < 9; i++) {
		Exec(msg, strlen(msg));
	}
	Exit(0);
}