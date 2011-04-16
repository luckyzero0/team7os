#include "officeData.h"

int main() {
	int i;

	for (i = 0; i < NUM_OF_EACH_TYPE_OF_CLERK; i++) {
		Exec("../test/netCashClerk", 20);
	}

	for (i = 0; i < NUM_SENATORS; i++) {
		Exec("../test/netSenator", 18);
	}
}