/*managerClerkTest.c*/
/* This test shows that the manager
 * wakes up clerks if they have people in their lines.
 * This is simulated by forking customers before clerks.
 * The forked ManagerClerkTest function sets the line lengths
 * to test values, but no customers are forked. The program
 * will exit after all clerks wake up.
 */


#include "office.h"
#include "syscall.h"
#include "utils.h"

int main(){
	TESTING = TRUE;	
	OfficeTest5();
	Exit(0);	

}

