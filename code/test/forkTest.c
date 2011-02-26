/*forkTest.c
Test for forking a thread. If successful, x = 10*/

#include "syscall.h"
#include "utils.h"

int x = 0;
void testProc();
int main(){


	/*Fork a thread*/	
	printf("Forking thread 1...\n",0,0,0,"","");
	Fork(testProc);

	/*fork another thread*/
	printf("Forking thread 2...\n",0,0,0,"","");	
	Fork(testProc);			

	Exit(0);
}


void testProc(){
	printf("Entering test proc with x = %d.\n", x, 0, 0, "", "");
	x+=5;
	printf("Entering test proc with x = %d.\n", x, 0, 0, "", "");
	Exit(0);
}

