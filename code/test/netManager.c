#include "officeData.h"
#include "utils.h"

void tryToWakeUpSenators();
void tryToWakeUpCustomers();
void tryToWakeUpClerks();

void initManagerData() {
	appPicLineLock = CreateLock("appPicLineLock",14);
	passLineLock = CreateLock("passLineLock",12);
	cashLineLock = CreateLock("cashLineLock",12);
	entryLock = CreateLock("entryLock", 9);

	regAppLineCV = CreateCondition("regAppLineCV",12);
	privAppLineCV = CreateCondition("privAppLineCV",13);
	regPicLineCV = CreateCondition("regPicLineCV",12);
	privPicLineCV = CreateCondition("privPicLineCV",13);
	regPassLineCV = CreateCondition("regPassLineCV",13);
	privPassLineCV = CreateCondition("privPassLineCV",14);
	regCashLineCV = CreateCondition("regCashLineCV",13);
	senatorWaitingRoomCV = CreateCondition("senatorWaitingRoomCV",20);
	customerWaitingRoomCV = CreateCondition("customerWaitingRoomCV",21);	
	managerWaitForCustomersCV = CreateCondition("managerWaitForCustomersCV", 25);

	appClerkStatuses = CreateMonitorArray("appClerkStatuses",16,NUM_OF_EACH_TYPE_OF_CLERK,0);
	picClerkStatuses = CreateMonitorArray("picClerkStatuses",16,NUM_OF_EACH_TYPE_OF_CLERK,0);
	appClerkLocks = CreateMonitorArray("appClerkLocks",13,NUM_OF_EACH_TYPE_OF_CLERK,-1);
	picClerkLocks = CreateMonitorArray("picClerkLocks",13,NUM_OF_EACH_TYPE_OF_CLERK,-1);
	appClerkCVs = CreateMonitorArray("appClerkCVs",11,NUM_OF_EACH_TYPE_OF_CLERK,-1);
	picClerkCVs = CreateMonitorArray("picClerkCVs",11,NUM_OF_EACH_TYPE_OF_CLERK,-1);

	passClerkStatuses = CreateMonitorArray("passClerkStatuses",17,NUM_OF_EACH_TYPE_OF_CLERK,0);
	passClerkLocks = CreateMonitorArray("passClerkLocks",14,NUM_OF_EACH_TYPE_OF_CLERK,-1);
	passClerkCVs = CreateMonitorArray("passClerkCVs",12,NUM_OF_EACH_TYPE_OF_CLERK,-1);

	cashClerkStatuses = CreateMonitorArray("cashClerkStatuses",17,NUM_OF_EACH_TYPE_OF_CLERK,0);
	cashClerkLocks = CreateMonitorArray("cashClerkLocks",14,NUM_OF_EACH_TYPE_OF_CLERK,-1);
	cashClerkCVs = CreateMonitorArray("cashClerkCVs",12,NUM_OF_EACH_TYPE_OF_CLERK,-1);

	regAppLineLength = CreateMonitor("regAppLineLength",16);
	privAppLineLength = CreateMonitor("privAppLineLength",17);
	regPicLineLength = CreateMonitor("regPicLineLength",16);
	privPicLineLength = CreateMonitor("privPicLineLength",17);
	regPassLineLength = CreateMonitor("regPassLineLength",17);
	privPassLineLength = CreateMonitor("privPassLineLength",18);
	regCashLineLength = CreateMonitor("regCashLineLength",17);
	senatorsInWaitingRoom = CreateMonitor("senatorsInWaitingRoom",21);
	senatorsInOffice = CreateMonitor("senatorsInOffice",16);
	customersInWaitingRoom = CreateMonitor("customersInWaitingRoom",22);
	customersInOffice = CreateMonitor("customersInOffice",17);


	appClerkMoney = CreateMonitorArray("appClerkMoney",13,NUM_OF_EACH_TYPE_OF_CLERK,0);
	picClerkMoney = CreateMonitorArray("picClerkMoney",13,NUM_OF_EACH_TYPE_OF_CLERK,0);
	passClerkMoney = CreateMonitorArray("passClerkMoney",14,NUM_OF_EACH_TYPE_OF_CLERK,0);
	cashClerkMoney = CreateMonitorArray("cashClerkMoney",14,NUM_OF_EACH_TYPE_OF_CLERK,0);
}

void tryToWakeUpCustomers(){ 
	/*senatorWaitingRoomLock->Acquire();*/
	tprintf("Manager: Seeing if there are customers to wake up...\n",0,0,0,"","");
	Acquire(entryLock);
	/*senatorOfficeLock->Acquire();*/
	if (GetMonitor(senatorsInWaitingRoom) + GetMonitor(senatorsInOffice) == 0){
		tprintf("Manager: There are no more senators in the office...\n", 0,0,0,"","");
		tprintf("Manager: Broadcasting to all customers in waiting. \n", 0,0,0,"","");
		Broadcast(customerWaitingRoomCV, entryLock);
	}
	Release(entryLock);	
	tprintf("Manager: Done checking if there are customers to wake up...\n",0,0,0,"","");

}

void tryToWakeUpSenators() {
	/*senatorWaitingRoomLock->Acquire();*/
	tprintf("Manager: Seeing if there are senators to wake up...\n",0,0,0,"","");
	Acquire(entryLock);
	if (GetMonitor(senatorsInWaitingRoom) > 0) {
		/*senatorWaitingRoomLock->Release();*/

		tprintf("Manager: There are senators in the waiting room! \n", 0,0,0,"","");
		/*acquire all line CVs*/
		/*entryLock->Acquire();*/

		if (GetMonitor(customersInOffice) > 0) { /*if there are customers in the office, tell them to get the hell out.*/
			tprintf("Manager: I need to tell all %d customers to GTFO\n", GetMonitor(customersInOffice),0,0,"","");
			/*entryLock->Release();*/

			tprintf("Manager: Acquiring appPicLineLock to wake up senators \n",0,0,0,"","");
			/*appPicLineLock->Acquire();*/
			Acquire(appPicLineLock);
			tprintf("Manager: Acquiring passLineLock\n",0,0,0,"","");
			/*passLineLock->Acquire();*/
			Acquire(passLineLock);
			tprintf("Manager: Acquiring cashLineLock\n",0,0,0,"","");
			/*cashLineLock->Acquire();*/
			Acquire(cashLineLock);

			/*Setting all lines to 0;*/
			SetMonitor(privAppLineLength, 0);
			SetMonitor(regAppLineLength, 0);
			SetMonitor(privPicLineLength, 0);
			SetMonitor(regPicLineLength, 0);

			SetMonitor(privPassLineLength, 0);
			SetMonitor(regPassLineLength, 0);

			SetMonitor(regCashLineLength, 0);



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


			/*entryLock->Acquire();*/
			Wait(managerWaitForCustomersCV, entryLock);

		}



		tprintf("Manager: Telling all %d senators to enter the office\n", GetMonitor(senatorsInWaitingRoom),0,0,"","");
		/*senatorWaitingRoomLock->Acquire();*/
		/*Acquire(senatorWaitingRoomLock);*/
		/*senatorWaitingRoomCV->Broadcast(senatorWaitingRoomLock);*/
		Broadcast(senatorWaitingRoomCV, entryLock);


		/*entryLock->Release();*/

	}
	Release(entryLock);
	tprintf("Manager: Done seeing if there are Senators to wake up...\n",0,0,0,"","");
}

void ManagerRun(){
	int i;
	int totalCashCollected;

	initManagerData();
	while(TRUE)
	{
		tprintf("Manager: Doing my job of waking people up.\n",0,0,0,"","");
		tryToWakeUpSenators();
		tryToWakeUpCustomers();

		tprintf("Manager: Checking the registers...\n",0,0,0,"","");
		totalCashCollected = 0;
		for (i = 0; i < NUM_OF_EACH_TYPE_OF_CLERK; i++) {
			totalCashCollected += GetMonitorArrayValue(cashClerkMoney, i);
		}
		if (totalCashCollected == (NUM_CUSTOMERS + NUM_SENATORS) * 100) {
			totalCashCollected = 0;
			/*we're done, print out everything*/
			for (i = 0; i < NUM_OF_EACH_TYPE_OF_CLERK; i++) {
				printf("Total money received from ApplicationClerk[%d] = %d\n",i, GetMonitorArrayValue(appClerkMoney, i),0,"","");
				totalCashCollected += GetMonitorArrayValue(appClerkMoney, i);
			}
			for (i = 0; i < NUM_OF_EACH_TYPE_OF_CLERK; i++) {
				printf("Total money received from PictureClerk[%d] = %d\n",i, GetMonitorArrayValue(appClerkMoney, i),0,"","");
				totalCashCollected += GetMonitorArrayValue(picClerkMoney, i);
			}
			for (i = 0; i < NUM_OF_EACH_TYPE_OF_CLERK; i++) {
				printf("Total money received from PassportClerk[%d] = %d\n",i, GetMonitorArrayValue(passClerkMoney, i),0,"","");
				totalCashCollected += GetMonitorArrayValue(passClerkMoney, i);
			}
			for (i = 0; i < NUM_OF_EACH_TYPE_OF_CLERK; i++) {
				printf("Total money received from Cashier[%d] = %d\n",i, GetMonitorArrayValue(cashClerkMoney, i),0,"","");
				totalCashCollected += GetMonitorArrayValue(cashClerkMoney, i);
			}

			printf("Total money made by office = %d\n", totalCashCollected,0,0,"","");

			/* Need to somehow get all the clerks to halt at this point. */
			Exit(0);
		}

		tprintf("Manager: Time to slavedrive my clerks. Checking the lines...\n",0,0,0,"","");
		tryToWakeUpClerks();
	}		
	Exit(0);
}


void tryToWakeUpClerks(){
	int i, wakeup;
	/*check AppLineLengths*/	
	/*appPicLineLock->Acquire();*/
	Acquire(appPicLineLock);
	printf("Manager: I spy [%d] customers in the AppLine\n", GetMonitor(regAppLineLength)+GetMonitor(privAppLineLength),0,0,"","");
	if(GetMonitor(regAppLineLength) + GetMonitor(privAppLineLength) > 3)
	{	
		tprintf("Manager: Making sure the AppClerks are working\n",0,0,0,"","");
		for(i = 0; i < NUM_OF_EACH_TYPE_OF_CLERK; i++)
		{
			/*put onbreak clerks to work*/
			if(GetMonitorArrayValue(appClerkStatuses, i) == CLERK_ON_BREAK)
			{	
				tprintf("Manager: Whipping AppClerk[%d] back to work\n",i,0,0,"","");
				/*appClerkLocks[i]->Acquire();		*/
				Acquire(GetMonitorArrayValue(appClerkLocks, i));
				SetMonitorArrayValue(appClerkStatuses, i, CLERK_COMING_BACK);
				/*appClerkCVs[i]->Signal(appClerkLocks[i]);*/
				Signal(GetMonitorArrayValue(appClerkCVs, i), GetMonitorArrayValue(appClerkLocks, i));
				printf("Manager calls an ApplicationClerk back from break\n",0,0,0,"","");
				/*appClerkLocks[i]->Release();*/
				Release(GetMonitorArrayValue(appClerkLocks, i));
				while(1); /* HACK */
				break;
			}
		}
		tprintf("Manager: Checking next line...\n",0,0,0,"","");
	}	
	else if(GetMonitor(regAppLineLength)+GetMonitor(privAppLineLength) > 0)
	{
		wakeup = -1;
		tprintf("Manager: Making sure at least one AppClerk is working\n",0,0,0,"","");
		for(i = 0; i < NUM_OF_EACH_TYPE_OF_CLERK; i++)
		{
			/*search to see if any clerks are available*/
			if(GetMonitorArrayValue(appClerkStatuses, i) == CLERK_AVAILABLE)
			{	
				tprintf("Manager: AppClerk[%d] is available. Moving on.\n",i,0,0,"","");					
				wakeup = -1;
				break;
			}
			else if (GetMonitorArrayValue(appClerkStatuses, i) == CLERK_BUSY)
			{
				tprintf("Manager: AppClerk[%d] is busy. Moving on. \n", i,0,0,"","");
				wakeup = -1;
				break;
			}
			else if (GetMonitorArrayValue(appClerkStatuses, i) == CLERK_COMING_BACK)
			{
				tprintf("Manager: AppClerk[%d] is coming back. Moving on. \n", i,0,0,"","");
				wakeup = -1;
				break;
			}  
			else if(GetMonitorArrayValue(appClerkStatuses, i) == CLERK_ON_BREAK)
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
			Acquire(GetMonitorArrayValue(appClerkLocks, wakeup));
			SetMonitorArrayValue(appClerkStatuses, wakeup, CLERK_COMING_BACK);
			/*appClerkCVs[wakeup]->Signal(appClerkLocks[wakeup]);*/
			Signal(GetMonitorArrayValue(appClerkCVs, wakeup), GetMonitorArrayValue(appClerkLocks, wakeup));
			printf("Manager calls back an ApplicationClerk from break\n",0,0,0,"","");
			/*appClerkLocks[wakeup]->Release();	*/
			Release(GetMonitorArrayValue(appClerkLocks, wakeup));
		}			
		tprintf("Manager: Checking next line...\n",0,0,0,"","");
	}


	printf("Manager: I spy [%d] customers in the PicLine\n", (GetMonitor(regPicLineLength)+GetMonitor(privPicLineLength)),0,0,"","");
	if(GetMonitor(regPicLineLength) + GetMonitor(privPicLineLength) > 3)
	{
		tprintf("Manager: Making sure the PicClerks are working\n",0,0,0,"","");
		for(i = 0; i < NUM_OF_EACH_TYPE_OF_CLERK; i++)
		{
			/*put onbreak clerks to work*/
			if(GetMonitorArrayValue(picClerkStatuses, i) == CLERK_ON_BREAK)
			{	
				tprintf("Manager: Whipping PicClerk[%d] back to work\n",i,0,0,"","");
				/*picClerkLocks[i]->Acquire();	*/
				Acquire(GetMonitorArrayValue(picClerkLocks, i));
				SetMonitorArrayValue(picClerkStatuses, i, CLERK_COMING_BACK);
				/*picClerkCVs[i]->Signal(picClerkLocks[i]);*/
				Signal(GetMonitorArrayValue(picClerkCVs, i), GetMonitorArrayValue(picClerkLocks, i));
				printf("Manager calls back a PictureClerk from break\n",0,0,0,"","");
				/*picClerkLocks[i]->Release();*/
				Release(GetMonitorArrayValue(picClerkLocks, i));
				break;
			}
		}
		tprintf("Manager: Checking next line...\n",0,0,0,"","");
	}
	else if(GetMonitor(regPicLineLength)+GetMonitor(privPicLineLength) > 0)
	{
		int wakeup = -1;
		tprintf("Manager: Making sure at least one PicClerk is working\n",0,0,0,"","");
		for(i = 0; i < NUM_OF_EACH_TYPE_OF_CLERK; i++)
		{
			/*search to see if any clerks are available*/
			if(GetMonitorArrayValue(picClerkStatuses, i) == CLERK_AVAILABLE)
			{	
				tprintf("Manager: PicClerk[%d] is available. Moving on.\n",i,0,0,"","");					
				wakeup = -1;
				break;
			}
			else if (GetMonitorArrayValue(picClerkStatuses, i) == CLERK_BUSY)
			{
				tprintf("Manager: PicClerk[%d] is busy. Moving on. \n", i,0,0,"","");
				wakeup = -1;
				break;
			}
			else if (GetMonitorArrayValue(picClerkStatuses, i) == CLERK_COMING_BACK)
			{
				tprintf("Manager: PicClerk[%d] is coming back. Moving on. \n", i,0,0,"","");
				wakeup = -1;
				break;
			} 
			else if(GetMonitorArrayValue(picClerkStatuses, i) == CLERK_ON_BREAK)
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
			Acquire(GetMonitorArrayValue(picClerkLocks, wakeup));
			SetMonitorArrayValue(picClerkStatuses, wakeup, CLERK_COMING_BACK);
			/*picClerkCVs[wakeup]->Signal(picClerkLocks[wakeup]);*/
			Signal(GetMonitorArrayValue(picClerkCVs, wakeup), GetMonitorArrayValue(picClerkLocks, wakeup));
			printf("Manager calls back a PictureClerk from break\n",0,0,0,"","");
			/*picClerkLocks[wakeup]->Release();	*/
			Release(GetMonitorArrayValue(picClerkLocks, wakeup));
		}
		tprintf("Manager: Checking next line...\n",0,0,0,"","");
	}
	/*appPicLineLock->Release();*/
	Release(appPicLineLock);

	/*passLineLock->Acquire();*/
	Acquire(passLineLock);
	printf("Manager: I spy [%d] customers in the PassLine\n", GetMonitor(regPassLineLength)+GetMonitor(privPassLineLength),0,0,"","");
	if(GetMonitor(regPassLineLength) + GetMonitor(privPassLineLength) > 3)
	{	
		tprintf("Manager: Making sure the PassClerks are working\n",0,0,0,"","");
		for(i = 0; i < NUM_OF_EACH_TYPE_OF_CLERK; i++)
		{
			/*put onbreak clerks to work*/
			if(GetMonitorArrayValue(passClerkStatuses, i) == CLERK_ON_BREAK)
			{	
				tprintf("Manager: Whipping PassClerk[%d] back to work\n",i,0,0,"","");
				/*passClerkLocks[i]->Acquire();		*/
				Acquire(GetMonitorArrayValue(passClerkLocks, i));
				SetMonitorArrayValue(passClerkStatuses, i, CLERK_COMING_BACK);
				/*passClerkCVs[i]->Signal(passClerkLocks[i]);*/
				Signal(GetMonitorArrayValue(passClerkCVs, i), GetMonitorArrayValue(passClerkLocks, i));
				printf("Manager calls an PassportClerk back from break\n",0,0,0,"","");
				/*passClerkLocks[i]->Release();*/
				Release(GetMonitorArrayValue(passClerkLocks, i));
				break;
			}
		}
		tprintf("Manager: Checking next line...\n",0,0,0,"","");
	}	
	else if(GetMonitor(regPassLineLength)+GetMonitor(privPassLineLength) > 0)
	{
		wakeup = -1;
		tprintf("Manager: Making sure at least one PassClerk is working\n",0,0,0,"","");
		for(i = 0; i < NUM_OF_EACH_TYPE_OF_CLERK; i++)
		{
			/*search to see if any clerks are available*/
			if(GetMonitorArrayValue(passClerkStatuses, i) == CLERK_AVAILABLE)
			{	
				tprintf("Manager: PassClerk[%d] is available. Moving on.\n",i,0,0,"","");					
				wakeup = -1;
				break;
			}
			else if (GetMonitorArrayValue(passClerkStatuses, i) == CLERK_BUSY)
			{
				tprintf("Manager: PassClerk[%d] is busy. Moving on. \n", i,0,0,"","");
				wakeup = -1;
				break;
			}
			else if (GetMonitorArrayValue(passClerkStatuses, i) == CLERK_COMING_BACK)
			{
				tprintf("Manager: PassClerk[%d] is coming back. Moving on. \n", i,0,0,"","");
				wakeup = -1;
				break;
			}  
			else if(GetMonitorArrayValue(passClerkStatuses, i) == CLERK_ON_BREAK)
			{
				/*we'll wake up this clerk if no one is available*/
				tprintf("Manager: Tagging PassClerk[%d] to work...\n", i,0,0,"","");
				wakeup = i;
			}
		}
		if(wakeup >= 0)
		{
			tprintf("Manager: Whipping PassClerk[%d] back to work\n",wakeup,0,0,"","");
			/*passClerkLocks[wakeup]->Acquire();	*/
			Acquire(GetMonitorArrayValue(passClerkLocks, wakeup));
			SetMonitorArrayValue(passClerkStatuses, wakeup, CLERK_COMING_BACK);
			/*passClerkCVs[wakeup]->Signal(passClerkLocks[wakeup]);*/
			Signal(GetMonitorArrayValue(passClerkCVs, wakeup), GetMonitorArrayValue(passClerkLocks, wakeup));
			printf("Manager calls back an PassportClerk from break\n",0,0,0,"","");
			/*passClerkLocks[wakeup]->Release();	*/
			Release(GetMonitorArrayValue(passClerkLocks, wakeup));
		}			
		tprintf("Manager: Checking next line...\n",0,0,0,"","");
	}
	Release(passLineLock);

	/*cashLineLock->Acquire();*/
	Acquire(cashLineLock);

	printf("Manager: I spy [%d] customers in the CashLine\n", GetMonitor(regCashLineLength),0,0,"","");
	if(GetMonitor(regCashLineLength) > 3)
	{	
		tprintf("Manager: Making sure the CashClerks are working\n",0,0,0,"","");
		for(i = 0; i < NUM_OF_EACH_TYPE_OF_CLERK; i++)
		{
			/*put onbreak clerks to work*/
			if(GetMonitorArrayValue(cashClerkStatuses, i) == CLERK_ON_BREAK)
			{	
				tprintf("Manager: Whipping CashClerk[%d] back to work\n",i,0,0,"","");
				/*cashClerkLocks[i]->Acquire();		*/
				Acquire(GetMonitorArrayValue(cashClerkLocks, i));
				SetMonitorArrayValue(cashClerkStatuses, i, CLERK_COMING_BACK);
				/*cashClerkCVs[i]->Signal(cashClerkLocks[i]);*/
				Signal(GetMonitorArrayValue(cashClerkCVs, i), GetMonitorArrayValue(cashClerkLocks, i));
				printf("Manager calls an CashierClerk back from break\n",0,0,0,"","");
				/*cashClerkLocks[i]->Release();*/
				Release(GetMonitorArrayValue(cashClerkLocks, i));
				break;
			}
		}
		tprintf("Manager: Checking next line...\n",0,0,0,"","");
	}	
	else if(GetMonitor(regCashLineLength) > 0)
	{
		wakeup = -1;
		tprintf("Manager: Making sure at least one CashClerk is working\n",0,0,0,"","");
		for(i = 0; i < NUM_OF_EACH_TYPE_OF_CLERK; i++)
		{
			/*search to see if any clerks are available*/
			if(GetMonitorArrayValue(cashClerkStatuses, i) == CLERK_AVAILABLE)
			{	
				tprintf("Manager: CashClerk[%d] is available. Moving on.\n",i,0,0,"","");					
				wakeup = -1;
				break;
			}
			else if (GetMonitorArrayValue(cashClerkStatuses, i) == CLERK_BUSY)
			{
				tprintf("Manager: CashClerk[%d] is busy. Moving on. \n", i,0,0,"","");
				wakeup = -1;
				break;
			}
			else if (GetMonitorArrayValue(cashClerkStatuses, i) == CLERK_COMING_BACK)
			{
				tprintf("Manager: CashClerk[%d] is coming back. Moving on. \n", i,0,0,"","");
				wakeup = -1;
				break;
			}  
			else if(GetMonitorArrayValue(cashClerkStatuses, i) == CLERK_ON_BREAK)
			{
				/*we'll wake up this clerk if no one is available*/
				tprintf("Manager: Tagging CashClerk[%d] to work...\n", i,0,0,"","");
				wakeup = i;
			}
		}
		if(wakeup >= 0)
		{
			tprintf("Manager: Whipping CashClerk[%d] back to work\n",wakeup,0,0,"","");
			/*cashClerkLocks[wakeup]->Acquire();	*/
			Acquire(GetMonitorArrayValue(cashClerkLocks, wakeup));
			SetMonitorArrayValue(cashClerkStatuses, wakeup, CLERK_COMING_BACK);
			/*cashClerkCVs[wakeup]->Signal(cashClerkLocks[wakeup]);*/
			Signal(GetMonitorArrayValue(cashClerkCVs, wakeup), GetMonitorArrayValue(cashClerkLocks, wakeup));
			printf("Manager calls back an CashierClerk from break\n",0,0,0,"","");
			/*cashClerkLocks[wakeup]->Release();	*/
			Release(GetMonitorArrayValue(cashClerkLocks, wakeup));
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

int main() {
	ManagerRun();
	Exit(0);
}