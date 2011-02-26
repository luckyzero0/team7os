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
  /*  int i, j, tmp;*/
	Write("Testing a basic write.\n", 25, ConsoleOutput);
	printf("About to initialize the array.\n", 0,0,0, "", "");
    /* first initialize the array, in reverse sorted order */
  /*  for (i = 0; i < 1024; i++)		
        A[i] = 1024 - i;

	printf("Initialized the array.\n", 0,0,0,"","");*/
    /* then sort! */
  /*  for (i = 0; i < 1023; i++)
        for (j = i; j < (1023 - i); j++)
	   if (A[j] > A[j + 1]) {	
	      tmp = A[j];
	      A[j] = A[j + 1];
	      A[j + 1] = tmp;
    	   }*/
    Exit(A[0]);		
}
