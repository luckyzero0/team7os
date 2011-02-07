//manager.cc
#include "office.h"
#include "system.h"
#include <iostream>

void tryToWakeUpSenators();
void tryToWakeUpCustomers();

void tryToWakeUpCustomers(){ 
	senatorWaitingRoomLock->Acquire();
	senatorOfficeLock->Acquire();
	if (senatorsInWaitingRoom+senatorsInOffice == 0){
		customerWaitingRoomLock->Acquire();
		customerWaitingRoomCV->Broadcast(customerWaitingRoomLock);
		customerWaitingRoomLock->Release();
	}
	senatorOfficeLock->Release();
	senatorWaitingRoomLock->Release();

	//What if a senator enters right now?!?!?!?!?!
}

void tryToWakeUpSenators() {
	senatorWaitingRoomLock->Acquire();
	if (senatorsInWaitingRoom > 0) {
		senatorWaitingRoomLock->Release();
		tprintf("Manager: There are senators in the waiting room! \n");
		//acquire all line CVs
		customerOfficeLock->Acquire();
		if (customersInOffice > 0) { //if there are customers in the office, tell them to get the hell out.
			tprintf("Manager: I need to tell all %d customers to GTFO\n", customersInOffice);
			customerOfficeLock->Release();
			tprintf("Manager: Acquiring appPicLineLock\n");
			appPicLineLock->Acquire();
			tprintf("Manager: Acquiring passLineLock\n");
			passLineLock->Acquire();
			tprintf("Manager: Acquiring cashLineLock\n");
			cashLineLock->Acquire();

			//Setting all lines to 0;
			privAppLineLength = 0;
			regAppLineLength = 0;
			privPicLineLength = 0;
			regPicLineLength = 0;

			privPassLineLength = 0;
			regPassLineLength = 0;

			regCashLineLength = 0;

		

			tprintf("Manager: Broadcasting to all lines\n");
			privAppLineCV->Broadcast(appPicLineLock);
			regAppLineCV->Broadcast(appPicLineLock);
			privPicLineCV->Broadcast(appPicLineLock);
			regPicLineCV->Broadcast(appPicLineLock);

			privPassLineCV->Broadcast(passLineLock);
			regPassLineCV->Broadcast(passLineLock);

			regCashLineCV->Broadcast(cashLineLock);


			tprintf("Manager: Releasing cashLineLock\n");
			cashLineLock->Release();
			tprintf("Manager: Releasing passLineLock\n");
			passLineLock->Release();
			tprintf("Manager: Releasing appPicLineLock\n");
			appPicLineLock->Release();

			
			customerOfficeLock->Acquire();
			while (customersInOffice > 0) {
				tprintf("Manager: Waiting for remaining %d customers to leave\n", customersInOffice);
				customerOfficeLock->Release();
				currentThread->Yield();
				customerOfficeLock->Acquire();
			}
		}

		customerOfficeLock->Release();
		
		tprintf("Manager: Telling all %d senators to enter the office\n", senatorsInWaitingRoom);
		senatorWaitingRoomLock->Acquire();
		senatorWaitingRoomCV->Broadcast(senatorWaitingRoomLock);
	}
	senatorWaitingRoomLock->Release();

	//might want some yields here
}

void ManagerRun(int notUsed){

	while(true)
	{
		tryToWakeUpSenators();
		tryToWakeUpCustomers();

		int totalCashCollected = 0;
		for (int i = 0; i < numCashClerks; i++) {
		  totalCashCollected += cashClerkMoney[i];
		}
		if (totalCashCollected == (numCustomers + numSenators) * 100) {
		  totalCashCollected = 0;
		  //we're done, print out everything
		  for (int i = 0; i < numAppClerks; i++) {
		    printf("Total money received from ApplicationClerk = %d\n", appClerkMoney[i]);
		    totalCashCollected += appClerkMoney[i];
		  }
		  for (int i = 0; i < numPicClerks; i++) {
		    printf("Total money received from PictureClerk = %d\n", picClerkMoney[i]);
		    totalCashCollected += picClerkMoney[i];
		  }
		  for (int i = 0; i < numPassClerks; i++) {
		    printf("Total money received from PassportClerk = %d\n", passClerkMoney[i]);
		    totalCashCollected += passClerkMoney[i];
		  }
		  for (int i = 0; i < numCashClerks; i++) {
		    printf("Total money received from Cashier = %d\n", cashClerkMoney[i]);
		    totalCashCollected += cashClerkMoney[i];
		  }

		  printf("Total money made by office = %d\n", totalCashCollected);
		  if (TESTING) {
		    exit(0);
		  } else {
		    break;
		  }
		}

		tprintf("Manager: Time to slavedrive my clerks. Checking the lines...\n");

		//check AppLineLenghts	
		appPicLineLock->Acquire();
		tprintf("Manager: I spy [%d] customers in the AppLine\n", (regAppLineLength+privAppLineLength));
		if(regAppLineLength + privAppLineLength > 3)
		{	
			tprintf("Manager: Making sure the AppClerks are working\n");
			for(int x = 0; x < MAX_APP_CLERKS; x++)
			{
				//put onbreak clerks to work
				if(appClerkStatuses[x] == CLERK_ON_BREAK)
				{	
					tprintf("Manager: Whipping AppClerk[%d] back to work\n",x);
					appClerkLocks[x]->Acquire();				
					appClerkStatuses[x] = CLERK_COMING_BACK;
					appClerkCVs[x]->Signal(appClerkLocks[x]);
					printf("Manager calls an ApplicationClerk back from break\n");
					appClerkLocks[x]->Release();
					break;
				}
			}
			tprintf("Manager: Checking next line...\n");
		}	
		else if(regAppLineLength+privAppLineLength > 0)
		{
			int wakeup = -1;
			tprintf("Manager: Making sure at least one AppClerk is working\n");
			for(int x = 0; x < MAX_APP_CLERKS; x++)
			{
				//search to see if any clerks are available
				if(appClerkStatuses[x] == CLERK_AVAILABLE)
				{	
					tprintf("Manager: AppClerk[%d] is available. Moving on.\n",x);					
					wakeup = -1;
					break;
				}
				else if (appClerkStatuses[x] == CLERK_BUSY)
				{
					tprintf("Manager: AppClerk[%d] is busy. Moving on. \n", x);
					wakeup = -1;
					break;
				}
				else if (appClerkStatuses[x] == CLERK_COMING_BACK)
				{
					tprintf("Manager: AppClerk[%d] is coming back. Moving on. \n", x);
					wakeup = -1;
					break;
				}  
				else if(appClerkStatuses[x] == CLERK_ON_BREAK)
				{
					//we'll wake up this clerk if no one is available
					tprintf("Manager: Tagging AppClerk[%d] to work...\n", x);
					wakeup = x;
				}
			}
			if(wakeup >= 0)
			{
				tprintf("Manager: Whipping AppClerk[%d] back to work\n",wakeup);
				appClerkLocks[wakeup]->Acquire();				
				appClerkStatuses[wakeup] = CLERK_COMING_BACK;
				appClerkCVs[wakeup]->Signal(appClerkLocks[wakeup]);
				printf("Manager calls back an ApplicationClerk from break\n");
				appClerkLocks[wakeup]->Release();				
			}			
			tprintf("Manager: Checking next line...\n");
		}


		tprintf("Manager: I spy [%d] customers in the PicLine\n", (regPicLineLength+privPicLineLength));
		if(regPicLineLength + privPicLineLength > 3)
		{
			tprintf("Manager: Making sure the PicClerks are working\n");
			for(int x = 0; x < MAX_PIC_CLERKS; x++)
			{
				//put onbreak clerks to work
				if(picClerkStatuses[x] == CLERK_ON_BREAK)
				{	
					tprintf("Manager: Whipping PicClerk[%d] back to work\n",x);
					picClerkLocks[x]->Acquire();				
					picClerkStatuses[x] = CLERK_COMING_BACK;
					picClerkCVs[x]->Signal(picClerkLocks[x]);
					printf("Manager calls back a PictureClerk from break\n");
					picClerkLocks[x]->Release();
					break;
				}
			}
			tprintf("Manager: Checking next line...\n");
		}
		else if(regPicLineLength+privPicLineLength > 0)
		{
			int wakeup = -1;
			tprintf("Manager: Making sure at least one PicClerk is working\n");
			for(int x = 0; x < MAX_PIC_CLERKS; x++)
			{
				//search to see if any clerks are available
				if(picClerkStatuses[x] == CLERK_AVAILABLE)
				{	
					tprintf("Manager: PicClerk[%d] is available. Moving on.\n",x);					
					wakeup = -1;
					break;
				}
				else if (picClerkStatuses[x] == CLERK_BUSY)
				{
					tprintf("Manager: PicClerk[%d] is busy. Moving on. \n", x);
					wakeup = -1;
					break;
				}
				else if (picClerkStatuses[x] == CLERK_COMING_BACK)
				{
					tprintf("Manager: PicClerk[%d] is coming back. Moving on. \n", x);
					wakeup = -1;
					break;
				} 
				else if(picClerkStatuses[x] == CLERK_ON_BREAK)
				{
					tprintf("Manager: Tagging PicClerk[%d] to work...\n", x);
					//we'll wake up this clerk if no one is available
					wakeup = x;
				}
			}
			if(wakeup >= 0)
			{
				tprintf("Manager: Whipping PicClerk[%d] back to work\n",wakeup);
				picClerkLocks[wakeup]->Acquire();				
				picClerkStatuses[wakeup] = CLERK_COMING_BACK;
				picClerkCVs[wakeup]->Signal(picClerkLocks[wakeup]);
				printf("Manager calls back a PictureClerk from break\n");
				picClerkLocks[wakeup]->Release();				
			}
			tprintf("Manager: Checking next line...\n");
		}
		appPicLineLock->Release();

		passLineLock->Acquire();
		tprintf("Manager: I spy [%d] customers in the PassLine\n", (regPassLineLength+privPassLineLength));
		if(regPassLineLength + privPassLineLength > 3)
		{
			tprintf("Manager: Making sure the PassClerks are working\n");
			for(int x = 0; x < MAX_PASS_CLERKS; x++)
			{
				//put onbreak clerks to work
				if(passClerkStatuses[x] == CLERK_ON_BREAK)
				{	
					tprintf("Manager: Whipping PassClerk[%d] back to work\n",x);
					passClerkLocks[x]->Acquire();				
					passClerkStatuses[x] = CLERK_COMING_BACK;
					passClerkCVs[x]->Signal(passClerkLocks[x]);
					printf("Manager calls back a PassportClerk from break\n");
					passClerkLocks[x]->Release();
					break;
				}
			}
			tprintf("Manager: Checking next line...\n");
		}
		else if(regPassLineLength+privPassLineLength > 0)
		{
			int wakeup = -1;
			tprintf("Manager: Making sure at least one PassClerk is working\n");
			for(int x = 0; x < MAX_PASS_CLERKS; x++)
			{
				//search to see if any clerks are available
				if(passClerkStatuses[x] == CLERK_AVAILABLE)
				{	
					tprintf("Manager: PassClerk[%d] is available. Moving on.\n",x);					
					wakeup = -1;
					break;
				}
				else if (passClerkStatuses[x] == CLERK_BUSY)
				{
					tprintf("Manager: AppClerk[%d] is busy. Moving on. \n", x);
					wakeup = -1;
					break;
				}
				else if (passClerkStatuses[x] == CLERK_COMING_BACK)
				{
					tprintf("Manager: PassClerk[%d] is coming back. Moving on. \n", x);
					wakeup = -1;
					break;
				} 
				else if(passClerkStatuses[x] == CLERK_ON_BREAK)
				{
					//we'll wake up this clerk if no one is available
					tprintf("Manager: Tagging PassClerk[%d] to work...\n", x);
					wakeup = x;
				}
			}
			if(wakeup >= 0)
			{
				tprintf("Manager: Whipping PassClerk[%d] back to work\n",wakeup);
				passClerkLocks[wakeup]->Acquire();				
				passClerkStatuses[wakeup] = CLERK_COMING_BACK;
				passClerkCVs[wakeup]->Signal(passClerkLocks[wakeup]);
				printf("Manager calls back a PassportClerk from break\n");
				passClerkLocks[wakeup]->Release();				
			}
			tprintf("Manager: Checking next line...\n");
		}
		passLineLock->Release();

		cashLineLock->Acquire();
		tprintf("Manager: I spy [%d] customers in the CashLine\n", (regCashLineLength));
		if(regCashLineLength >= 3)
		{
			tprintf("Manager: Making sure the CashClerks are working\n");
			for(int x = 0; x < MAX_CASH_CLERKS; x++)
			{
				//put onbreak clerks to work
				if(cashClerkStatuses[x] == CLERK_ON_BREAK)
				{	
					tprintf("Manager: Whipping CashClerk[%d] back to work\n",x);
					cashClerkLocks[x]->Acquire();				
					cashClerkStatuses[x] = CLERK_COMING_BACK;
					cashClerkCVs[x]->Signal(cashClerkLocks[x]);
					printf("Manager calls back a cashier from break\n");
					cashClerkLocks[x]->Release();
					break;
				}
			}
			tprintf("Manager: Checking next line...\n");
		}
		else if(regCashLineLength > 0)
		{
			int wakeup = -1;
			tprintf("Manager: Making sure at least one CashClerk is working\n");
			for(int x = 0; x < MAX_CASH_CLERKS; x++)
			{
				//search to see if any clerks are available
				if(cashClerkStatuses[x] == CLERK_AVAILABLE)
				{	
					tprintf("Manager: CashClerk[%d] is available. Moving on.\n",x);					
					wakeup = -1;
					break;
				}
				else if (cashClerkStatuses[x] == CLERK_BUSY)
				{
					tprintf("Manager: AppClerk[%d] is busy. Moving on. \n", x);
					wakeup = -1;
					break;
				}
				else if (cashClerkStatuses[x] == CLERK_COMING_BACK)
				{
					tprintf("Manager: CashClerk[%d] is coming back. Moving on. \n", x);
					wakeup = -1;
					break;
				} 
				else if(cashClerkStatuses[x] == CLERK_ON_BREAK)
				{
					//we'll wake up this clerk if no one is available
					tprintf("Manager: Tagging CashClerk[%d] to work...\n", x);
					wakeup = x;
				}
			}
			if(wakeup >= 0)
			{
				tprintf("Manager: Whipping CashClerk[%d] back to work\n",wakeup);
				cashClerkLocks[wakeup]->Acquire();				
				cashClerkStatuses[wakeup] = CLERK_COMING_BACK;
				cashClerkCVs[wakeup]->Signal(cashClerkLocks[wakeup]);
				printf("Manager calls back a Cashier from break\n");
				cashClerkLocks[wakeup]->Release();				
			}
			tprintf("Manager: Checking next line...\n");
		}
		tprintf("Manager: Yielding to let other threads do work.\n");
		cashLineLock->Release();
		for (int i = 0; i < 50; i++) {
			currentThread->Yield();
		}
	}
}

