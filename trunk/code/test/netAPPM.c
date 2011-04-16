#include "officeData.h"

int main() {
	int i;

	for (i = 0; i < NUM_OF_EACH_TYPE_OF_CLERK; i++) {
		Exec("../test/netAppClerk", 19);
		Exec("../test/netPicClerk", 19);
		Exec("../test/netPassClerk", 20);
	}

	Exec("../test/netManager", 18);
}