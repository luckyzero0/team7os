
#include "office.h"
#include "system.h"
#include <iostream>

void tryToWakeUpSenators();
void tryToWakeUpCustomers();

void tryToWakeUpCustomers(){ 
	/*senatorWaitingRoomLock->Acquire();*/
	Acquire(senatorWaitingRoomLock);
	/*senatorOfficeLock->Acquire();*/
	Acquire(senatorOfficeLock);
	if (senatorsInWaitingRoom+senatorsInOffice == 0){
		/*customerWaitingRoomLock->Acquire();*/
		Acquire(customerWaitingRoomLock);
		/*customerWaitingRoomCV->Broadcast(customerWaitingRoomLock);*/
		Broadcast(customerWaitingRoomCV, customerWaitingRoomLock);
		/*customerWaitingRoomLock->Release();*/
		Release(customerWaitingRoomLock);
	}
	/*senatorOfficeLock->Release();
	senatorWaitingRoomLock->Release();*/
	Release(senatorOfficeLock);
	Release(senatorWaitingRoomLock);

}

void tryToWakeUpSenators() {
	/*senatorWaitingRoomLock->Acquire();*/
	Acquire(senatorWaitingRoomLock);
	if (senatorsInWaitingRoom > 0) {
		/*senatorWaitingRoomLock->Release();*/
		Release(senatorWaitingRoomLock);
		tprintf("Manager: There are senators in the waiting room! \n");
		/*acquire all line CVs*/
		/*customerOfficeLock->Acquire();*/
		Acquire(customerOfficeLock);
		if (customersInOffice > 0) { //if there are customers in the office, tell them to get the hell out.
			tprintf("Manager: I need to tell all %d customers to GTFO\n", customersInOffice);
			/*customerOfficeLock->Release();*/
			Release(customerOfficeLock);
			tprintf("Manager: Acquiring appPicLineLock\n");
			/*appPicLineLock->Acquire();*/
			Acquire(appPicLineLock);
			tprintf("Manager: Acquiring passLineLock\n");
			/*passLineLock->Acquire();*/
			Acquire(passLineLock);
			tprintf("Manager: Acquiring cashLineLock\n");
			/*cashLineLock->Acquire();*/
			Acquire(cashLineLock);

			//Setting all lines to 0;
			privAppLineLength = 0;
			regAppLineLength = 0;
			privPicLineLength = 0;
			regPicLineLength = 0;

			privPassLineLength = 0;
			regPassLineLength = 0;

			regCashLineLength = 0;

		

			tprintf("Manager: Broadcasting to all lines\n");
			/*privAppLineCV->Broadcast(appPicLineLock);
			regAppLineCV->Broadcast(appPicLineLock);
			privPicLineCV->Broadcast(appPicLineLock);
			regPicLineCV->Broadcast(appPicLineLock);

			privPassLineCV->Broadcast(passLineLock);
			regPassLineCV->Broadcast(passLineLock);

			regCashLineCV->Broadcast(cashLineLock);*/
			Broadcast(privAppLineCV, appPicLineLock);
			Broadcast(regAppLineCV, appPicLineLock);
			Broadcast(privPicLineCV, appPicLineLock);
			Broadcast(regPicLineCV, appPicLineLock);

			Broadcast(privPassLineCV, passLineLock);
			Broadcast(regPassLineCV, passLineLock);

			Broadcast(regCashLineCV, cashLineLock);


			tprintf("Manager: Releasing cashLineLock\n");
			/*cashLineLock->Release();*/
			Release(cashLineLock);
			tprintf("Manager: Releasing passLineLock\n");
			/*passLineLock->Release();*/
			Release(passLineLock);
			tprintf("Manager: Releasing appPicLineLock\n");
			/*appPicLineLock->Release();*/
			ReleasE(appPicLineLock);

			
			/*customerOfficeLock->Acquire();*/
			Acquire(customerOfficeLock);
			while (customersInOffice > 0) {
				tprintf("Manager: Waiting for remaining %d customers to leave\n", customersInOffice);
				/*customerOfficeLock->Release();*/
				Release(customerOfficeLock);
				currentThread->Yield();
				/*customerOfficeLock->Acquire();*/
				Acquire(customerOfficeLock);
			}
		}

		/*customerOfficeLock->Release();*/
		Release(customerOfficeLock);
		
		tprintf("Manager: Telling all %d senators to enter the office\n", senatorsInWaitingRoom);
		/*senatorWaitingRoomLock->Acquire();*/
		Acquire(senatorWaitingRoomLock);
		/*senatorWaitingRoomCV->Broadcast(senatorWaitingRoomLock);*/
		Broadcast(senatorWaitingRoomCV, senatorWaitingRoomLock);
	}
	/*senatorWaitingRoomLock->Release();*/
	Release(senatorWaitingRoomLock);

}

void ManagerRun(int notUsed){
	int i, wakeup;
	int totalCashCollected;
	while(true)
	{
		tryToWakeUpSenators();
		tryToWakeUpCustomers();

		totalCashCollected = 0;
		for (i = 0; i < numCashClerks; i++) {
		  totalCashCollected += cashClerkMoney[i];
		}
		if (totalCashCollected == (numCustomers + numSenators) * 100) {
		  totalCashCollected = 0;
		  /*we're done, print out everything*/
		  for (i = 0; i < numAppClerks; i++) {
		    printf("Total money received from ApplicationClerk = %d\n", appClerkMoney[i]);
		    totalCashCollected += appClerkMoney[i];
		  }
		  for (i = 0; i < numPicClerks; i++) {
		    printf("Total money received from PictureClerk = %d\n", picClerkMoney[i]);
		    totalCashCollected += picClerkMoney[i];
		  }
		  for (i = 0; i < numPassClerks; i++) {
		    printf("Total money received from PassportClerk = %d\n", passClerkMoney[i]);
		    totalCashCollected += passClerkMoney[i];
		  }
		  for (i = 0; i < numCashClerks; i++) {
		    printf("Total money received from Cashier = %d\n", cashClerkMoney[i]);
		    totalCashCollected += cashClerkMoney[i];
		  }

		  printf("Total money made by office = %d\n", totalCashCollected);
		  if (TESTING) {
		    Exit(0);
		  } else {
		    break;
		  }
		}

		tprintf("Manager: Time to slavedrive my clerks. Checking the lines...\n");

		/*check AppLineLengths*/	
		/*appPicLineLock->Acquire();*/
		Acquire(appPicLineLock);
		tprintf("Manager: I spy [%d] customers in the AppLine\n", (regAppLineLength+privAppLineLength));
		if(regAppLineLength + privAppLineLength > 3)
		{	
			tprintf("Manager: Making sure the AppClerks are working\n");
			for(i = 0; i < MAX_APP_CLERKS; i++)
			{
				/*put onbreak clerks to work*/
				if(appClerkStatuses[i] == CLERK_ON_BREAK)
				{	
					tprintf("Manager: Whipping AppClerk[%d] back to work\n",i);
					/*appClerkLocks[i]->Acquire();		*/
					Acquire(appClerkLocks[i]);
					appClerkStatuses[i] = CLERK_COMING_BACK;
					/*appClerkCVs[i]->Signal(appClerkLocks[i]);*/
					Signal(appClerkCVs[i], appClerkLocks[i]);
					printf("Manager calls an ApplicationClerk back from break\n");
					/*appClerkLocks[i]->Release();*/
					Release(appClerkLocks[i]);
					break;
				}
			}
			tprintf("Manager: Checking next line...\n");
		}	
		else if(regAppLineLength+privAppLineLength > 0)
		{
			wakeup = -1;
			tprintf("Manager: Making sure at least one AppClerk is working\n");
			for(i = 0; i < MAX_APP_CLERKS; i++)
			{
				//search to see if any clerks are available
				if(appClerkStatuses[i] == CLERK_AVAILABLE)
				{	
					tprintf("Manager: AppClerk[%d] is available. Moving on.\n",i);					
					wakeup = -1;
					break;
				}
				else if (appClerkStatuses[i] == CLERK_BUSY)
				{
					tprintf("Manager: AppClerk[%d] is busy. Moving on. \n", i);
					wakeup = -1;
					break;
				}
				else if (appClerkStatuses[i] == CLERK_COMING_BACK)
				{
					tprintf("Manager: AppClerk[%d] is coming back. Moving on. \n", i);
					wakeup = -1;
					break;
				}  
				else if(appClerkStatuses[i] == CLERK_ON_BREAK)
				{
					//we'll wake up this clerk if no one is available
					tprintf("Manager: Tagging AppClerk[%d] to work...\n", i);
					wakeup = i;
				}
			}
			if(wakeup >= 0)
			{
				tprintf("Manager: Whipping AppClerk[%d] back to work\n",wakeup);
				/*appClerkLocks[wakeup]->Acquire();	*/
				Acquire(appClerkLocks[wakeup]);
				appClerkStatuses[wakeup] = CLERK_COMING_BACK;
				/*appClerkCVs[wakeup]->Signal(appClerkLocks[wakeup]);*/
				Signal(appClerkCVs[wakeup], appClerkLocks[wakeup]);
				printf("Manager calls back an ApplicationClerk from break\n");
				/*appClerkLocks[wakeup]->Release();	*/
				Release(appClerkLocks[wakeup]);
			}			
			tprintf("Manager: Checking next line...\n");
		}


		tprintf("Manager: I spy [%d] customers in the PicLine\n", (regPicLineLength+privPicLineLength));
		if(regPicLineLength + privPicLineLength > 3)
		{
			tprintf("Manager: Making sure the PicClerks are working\n");
			for(i = 0; i < MAX_PIC_CLERKS; i++)
			{
				//put onbreak clerks to work
				if(picClerkStatuses[i] == CLERK_ON_BREAK)
				{	
					tprintf("Manager: Whipping PicClerk[%d] back to work\n",i);
					/*picClerkLocks[i]->Acquire();	*/
					Acquire(picClerkLocks[i]);
					picClerkStatuses[i] = CLERK_COMING_BACK;
					/*picClerkCVs[i]->Signal(picClerkLocks[i]);*/
					Signal(picClerkCVs[i], picClerkLocks[i]);
					printf("Manager calls back a PictureClerk from break\n");
					/*picClerkLocks[i]->Release();*/
					Release(picClerkLocks[i]);
					break;
				}
			}
			tprintf("Manager: Checking next line...\n");
		}
		else if(regPicLineLength+privPicLineLength > 0)
		{
			int wakeup = -1;
			tprintf("Manager: Making sure at least one PicClerk is working\n");
			for(i = 0; i < MAX_PIC_CLERKS; i++)
			{
				//search to see if any clerks are available
				if(picClerkStatuses[i] == CLERK_AVAILABLE)
				{	
					tprintf("Manager: PicClerk[%d] is available. Moving on.\n",i);					
					wakeup = -1;
					break;
				}
				else if (picClerkStatuses[i] == CLERK_BUSY)
				{
					tprintf("Manager: PicClerk[%d] is busy. Moving on. \n", i);
					wakeup = -1;
					break;
				}
				else if (picClerkStatuses[i] == CLERK_COMING_BACK)
				{
					tprintf("Manager: PicClerk[%d] is coming back. Moving on. \n", i);
					wakeup = -1;
					break;
				} 
				else if(picClerkStatuses[i] == CLERK_ON_BREAK)
				{
					tprintf("Manager: Tagging PicClerk[%d] to work...\n", i);
					//we'll wake up this clerk if no one is available
					wakeup = i;
				}
			}
			if(wakeup >= 0)
			{
				tprintf("Manager: Whipping PicClerk[%d] back to work\n",wakeup);
				/*picClerkLocks[wakeup]->Acquire();*/
				Acquire(picClerkLocks[wakeup]);
				picClerkStatuses[wakeup] = CLERK_COMING_BACK;
				/*picClerkCVs[wakeup]->Signal(picClerkLocks[wakeup]);*/
				Signal(picClerkCVs[wakeup], picClerkLocks[wakeup]);
				printf("Manager calls back a PictureClerk from break\n");
				/*picClerkLocks[wakeup]->Release();	*/
				Release(picClerkLocks[wakeup]);
			}
			tprintf("Manager: Checking next line...\n");
		}
		/*appPicLineLock->Release();*/
		Release(appPicLineLock);

		/*passLineLock->Acquire();*/
		Acquire(passLineLock);
		tprintf("Manager: I spy [%d] customers in the PassLine\n", (regPassLineLength+privPassLineLength));
		if(regPassLineLength + privPassLineLength > 3)
		{
			tprintf("Manager: Making sure the PassClerks are working\n");
			for(i = 0; i < MAX_PASS_CLERKS; i++)
			{
				//put onbreak clerks to work
				if(passClerkStatuses[i] == CLERK_ON_BREAK)
				{	
					tprintf("Manager: Whipping PassClerk[%d] back to work\n",i);
					/*passClerkLocks[i]->Acquire();	*/
					Acquire(passClerkLocks[i]);
					passClerkStatuses[i] = CLERK_COMING_BACK;
					/*passClerkCVs[i]->Signal(passClerkLocks[i]);*/
					Signal(passClerkCVs[i], passClerkLocks[i]);
					printf("Manager calls back a PassportClerk from break\n");
					/*passClerkLocks[i]->Release();*/
					Release(passClerkLocks[i]);
					break;
				}
			}
			tprintf("Manager: Checking next line...\n");
		}
		else if(regPassLineLength+privPassLineLength > 0)
		{
			int wakeup = -1;
			tprintf("Manager: Making sure at least one PassClerk is working\n");
			for(i = 0; i < MAX_PASS_CLERKS; i++)
			{
				//search to see if any clerks are available
				if(passClerkStatuses[i] == CLERK_AVAILABLE)
				{	
					tprintf("Manager: PassClerk[%d] is available. Moving on.\n",i);					
					wakeup = -1;
					break;
				}
				else if (passClerkStatuses[i] == CLERK_BUSY)
				{
					tprintf("Manager: AppClerk[%d] is busy. Moving on. \n", i);
					wakeup = -1;
					break;
				}
				else if (passClerkStatuses[i] == CLERK_COMING_BACK)
				{
					tprintf("Manager: PassClerk[%d] is coming back. Moving on. \n", i);
					wakeup = -1;
					break;
				} 
				else if(passClerkStatuses[i] == CLERK_ON_BREAK)
				{
					//we'll wake up this clerk if no one is available
					tprintf("Manager: Tagging PassClerk[%d] to work...\n", i);
					wakeup = i;
				}
			}
			if(wakeup >= 0)
			{
				tprintf("Manager: Whipping PassClerk[%d] back to work\n",wakeup);
				/*passClerkLocks[wakeup]->Acquire();		*/
				Acquire(passClerkLocks[wakeup]);
				passClerkStatuses[wakeup] = CLERK_COMING_BACK;
				/*passClerkCVs[wakeup]->Signal(passClerkLocks[wakeup]);*/
				Signal(passClerkCVs[wakeup], passClerkLocks[wakeup]);
				printf("Manager calls back a PassportClerk from break\n");
				/*passClerkLocks[wakeup]->Release();*/
				Release(passClerkLocks[wakeup]);
			}
			tprintf("Manager: Checking next line...\n");
		}
		/*passLineLock->Release();*/
		Release(passLineLock);

		/*cashLineLock->Acquire();*/
		Acquire(cashLineLock);
		tprintf("Manager: I spy [%d] customers in the CashLine\n", (regCashLineLength));
		if(regCashLineLength >= 3)
		{
			tprintf("Manager: Making sure the CashClerks are working\n");
			for(int i = 0; i < MAX_CASH_CLERKS; i++)
			{
				/*put onbreak clerks to work*/
				if(cashClerkStatuses[i] == CLERK_ON_BREAK)
				{	
					tprintf("Manager: Whipping CashClerk[%d] back to work\n",i);
					/*cashClerkLocks[i]->Acquire();	*/
					Acquire(cashClerkLocks[i]);
					cashClerkStatuses[i] = CLERK_COMING_BACK;
					/*cashClerkCVs[i]->Signal(cashClerkLocks[i]);*/
					Signal(cashClerkCVs[i]. cashClerkLocks[i]);
					printf("Manager calls back a cashier from break\n");
					/*cashClerkLocks[i]->Release();*/
					Release(cashClerkLocks[i]);
					break;
				}
			}
			tprintf("Manager: Checking next line...\n");
		}
		else if(regCashLineLength > 0)
		{
			int wakeup = -1;
			tprintf("Manager: Making sure at least one CashClerk is working\n");
			for( i = 0; i < MAX_CASH_CLERKS; i++)
			{
				/*search to see if any clerks are available*/
				if(cashClerkStatuses[i] == CLERK_AVAILABLE)
				{	
					tprintf("Manager: CashClerk[%d] is available. Moving on.\n",i);					
					wakeup = -1;
					break;
				}
				else if (cashClerkStatuses[i] == CLERK_BUSY)
				{
					tprintf("Manager: AppClerk[%d] is busy. Moving on. \n", i);
					wakeup = -1;
					break;
				}
				else if (cashClerkStatuses[i] == CLERK_COMING_BACK)
				{
					tprintf("Manager: CashClerk[%d] is coming back. Moving on. \n", i);
					wakeup = -1;
					break;
				} 
				else if(cashClerkStatuses[i] == CLERK_ON_BREAK)
				{
					/*we'll wake up this clerk if no one is available*/
					tprintf("Manager: Tagging CashClerk[%d] to work...\n", i);
					wakeup = i;
				}
			}
			if(wakeup >= 0)
			{
				tprintf("Manager: Whipping CashClerk[%d] back to work\n",wakeup);
				/*cashClerkLocks[wakeup]->Acquire();*/
				Acquire(cashClerkLocks[wakeup]);
				cashClerkStatuses[wakeup] = CLERK_COMING_BACK;
				/*cashClerkCVs[wakeup]->Signal(cashClerkLocks[wakeup]);*/
				Signal(cashClerkCVs[wakeup], cashClerkLocks[wakeup]);
				printf("Manager calls back a Cashier from break\n");
				/*cashClerkLocks[wakeup]->Release();	*/
				Release(cashClerkLocks[wakeup]);
			}
			tprintf("Manager: Checking next line...\n");
		}
		tprintf("Manager: Yielding to let other threads do work.\n");
		/*cashLineLock->Release();*/
		Release(cashLineLock);
		for (i = 0; i < 50; i++) {
			currentThread->Yield();
		}
	}
}

