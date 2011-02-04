// File for the Application, Picture, Passport and Cashier Clerks
#include "system.h" 
#include "thread.h"
#include "office.h"
#include <stdio.h>

void AppClerkRun(int index){
	while (true){
		//  printf("AppClerk %d: has acquired the appPicLineLock\n", index);
		appPicLineLock->Acquire();

		//Checking if anyone is in line
		if (privAppLineLength+regAppLineLength>0){

			if (privAppLineLength>0){ //Checking if anyone is in priv line
				printf("AppClerk %d: has spotted customer in privAppLine(length = %d)\n",index,privAppLineLength);
				privAppLineLength--;
				printf("AppClerk %d: Becoming Available!\n",index);
				appClerkStatuses[index] = CLERK_AVAILABLE;
				printf("AppClerk %d: Signaling Condition variable (length of priv line is now %d)\n", index,privAppLineLength);
			}
			else{ //Check if anyone is in reg line
				printf("AppClerk %d: has spotted customer in regAppLine (length = %d)\n",index, regAppLineLength);
				regAppLineLength--;
				printf("AppClerk %d: Becoming Available!\n",index);
				appClerkStatuses[index] = CLERK_AVAILABLE;
				printf("AppClerk %d: Signaling Condition variable (length of reg line is now %d)\n", index,regAppLineLength);
			}
			//Customer - Clerk interaction
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
		else{ //No one in line

			appPicLineLock->Release();

			if(appClerkStatuses[index]!=CLERK_ON_BREAK){
				printf("AppClerk %d: Going on Break!\n", index);
				appClerkStatuses[index] = CLERK_ON_BREAK;
			}
		}
		//	

		currentThread->Yield();
	}



}

void PicClerkRun(int index){
	while (true){
		appPicLineLock->Acquire();

		//Checking if anyone is line
		if(privPicLineLength+regPicLineLength>0){

			if (privPicLineLength>0){	//Checking if anyone is in priv line
				printf("PicClerk %d: has spotted customer in privPicLine(length = %d)\n",index,privPicLineLength);
				privPicLineLength--;
				printf("PicClerk %d: Becoming Available!\n",index);
				picClerkStatuses[index] = CLERK_AVAILABLE;
				printf("PicClerk %d: Signaling Condition variable (length of priv line is now %d)\n", index,privPicLineLength);
			}
			else{ //Checking if anyone is in reg line
				printf("PicClerk %d: has spotted customer in regPicLine (length = %d)\n",index, regPicLineLength);
				regPicLineLength--;
				printf("PicClerk %d: Becoming Available!\n",index);
				picClerkStatuses[index] = CLERK_AVAILABLE;
				printf("PicClerk %d: Signaling Condition variable (length of reg line is now %d)\n", index,regPicLineLength);
			}

			//Customer - Clerk Interaction
			privPicLineCV->Signal(appPicLineLock);
			printf("PicClerk %d: Acquiring my own lock\n",index);
			picClerkLocks[index]->Acquire();
			printf("PicClerk %d: Releasing picPicLineLock\n",index);
			appPicLineLock->Release();
			do{
				printf("PicClerk %d: Taking Customer's picture! Putting myself to sleep...\n",index);
				picClerkCVs[index]->Wait(picClerkLocks[index]);
				if(picClerkData[index] == TRUE)
					printf("PicClerk %d: Just woke up, Customer liked their picture!\n",index);
				else{
					printf("PicClerk %d: Just woke up, Customer did not like their picture. Taking picture again.\n",index);
					currentThread->Yield();
				}
			}while(picClerkData[index] == FALSE);

			printf("PicClerk %d: Signaling my picClerkCV\n", index);
			picClerkCVs[index]->Signal(picClerkLocks[index]);
			printf("PicClerk %d: Releasing my own lock\n", index);
			picClerkLocks[index]->Release();
		}

		else{ //No one in line
			appPicLineLock->Release();

			if(appClerkStatuses[index]!=CLERK_ON_BREAK){
				printf("AppClerk %d: Going on Break!\n", index);
				appClerkStatuses[index] = CLERK_ON_BREAK;
			}
		
		}

		currentThread->Yield();
	}
}
