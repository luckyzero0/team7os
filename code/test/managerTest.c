/*ManagerTest.c*/
/*
 * Test to ensure that the
 * Manager does not check multiple clerks
 * at the same time. 2 of each clerk are forked,
 * and their cash values set to 500 each, except
 * the cashier, who has 0. This is to trigger
 * the managers cashChecking routine, which executes
 * when cashier's cash == numCustomers*100. No customers
 * are forked, so the value must be 0.
 */
#include "office.h"
#include "syscall.h"
#include "utils.h"
int main(){
	TESTING = TRUE;
	OfficeTest2();
	Exit(0);

}
