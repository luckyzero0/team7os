// File for the Application, Picture, Passport and Cashier Clerks
#include "system.h" 
#include "thread.h"
#include "office.h"
#include <stdio.h>

char* getCustomerType(){
	int check=0;
	senatorOfficeLock->Acquire();
	if (senatorsInOffice>0){
		check = 1;
	}
	senatorOfficeLock->Release();
	if (check == 0){
		return "Customer";
	}else{
		return "Senator";
	}
}

void appClerkFileData(int SSN){
	int randomNum = rand()%80 + 20;
	for(int i = 0; i< randomNum; i++){
		currentThread->Yield();
	}
	appFiled[SSN] = TRUE;
	tprintf("ApplicationClerk informs %s with SSN %d that the application has been filed\n", getCustomerType(), SSN); //Not sure if I am to print this or not

}

void picClerkFileData(int SSN){
	int randomNum = rand()%80 + 20;
	for(int i = 0; i< randomNum; i++){
		currentThread->Yield();
	}
	picFiled[SSN] = TRUE;
	tprintf("PictureClerk informs %s with SSN %d that the picture has been filed\n", getCustomerType(), SSN); //Not sure if I am to print this or not

}

void passClerkFileData(int SSN){
	int randomNum = rand()%80 + 20;
	for(int i = 0; i< randomNum; i++){
		currentThread->Yield();
	}
	passFiled[SSN] = TRUE;
	tprintf("PassportClerk informs %s with SSN %d that the passport has been filed\n", getCustomerType(), SSN); //Not sure if I am to print this or not

}




void AppClerkRun(int index){
	while (true){
		tprintf("AppClerk %d: has acquired the appPicLineLock\n", index);
		appPicLineLock->Acquire();

		//Checking if anyone is in line
		if (privAppLineLength+regAppLineLength>0){

			if (privAppLineLength>0){ //Checking if anyone is in priv line
				tprintf("AppClerk %d: has spotted %s in privAppLine(length = %d)\n",index, getCustomerType(), privAppLineLength);
				privAppLineLength--;
				tprintf("AppClerk %d: Becoming Available!\n",index);
				appClerkStatuses[index] = CLERK_AVAILABLE;
				tprintf("AppClerk %d: Signaling Condition variable (length of priv line is now %d)\n", index,privAppLineLength);
				privAppLineCV->Signal(appPicLineLock);
			}
			else{ //Check if anyone is in reg line
				tprintf("AppClerk %d: has spotted %s in regAppLine (length = %d)\n",index, getCustomerType(),regAppLineLength);
				regAppLineLength--;
				tprintf("AppClerk %d: Becoming Available!\n",index);
				appClerkStatuses[index] = CLERK_AVAILABLE;
				tprintf("AppClerk %d: Signaling Condition variable (length of reg line is now %d)\n", index,regAppLineLength);
				regAppLineCV->Signal(appPicLineLock);
			}

			//Customer/Senator - Clerk interaction
			tprintf("AppClerk %d: Acquiring my own lock\n",index);
			appClerkLocks[index]->Acquire();
			tprintf("AppClerk %d: Releasing appPicLineLock\n",index);
			appPicLineLock->Release();
			tprintf("AppClerk %d: Putting myself to sleep...\n",index);
			appClerkCVs[index]->Wait(appClerkLocks[index]);
			tprintf("AppClerk %d: Just woke up!\n",index);
			
			int SSN = appClerkSSNs[index];
			tprintf("AppClerk %d: Just receieved %s's SSN: %d\n",index, getCustomerType(), SSN);

			if (appClerkBribed[index] == TRUE){
				//printf("AppClerk [%d]: I've just been bribed for $500! Total amount I've collected to so far: $%d\n", index, appClerkMoney[index]);
				printf("ApplicationClerk [%d] accepts money = 500 from %s with SSN %d\n", index, getCustomerType(), appClerkSSNs[index]);
				appClerkBribed[index] = FALSE;
			}

			printf("ApplicationClerk [%d] informs %s with SSN %d that the procedure has been completed.\n", index, getCustomerType(), SSN); //Am not sure if I am to print this out or not
			
		
			Thread* newThread = new Thread("Filing Thread");
			newThread->Fork((VoidFunctionPtr)appClerkFileData, SSN);
			for (int i=0; i<10; i++){
				tprintf("AppFiled: %d,    PicFiled: %d\n",appFiled[i],picFiled[i]);
			}
			tprintf("AppClerk %d: Signaling my appClerkCV\n", index);
			appClerkCVs[index]->Signal(appClerkLocks[index]);
			tprintf("AppClerk %d: Releasing my own lock\n", index);
			appClerkLocks[index]->Release();

		}
		else{ //No one in line
			appPicLineLock->Release();
			appClerkLocks[index]->Acquire();
			printf("ApplicationClerk [%d] is going on break\n", index);
			appClerkStatuses[index] = CLERK_ON_BREAK;
			appClerkCVs[index]->Wait(appClerkLocks[index]);
			printf("ApplicationClerk [%d] returned from break\n", index);
			appClerkLocks[index]->Release();
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
				tprintf("PicClerk %d: has spotted %s in privPicLine(length = %d)\n",index, getCustomerType(), privPicLineLength);
				privPicLineLength--;
				tprintf("PicClerk %d: Becoming Available!\n",index);
				picClerkStatuses[index] = CLERK_AVAILABLE;
				tprintf("PicClerk %d: Signaling Condition variable (length of priv line is now %d)\n", index,privPicLineLength);
				privPicLineCV->Signal(appPicLineLock);
			}
			else{ //Checking if anyone is in reg line
				tprintf("PicClerk %d: has spotted %s in regPicLine (length = %d)\n",index, getCustomerType(), regPicLineLength);
				regPicLineLength--;
				tprintf("PicClerk %d: Becoming Available!\n",index);
				picClerkStatuses[index] = CLERK_AVAILABLE;
				tprintf("PicClerk %d: Signaling Condition variable (length of reg line is now %d)\n", index,regPicLineLength);
				regPicLineCV->Signal(appPicLineLock);
			}

			//Customer/Senator - Clerk Interaction

			tprintf("PicClerk %d: Acquiring my own lock\n",index);
			picClerkLocks[index]->Acquire();
			tprintf("PicClerk %d: Releasing appPicLineLock\n",index);
			appPicLineLock->Release();

			tprintf("PicClerk %d: Putting myself to sleep...\n",index);
			picClerkCVs[index]->Wait(picClerkLocks[index]);
			int SSN = picClerkSSNs[index];
			if (picClerkBribed[index] == TRUE){
				//printf("PicClerk %d: I've just been bribed for $500! Total amount I've collected to so far: $%d\n", index, picClerkMoney[index]);
				printf("PictureClerk %d accepts money = 500 from %s with SSN %d\n", index, getCustomerType(), index);
				picClerkBribed[index] = FALSE;
			}
			int count = 1;
			do{
				if (count==1){
					printf("PictureClerk [%d] takes picture of %s with SSN %d\n", index, getCustomerType(), SSN);
				}else{
					printf("PictureClerk [%d] takes picture of %s with SSN %d again\n", index, getCustomerType(), SSN);
				}
				tprintf("PicClerk %d: Taking picture of %s for the %dst/nd/rd/th time (Signaling my CV)!\n",index, getCustomerType(), count);
				picClerkCVs[index]->Signal(picClerkLocks[index]);
				tprintf("PicClerk %d: Going to sleep...\n",index);
				picClerkCVs[index]->Wait(picClerkLocks[index]);

				if(happyWithPhoto[index] == TRUE){
					SSN = picClerkSSNs[index];
					//picFiled[SSN] = TRUE;  //**********NEEDS TO BE FORKED IN THE FUTURE***********************
					Thread* t = new Thread("Pass Filing Thread");
					t->Fork((VoidFunctionPtr)picClerkFileData, SSN);
					for (int i=0; i<10; i++){
						tprintf("AppFiled: %d,    PicFiled: %d\n",appFiled[i],picFiled[i]);
					}
					tprintf("PicClerk %d: Just woke up, %s with SSN %d liked their picture!\n",index, getCustomerType(), SSN);
				}
				else{
					tprintf("PicClerk %d: Just woke up, %s did not like their picture.\n",index, getCustomerType());
				}

				count++;

			}while(happyWithPhoto[index] == FALSE);
			printf("PictureClerk [%d] informs %s with SSN %d that the procedure has been completed\n", index, getCustomerType(), SSN); //Am not sure if I am to print this out or not

			picClerkCVs[index]->Signal(picClerkLocks[index]);
			tprintf("PicClerk %d: Releasing my own lock\n", index);
			picClerkLocks[index]->Release();
		}

		else{ //No one in line
			appPicLineLock->Release();
			picClerkLocks[index]->Acquire();
			printf("PictureClerk [%d] is going on break\n", index);
			picClerkStatuses[index] = CLERK_ON_BREAK;
			picClerkCVs[index]->Wait(picClerkLocks[index]);
			printf("PictureClerk [%d] returned from break\n", index);
			picClerkLocks[index]->Release();
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
				tprintf("PassClerk %d: has spotted %s in privPassLine(length = %d)\n", index, getCustomerType(), privPassLineLength);
				privPassLineLength--;
				tprintf("PassClerk %d: Becoming Available!\n",index);
				passClerkStatuses[index] = CLERK_AVAILABLE;
				tprintf("PassClerk %d: Signaling Condition variable (length of priv line is now %d)\n", index,privPassLineLength);
				privPassLineCV->Signal(passLineLock);
			}
			else{ //Check if anyone is in reg line
				tprintf("PassClerk %d: has spotted %s in regPassLine (length = %d)\n",index, getCustomerType(), regPassLineLength);
				regPassLineLength--;
				tprintf("PassClerk %d: Becoming Available!\n",index);
				passClerkStatuses[index] = CLERK_AVAILABLE;
				tprintf("PassClerk %d: Signaling Condition variable (length of reg line is now %d)\n", index,regPassLineLength);
				regPassLineCV->Signal(passLineLock);
			}
			//Customer/Senator - Clerk interaction

			tprintf("PassClerk %d: Acquiring my own lock\n",index);
			passClerkLocks[index]->Acquire();
			tprintf("PassClerk %d: Releasing passLineLock\n",index);
			passLineLock->Release();
			tprintf("PassClerk %d: Putting myself to sleep...\n",index);
			passClerkCVs[index]->Wait(passClerkLocks[index]);
			tprintf("PassClerk %d: Just woke up!\n",index);
			int SSN = passClerkSSNs[index];
			if (passClerkBribed[index] == TRUE){
				//printf("PassClerk %d: I've just been bribed for $500! Total amount I've collected to so far: $%d\n", index, passClerkMoney[index]);
				printf("PassportClerk %d accepts money = 500 from %s with SSN %d\n", index, getCustomerType(), index);
				passClerkBribed[index] = FALSE;
			}
			int SSN = passClerkSSNs[index];
					
			if(appFiled[SSN] == FALSE || picFiled[SSN] == FALSE){
				for (int i=0; i<10; i++){
					tprintf("AppFiled: %d,    PicFiled: %d,     PassFiled: %d\n",appFiled[i],picFiled[i], passFiled[i]);
				}
				//printf("PassClerk [%d] %s with SSN %d does not have both picture and application filed! *SPANK*\n", index, getCustomerType(), SSN);
				printf("PassportClerk [%d] gives invalid certification to %s with SSN %d\n", index, getCustomerType(), SSN); 
				printf("PassportClerk [%d] punishes %s with SSN %d to wait\n", index, getCustomerType(), SSN); 
				passPunish[index] = TRUE;
			}
			else{
				//printf("PassClerk %d: %s with SSN %d has everything filed correctly!\n",index, getCustomerType(), SSN);
				printf("PassportClerk [%d] gives valid certification to %s with SSN %d\n", index, getCustomerType(), SSN); 
				
				passPunish[index] = FALSE;
				//passFiled[SSN] = TRUE; //**********THIS SHOULD BE FORKED IN THE FUTURE*****************
				Thread* newThread = new Thread("Passport Filing Thread");
				newThread->Fork((VoidFunctionPtr)passClerkFileData, SSN);

				//DEBUG
				for (int i=0; i<10; i++){
					tprintf("AppFiled: %d,    PicFiled: %d,     PassFiled: %d\n", appFiled[i], picFiled[i], passFiled[i]);
				}
				printf("PassportClerk [%d] informs %s with SSN %d that the procedure has completed.\n", index, getCustomerType(), SSN); //Am not sure if I am to print this out or not
			}
			

			//printf("PassClerk %d: Just receieved %s's SSN: %d\n",index, getCustomerType(), SSN);
			tprintf("PassClerk %d: Signaling my passClerkCV\n", index);
			passClerkCVs[index]->Signal(passClerkLocks[index]);
			tprintf("PassClerk %d: Releasing my own lock\n", index);
			passClerkLocks[index]->Release();

		}
		else{ //No one in line
			passLineLock->Release();
			passClerkLocks[index]->Acquire();
			printf("PassportClerk [%d] is going on break\n", index);
			passClerkStatuses[index] = CLERK_ON_BREAK;
			passClerkCVs[index]->Wait(passClerkLocks[index]);
			printf("PassportClerk [%d] returned from break\n", index);
			passClerkLocks[index]->Release();
		}
		currentThread->Yield();
	}
}

//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------

void CashClerkRun(int index){
	while (true){
		//  printf("CashClerk %d: has acquired the cashLineLock\n", index);
		cashLineLock->Acquire();

		//There is no priv cash line, as Customers/Senators don't have enough money     

		//Checking if anyone is in line
		//if (privCashLineLength+regCashLineLength>0){

		/*if (privCashLineLength>0){ //Checking if anyone is in priv line
		printf("CashClerk %d: has spotted %s in privCashLine(length = %d)\n",index,privCashLineLength);
		privCashLineLength--;
		printf("CashClerk %d: Becoming Available!\n",index);
		cashClerkStatuses[index] = CLERK_AVAILABLE;
		printf("CashClerk %d: Signaling Condition variable (length of priv line is now %d)\n", index,privCashLineLength);
		privCashLineCV->Signal(cashLineLock);
		}*/
		if (regCashLineLength>0){ //Check if anyone is in reg line
			tprintf("CashClerk %d: has spotted %s in regCashLine (length = %d)\n",index, getCustomerType(), regCashLineLength);
			regCashLineLength--;
			tprintf("CashClerk %d: Becoming Available!\n",index);
			cashClerkStatuses[index] = CLERK_AVAILABLE;
			tprintf("CashClerk %d: Signaling Condition variable (length of reg line is now %d)\n", index,regCashLineLength);
			regCashLineCV->Signal(cashLineLock);

			//Customer/Senator - Clerk interaction

			tprintf("CashClerk %d: Acquiring my own lock\n",index);
			cashClerkLocks[index]->Acquire();
			tprintf("CashClerk %d: Releasing cashLineLock\n",index);
			cashLineLock->Release();
			tprintf("CashClerk %d: Putting myself to sleep...\n",index);
			cashClerkCVs[index]->Wait(cashClerkLocks[index]);
			tprintf("CashClerk %d: Just woke up!\n",index);
			int SSN = cashClerkSSNs[index];
		
			if(passFiled[SSN] == FALSE){
				for (int i=0; i<10; i++){
					tprintf("AppFiled: %d,    PicFiled: %d,     PassFiled: %d,    CashFiled: %d\n", appFiled[i], picFiled[i], passFiled[i], cashFiled[i]);
				}
				//printf("CashClerk %d: %s with SSN %d does not have both picture and application filed! *SPANK*\n", index, getCustomerType(), SSN);
				printf("Cashier [%d] gives invalid certification to %s with SSN %d\n", index, getCustomerType(), SSN);
				printf("Cashier [%d] punishes %s with SSN %d to wait\n", index, getCustomerType(), SSN); 
				
				cashPunish[index] = TRUE;
			}
			else{
				tprintf("CashClerk %d: %s with SSN %d has everything filed correctly!\n",index, getCustomerType(), SSN);
				printf("Cashier [%d] gives valid certification to %s with SSN %d\n", index, getCustomerType(), SSN);
				printf("Cashier [%d] records %s with SSN %d's passport\n", index, getCustomerType(), SSN);
				printf("Cashier [%d] has recorded the passport for %s with SSN %d\n", index, getCustomerType(), SSN);
				cashPunish[index] = FALSE;
				cashFiled[SSN] = TRUE;
				printf("Cashier [%d] accepts money = 100 from %s with SSN %d\n", index, getCustomerType(), SSN);
				cashClerkCVs[index]->Signal(cashClerkLocks[index]);
				cashClerkCVs[index]->Wait(cashClerkLocks[index]);
				tprintf("CashClerk %d: Total money collected: $%d\n",index, cashClerkMoney[index]);

				//DEBUG
				for (int i=0; i<10; i++){
					tprintf("AppFiled: %d,    PicFiled: %d,     PassFiled: %d,    CashFiled: %d\n", appFiled[i], picFiled[i], passFiled[i], cashFiled[i]);
				}

			}

			tprintf("CashClerk %d: Signaling my cashClerkCV\n", index);
			cashClerkCVs[index]->Signal(cashClerkLocks[index]);
			tprintf("CashClerk %d: Releasing my own lock\n", index);
			cashClerkLocks[index]->Release();


		}
		else{ //No one in line

			cashLineLock->Release();
			cashClerkLocks[index]->Acquire();
			printf("Cashier [%d] is going on break\n", index);
			cashClerkStatuses[index] = CLERK_ON_BREAK;
			cashClerkCVs[index]->Wait(cashClerkLocks[index]);
			printf("Cashier [%d] returned from break\n", index);
			cashClerkLocks[index]->Release();
		}
		currentThread->Yield();

	}

}
