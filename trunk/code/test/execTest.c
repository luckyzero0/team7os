/*execTest.c*/
#include "syscall.h"
#include "utils.h"

char* file;
int main(){
	
	printf("Trying to exec a non-existent file...\n",0,0,0,"","");	
	file = "../test/BADFILENAMELOLZ";
	Exec(file,strlen(file));
	
	printf("Trying to exec a non-binary file...\n",0,0,0,"","");
	file = "../test/testLocks.c";
	Exec(file, strlen(file));
	
	printf("Trying to exec with bad strLength...\n",0,0,0,"","");
	file = "../test/forkTest";
	Exec(file,2);
	
	
	printf("It isn't enough, we need to go deeper.\n",0,0,0,"","");
	file = "../test/exec_testLocks";
	Exec(file,strlen(file));
	
	printf("And finally a normal Exec.\n",0,0,0,"","");
	file = "../test/forkTest";
	Exec(file,strlen(file));
	
	printf("All Exec() tests executed.\n",0,0,0,"","");
	
	Exit(0);
}


