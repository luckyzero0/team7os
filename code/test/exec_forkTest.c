#include "syscall.h"
#include "utils.h"

int main() {
	int i;
	char* name;
	for (i = 0; i < 2; i++) {
		name = "../test/forkTest";
		Exec(name, strlen(name));
	}
	Exit(0);
}