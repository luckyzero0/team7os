//manager.cc
#include "office.h"
#include "system.h"
#include <iostream>

void tryToWakeupSenators();

void ManagerRun(int notUsed){
	
	while(true)
	{
	  tryToWakeupSenators();

	  customerOfficeLock->Acquire();
	  printf("Manager: There are [%d] Customer in the office.\n",customersInOffice);
	  if(customersInOffice == 0) {
	    customerOfficeLock->Release();
	    printf("Manager: No more customers in the store, we're done.\n");
	    for (int i = 0; i < MAX_CUSTOMERS; i++) {
	      printf("Customer[%2d]: AppFiled:%d, PicFiled:%d, PassFiled:%d, CashFiled:%d\n", i, appFiled[i], picFiled[i], passFiled[i], cashFiled[i]);
	    }
	    if(TESTING) {
	      printf("Manager: In test mode -- tabulating totals.\n");
	      int totalPassportMoney = 0;
	      for (int i = 0; i < MAX_CASH_CLERKS; i++) {
		totalPassportMoney += cashClerkMoney[i];
	      }
	      if (totalPassportMoney == numCustomers * 100) {
		exit(0);
	      } else {
		exit(1);
	      }
	    }
	    else {
	      break;
	    }
	  }
	  customerOfficeLock->Release();
		printf("Manager: Time to slavedrive my clerks. Checking the lines...\n");
		
		//check AppLineLenghts	
		appPicLineLock->Acquire();
		printf("Manager: I spy [%d] customers in the AppLine\n", (regAppLineLength+privAppLineLength));
		if(regAppLineLength + privAppLineLength > 3)
		{	
			printf("Manager: Making sure the AppClerks are working\n");
			for(int x = 0; x < MAX_APP_CLERKS; x++)
			{
				//put onbreak clerks to work
				if(appClerkStatuses[x] == CLERK_ON_BREAK)
				{	
					printf("Manager: Whipping AppClerk[%d] back to work\n",x);
					appClerkLocks[x]->Acquire();				
					appClerkStatuses[x] = CLERK_COMING_BACK;
					appClerkCVs[x]->Signal(appClerkLocks[x]);
					printf("Manager: AppClerk[%x] is now coming back.\n",x);
					appClerkLocks[x]->Release();
					break;
				}
			}
			printf("Manager: Checking next line...\n");
		}	
		else if(regAppLineLength+privAppLineLength > 0)
		{
			int wakeup = -1;
			printf("Manager: Making sure at least one AppClerk is working\n");
			for(int x = 0; x < MAX_APP_CLERKS; x++)
			{
				//search to see if any clerks are available
				if(appClerkStatuses[x] == CLERK_AVAILABLE)
				{	
					printf("Manager: AppClerk[%d] is available. Moving on.\n",x);					
					wakeup = -1;
					break;
				}
				else if (appClerkStatuses[x] == CLERK_BUSY)
				{
				  printf("Manager: AppClerk[%d] is busy. Moving on. \n", x);
				  wakeup = -1;
				  break;
				}
				else if (appClerkStatuses[x] == CLERK_COMING_BACK)
				{
				  printf("Manager: AppClerk[%d] is coming back. Moving on. \n", x);
				  wakeup = -1;
				  break;
				}  
				else if(appClerkStatuses[x] == CLERK_ON_BREAK)
				{
					//we'll wake up this clerk if no one is available
					printf("Manager: Tagging AppClerk[%d] to work...\n", x);
					wakeup = x;
				}
			}
			if(wakeup >= 0)
			{
				printf("Manager: Whipping AppClerk[%d] back to work\n",wakeup);
				appClerkLocks[wakeup]->Acquire();				
				appClerkStatuses[wakeup] = CLERK_COMING_BACK;
				appClerkCVs[wakeup]->Signal(appClerkLocks[wakeup]);
				printf("Manager: AppClerk[%x] is now coming back.\n",wakeup);
				appClerkLocks[wakeup]->Release();				
			}			
			printf("Manager: Checking next line...\n");
		}
	
		
		printf("Manager: I spy [%d] customers in the PicLine\n", (regPicLineLength+privPicLineLength));
		if(regPicLineLength + privPicLineLength > 3)
		{
			printf("Manager: Making sure the PicClerks are working\n");
			for(int x = 0; x < MAX_PIC_CLERKS; x++)
			{
				//put onbreak clerks to work
				if(picClerkStatuses[x] == CLERK_ON_BREAK)
				{	
					printf("Manager: Whipping PicClerk[%d] back to work\n",x);
					picClerkLocks[x]->Acquire();				
					picClerkStatuses[x] = CLERK_COMING_BACK;
					picClerkCVs[x]->Signal(picClerkLocks[x]);
					printf("Manager: PicClerk[%d] is now coming back.\n",x);
					picClerkLocks[x]->Release();
					break;
				}
			}
			printf("Manager: Checking next line...\n");
		}
		else if(regPicLineLength+privPicLineLength > 0)
		{
			int wakeup = -1;
			printf("Manager: Making sure at least one PicClerk is working\n");
			for(int x = 0; x < MAX_PIC_CLERKS; x++)
			{
				//search to see if any clerks are available
				if(picClerkStatuses[x] == CLERK_AVAILABLE)
				{	
					printf("Manager: PicClerk[%d] is available. Moving on.\n",x);					
					wakeup = -1;
					break;
				}
				else if (picClerkStatuses[x] == CLERK_BUSY)
				{
				  printf("Manager: PicClerk[%d] is busy. Moving on. \n", x);
				  wakeup = -1;
				  break;
				}
				else if (picClerkStatuses[x] == CLERK_COMING_BACK)
				{
				  printf("Manager: PicClerk[%d] is coming back. Moving on. \n", x);
				  wakeup = -1;
				  break;
				} 
				else if(picClerkStatuses[x] == CLERK_ON_BREAK)
				{
					printf("Manager: Tagging PicClerk[%d] to work...\n", x);
					//we'll wake up this clerk if no one is available
					wakeup = x;
				}
			}
			if(wakeup >= 0)
			{
				printf("Manager: Whipping PicClerk[%d] back to work\n",wakeup);
				picClerkLocks[wakeup]->Acquire();				
				picClerkStatuses[wakeup] = CLERK_COMING_BACK;
				picClerkCVs[wakeup]->Signal(picClerkLocks[wakeup]);
				printf("Manager: PicClerk[%x] is now coming back.\n",wakeup);
				picClerkLocks[wakeup]->Release();				
			}
			printf("Manager: Checking next line...\n");
		}
		appPicLineLock->Release();
		
		passLineLock->Acquire();
		printf("Manager: I spy [%d] customers in the PassLine\n", (regPassLineLength+privPassLineLength));
		if(regPassLineLength + privPassLineLength > 3)
		{
		        printf("Manager: Making sure the PassClerks are working\n");
			for(int x = 0; x < MAX_PASS_CLERKS; x++)
			{
				//put onbreak clerks to work
				if(passClerkStatuses[x] == CLERK_ON_BREAK)
				{	
					printf("Manager: Whipping PassClerk[%d] back to work\n",x);
					passClerkLocks[x]->Acquire();				
					passClerkStatuses[x] = CLERK_COMING_BACK;
					passClerkCVs[x]->Signal(passClerkLocks[x]);
					printf("Manager: PassClerk[%x] is now coming back.\n",x);
					passClerkLocks[x]->Release();
					break;
				}
			}
			printf("Manager: Checking next line...\n");
		}
		else if(regPassLineLength+privPassLineLength > 0)
		{
			int wakeup = -1;
			printf("Manager: Making sure at least one PassClerk is working\n");
			for(int x = 0; x < MAX_PASS_CLERKS; x++)
			{
				//search to see if any clerks are available
				if(passClerkStatuses[x] == CLERK_AVAILABLE)
				{	
					printf("Manager: PassClerk[%d] is available. Moving on.\n",x);					
					wakeup = -1;
					break;
				}
				else if (passClerkStatuses[x] == CLERK_BUSY)
				{
				  printf("Manager: AppClerk[%d] is busy. Moving on. \n", x);
				  wakeup = -1;
				  break;
				}
				else if (passClerkStatuses[x] == CLERK_COMING_BACK)
				{
				  printf("Manager: PassClerk[%d] is coming back. Moving on. \n", x);
				  wakeup = -1;
				  break;
				} 
				else if(passClerkStatuses[x] == CLERK_ON_BREAK)
				{
					//we'll wake up this clerk if no one is available
					printf("Manager: Tagging PassClerk[%d] to work...\n", x);
					wakeup = x;
				}
			}
			if(wakeup >= 0)
			{
				printf("Manager: Whipping PassClerk[%d] back to work\n",wakeup);
				passClerkLocks[wakeup]->Acquire();				
				passClerkStatuses[wakeup] = CLERK_COMING_BACK;
				passClerkCVs[wakeup]->Signal(passClerkLocks[wakeup]);
				printf("Manager: PassClerk[%x] is now coming back.\n",wakeup);
				passClerkLocks[wakeup]->Release();				
			}
			printf("Manager: Checking next line...\n");
		}
		passLineLock->Release();
		
		cashLineLock->Acquire();
		printf("Manager: I spy [%d] customers in the CashLine\n", (regCashLineLength));
		if(regCashLineLength >= 3)
		{
			printf("Manager: Making sure the CashClerks are working\n");
			for(int x = 0; x < MAX_CASH_CLERKS; x++)
			{
				//put onbreak clerks to work
				if(cashClerkStatuses[x] == CLERK_ON_BREAK)
				{	
					printf("Manager: Whipping CashClerk[%d] back to work\n",x);
					cashClerkLocks[x]->Acquire();				
					cashClerkStatuses[x] = CLERK_COMING_BACK;
					cashClerkCVs[x]->Signal(cashClerkLocks[x]);
					printf("Manager: CashClerk[%x] is now coming back.\n",x);
					cashClerkLocks[x]->Release();
					break;
				}
			}
			printf("Manager: Checking next line...\n");
		}
		else if(regCashLineLength > 0)
		{
			int wakeup = -1;
			printf("Manager: Making sure at least one CashClerk is working\n");
			for(int x = 0; x < MAX_CASH_CLERKS; x++)
			{
				//search to see if any clerks are available
				if(cashClerkStatuses[x] == CLERK_AVAILABLE)
				{	
					printf("Manager: CashClerk[%d] is available. Moving on.\n",x);					
					wakeup = -1;
					break;
				}
				else if (cashClerkStatuses[x] == CLERK_BUSY)
				{
				  printf("Manager: AppClerk[%d] is busy. Moving on. \n", x);
				  wakeup = -1;
				  break;
				}
				else if (cashClerkStatuses[x] == CLERK_COMING_BACK)
				{
				  printf("Manager: CashClerk[%d] is coming back. Moving on. \n", x);
				  wakeup = -1;
				  break;
				} 
				else if(cashClerkStatuses[x] == CLERK_ON_BREAK)
				{
					//we'll wake up this clerk if no one is available
					printf("Manager: Tagging CashClerk[%d] to work...\n", x);
					wakeup = x;
				}
			}
			if(wakeup >= 0)
			{
				printf("Manager: Whipping CashClerk[%d] back to work\n",wakeup);
				cashClerkLocks[wakeup]->Acquire();				
				cashClerkStatuses[wakeup] = CLERK_COMING_BACK;
				cashClerkCVs[wakeup]->Signal(cashClerkLocks[wakeup]);
				printf("Manager: CashClerk[%x] is now coming back.\n",wakeup);
				cashClerkLocks[wakeup]->Release();				
			}
			printf("Manager: Checking next line...\n");
		}
		printf("Manager: Yielding to let other threads do work.\n");
		cashLineLock->Release();
		for (int i = 0; i < 50; i++) {
		  currentThread->Yield();
		}
	}
}

void tryToWakeupSenators() {
  senatorWaitingRoomLock->Acquire();
  if (senatorsInWaitingRoom > 0) {
    senatorWaitingRoomLock->Release();
    //acquire all line CVs
    customerOfficeLock->Acquire();
    if (customersInOffice > 0) { //if there are customers in the office, tell them to get the hell out.
      customerOfficeLock->Release();
      appPicLineLock->Acquire();
      passLineLock->Acquire();
      cashLineLock->Acquire();

      privAppLineCV->Broadcast(appPicLineLock);
      regAppLineCV->Broadcast(appPicLineLock);
      privPicLineCV->Broadcast(appPicLineLock);
      regPicLineCV->Broadcast(appPicLineLock);

      privPassLineCV->Broadcast(passLineLock);
      regPassLineCV->Broadcast(passLineLock);

      regCashLineCV->Broadcast(cashLineLock);

      cashLineLock->Release();
      passLineLock->Release();
      appPicLineLock->Release();

      customerOfficeLock->Acquire();
      while (customersInOffice > 0) {
	customerOfficeLock->Release();
	currentThread->Yield();
	customerOfficeLock->Acquire();
      }
    }

    customerOfficeLock->Release();

    senatorWaitingRoomLock->Acquire();
    senatorWaitingRoomCV->Broadcast(senatorWaitingRoomLock);
  }
  senatorWaitingRoomLock->Release();

  //might want some yields here
}
