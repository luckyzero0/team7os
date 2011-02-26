#include "syscall.h"
#include "utils.h"

int main() {
	Exec("../test/testLocks", 17);
	while(1);
}