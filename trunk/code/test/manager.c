
#include "office.h"
#include "utils.h"

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
		tprintf("Manager: There are senators in the waiting room! \n", 0,0,0,"","");
		/*acquire all line CVs*/
		/*customerOfficeLock->Acquire();*/
		Acquire(customerOfficeLock);
		if (customersInOffice > 0) { /*if there are customers in the office, tell them to get the hell out.*/
			tprintf("Manager: I need to tell all %d customers to GTFO\n", customersInOffice,0,0,"","");
			/*customerOfficeLock->Release();*/
			Release(customerOfficeLock);
			tprintf("Manager: Acquiring appPicLineLock\n",0,0,0,"","");
			/*appPicLineLock->Acquire();*/
			Acquire(appPicLineLock);
			tprintf("Manager: Acquiring passLineLock\n",0,0,0,"","");
			/*passLineLock->Acquire();*/
			Acquire(passLineLock);
			tprintf("Manager: Acquiring cashLineLock\n",0,0,0,"","");
			/*cashLineLock->Acquire();*/
			Acquire(cashLineLock);

			/*Setting all lines to 0;*/
			privAppLineLength = 0;
			regAppLineLength = 0;
			privPicLineLength = 0;
			regPicLineLength = 0;

			privPassLineLength = 0;
			regPassLineLength = 0;

			regCashLineLength = 0;

		

			tprintf("Manager: Broadcasting to all lines\n",0,0,0,"","");
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


			tprintf("Manager: Releasing cashLineLock\n",0,0,0,"","");
			/*cashLineLock->Release();*/
			Release(cashLineLock);
			tprintf("Manager: Releasing passLineLock\n",0,0,0,"","");
			/*passLineLock->Release();*/
			Release(passLineLock);
			tprintf("Manager: Releasing appPicLineLock\n",0,0,0,"","");
			/*appPicLineLock->Release();*/
			Release(appPicLineLock);

			
			/*customerOfficeLock->Acquire();*/
			Acquire(customerOfficeLock);
			while (customersInOffice > 0) {
				tprintf("Manager: Waiting for remaining %d customers to leave\n", customersInOffice,0,0,"","");
				/*customerOfficeLock->Release();*/
				Release(customerOfficeLock);
				Yield();
				/*customerOfficeLock->Acquire();*/
				Acquire(customerOfficeLock);
			}
		}

		/*customerOfficeLock->Release();*/
		Release(customerOfficeLock);
		
		tprintf("Manager: Telling all %d senators to enter the office\n", senatorsInWaitingRoom,0,0,"","");
		/*senatorWaitingRoomLock->Acquire();*/
		Acquire(senatorWaitingRoomLock);
		/*senatorWaitingRoomCV->Broadcast(senatorWaitingRoomLock);*/
		Broadcast(senatorWaitingRoomCV, senatorWaitingRoomLock);
	}
	/*senatorWaitingRoomLock->Release();*/
	Release(senatorWaitingRoomLock);

}

void ManagerRun(){
	int i, wakeup;
	int totalCashCollected;
	while(TRUE)
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
		    printf("Total money received from ApplicationClerk[%d] = %d\n",i, appClerkMoney[i],0,"","");
		    totalCashCollected += appClerkMoney[i];
		  }
		  for (i = 0; i < numPicClerks; i++) {
		    printf("Total money received from PictureClerk[%d] = %d\n",i, picClerkMoney[i],0,"","");
		    totalCashCollected += picClerkMoney[i];
		  }
		  for (i = 0; i < numPassClerks; i++) {
		    printf("Total money received from PassportClerk[%d] = %d\n",i, passClerkMoney[i],0,"","");
		    totalCashCollected += passClerkMoney[i];
		  }
		  for (i = 0; i < numCashClerks; i++) {
		    printf("Total money received from Cashier[%d] = %d\n",i, cashClerkMoney[i],0,"","");
		    totalCashCollected += cashClerkMoney[i];
		  }

		  printf("Total money made by office = %d\n", totalCashCollected,0,0,"","");
		  if (TESTING) {
		    Exit(0);
		  } else {
		    break;
		  }
		}

		tprintf("Manager: Time to slavedrive my clerks. Checking the lines...\n",0,0,0,"","");

		/*check AppLineLengths*/	
		/*appPicLineLock->Acquire();*/
		Acquire(appPicLineLock);
		tprintf("Manager: I spy [%d] customers in the AppLine\n", (regAppLineLength+privAppLineLength),0,0,"","");
		if(regAppLineLength + privAppLineLength > 3)
		{	
			tprintf("Manager: Making sure the AppClerks are working\n",0,0,0,"","");
			for(i = 0; i < MAX_APP_CLERKS; i++)
			{
				/*put onbreak clerks to work*/
				if(appClerkStatuses[i] == CLERK_ON_BREAK)
				{	
					tprintf("Manager: Whipping AppClerk[%d] back to work\n",i,0,0,"","");
					/*appClerkLocks[i]->Acquire();		*/
					Acquire(appClerkLocks[i]);
					appClerkStatuses[i] = CLERK_COMING_BACK;
					/*appClerkCVs[i]->Signal(appClerkLocks[i]);*/
					Signal(appClerkCVs[i], appClerkLocks[i]);
					printf("Manager calls an ApplicationClerk back from break\n",0,0,0,"","");
					/*appClerkLocks[i]->Release();*/
					Release(appClerkLocks[i]);
					break;
				}
			}
			tprintf("Manager: Checking next line...\n",0,0,0,"","");
		}	
		else if(regAppLineLength+privAppLineLength > 0)
		{
			wakeup = -1;
			tprintf("Manager: Making sure at least one AppClerk is working\n",0,0,0,"","");
			for(i = 0; i < MAX_APP_CLERKS; i++)
			{
				/*search to see if any clerks are available*/
				if(appClerkStatuses[i] == CLERK_AVAILABLE)
				{	
					tprintf("Manager: AppClerk[%d] is available. Moving on.\n",i,0,0,"","");					
					wakeup = -1;
					break;
				}
				else if (appClerkStatuses[i] == CLERK_BUSY)
				{
					tprintf("Manager: AppClerk[%d] is busy. Moving on. \n", i,0,0,"","");
					wakeup = -1;
					break;
				}
				else if (appClerkStatuses[i] == CLERK_COMING_BACK)
				{
					tprintf("Manager: AppClerk[%d] is coming back. Moving on. \n", i,0,0,"","");
					wakeup = -1;
					break;
				}  
				else if(appClerkStatuses[i] == CLERK_ON_BREAK)
				{
					/*we'll wake up this clerk if no one is available*/
					tprintf("Manager: Tagging AppClerk[%d] to work...\n", i,0,0,"","");
					wakeup = i;
				}
			}
			if(wakeup >= 0)
			{
				tprintf("Manager: Whipping AppClerk[%d] back to work\n",wakeup,0,0,"","");
				/*appClerkLocks[wakeup]->Acquire();	*/
				Acquire(appClerkLocks[wakeup]);
				appClerkStatuses[wakeup] = CLERK_COMING_BACK;
				/*appClerkCVs[wakeup]->Signal(appClerkLocks[wakeup]);*/
				Signal(appClerkCVs[wakeup], appClerkLocks[wakeup]);
				printf("Manager calls back an ApplicationClerk from break\n",0,0,0,"","");
				/*appClerkLocks[wakeup]->Release();	*/
				Release(appClerkLocks[wakeup]);
			}			
			tprintf("Manager: Checking next line...\n",0,0,0,"","");
		}


		tprintf("Manager: I spy [%d] customers in the PicLine\n", (regPicLineLength+privPicLineLength),0,0,"","");
		if(regPicLineLength + privPicLineLength > 3)
		{
			tprintf("Manager: Making sure the PicClerks are working\n",0,0,0,"","");
			for(i = 0; i < MAX_PIC_CLERKS; i++)
			{
				/*put onbreak clerks to work*/
				if(picClerkStatuses[i] == CLERK_ON_BREAK)
				{	
					tprintf("Manager: Whipping PicClerk[%d] back to work\n",i,0,0,"","");
					/*picClerkLocks[i]->Acquire();	*/
					Acquire(picClerkLocks[i]);
					picClerkStatuses[i] = CLERK_COMING_BACK;
					/*picClerkCVs[i]->Signal(picClerkLocks[i]);*/
					Signal(picClerkCVs[i], picClerkLocks[i]);
					printf("Manager calls back a PictureClerk from break\n",0,0,0,"","");
					/*picClerkLocks[i]->Release();*/
					Release(picClerkLocks[i]);
					break;
				}
			}
			tprintf("Manager: Checking next line...\n",0,0,0,"","");
		}
		else if(regPicLineLength+privPicLineLength > 0)
		{
			int wakeup = -1;
			tprintf("Manager: Making sure at least one PicClerk is working\n",0,0,0,"","");
			for(i = 0; i < MAX_PIC_CLERKS; i++)
			{
				/*search to see if any clerks are available*/
				if(picClerkStatuses[i] == CLERK_AVAILABLE)
				{	
					tprintf("Manager: PicClerk[%d] is available. Moving on.\n",i,0,0,"","");					
					wakeup = -1;
					break;
				}
				else if (picClerkStatuses[i] == CLERK_BUSY)
				{
					tprintf("Manager: PicClerk[%d] is busy. Moving on. \n", i,0,0,"","");
					wakeup = -1;
					break;
				}
				else if (picClerkStatuses[i] == CLERK_COMING_BACK)
				{
					tprintf("Manager: PicClerk[%d] is coming back. Moving on. \n", i,0,0,"","");
					wakeup = -1;
					break;
				} 
				else if(picClerkStatuses[i] == CLERK_ON_BREAK)
				{
					tprintf("Manager: Tagging PicClerk[%d] to work...\n", i,0,0,"","");
					/*we'll wake up this clerk if no one is available*/
					wakeup = i;
				}
			}
			if(wakeup >= 0)
			{
				tprintf("Manager: Whipping PicClerk[%d] back to work\n",wakeup,0,0,"","");
				/*picClerkLocks[wakeup]->Acquire();*/
				Acquire(picClerkLocks[wakeup]);
				picClerkStatuses[wakeup] = CLERK_COMING_BACK;
				/*picClerkCVs[wakeup]->Signal(picClerkLocks[wakeup]);*/
				Signal(picClerkCVs[wakeup], picClerkLocks[wakeup]);
				printf("Manager calls back a PictureClerk from break\n",0,0,0,"","");
				/*picClerkLocks[wakeup]->Release();	*/
				Release(picClerkLocks[wakeup]);
			}
			tprintf("Manager: Checking next line...\n",0,0,0,"","");
		}
		/*appPicLineLock->Release();*/
		Release(appPicLineLock);

		/*passLineLock->Acquire();*/
		Acquire(passLineLock);
		tprintf("Manager: I spy [%d] customers in the PassLine\n", (regPassLineLength+privPassLineLength),0,0,"","");
		if(regPassLineLength + privPassLineLength > 3)
		{
			tprintf("Manager: Making sure the PassClerks are working\n",0,0,0,"","");
			for(i = 0; i < MAX_PASS_CLERKS; i++)
			{
				/*put onbreak clerks to work*/
				if(passClerkStatuses[i] == CLERK_ON_BREAK)
				{	
					tprintf("Manager: Whipping PassClerk[%d] back to work\n",i,0,0,"","");
					/*passClerkLocks[i]->Acquire();	*/
					Acquire(passClerkLocks[i]);
					passClerkStatuses[i] = CLERK_COMING_BACK;
					/*passClerkCVs[i]->Signal(passClerkLocks[i]);*/
					Signal(passClerkCVs[i], passClerkLocks[i]);
					printf("Manager calls back a PassportClerk from break\n",0,0,0,"","");
					/*passClerkLocks[i]->Release();*/
					Release(passClerkLocks[i]);
					break;
				}
			}
			tprintf("Manager: Checking next line...\n",0,0,0,"","");
		}
		else if(regPassLineLength+privPassLineLength > 0)
		{
			int wakeup = -1;
			tprintf("Manager: Making sure at least one PassClerk is working\n",0,0,0,"","");
			for(i = 0; i < MAX_PASS_CLERKS; i++)
			{
				/*search to see if any clerks are available*/
				if(passClerkStatuses[i] == CLERK_AVAILABLE)
				{	
					tprintf("Manager: PassClerk[%d] is available. Moving on.\n",i,0,0,"","");					
					wakeup = -1;
					break;
				}
				else if (passClerkStatuses[i] == CLERK_BUSY)
				{
					tprintf("Manager: AppClerk[%d] is busy. Moving on. \n", i,0,0,"","");
					wakeup = -1;
					break;
				}
				else if (passClerkStatuses[i] == CLERK_COMING_BACK)
				{
					tprintf("Manager: PassClerk[%d] is coming back. Moving on. \n", i,0,0,"","");
					wakeup = -1;
					break;
				} 
				else if(passClerkStatuses[i] == CLERK_ON_BREAK)
				{
					/*we'll wake up this clerk if no one is available*/
					tprintf("Manager: Tagging PassClerk[%d] to work...\n", i,0,0,"","");
					wakeup = i;
				}
			}
			if(wakeup >= 0)
			{
				tprintf("Manager: Whipping PassClerk[%d] back to work\n",wakeup,0,0,"","");
				/*passClerkLocks[wakeup]->Acquire();		*/
				Acquire(passClerkLocks[wakeup]);
				passClerkStatuses[wakeup] = CLERK_COMING_BACK;
				/*passClerkCVs[wakeup]->Signal(passClerkLocks[wakeup]);*/
				Signal(passClerkCVs[wakeup], passClerkLocks[wakeup]);
				printf("Manager calls back a PassportClerk from break\n",0,0,0,"","");
				/*passClerkLocks[wakeup]->Release();*/
				Release(passClerkLocks[wakeup]);
			}
			tprintf("Manager: Checking next line...\n",0,0,0,"","");
		}
		/*passLineLock->Release();*/
		Release(passLineLock);

		/*cashLineLock->Acquire();*/
		Acquire(cashLineLock);
		tprintf("Manager: I spy [%d] customers in the CashLine\n", (regCashLineLength),0,0,"","");
		if(regCashLineLength >= 3)
		{
			tprintf("Manager: Making sure the CashClerks are working\n",0,0,0,"","");
			for(i = 0; i < MAX_CASH_CLERKS; i++)
			{
				/*put onbreak clerks to work*/
				if(cashClerkStatuses[i] == CLERK_ON_BREAK)
				{	
					tprintf("Manager: Whipping CashClerk[%d] back to work\n",i,0,0,"","");
					/*cashClerkLocks[i]->Acquire();	*/
					Acquire(cashClerkLocks[i]);
					cashClerkStatuses[i] = CLERK_COMING_BACK;
					/*cashClerkCVs[i]->Signal(cashClerkLocks[i]);*/
					Signal(cashClerkCVs[i], cashClerkLocks[i]);
					printf("Manager calls back a cashier from break\n",0,0,0,"","");
					/*cashClerkLocks[i]->Release();*/
					Release(cashClerkLocks[i]);
					break;
				}
			}
			tprintf("Manager: Checking next line...\n",0,0,0,"","");
		}
		else if(regCashLineLength > 0)
		{
			int wakeup = -1;
			tprintf("Manager: Making sure at least one CashClerk is working\n",0,0,0,"","");
			for( i = 0; i < MAX_CASH_CLERKS; i++)
			{
				/*search to see if any clerks are available*/
				if(cashClerkStatuses[i] == CLERK_AVAILABLE)
				{	
					tprintf("Manager: CashClerk[%d] is available. Moving on.\n",i,0,0,"","");					
					wakeup = -1;
					break;
				}
				else if (cashClerkStatuses[i] == CLERK_BUSY)
				{
					tprintf("Manager: AppClerk[%d] is busy. Moving on. \n", i,0,0,"","");
					wakeup = -1;
					break;
				}
				else if (cashClerkStatuses[i] == CLERK_COMING_BACK)
				{
					tprintf("Manager: CashClerk[%d] is coming back. Moving on. \n", i,0,0,"","");
					wakeup = -1;
					break;
				} 
				else if(cashClerkStatuses[i] == CLERK_ON_BREAK)
				{
					/*we'll wake up this clerk if no one is available*/
					tprintf("Manager: Tagging CashClerk[%d] to work...\n", i,0,0,"","");
					wakeup = i;
				}
			}
			if(wakeup >= 0)
			{
				tprintf("Manager: Whipping CashClerk[%d] back to work\n",wakeup,0,0,"","");
				/*cashClerkLocks[wakeup]->Acquire();*/
				Acquire(cashClerkLocks[wakeup]);
				cashClerkStatuses[wakeup] = CLERK_COMING_BACK;
				/*cashClerkCVs[wakeup]->Signal(cashClerkLocks[wakeup]);*/
				Signal(cashClerkCVs[wakeup], cashClerkLocks[wakeup]);
				printf("Manager calls back a Cashier from break\n",0,0,0,"","");
				/*cashClerkLocks[wakeup]->Release();	*/
				Release(cashClerkLocks[wakeup]);
			}
			tprintf("Manager: Checking next line...\n",0,0,0,"","");
		}
		tprintf("Manager: Yielding to let other threads do work.\n",0,0,0,"","");
		/*cashLineLock->Release();*/
		Release(cashLineLock);
		for (i = 0; i < 50; i++) {
			Yield();
		}
	}
	Exit(0);
}

