/* exec_office.c */

#include "utils.h"
#include "syscall.h"

int main() {
	int howMany;

	howMany = 2;
	for (int i = 0; i < howMany; i++) {
		Exec("../test/office", 14);
	}

	Exit(0);
}