/* forkArg.c */
#include "utils.h"
#include "syscall.h"

void threadWithArg1() {
	int arg;

	arg = GetForkArg();

	printf("threadWithArg1 has an arg of %d. ", arg, 0,0,"","");
	if (arg == 1) {
		printf("SUCCESS!\n",0,0,0,"","");
	} else {
		printf("FAILURE!\n",0,0,0,"","");
	}

	Exit(0);
}

void threadWithArg22() {
	int arg;

	arg = GetForkArg();

	printf("threadWithArg22 has an arg of %d. ", arg, 0,0,"","");
	if (arg == 22) {
		printf("SUCCESS!\n",0,0,0,"","");
	} else {
		printf("FAILURE!\n",0,0,0,"","");
	}

	Exit(0);
}

void threadWithNoArg() {
	int arg;

	arg = GetForkArg();

	printf("threadWithArg22 has an undefined arg, and the GetForkArg gives back a %d.\n", arg, 0,0,"","");
	Exit(0);
}

int main() {
	printf("Doing some basic testing of ForkWithArg.\n", 0,0,0,"","");
	printf("Forking a thread with arg = 1\n", 0,0,0,"","");
	ForkWithArg(threadWithArg1, 1);

	printf("Forking a thread with arg = 22\n", 0,0,0,"","");
	ForkWithArg(threadWithArg22, 22);

	printf("Forking a thread without an arg, arg in there is defaulted to -2 (no real reason).\n", 0,0,0,"","");
	Fork(threadWithNoArg);

	Exit(0);
}

