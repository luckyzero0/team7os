/* exec_office.c */
/* 
 * Execs 4 office programs to run simultaneously to 
 * test multiprogramming.
 */


#include "utils.h"
#include "syscall.h"

int main() {
	int howMany;
	int i;

	howMany = 4;
	for (i = 0; i < howMany; i++) {
		Exec("../test/officeMain", 18);
	}

	Exit(0);
}