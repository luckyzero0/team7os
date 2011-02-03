// File for the Application, Picture, 

#include "office.h"
#include <stdio.h>

void AppClerkRun(int index){
	while (true){
	        printf("\nAppClerk %d: has acquired the appPicLineLock", index);
		appPicLineLock->Acquire();
		
		if (privAppLineLength>0){
		 
	                printf("\nAppClerk %d: has spotted customer in privAppLine(length = %d)",index,privAppLineLength);
			privAppLineLength--;
			printf("\nAppClerk %d: Becoming Available!",index);
			appClerkStatuses[index] = CLERK_AVAILABLE;
			printf("\nAppClerk %d: Signaling Condition variable (length of priv line is now %d)", index,privAppLineLength);
			privAppLineCV->Signal(appPicLineLock);
			
		}
		else if (regAppLineLength>0){
		        printf("\nAppClerk %d: has spotted customer in regAppLine (length = %d)",index, regAppLineLength);
			regAppLineLength--;
			printf("\nAppClerk %d: Becoming Available!",index);
			appClerkStatuses[index] = CLERK_AVAILABLE;
			printf("\nAppClerk %d: Signaling Condition variable (length of reg line is now %d)", index,privAppLineLength);
			regAppLineCV->Signal(appPicLineLock);
			
		}
		else{
		  printf("AppClerk %d: Going on Break!", index);
			appClerkStatuses[index] = CLERK_ON_BREAK;
		}
		printf("AppClerk %d: Releasing Lock", index);
		appPicLineLock->Release();
	}
		
		
		
}

void PicClerkRun(int index){
	while (true){
		appPicLineLock->Acquire();
		if (privPicLineLength>0){
			privPicLineLength--;
			picClerkStatuses[index] = CLERK_AVAILABLE;
			privPicLineCV->Signal(appPicLineLock);
			
		}
		else if (regAppLineLength>0){
			regPicLineLength--;
			picClerkStatuses[index] = CLERK_AVAILABLE;
			regPicLineCV->Signal(appPicLineLock);			
		}
		else{
			picClerkStatuses[index] = CLERK_ON_BREAK;
		}
		picPicLineLock->Release();
	}
}
