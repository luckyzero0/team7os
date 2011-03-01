/*customerCashiertest.c*/
/*Checks to make sure customers make it through the office,
 * but not until the cashier approves their passport.
 */
#include "office.h"
#include "syscall.h"
#include "utils.h"

int main(){
	TESTING = TRUE;
	needManager = FALSE;
	OfficeTest3();
	Exit(0);	

}
