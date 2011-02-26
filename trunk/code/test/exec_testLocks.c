#include "syscall.h"
#include "utils.h"

int main() {
	char* name = "../test/testLocks";
	Exec(name, strlen(name));
	printf("Exec finished calling!\n", 0,0,0,"","");
	Exit(0);
}