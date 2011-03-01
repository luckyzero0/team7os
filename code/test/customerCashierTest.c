/*customerCashiertest.c*/
/*
 *	Test to demonstrate customers entering and leaving the passport office,
 *  but only interactions with the cashier clerk, namely that they only leave once
 *  they received their passport, and that the cashclerk only interacts with them
 *  one at a time. Forks 3 customers to only do a routine to run the doCashClerk() 
 *	in customer, and sets valid = true for all of them, so they can pay the cashier
 * 	and leave.
*/
#include "office.h"
#include "syscall.h"
#include "utils.h"

int main(){
	TESTING = TRUE;	
	OfficeTest3();
	Exit(0);	

}
