/*ManagerTest.c*/
/*
 * Test to ensure that the
 * Manager does not check multiple clerks
 * at the same time. (??)
 */
#include "office.h"
#include "syscall.h"
#include "utils.h"
int main(){
	TESTING = TRUE;
	OfficeTest2();
	Exit(0);

}
