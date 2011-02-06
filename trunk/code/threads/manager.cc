//manager.cc
#include "office.h"
#include "system.h"
#include <iostream>

void ManagerRun(int notUsed){
	
	printf("Manager: Time to slavedrive my clerks. Checking the lines...\n");
	
	//check AppLineLenghts	
	printf("Manager: I spy [%d] customers in the AppLine\n", (regAppLineLength+privAppLineLength));
	if(regAppLineLength + privAppLineLength > 3)
	{	
		printf("Manager: Making sure the AppClerks are working\n");
		appPicLineLock->Acquire();
		for(int x = 0; x < MAX_APP_CLERKS; x++)
		{
			//put onbreak clerks to work
			if(appClerkStatuses[x] == CLERK_ON_BREAK)
			{	
				printf("Manager: Whipping AppClerk[%d] back to work",x);
				appClerkLocks[x]->Acquire();				
				appClerkStatuses[x] = CLERK_AVAILABLE;
				appClerkCVs[x]->Signal(appClerkLocks[x]);
				printf("Manager: AppClerk[%x] is now available.",x);
				appClerkLocks[x]->Release();
			}
		}
	}	
	
	printf("Manager: I spy [%d] customers in the PicLine\n", (regPicLineLength+privPicLineLength));
	if(regPicLineLength + privPicLineLength > 3)
	{
		printf("Manager: Making sure the PicClerks are working\n");
		appPicLineLock->Acquire();
		for(int x = 0; x < MAX_PIC_CLERKS; x++)
		{
			//put onbreak clerks to work
			if(picClerkStatuses[x] == CLERK_ON_BREAK)
			{	
				printf("Manager: Whipping PicClerk[%d] back to work",x);
				picClerkLocks[x]->Acquire();				
				picClerkStatuses[x] = CLERK_AVAILABLE;
				picClerkCVs[x]->Signal(picClerkLocks[x]);
				printf("Manager: PicClerk[%x] is now available.",x);
				picClerkLocks[x]->Release();
			}
		}
	}
	
	printf("Manager: I spy [%d] customers in the PassLine\n", (regPassLineLength+privPassLineLength));
	if(regPassLineLength + privPassLineLength > 3)
	{
		printf("Manager: Making sure the PassClerks are working\n");
		passLineLock->Acquire();
		for(int x = 0; x < MAX_PASS_CLERKS; x++)
		{
			//put onbreak clerks to work
			if(passClerkStatuses[x] == CLERK_ON_BREAK)
			{	
				printf("Manager: Whipping PassClerk[%d] back to work",x);
				passClerkLocks[x]->Acquire();				
				passClerkStatuses[x] = CLERK_AVAILABLE;
				passClerkCVs[x]->Signal(passClerkLocks[x]);
				printf("Manager: PassClerk[%x] is now available.",x);
				passClerkLocks[x]->Release();
			}
		}
	}
	
	printf("Manager: I spy [%d] customers in the CashLine\n", (regPassLineLength+privPassLineLength));
	if(regCashLineLength >= 3)
	{
		printf("Manager: Making sure the CashClerks are working\n");
		cashLineLock->Acquire();
		for(int x = 0; x < MAX_CASH_CLERKS; x++)
		{
			//put onbreak clerks to work
			if(cashClerkStatuses[x] == CLERK_ON_BREAK)
			{	
				printf("Manager: Whipping CashClerk[%d] back to work",x);
				cashClerkLocks[x]->Acquire();				
				cashClerkStatuses[x] = CLERK_AVAILABLE;
				cashClerkCVs[x]->Signal(cashClerkLocks[x]);
				printf("Manager: CashClerk[%x] is now available.",x);
				cashClerkLocks[x]->Release();
			}
		}
	}
		
}
