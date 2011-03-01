/*customerCashiertest.c*/
/*
 *	Test to demonstrate customers entering and leaving the passport office,
 *  but only interactions with the cashier clerk, namely that they only leave once
 *  they received their passport, and that the cashclerk only interacts with them
 *  one at a time.
*/
#include "office.h"
#include "syscall.h"
#include "utils.h"

int main(){
	TESTING = TRUE;	
	OfficeTest3();
	Exit(0);	

}
