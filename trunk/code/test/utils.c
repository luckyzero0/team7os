/*utils.c */
#include "syscall.h"
int strlen(char* str){

	int i = 0;
	while(str[i] != '\0')
		i++;

	return i;
}

/*printf-ish function, but without VA list*/
void print(char* str, int arg1, int arg2, int arg3, char* arg4, char* arg5){
	char out[100];	
	char* dmsg;
	int i = 0;
	int intArg = 0;
	int x;
	int outIndex = 0;
	
	char arg1buf[12];
	char arg2buf[12];
	char arg3buf[12];
	
	itoa(arg1, arg1buf);
	itoa(arg2, arg2buf);
	itoa(arg3, arg3buf);
	
	while(str[i] != '\0')
	{
		if(str[i] == '%')
		{
			if(str[i+1] == 'd')
			{
				intArg++;
				if(intArg == 1)
				{
					for(x = 0; x < strlen(arg1buf); x++)
					{
						out[outIndex] = arg1buf[x];
						outIndex++;
					}
					i+=2;
				}
			}
		}
		else
		{
			out[outIndex] = str[i];
			outIndex++;
			i++;
		}
	}
	
	Write(out,strlen(out),ConsoleOutput);
	
	
}

void itoa(int n, char s[])
{
     int i, sign;
 
     if ((sign = n) < 0)  /* record sign */
         n = -n;          /* make n positive */
     i = 0;
     do {       /* generate digits in reverse order */
         s[i++] = n % 10 + '0';   /* get next digit */
     } while ((n /= 10) > 0);     /* delete it */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);     
}

/* reverse:  reverse string s in place */
void reverse(char s[])
{
     int i, j;
     char c;
 
     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }
