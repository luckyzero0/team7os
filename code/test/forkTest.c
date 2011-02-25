/*forkTest.c
Test for forking a thread. If successful, x = 10*/

#include "syscall.h"
#include "utils.h"
int x = 0;
void testProc();
int main(){
	
	
	/*Fork a thread*/
	char* msg = "Forking thread 1...\n";
	Write(msg,strlen(msg),ConsoleOutput);
	print("Also testing a print %d suckas!\n", 49,0,0,"s","t");
	
	Fork(testProc);
	
	/*fork another thread*/
	msg = "Forking thread 2...\n";
	Write(msg,strlen(msg),ConsoleOutput);
	Fork(testProc);
		

  return 0;
}


void testProc(){
	Write("Entering testProc",17, ConsoleOutput);
	x+=5;
}
	
