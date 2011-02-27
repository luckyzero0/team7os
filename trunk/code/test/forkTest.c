/*forkTest.c
Test for forking a thread. If successful, x = 10*/

#include "syscall.h"
#include "utils.h"

int x = 0;
int i = 0;
int null = 0;
int bad = 3286987;
int unaligned = 13;
void testProc();
int main(){


	/*Fork a thread*/	
	printf("Forking thread 1...\n",0,0,0,"","");
	Fork(testProc);

	/*fork another thread*/
	printf("Forking thread 2...\n",0,0,0,"","");	
	Fork(testProc);					
		
	printf("Trying to pass bad(out of bounds) function pointers...\n",0,0,0,"","");
	Fork(bad);
	Fork(-1);
	
	printf("Fork bombing the system...\n",0,0,0,"","");
	Fork(null);
	
	printf("Trying to pass an unaligned function address...\n",0,0,0,"","");
	Fork(unaligned);

	printf("WHAT THE FUCK!?",0,0,0,"","");
	printf("Trying to pass generically bad fnAddresses from 0 to %d...\n",testProc,0,0,"","");
	while(i < testProc)
	{
		printf("Trying to pass generically bad function addresses, this one is %d...\n",i,0,0,"","");
		Fork(i);
		i++;
	}

	Exit(0);
}


void testProc(){
	printf("Entering test proc with x = %d.\n", x, 0, 0, "", "");
	x+=5;
	printf("Exiting test proc with x = %d.\n", x, 0, 0, "", "");
	Exit(0);
}

