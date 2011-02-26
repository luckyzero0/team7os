#include "syscall.h"
#include "utils.h"

int main() {
	char* name = "../test/testLocks";
	printf("###Exec started calling!\n", 0,0,0,"","");
	Exec(name, strlen(name));
	printf("###Exec finished calling!\n", 0,0,0,"","");
/*	Exit(0); */
	while(1) name = name;
}