
#include "office.h"
#include "system.h"
#include <iostream>


static void doAppClerk(int* index, int* cashDollars);
static void doPicClerk(int* index, int* cashDollars);
static void doPassPortClerk(int* index, int* cashDollars);
static void doCashierClerk(int* index, int* cashDollars);

int waitAndRestart(int lineToExit, int index){
	tprintf("Customer [%d]: waitAndRestart entered\n", index);
	/*senatorWaitingRoomLock->Acquire();*/
	Acquire(senatorWaitingRoomLock);
	if (senatorsInWaitingRoom>0){
		if(lineToExit > -1){
			/*lineToExit->Release();*/
			Release(lineToExit);
		}
		tprintf("Customer [%d]: There are %d senators in waiting room\n", index, senatorsInWaitingRoom);
		printf("Customer [%d] leaves the Passport Office as a senator arrives.",index);
		/*senatorWaitingRoomLock->Release();*/
		Release(senatorWaitingRoomLock);
		/*customerWaitingRoomLock->Acquire();*/
		Acquire(customerWaitingRoomLock);
		/*customerOfficeLock->Acquire();*/
		Acquire(customerOfficeLock);
		customersInWaitingRoom++;
		customersInOffice--;
		/*customerOfficeLock->Release();*/
		Release(customerOfficeLock);
		tprintf("Customer [%d]: Waiting in the Waiting room..\n", index);
		/*customerWaitingRoomCV->Wait(customerWaitingRoomLock);*/
		Wait(customerWaitingRoomCV, customerWaitingRoomLock);
		tprintf("Customer [%d]: No more senators! Returning to passport office\n", index);
		/*customerOfficeLock->Acquire();*/
		Acquire(customerOfficeLock);
		customersInWaitingRoom--;
		customersInOffice++;
		/*customerOfficeLock->Release();*/
		Release(customerOfficeLock);
		/*customerWaitingRoomLock->Release();*/
		Release(customerWaitingRoomLock);
		if(lineToExit > -1){
			/*lineToExit->Acquire();*/
			Acquire(lineToExit);
		}
		return TRUE;
	}else{
		tprintf("Customer [%d]: There are no senators waiting.. carrying on\n", index);
		/*senatorWaitingRoomLock->Release();*/
		Release(senatorWaitingRoomLock);
		return FALSE;
	}

}

void CustomerRun(int index) {	

	int cashDollars;
	int clerkStatus;

	tprintf("Customer [%d]: Deciding whether to go to waiting room or straight into office...\n",index);
	/*senatorWaitingRoomLock->Acquire();*/
	Acquire(senatorWaitingRoomLock);
	/*senatorOfficeLock->Acquire();*/
	Acquire(senatorOfficeLock);
	while(senatorsInOffice+senatorsInWaitingRoom > 0){ /*check for senators as we enter office*/
		tprintf("Customer [%d]: Senator is in the office or waiting room.. have to wait...\n",index);
		/*senatorOfficeLock->Release();*/
		Release(senatorOfficeLock);
		/*senatorWaitingRoomLock->Release();*/
		Release(senatorWaitingRoomLock);
		/*customerWaitingRoomLock->Acquire();*/
		Acquire(customerWaitingRoomLock);
		customersInWaitingRoom++;
		tprintf("Customer [%d]: In the waiting room, taking a nap...\n", index);
		/*customerWaitingRoomCV->Wait(customerWaitingRoomLock);*/
		Wait(customerWaitingRoomCV, customerWaitingRoomLock);
		tprintf("Customer [%d]: No more senators! Time to enter the office\n",index);
		customersInWaitingRoom++;
		/*customerWaitingRoomLock->Release(); 
		senatorWaitingRoomLock->Acquire();
		senatorOfficeLock->Acquire();*/
		Release(customerWaitingRoomLock);
		Acquire(senatorWaitingRoomLock);
		Acquire(senatorOfficeLock);

	}

	tprintf("Customer [%d]: Entering the passport office...\n",index);

	/*customer start up code*/		
	cashDollars = ((rand() % 4) * 500) + 100;	
	clerkStatus;

	printf("Customer [%d] has money = [$%d]\n",index,cashDollars);
	/*customerOfficeLock->Acquire();*/
	Acquire(customerOfficeLock);
	customersInOffice++;
	/*customerOfficeLock->Release();
	senatorOfficeLock->Release();
	senatorWaitingRoomLock->Release();*/
	Release(customerWaitingRoomLock);
	Release(senatorWaitingRoomLock);
	Release(senatorOfficeLock);
	

	/*choose line*/		
	tprintf("Customer [%d]: Deciding between AppClerk and PictureClerk...\n", index);
	if(cashDollars > 100) /*find priveledged line with shortest length*/
	{
		/*appPicLineLock->Acquire();*/
		Acquire(appPicLineLock);
		printf("Customer [%d] finds the minimum [priveledged] queue for [ApplicationClerk/PictureClerk]\n", index);
		if(privAppLineLength<=privPicLineLength)
		{
			/*appPicLineLock->Release();*/
			Release(appPicLineLock);
			printf("Customer [%d] goes to [ApplicationClerk].\n",index);			
			doAppClerk(&index, &cashDollars);
			waitAndRestart(NULL, index); /*SENATOR*/
			doPicClerk(&index, &cashDollars);
			waitAndRestart(NULL, index); /*SENATOR*/
		}
		else
		{
			/*appPicLineLock->Release();*/
			Release(appPicLineLock);
			printf("Customer [%d] goes to [PictureClerk].\n",index);	
			doPicClerk(&index, &cashDollars);
			waitAndRestart(NULL, index); /*SENATOR*/
			doAppClerk(&index, &cashDollars);
			waitAndRestart(NULL, index); /*SENATOR*/
		}	
	}
	else/*find regular line with shortest length*/
	{
		/*appPicLineLock->Acquire();*/
		Acquire(appPicLineLock);
		printf("Customer [%d] finds the minimum [regular] queue for [ApplicationClerk/PictureClerk]\n", index);
		if(regAppLineLength<=regPicLineLength)
		{
			/*appPicLineLock->Release();*/
			Release(appPicLineLock);
			printf("Customer [%d] goes to [ApplicationClerk].\n",index);	
			doAppClerk(&index, &cashDollars);
			waitAndRestart(NULL, index); /*SENATOR*/
			doPicClerk(&index, &cashDollars);
			waitAndRestart(NULL, index); /*SENATOR*/
		}
		else
		{
			/*appPicLineLock->Release();*/
			Release(appPicLineLock);
			printf("Customer [%d] goes to [PictureClerk].\n",index);
			doPicClerk(&index, &cashDollars);
			waitAndRestart(NULL, index); /*SENATOR*/
			doAppClerk(&index, &cashDollars);
			waitAndRestart(NULL, index); /*SENATOR*/
		}	
	}

	/*hit up the passport clerk*/
	doPassPortClerk(&index, &cashDollars);
	

	/*proceed to cashier*/
	doCashierClerk(&index, &cashDollars);
}

static void doAppClerk(int* index, int* cashDollars)
{
	bool privLine = false;
	int myClerk, x;


	while(true)
	{	
		myClerk = -1;
		tprintf("Customer [%d]: Going to the AppClerk\n",*index);
		/*appPicLineLock->Acquire();	*/
		Acquire(appPicLineLock);	
		tprintf("Customer [%d]: What line should I choose for the AppClerk?\n",*index);
		/*check for senator*/
		if(*cashDollars > 100) /*get in a privledged line*/
		{					
			privLine = true;					
			tprintf("Customer [%d]: Priveleged line, baby. CashDollars = $%d\n",*index,*cashDollars);
			privAppLineLength++;
			tprintf("Customer [%d]: Waiting in the Priveledged Line for next available AppClerk\n",*index);
			/*privAppLineCV->Wait(appPicLineLock); //wait for clerk	*/
			Wait(privAppLineCV, appPicLineLock);					
		}
		else /*get in a normal line*/
		{
			tprintf("Customer [%d]: Regular line, suckas. CashDollars = $%d\n",*index,*cashDollars);
			regAppLineLength++;
			tprintf("Customer [%d]: Waiting in the Regular Line for next available AppClerk\n",*index);
			/*regAppLineCV->Wait(appPicLineLock); //wait for clerk	*/
			Wait(regAppLineCV, appPicLineLock);
		}

		tprintf("Customer [%d]: Finding available AppClerk...\n",*index);
		for(x = 0; x < MAX_APP_CLERKS; x++)
		{				
			if(appClerkStatuses[x] == CLERK_AVAILABLE) /*find available clerk*/
			{
				myClerk = x;					
				tprintf("Customer [%d]: Going to chill with AppClerk [%d]\n",*index,myClerk);					
				appClerkStatuses[myClerk] = CLERK_BUSY;					
				break;				
			}
			else
				tprintf("Customer [%d]: AppClerk [%d] is unavailable\n",*index,x);

		}
		if (myClerk == -1) 
		{
			if (waitAndRestart(appPicLineLock, *index))
			{           /*SENATOR*/
				if(privLine)
					printf("Customer [%d] joins the [priveleged] wait queue for [Application] Clerk.\n",*index);
				else
					printf("Customer [%d] joins the [regular] wait queue for [Application] Clerk.\n",*index);
				continue;
			} 
			else 
			{
				tprintf("APP_CLERK: ESCAPED WITHOUT AN AVAILABLE CLERK, AND WITHOUT GOING TO WAITING ROOM!\n");
				Exit(1); /*FAIL*/ 
			}	
		}
		/*appClerkLocks[myClerk]->Acquire();
		appPicLineLock->Release();*/
		Acquire(appClerkLocks[myClerk]);
		Release(appPicLineLock);							
		if(privLine)
		{
			printf("Customer [%d] is willing to pay $500 to ApplicationClerk [%d] for moving ahead in line\n",*index, myClerk);				
			appClerkMoney[myClerk] += 500;
			appClerkBribed[myClerk] = true;
			*cashDollars -= 500;
		}
		tprintf("Customer [%d]: Interacting with AppClerk [%d]\n",*index,myClerk);
		/*interact with clerk	*/
		printf("Customer [%d] gives application to ApplicationClerk [%d] = [SSN: %d].\n", *index,myClerk,*index);		
		appClerkSSNs[myClerk] = *index; 	
		/*appClerkCVs[myClerk]->Signal(appClerkLocks[myClerk]);*/
		Signal(appClerkCVs[myClerk], appClerkLocks[myClerk]);
		/*appClerkCVs[myClerk]->Wait(appClerkLocks[myClerk]);*/
		Wait(appClerkCVs[myClerk], appClerkLocks[myClerk]);
		printf("Customer [%d] is informed by ApplicationClerk [%d] that the application has been filed.\n", *index, myClerk);
		tprintf("Customer [%d]: Done and done.\n",*index);
		/*appClerkLocks[myClerk]->Release();*/
		Release(appClerkLocks[myClerk]);
		tprintf("Customer [%d]: Going to next clerk...\n",*index);
		break;
	}
}

static void doPicClerk(int* index, int* cashDollars)
{

	bool privLine = false;
	int myClerk, x;
	
	while(true)
	{	
		myClerk = -1;
		tprintf("Customer [%d]: Going to the PicClerk\n",*index);
		/*appPicLineLock->Acquire();	*/
		Acquire(appPicLineLock);
		tprintf("Customer [%d]: What line should I choose for the PicClerk?\n",*index);
		/*check for senator*/
		if(*cashDollars > 100) /*get in a privledged line*/
		{						
			privLine = true;
			tprintf("Customer [%d]: Priveleged line, baby. CashDollars = $%d\n",*index,*cashDollars);
			privPicLineLength++;
			tprintf("Customer [%d]: Waiting in the Priveledged Line for next available PicClerk\n",*index);
			/*privPicLineCV->Wait(appPicLineLock);		*/
			Wait(privPicLineCV, appPicLineLock);	
		}
		else /*get in a normal line*/
		{
			tprintf("Customer [%d]: Regular line, suckas. CashDollars = $%d\n",*index,*cashDollars);
			regPicLineLength++;
			tprintf("Customer [%d]: Waiting in the Regluar Line for next available PicClerk\n",*index);
			/*regPicLineCV->Wait(appPicLineLock);			*/
			Wait(regPicLineCV, appPicLineLock);
		}
		tprintf("Customer [%d]: Finding available PicClerk...\n",*index);
		for(x = 0; x < MAX_PIC_CLERKS; x++)
		{					

			if(picClerkStatuses[x] == CLERK_AVAILABLE)
			{
				myClerk = x;					
				tprintf("Customer [%d]: Going to chill with PicClerk[%d]\n",*index,myClerk);					
				picClerkStatuses[myClerk] = CLERK_BUSY;					
				break;				
			}
			else
				tprintf("Customer [%d]: PicClerk [%d] is unavailable\n",*index,x);
		}
		if (myClerk == -1) 
		{
			if (waitAndRestart(appPicLineLock, *index))
			{           /*SENATOR*/
				if(privLine)
					printf("Customer [%d] joins the [priveleged] wait queue for [Picture] Clerk.\n",*index);
				else
					printf("Customer [%d] joins the [regular] wait queue for [Picture] Clerk.\n",*index);
				continue;
			} 
			else 
			{
				tprintf("APP_CLERK: ESCAPED WITHOUT AN AVAILABLE CLERK, AND WITHOUT GOING TO WAITING ROOM!\n");
				Exit(1); /*FAIL*/ 
			}	
		}
		/*picClerkLocks[myClerk]->Acquire();
		appPicLineLock->Release();*/
		Acquire(picClerkLocks[myClerk]);
		Release(appPicLineLock);
						
		picClerkSSNs[myClerk] = *index;		
		if(privLine)
		{
			printf("Customer [%d] is willing to pay $500 to PictureClerk[%d] for moving ahead in line\n",*index, myClerk);				
			picClerkMoney[myClerk] += 500;
			picClerkBribed[myClerk] = true;
			*cashDollars -= 500;
		}
		tprintf("Customer [%d] Goes to PictureClerk[%d]\n",*index,myClerk);
		/*interact with clerk, loop enabled picture to be taken multiple times*/
		while(happyWithPhoto[myClerk] == FALSE)
		{

			tprintf("Customer [%d]: Getting my picture taken...\n",*index);
			/*picClerkCVs[myClerk]->Signal(picClerkLocks[myClerk]);
			picClerkCVs[myClerk]->Wait(picClerkLocks[myClerk]);	*/
			Signal(picClerkCVs[myClerk], picClerkLocks[myClerk]);
			Wait(picClerkCVs[myClerk], picClerkLocks[myClerk]);	

			/*did I like my picture?*/
			if(rand()%10 > 5)
			{
				happyWithPhoto[myClerk] = TRUE;
				printf("Customer [%d] [likes] the picture provided by PictureClerk[%d]\n", *index,myClerk);
			}
			else
			{
				printf("Customer [%d] [dislikes] the picture provided by PictureClerk[%d]\n", *index,myClerk);
				printf("The picture of Customer [%d] will be taken again.\n", *index);
			}		

			/*picClerkCVs[myClerk]->Signal(picClerkLocks[myClerk]);	
			picClerkCVs[myClerk]->Wait(picClerkLocks[myClerk]);		*/
			Signal(picClerkCVs[myClerk], picClerkLocks[myClerk]);
			Wait(picClerkCVs[myClerk], picClerkLocks[myClerk]);				

		}

		happyWithPhoto[myClerk] = FALSE; /*needs to be reset for future customers*/

		tprintf("Customer [%d]: Picture taken. Like a boss.\n",*index);
		/*picClerkLocks[myClerk]->Release();			*/
		Release(picClerkLocks[myClerk]);

		tprintf("Customer [%d]: Done and done.\n",*index);											
		break;
	}
}

static void doPassPortClerk(int *index, int* cashDollars){	

	bool privLined = false;
	bool bribed = false;
	int myClerk, x, rando;

	while(true)
	{	
		if (waitAndRestart(NULL, *index)){
			continue;
		}
		myClerk = -1;
		tprintf("Customer [%d]: Going to the PassportClerk\n",*index);
		tprintf("Customer [%d]: Acquiring PassLineLock...\n",*index);		
		/*passLineLock->Acquire();*/
		Acquire(passLineLock);
		printf("Customer [%d]: What line should I choose for the PassportClerk?\n",*index);
		/*check for senator*/
		if(*cashDollars > 100 || privLined) /*get in a privledged line*/
		{								
			privLined = true;				
			tprintf("Customer [%d]: Priveleged line, baby. CashDollars = $%d\n",*index,*cashDollars);
			privPassLineLength++;
			tprintf("Customer [%d]: Waiting in the Priveledged Line for next available PassportClerk\n",*index);
			/*privPassLineCV->Wait(passLineLock);*/
			Wait(privPassLineCV, passLineLock);			
		}
		else /*get in a normal line*/
		{
			tprintf("Customer [%d]: Regular line, suckas. CashDollars = $%d\n",*index,*cashDollars);
			regPassLineLength++;
			tprintf("Customer [%d]: Waiting in the Regular Line for next available PassportClerk\n",*index);
			/*regPassLineCV->Wait(passLineLock);		*/
			Wait(regPassLineCV, passLineLock);	
		}
		tprintf("Customer [%d]: Finding available PassClerk...\n",*index);
		for(x = 0; x < MAX_PASS_CLERKS; x++)
		{				
			if(passClerkStatuses[x] == CLERK_AVAILABLE)
			{
				myClerk = x;					
				tprintf("Customer [%d]: Going to chill with PassClerk[%d]\n",*index,myClerk);					
				passClerkStatuses[myClerk] = CLERK_BUSY;					
				break;				
			}
			else
				tprintf("Customer [%d]: PassClerk[%d] is unavailable\n",*index,x);

		}
		if (myClerk == -1) 
		{
			if (waitAndRestart(passLineLock, *index))
			{           /*SENATOR*/
				if(privLined)
					printf("Customer [%d] joins the [priveleged] wait queue for [Passport] Clerk.\n",*index);
				else
					printf("Customer [%d] joins the [regular] wait queue for [Passport] Clerk.\n",*index);
				continue;
			} 
			else 
			{
				tprintf("APP_CLERK: ESCAPED WITHOUT AN AVAILABLE CLERK, AND WITHOUT GOING TO WAITING ROOM!\n");
				Exit(1); /*FAIL*/ 
			}	
		}
		/*passClerkLocks[myClerk]->Acquire();*/
		Acquire(passClerkLocks[myClerk]);
		/*passLineLock->Release();	*/
		Release(passLineLock);						
		passClerkSSNs[myClerk] = *index;
		if(privLined && !bribed)
		{
			bribed = true;
			printf("Customer [%d] is willing to pay $500 to PassportClerk[%d] for moving ahead in line\n",*index,myClerk);
			passClerkMoney[myClerk] += 500;
			passClerkBribed[myClerk] = true;
			*cashDollars -= 500;
		}
		printf("Customer [%d] goes to PassportClerk[%d]\n",*index,myClerk);
		/*interact with clerk		*/	
		/*passClerkCVs[myClerk]->Signal(passClerkLocks[myClerk]);
		passClerkCVs[myClerk]->Wait(passClerkLocks[myClerk]);*/
		Signal(passClerkCVs[myClerk], passClerkLocks[myClerk]);
		Wait(passClerkCVs[myClerk], passClerkLocks[myClerk]);
		/*get passport from clerk, if not ready, go to the back of the line?*/
		if(!passPunish[myClerk])
		{
			printf("Customer [%d] is [certified] by PassportClerk[%d]\n",*index,myClerk);
			tprintf("Customer [%d]: Passport. GET!.\n", *index);						
			tprintf("Customer [%d]: Done and done.\n",*index);
			/*passClerkLocks[myClerk]->Release();*/
			Release(passClerkLocks[myClerk]);
			tprintf("Customer [%d]: Going to next clerk...\n",*index);
			break;				
		}
		/*passClerkLocks[myClerk]->Release();*/
		Release(passClerkLocks[myClerk]);
		printf("Customer [%d] is [not certified] by PassportClerk[%d]\n",*index,myClerk);
		printf("Customer [%d] is being forced to wait by PassportClerk[%d]\n",*index,myClerk);
		rando = rand()%80+20;					
		for(x = 0; x < rando; x++)
			currentThread->Yield();															
	}

}


static void doCashierClerk(int* index, int* cashDollars)
{	
	int myClerk, x, rando;
	
	while(true)
	{	
		if (waitAndRestart(NULL, *index)){
			continue;
		}
		myClerk = -1;
		tprintf("Customer [%d]: Going to the CashClerk\n",*index);	
		/*cashLineLock->Acquire();	*/
		Acquire(cashLineLock);
		
		regCashLineLength++;
		tprintf("Customer [%d]: Waiting in the Regular Line for next available CashClerk\n",*index);
		/*regCashLineCV->Wait(cashLineLock);	*/
		Wait(regCashLineCV, cashLineLock);		
				
		tprintf("Customer[%d]: Finding available CashClerk...\n",*index);
		for(x = 0; x < MAX_CASH_CLERKS; x++)
		{				
			if(cashClerkStatuses[x] == CLERK_AVAILABLE)
			{
				myClerk = x;					
				tprintf("Customer [%d]: Going to chill with CashClerk[%d]\n",*index,myClerk);					
				cashClerkStatuses[myClerk] = CLERK_BUSY;					
				break;				
			}
			else
				tprintf("Customer [%d]: CashClerk[%d] is unavailable\n",*index,x);

		}	
		if (myClerk == -1) 
		{
			if (waitAndRestart(cashLineLock, *index))
			{           /*SENATOR*/				
				printf("Customer [%d] joins the [regular] wait queue for [Cashier] Clerk.\n",*index);
				continue;
			} 
			else 
			{
				tprintf("APP_CLERK: ESCAPED WITHOUT AN AVAILABLE CLERK, AND WITHOUT GOING TO WAITING ROOM!\n");
				Exit(1); /*FAIL */
			}	
		}
		
		/*cashClerkLocks[myClerk]->Acquire();
		cashLineLock->Release();		*/
		Acquire(cashClerkLocks[myClerk]);
		Release(cashLineLock);
							
		cashClerkSSNs[myClerk] = *index;			
		printf("Customer [%d] goes to CashClerk[%d]\n",*index,myClerk);			
		/*interact with clerk			*/
		/*cashClerkCVs[myClerk]->Signal(cashClerkLocks[myClerk]);
		cashClerkCVs[myClerk]->Wait(cashClerkLocks[myClerk]);*/
		Signal(cashClerkCVs[myClerk], cashClerkLocks[myClerk]);
		Wait(cashClerkCVs[myClerk], cashClerkLocks[myClerk]);
		
		/*pay for passport. If it's not processed, get back in line*/
		if(!cashPunish[myClerk])
		{				
			printf("Customer [%d] gets [valid] certification by Cashier[%d]\n",*index,myClerk);
			printf("Customer [%d] pays $100 to Cashier[%d] for their passport\n",*index,myClerk);
			cashClerkMoney[myClerk] += 100;				
			*cashDollars-=100;
			/*cashClerkCVs[myClerk]->Signal(cashClerkLocks[myClerk]);
			cashClerkCVs[myClerk]->Wait(cashClerkLocks[myClerk]);*/
			Signal(cashClerkCVs[myClerk], cashClerkLocks[myClerk]);
			Wait(cashClerkCVs[myClerk], cashClerkLocks[myClerk]);
			
			tprintf("Customer [%d]: Passport paid for like a pro. CashDollars = [$%d]\n", *index, *cashDollars);											
			/*cashClerkLocks[myClerk]->Release();*/
			Release(cashClerkLocks[myClerk]);
			
			printf("Customer [%d] leaves the passport office...\n",*index);
			/*customerOfficeLock->Acquire();*/
			Acquire(customerOfficeLock);
			customersInOffice--;
			/*customerOfficeLock->Release();*/
			Release(customerOfficeLock);
			break;				
		}
		/*cashClerkLocks[myClerk]->Release();*/
		Release(cashClerkLocks[myClerk]);
		printf("Customer [%d] gets [invalid] certification by Cashier[%d]\n",*index,myClerk);
		printf("Customer [%d] is punished to wait by Cashier[%d]\n",*index,myClerk);
		tprintf("Customer [%d]: NOT READY!? Going back to the end of the line...\n",*index);
		rando = rand()%80+20;					
		for(x = 0; x < rando; x++)
			currentThread->Yield();																				
	}
}
