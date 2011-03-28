#include "syscall.h"
#include "utils.h"

int main() {
	int i;
	char* name;
	for (i = 0; i < 6; i++) {
		name = "../test/matmult";
		Exec(name, strlen(name));
	}
}