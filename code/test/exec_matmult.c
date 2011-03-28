#include "syscall.h"
#include "utils.h"

int main() {
	for (int i = 0; i < 9; i++) {
		char* name = "../test/matmult";
		Exec(name, strlen(name));
	}
}