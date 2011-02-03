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
			printf("AppClerk %d: Acquiring my own lock\n",index);
			appClerkLocks[index]->Acquire();
			printf("AppClerk %d: Releasing appPicLineLock\n",index);
			appPicLineLock->Release();
			printf("AppClerk %d: Putting myself to sleep...\n",index);
			appClerkCVs[index]->Wait(appClerkLocks[index]);
			printf("AppClerk %d: Just woke up!\n",index);
			int SSN = appClerkData[index];
			printf("AppClerk %d: Just receieved Customer's SSN: %d\n",index, SSN);
			printf("AppClerk %d: Signaling my appClerkCV\n", index);
			appClerkCVs[index]->Signal(appClerkLocks[index]);
			printf("AppClerk %d: Releasing my own lock\n", index);
			appClerkLocks[index]->Release();
			
		}
		else if (regAppLineLength>0){
		    printf("AppClerk %d: has spotted customer in regAppLine (length = %d)\n",index, regAppLineLength);
			regAppLineLength--;
			printf("AppClerk %d: Becoming Available!\n",index);
			appClerkStatuses[index] = CLERK_AVAILABLE;
			printf("AppClerk %d: Signaling Condition variable (length of priv line is now %d)\n", index,privAppLineLength);
			privAppLineCV->Signal(appPicLineLock);
			printf("AppClerk %d: Acquiring my own lock\n",index);
			appClerkLocks[index]->Acquire();
			printf("AppClerk %d: Releasing appPicLineLock\n",index);
			appPicLineLock->Release();
			printf("AppClerk %d: Putting myself to sleep...\n",index);
			appClerkCVs[index]->Wait(appClerkLocks[index]);
			printf("AppClerk %d: Just woke up!\n",index);
			int SSN = appClerkData[index];
			printf("AppClerk %d: Just receieved Customer's SSN: %d\n",index, SSN);
			printf("AppClerk %d: Signaling my appClerkCV\n", index);
			appClerkCVs[index]->Signal(appClerkLocks[index]);
			printf("AppClerk %d: Releasing my own lock\n", index);
			appClerkLocks[index]->Release();
					
			
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
			picClerkLocks[index]->Acquire();
			appPicLineLock->Release();
			picClerkCVs[index]->Wait(picClerkLocks[index]);
			
		}
		else if (regPicLineLength>0){
			printf("PicClerk %d: has spotted customer in regPicLine (length = %d)\n",index, regPicLineLength);
			regPicLineLength--;
			printf("PicClerk %d: Becoming Available!\n",index);
			picClerkStatuses[index] = CLERK_AVAILABLE;
			printf("PicClerk %d: Signaling Condition variable (length of reg line is now %d)\n", index,privPicLineLength);
			regPicLineCV->Signal(appPicLineLock);
			picClerkLocks[index]->Acquire();
			appPicLineLock->Release();
			picClerkCVs[index]->Wait(picClerkLocks[index]);
		}
		/*else{
			printf("PicClerk %d: Going on Break!", index);
			picClerkStatuses[index] = CLERK_ON_BREAK;
		}*/
		appPicLineLock->Release();
		currentThread->Yield();
	}
}
