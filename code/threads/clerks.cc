// File for the Application, Picture, Passport and Cashier Clerks
#include "system.h" 
#include "thread.h"
#include "office.h"
#include <stdio.h>

void AppClerkRun(int index){
  	while (true){
	  //  printf("AppClerk %d: has acquired the appPicLineLock\n", index);
		appPicLineLock->Acquire();
		
		if (privAppLineLength>0){
		 
	        printf("AppClerk %d: has spotted customer in privAppLine(length = %d)\n",index,privAppLineLength);
			privAppLineLength--;
			printf("AppClerk %d: Becoming Available!\n",index);
			appClerkStatuses[index] = CLERK_AVAILABLE;
			printf("AppClerk %d: Signaling Condition variable (length of priv line is now %d)\n", index,privAppLineLength);
			privAppLineCV->Signal(appPicLineLock);
			
		}
		else if (regAppLineLength>0){
		    printf("AppClerk %d: has spotted customer in regAppLine (length = %d)\n",index, regAppLineLength);
			regAppLineLength--;
			printf("AppClerk %d: Becoming Available!\n",index);
			appClerkStatuses[index] = CLERK_AVAILABLE;
			printf("AppClerk %d: Signaling Condition variable (length of reg line is now %d)\n", index,privAppLineLength);
			regAppLineCV->Signal(appPicLineLock);
			
		}
		/*	else{
		  printf("AppClerk %d: Going on Break!", index);
			appClerkStatuses[index] = CLERK_ON_BREAK;
			}*/
		//	printf("AppClerk %d: Releasing Lock", index);
		appPicLineLock->Release();
		currentThread->Yield();
      	}
		
		
		
}

void PicClerkRun(int index){
  	while (true){
		appPicLineLock->Acquire();
		if (privPicLineLength>0){
		 printf("PicClerk %d: has spotted customer in privPicLine(length = %d)\n",index,privPicLineLength);
			privPicLineLength--;
			printf("PicClerk %d: Becoming Available!\n",index);
			picClerkStatuses[index] = CLERK_AVAILABLE;
			printf("PicClerk %d: Signaling Condition variable (length of priv line is now %d)\n", index,privPicLineLength);
			privPicLineCV->Signal(appPicLineLock);
			
		}
		else if (regPicLineLength>0){
			printf("PicClerk %d: has spotted customer in regPicLine (length = %d)\n",index, regPicLineLength);
			regPicLineLength--;
			printf("PicClerk %d: Becoming Available!\n",index);
			picClerkStatuses[index] = CLERK_AVAILABLE;
			printf("PicClerk %d: Signaling Condition variable (length of reg line is now %d)\n", index,privPicLineLength);
			regPicLineCV->Signal(appPicLineLock);			
		}
		/*else{
			printf("PicClerk %d: Going on Break!", index);
			picClerkStatuses[index] = CLERK_ON_BREAK;
		}*/
		appPicLineLock->Release();
		currentThread->Yield();
	}
}
