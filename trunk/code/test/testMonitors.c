/*TestMonitors.c*/

#include "syscall.h"
#include "utils.h"

int main(){
	int myMV;
	int result;
	
	printf("Creating a new monitor variable...\n",0,0,0,"","");
	myMV = CreateMonitor("myMV",4);
	
	printf("Setting myMV = 9001\n",0,0,0,"","");
	SetMonitor(myMV,9001);
	
	
	printf("Getting myMV...\n",0,0,0,"","");	
	printf("MyMV = [%d]\n",GetMonitor(myMV),0,0,"","");
	
	printf("Done!\n",0,0,0,"","");
	
	Exit(0);
}
