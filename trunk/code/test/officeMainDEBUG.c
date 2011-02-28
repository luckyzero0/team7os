/*officeMainDEBUG.c*/
/*
 * Runs the main office program with testing flag = true;
 */

#include "utils.h"
#include "syscall.h"
#include "office.h"

int main(){
	TESTING = TRUE;
	Fork(Office);
	Exit(0);
}
