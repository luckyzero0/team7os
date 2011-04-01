/*utils.h*/
/*
 *	These are some utility functions that we mostly used to 
 *	help facilitate testing/debugging, primarily by using
 *	a printf implementation, as stdlibs are not available
 *	in nachos' "special" flavor of C.	
*/

#ifndef UTILS
#define UTILS

/*
 *	Calculates the length of a char* by counting chars
 *	to the first '\0'
*/
extern int strlen(char* str);


/*
 *	Works pretty much the same as std::printf, but without a VA
 *	list, because they don't exist in Nachos-C
*/
extern void printf(char* str, int arg1, int arg2, int arg3, char* arg4, char* arg5);

/*
 *	itoa implementation borrowed from wikipedia. Does some funky
 *	modulus math to determine digits in a char[]. Used as a helper
 *	in printf to convert integer args into strings compatible
 *	with the Write syscall
*/
extern void itoa(int n, char s[]);

extern void strcpy(char dest[], char source[]);

extern void strcat(char dest[], char source[]);


/*
 *	reverse implementation borrowed from wikipedia. Reverses
 *	the characters in a char[]. Used as a helper in
 *	itoa, as the digits are determined in reverse order.
*/
extern void reverse(char s[]);

#endif UTILS