/*officeMain.c*/
/*
 * Runs the main office program.
 */

#include "utils.h"
#include "syscall.h"
#include "office.h"

int main(){
	Fork(Office);
	Exit(0);
}
