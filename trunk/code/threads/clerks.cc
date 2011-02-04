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
				privAppLineCV->Signal(appPicLineLock);
			}
			else{ //Check if anyone is in reg line
				printf("AppClerk %d: has spotted customer in regAppLine (length = %d)\n",index, regAppLineLength);
				regAppLineLength--;
				printf("AppClerk %d: Becoming Available!\n",index);
				appClerkStatuses[index] = CLERK_AVAILABLE;
				printf("AppClerk %d: Signaling Condition variable (length of reg line is now %d)\n", index,regAppLineLength);
				regAppLineCV->Signal(appPicLineLock);
			}
			//Customer - Clerk interaction

			printf("AppClerk %d: Acquiring my own lock\n",index);
			appClerkLocks[index]->Acquire();
			printf("AppClerk %d: Releasing appPicLineLock\n",index);
			appPicLineLock->Release();
			printf("AppClerk %d: Putting myself to sleep...\n",index);
			appClerkCVs[index]->Wait(appClerkLocks[index]);
			printf("AppClerk %d: Just woke up!\n",index);
			int SSN = appClerkSSNs[index];
			printf("AppClerk %d: Just receieved Customer's SSN: %d\n",index, SSN);
			appFiled[SSN] = TRUE; //***********NEEDS TO BE FORKED IN THE FUTURE***********************
			printf("AppClerk %d: Signaling my appClerkCV\n", index);
			appClerkCVs[index]->Signal(appClerkLocks[index]);
			printf("AppClerk %d: Releasing my own lock\n", index);

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

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------

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
				privPicLineCV->Signal(appPicLineLock);
			}
			else{ //Checking if anyone is in reg line
				printf("PicClerk %d: has spotted customer in regPicLine (length = %d)\n",index, regPicLineLength);
				regPicLineLength--;
				printf("PicClerk %d: Becoming Available!\n",index);
				picClerkStatuses[index] = CLERK_AVAILABLE;
				printf("PicClerk %d: Signaling Condition variable (length of reg line is now %d)\n", index,regPicLineLength);
				regPicLineCV->Signal(appPicLineLock);
			}

			//Customer - Clerk Interaction

			printf("PicClerk %d: Acquiring my own lock\n",index);
			picClerkLocks[index]->Acquire();
			printf("PicClerk %d: Releasing appPicLineLock\n",index);
			appPicLineLock->Release();

			printf("PicClerk %d: Putting myself to sleep...\n",index);
			picClerkCVs[index]->Wait(picClerkLocks[index]);
			int count = 1;
			do{
				printf("PicClerk %d: Taking picture of customer for the %dst/nd/rd/th time (Signalling my CV)!\n",index, count);
				picClerkCVs[index]->Signal(picClerkLocks[index]);
				printf("PicClerk %d: Going to sleep...\n",index);
				picClerkCVs[index]->Wait(picClerkLocks[index]);

				if(happyWithPhoto[index] == TRUE){
					int SSN = picClerkSSNs[index];
					if (SSN >= 6) { // HACK, REMOVE LATER
					  printf("SSN >= 6!  Cannot BE SO! FAIL!");
					  exit(0);
					}
					picFiled[SSN] = TRUE;  //**********NEEDS TO BE FORKED IN THE FUTURE***********************
					for (int i=0; i<10; i++){
						printf("AppFiled: %d,    PicFiled: %d\n",appFiled[i],picFiled[i]);
					}
					printf("PicClerk %d: Just woke up, Customer with SSN %d liked their picture!\n",index, SSN);
				}
				else{
					printf("PicClerk %d: Just woke up, Customer did not like their picture. Taking picture again.\n",index);
				}

				count++;

			}while(happyWithPhoto[index] == FALSE);

			picClerkCVs[index]->Signal(picClerkLocks[index]);
			printf("PicClerk %d: Releasing my own lock\n", index);
			picClerkLocks[index]->Release();

		}

		else{ //No one in line
			appPicLineLock->Release();

			if(picClerkStatuses[index]!=CLERK_ON_BREAK){
				printf("PicClerk %d: Going on Break!\n", index);
				picClerkStatuses[index] = CLERK_ON_BREAK;
			}

		}

		currentThread->Yield();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------

void PassClerkRun(int index){
	while (true){
		//  printf("PassClerk %d: has acquired the passLineLock\n", index);
		passLineLock->Acquire();

		//Checking if anyone is in line
		if (privPassLineLength+regPassLineLength>0){

			if (privPassLineLength>0){ //Checking if anyone is in priv line
				printf("PassClerk %d: has spotted customer in privPassLine(length = %d)\n",index,privPassLineLength);
				privPassLineLength--;
				printf("PassClerk %d: Becoming Available!\n",index);
				passClerkStatuses[index] = CLERK_AVAILABLE;
				printf("PassClerk %d: Signaling Condition variable (length of priv line is now %d)\n", index,privPassLineLength);
				privPassLineCV->Signal(passLineLock);
			}
			else{ //Check if anyone is in reg line
				printf("PassClerk %d: has spotted customer in regPassLine (length = %d)\n",index, regPassLineLength);
				regPassLineLength--;
				printf("PassClerk %d: Becoming Available!\n",index);
				passClerkStatuses[index] = CLERK_AVAILABLE;
				printf("PassClerk %d: Signaling Condition variable (length of reg line is now %d)\n", index,regPassLineLength);
				regPassLineCV->Signal(passLineLock);
			}
			//Customer - Clerk interaction

			printf("PassClerk %d: Acquiring my own lock\n",index);
			passClerkLocks[index]->Acquire();
			printf("PassClerk %d: Releasing passLineLock\n",index);
			passLineLock->Release();
			printf("PassClerk %d: Putting myself to sleep...\n",index);
			passClerkCVs[index]->Wait(passClerkLocks[index]);
			printf("PassClerk %d: Just woke up!\n",index);
			int SSN = passClerkSSNs[index];
			if(appFiled[SSN] == FALSE || picFiled[SSN] == FALSE){
				for (int i=0; i<10; i++){
					printf("AppFiled: %d,    PicFiled: %d\n",appFiled[i],picFiled[i]);
				}
				printf("PassClerk %d: Customer with SSN %d does not have both picture and application filed! *SPANK*\n", index, SSN);
				passPunish[SSN] = TRUE;
			}
			else{
				printf("PassClerk %d: Customer with SSN %d has everything filed correctly!\n",index, SSN);
				passPunish[SSN] = FALSE;
				passFiled[SSN] = TRUE; //**********THIS SHOULD BE FORKED IN THE FUTURE*****************
			}

			//printf("PassClerk %d: Just receieved Customer's SSN: %d\n",index, SSN);
			printf("PassClerk %d: Signaling my passClerkCV\n", index);
			passClerkCVs[index]->Signal(passClerkLocks[index]);
			printf("PassClerk %d: Releasing my own lock\n", index);
			passClerkLocks[index]->Release();

		}
		else{ //No one in line

			passLineLock->Release();

			if(passClerkStatuses[index]!=CLERK_ON_BREAK){
				printf("PassClerk %d: Going on Break!\n", index);
				passClerkStatuses[index] = CLERK_ON_BREAK;
			}
		}


		currentThread->Yield();
	}



}
