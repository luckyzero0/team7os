/*forkTest.c
Test for forking a thread. If successful, x = 10*/

#include "syscall.h"

int x = 0;
void testProc();
int main(){

	/*Fork a thread*/
	Fork(testProc);
	
	/*fork another thread*/
	Fork(testProc);

  return 0;
}


void testProc(){
	write("Entering testProc",17);
	x+=5;
}
	
