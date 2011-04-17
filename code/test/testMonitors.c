/*TestMonitors.c*/

#include "syscall.h"
#include "utils.h"

int main(){
	int myMVArr;
	int result;
	
	printf("Creating a new monitor array variable...\n",0,0,0,"","");
	myMVArr = CreateMonitorArray("myMV",4,10,0);
	
	printf("Setting myMV[1] = 9001\n",0,0,0,"","");
	SetMonitorArrayValue(myMVArr,1,9001);
	
	
	printf("Getting myMV...\n",0,0,0,"","");	
	printf("MyMV[1] = [%d]\n",GetMonitorArrayValue(myMVArr,1),0,0,"","");
	
	printf("Done!\n",0,0,0,"","");
	
	Exit(0);
}
