/* sort.c 
*    Test program to sort a large number of integers.
*
*    Intention is to stress virtual memory system.
*
*    Ideally, we could read the unsorted array off of the file system,
*	and store the result back to the file system!
*/

#include "syscall.h"
#include "utils.h"

int A[1024];	/* size of physical memory; with code, we'll run out of space!*/

int
	main()
{
	int i, j, tmp;
	char* msg = "Testing a basic write.\n";
	Write(msg, strlen(msg), ConsoleOutput);
	msg = "About to initialize the array.\n";
	Write(msg, strlen(msg), ConsoleOutput);
	/* first initialize the array, in reverse sorted order */
	for (i = 0; i < 1024; i++)	{	
		A[i] = 1024 - i;
	}
	msg = "Initialized the array.\n";
	Write(msg, strlen(msg), ConsoleOutput);
	/*	printf("Initialized the array.\n", 0,0,0,"","");*/
	/* then sort! */
	for (i = 0; i < 1023; i++) {
		/*msg = "In the top loop!\n";
		Write(msg, strlen(msg), ConsoleOutput);*/
		for (j = i; j < (1023 - i); j++) {
			if (A[j] > A[j + 1]) {	
				tmp = A[j];
				A[j] = A[j + 1];
				A[j + 1] = tmp;
			}
		}
	}

	Write("A", 1, ConsoleOutput);
	Halt();
	/*Exit(A[0]);		*/
}
