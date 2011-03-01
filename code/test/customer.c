
#include "office.h"
#include "utils.h"

static void doAppClerk(int* index, int* cashDollars, int needToAcquire);
static void doPicClerk(int* index, int* cashDollars, int needToAcquire);
static void doPassPortClerk(int* index, int* cashDollars);
static void doCashierClerk(int* index, int* cashDollars);

int waitAndRestart(LockID lineToExit, int index){
	tprintf("Customer [%d]: waitAndRestart entered\n", index,0,0,"","");
	/*senatorWaitingRoomLock->Acquire();*/
	Acquire(entryLock);
	if (senatorsInWaitingRoom>0){
		if(lineToExit > -1){
			/*lineToExit->Release();*/
			Release(lineToExit);
		}
		tprintf("Customer [%d]: There are %d senators in waiting room\n", index, senatorsInWaitingRoom,0,"","");
		printf("Customer [%d] leaves the Passport Office as a senator arrives.\n",index,0,0,"","");
		/*senatorWaitingRoomLock->Release();*/
		customersInWaitingRoom++;
		customersInOffice--;
		Wait(customerWaitingRoomCV, entryLock);
		customersInWaitingRoom--;
		customersInOffice++;
		Release(entryLock);
		if(lineToExit > -1){
			/*lineToExit->Acquire();*/
			Acquire(lineToExit);
		}
		return TRUE;
	}else{
		tprintf("Customer [%d]: There are no senators waiting.. carrying on\n", index,0,0,"","");
		/*senatorWaitingRoomLock->Release();*/
		Release(entryLock);
		return FALSE;
	}

}

void CustomerRun() {	

	int cashDollars;
	int clerkStatus;
	int index;
	int tid;

	tid = GetThreadID();

	Acquire(customerSenatorUIDLock);
	index = customerSenatorUID++;
	Release(customerSenatorUIDLock);

	tprintf("Customer [%d]: Deciding whether to go to waiting room or straight into office...\n",index,0,0,"","");
	/*senatorWaitingRoomLock->Acquire();*/
	Acquire(entryLock);
	while(senatorsInOffice+senatorsInWaitingRoom > 0){ /*check for senators as we enter office*/
		tprintf("Customer [%d]: Senator is in the office or waiting room.. have to wait...\n",index,0,0,"","");
		customersInWaitingRoom++;
		tprintf("Customer [%d]: In the waiting room, taking a nap...\n", index,0,0,"","");
		/*customerWaitingRoomCV->Wait(customerWaitingRoomLock);*/
		Wait(customerWaitingRoomCV, entryLock);
		tprintf("Customer [%d]: No more senators! Time to enter the office\n",index,0,0,"","");
		customersInWaitingRoom--;
	}

	tprintf("Customer [%d]: Entering the passport office...\n",index,0,0,"","");

	/*customer start up code*/		
	cashDollars = ((Rand() % 4) * 500) + 100;	

	printf("Customer [%d] has money = [$%d] ... tid = %d\n",index,cashDollars,tid,"","");
	customersInOffice++;
	Release(entryLock);
	

	/*choose line*/		
	tprintf("Customer [%d]: Deciding between AppClerk and PictureClerk...\n", index,0,0,"","");
	if(cashDollars > 100) /*find priveledged line with shortest length*/
	{
		/*appPicLineLock->Acquire();*/
		Acquire(appPicLineLock);
		printf("Customer [%d] finds the minimum [priveledged] queue for [ApplicationClerk/PictureClerk]\n", index,0,0,"","");
		if(privAppLineLength<=privPicLineLength)
		{
			/*appPicLineLock->Release();*/
			printf("Customer [%d] goes to [ApplicationClerk].\n",index,0,0,"","");			
			doAppClerk(&index, &cashDollars, FALSE);
			waitAndRestart(-1, index); /*SENATOR*/
			doPicClerk(&index, &cashDollars, TRUE);
			waitAndRestart(-1, index); /*SENATOR*/
		}
		else
		{
			/*appPicLineLock->Release();*/
			printf("Customer [%d] goes to [PictureClerk].\n",index,0,0,"","");	
			doPicClerk(&index, &cashDollars, FALSE);
			waitAndRestart(-1, index); /*SENATOR*/
			doAppClerk(&index, &cashDollars, TRUE);
			waitAndRestart(-1, index); /*SENATOR*/
		}	
	}
	else/*find regular line with shortest length*/
	{
		/*appPicLineLock->Acquire();*/
		Acquire(appPicLineLock);
		printf("Customer [%d] finds the minimum [regular] queue for [ApplicationClerk/PictureClerk]\n", index,0,0,"","");
		if(regAppLineLength<=regPicLineLength)
		{
			/*appPicLineLock->Release();*/
			printf("Customer [%d] goes to [ApplicationClerk].\n",index,0,0,"","");	
			doAppClerk(&index, &cashDollars, FALSE);
			waitAndRestart(-1, index); /*SENATOR*/
			doPicClerk(&index, &cashDollars, TRUE);
			waitAndRestart(-1, index); /*SENATOR*/
		}
		else
		{
			/*appPicLineLock->Release();*/
			printf("Customer [%d] goes to [PictureClerk].\n",index,0,0,"","");
			doPicClerk(&index, &cashDollars, FALSE);
			waitAndRestart(-1, index); /*SENATOR*/
			doAppClerk(&index, &cashDollars, TRUE);
			waitAndRestart(-1, index); /*SENATOR*/
		}	
	}

	/*hit up the passport clerk*/
	doPassPortClerk(&index, &cashDollars);
	

	/*proceed to cashier*/
	doCashierClerk(&index, &cashDollars);

	Exit(0);
}

static void doAppClerk(int* index, int* cashDollars, int needToAcquire)
{
	bool privLine = FALSE;
	int myClerk, x;


	while(TRUE)
	{	
		myClerk = -1;
		tprintf("Customer [%d]: Going to the AppClerk\n",*index,0,0,"","");
		/*appPicLineLock->Acquire();	*/
		if (needToAcquire) {
			Acquire(appPicLineLock);
		}
		needToAcquire = 1;
		tprintf("Customer [%d]: What line should I choose for the AppClerk?\n",*index,0,0,"","");
		/*check for senator*/
		if(*cashDollars > 100) /*get in a privledged line*/
		{					
			privLine = TRUE;					
			tprintf("Customer [%d]: Priveleged line, baby. CashDollars = $%d\n",*index,*cashDollars,0,"","");
			privAppLineLength++;
			tprintf("Customer [%d]: Waiting in the Priveledged Line for next available AppClerk\n",*index,0,0,"","");
			/*privAppLineCV->Wait(appPicLineLock); //wait for clerk	*/
			Wait(privAppLineCV, appPicLineLock);					
		}
		else /*get in a normal line*/
		{
			tprintf("Customer [%d]: Regular line, suckas. CashDollars = $%d\n",*index,*cashDollars,0,"","");
			regAppLineLength++;
			tprintf("Customer [%d]: Waiting in the Regular Line for next available AppClerk\n",*index,0,0,"","");
			/*regAppLineCV->Wait(appPicLineLock); //wait for clerk	*/
			Wait(regAppLineCV, appPicLineLock);
		}

		tprintf("Customer [%d]: Finding available AppClerk...\n",*index,0,0,"","");
		for(x = 0; x < MAX_APP_CLERKS; x++)
		{				
			if(appClerkStatuses[x] == CLERK_AVAILABLE) /*find available clerk*/
			{
				myClerk = x;					
				tprintf("Customer [%d]: Going to chill with AppClerk [%d]\n",*index,myClerk,0,"","");					
				appClerkStatuses[myClerk] = CLERK_BUSY;					
				break;				
			}
			else
				tprintf("Customer [%d]: AppClerk [%d] is unavailable\n",*index,x,0,"","");

		}
		if (myClerk == -1) 
		{
			if (waitAndRestart(appPicLineLock, *index))
			{           /*SENATOR*/
				if(privLine)
					printf("Customer [%d] joins the [priveleged] wait queue for [Application] Clerk.\n",*index,0,0,"","");
				else
					printf("Customer [%d] joins the [regular] wait queue for [Application] Clerk.\n",*index,0,0,"","");
				continue;
			} 
			else 
			{
				tprintf("APP_CLERK: ESCAPED WITHOUT AN AVAILABLE CLERK, AND WITHOUT GOING TO WAITING ROOM!\n",0,0,0,"","");
				Exit(1); /*FAIL*/ 
			}	
		}
		/*appClerkLocks[myClerk]->Acquire();
		appPicLineLock->Release();*/
		Acquire(appClerkLocks[myClerk]);
		Release(appPicLineLock);							
		if(privLine)
		{
			printf("Customer [%d] is willing to pay $500 to ApplicationClerk [%d] for moving ahead in line\n",*index, myClerk,0,"","");				
			appClerkMoney[myClerk] += 500;
			appClerkBribed[myClerk] = TRUE;
			*cashDollars -= 500;
		}
		tprintf("Customer [%d]: Interacting with AppClerk [%d]\n",*index,myClerk,0,"","");
		/*interact with clerk	*/
		printf("Customer [%d] gives application to ApplicationClerk [%d] = [SSN: %d].\n", *index,myClerk,*index,"","");		
		appClerkSSNs[myClerk] = *index; 	
		/*appClerkCVs[myClerk]->Signal(appClerkLocks[myClerk]);*/
		Signal(appClerkCVs[myClerk], appClerkLocks[myClerk]);
		/*appClerkCVs[myClerk]->Wait(appClerkLocks[myClerk]);*/
		Wait(appClerkCVs[myClerk], appClerkLocks[myClerk]);
		printf("Customer [%d] is informed by ApplicationClerk [%d] that the application has been filed.\n", *index, myClerk,0,"","");
		tprintf("Customer [%d]: Done and done.\n",*index,0,0,"","");
		/*appClerkLocks[myClerk]->Release();*/
		Release(appClerkLocks[myClerk]);
		tprintf("Customer [%d]: Going to next clerk...\n",*index,0,0,"","");
		break;
	}
}

static void doPicClerk(int* index, int* cashDollars, int needToAcquire)
{

	bool privLine = FALSE;
	int myClerk, x;
	
	while(TRUE)
	{	
		myClerk = -1;
		tprintf("Customer [%d]: Going to the PicClerk\n",*index,0,0,"","");
		/*appPicLineLock->Acquire();	*/
		if (needToAcquire) {
			Acquire(appPicLineLock);
		}
		needToAcquire = 1;
		tprintf("Customer [%d]: What line should I choose for the PicClerk?\n",*index,0,0,"","");
		/*check for senator*/
		if(*cashDollars > 100) /*get in a privledged line*/
		{						
			privLine = TRUE;
			tprintf("Customer [%d]: Priveleged line, baby. CashDollars = $%d\n",*index,*cashDollars,0,"","");
			privPicLineLength++;
			tprintf("Customer [%d]: Waiting in the Priveledged Line for next available PicClerk\n",*index,0,0,"","");
			/*privPicLineCV->Wait(appPicLineLock);		*/
			Wait(privPicLineCV, appPicLineLock);	
		}
		else /*get in a normal line*/
		{
			tprintf("Customer [%d]: Regular line, suckas. CashDollars = $%d\n",*index,*cashDollars,0,"","");
			regPicLineLength++;
			tprintf("Customer [%d]: Waiting in the Regluar Line for next available PicClerk\n",*index,0,0,"","");
			/*regPicLineCV->Wait(appPicLineLock);			*/
			Wait(regPicLineCV, appPicLineLock);
		}
		tprintf("Customer [%d]: Finding available PicClerk...\n",*index,0,0,"","");
		for(x = 0; x < MAX_PIC_CLERKS; x++)
		{					

			if(picClerkStatuses[x] == CLERK_AVAILABLE)
			{
				myClerk = x;					
				tprintf("Customer [%d]: Going to chill with PicClerk[%d]\n",*index,myClerk,0,"","");					
				picClerkStatuses[myClerk] = CLERK_BUSY;					
				break;				
			}
			else
				tprintf("Customer [%d]: PicClerk [%d] is unavailable\n",*index,x,0,"","");
		}
		if (myClerk == -1) 
		{
			if (waitAndRestart(appPicLineLock, *index))
			{           /*SENATOR*/
				if(privLine)
					printf("Customer [%d] joins the [priveleged] wait queue for [Picture] Clerk.\n",*index,0,0,"","");
				else
					printf("Customer [%d] joins the [regular] wait queue for [Picture] Clerk.\n",*index,0,0,"","");
				continue;
			} 
			else 
			{
				tprintf("APP_CLERK: ESCAPED WITHOUT AN AVAILABLE CLERK, AND WITHOUT GOING TO WAITING ROOM!\n",0,0,0,"","");
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
			printf("Customer [%d] is willing to pay $500 to PictureClerk[%d] for moving ahead in line\n",*index, myClerk,0,"","");				
			picClerkMoney[myClerk] += 500;
			picClerkBribed[myClerk] = TRUE;
			*cashDollars -= 500;
		}
		tprintf("Customer [%d] Goes to PictureClerk[%d]\n",*index,myClerk,0,"","");
		/*interact with clerk, loop enabled picture to be taken multiple times*/
		while(happyWithPhoto[myClerk] == FALSE)
		{

			tprintf("Customer [%d]: Getting my picture taken...\n",*index,0,0,"","");
			/*picClerkCVs[myClerk]->Signal(picClerkLocks[myClerk]);
			picClerkCVs[myClerk]->Wait(picClerkLocks[myClerk]);	*/
			Signal(picClerkCVs[myClerk], picClerkLocks[myClerk]);
			Wait(picClerkCVs[myClerk], picClerkLocks[myClerk]);	

			/*did I like my picture?*/
			if(Rand()%10 > 5)
			{
				happyWithPhoto[myClerk] = TRUE;
				printf("Customer [%d] [likes] the picture provided by PictureClerk[%d]\n", *index,myClerk,0,"","");
			}
			else
			{
				printf("Customer [%d] [dislikes] the picture provided by PictureClerk[%d]\n", *index,myClerk,0,"","");
				printf("The picture of Customer [%d] will be taken again.\n", *index,0,0,"","");
			}		

			/*picClerkCVs[myClerk]->Signal(picClerkLocks[myClerk]);	
			picClerkCVs[myClerk]->Wait(picClerkLocks[myClerk]);		*/
			Signal(picClerkCVs[myClerk], picClerkLocks[myClerk]);
			Wait(picClerkCVs[myClerk], picClerkLocks[myClerk]);				

		}

		happyWithPhoto[myClerk] = FALSE; /*needs to be reset for future customers*/

		tprintf("Customer [%d]: Picture taken. Like a boss.\n",*index,0,0,"","");
		/*picClerkLocks[myClerk]->Release();			*/
		Release(picClerkLocks[myClerk]);

		tprintf("Customer [%d]: Done and done.\n",*index,0,0,"","");											
		break;
	}
}

static void doPassPortClerk(int *index, int* cashDollars){	

	bool privLined = FALSE;
	bool bribed = FALSE;
	int myClerk, x, rando;

	while(TRUE)
	{	
		if (waitAndRestart(-1, *index)){
			continue;
		}
		myClerk = -1;
		tprintf("Customer [%d]: Going to the PassportClerk\n",*index,0,0,"","");
		tprintf("Customer [%d]: Acquiring PassLineLock...\n",*index,0,0,"","");		
		/*passLineLock->Acquire();*/
		Acquire(passLineLock);
		printf("Customer [%d]: What line should I choose for the PassportClerk?\n",*index,0,0,"","");
		/*check for senator*/
		if(*cashDollars > 100 || privLined) /*get in a privledged line*/
		{								
			privLined = TRUE;				
			tprintf("Customer [%d]: Priveleged line, baby. CashDollars = $%d\n",*index,*cashDollars,0,"","");
			privPassLineLength++;
			tprintf("Customer [%d]: Waiting in the Priveledged Line for next available PassportClerk\n",*index,0,0,"","");
			/*privPassLineCV->Wait(passLineLock);*/
			Wait(privPassLineCV, passLineLock);			
		}
		else /*get in a normal line*/
		{
			tprintf("Customer [%d]: Regular line, suckas. CashDollars = $%d\n",*index,*cashDollars,0,"","");
			regPassLineLength++;
			tprintf("Customer [%d]: Waiting in the Regular Line for next available PassportClerk\n",*index,0,0,"","");
			/*regPassLineCV->Wait(passLineLock);		*/
			Wait(regPassLineCV, passLineLock);	
		}
		tprintf("Customer [%d]: Finding available PassClerk...\n",*index,0,0,"","");
		for(x = 0; x < MAX_PASS_CLERKS; x++)
		{				
			if(passClerkStatuses[x] == CLERK_AVAILABLE)
			{
				myClerk = x;					
				tprintf("Customer [%d]: Going to chill with PassClerk[%d]\n",*index,myClerk,0,"","");					
				passClerkStatuses[myClerk] = CLERK_BUSY;					
				break;				
			}
			else
				tprintf("Customer [%d]: PassClerk[%d] is unavailable\n",*index,x,0,"","");

		}
		if (myClerk == -1) 
		{
			if (waitAndRestart(passLineLock, *index))
			{           /*SENATOR*/
				if(privLined)
					printf("Customer [%d] joins the [priveleged] wait queue for [Passport] Clerk.\n",*index,0,0,"","");
				else
					printf("Customer [%d] joins the [regular] wait queue for [Passport] Clerk.\n",*index,0,0,"","");
				continue;
			} 
			else 
			{
				tprintf("APP_CLERK: ESCAPED WITHOUT AN AVAILABLE CLERK, AND WITHOUT GOING TO WAITING ROOM!\n",0,0,0,"","");
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
			bribed = TRUE;
			printf("Customer [%d] is willing to pay $500 to PassportClerk[%d] for moving ahead in line\n",*index,myClerk,0,"","");
			passClerkMoney[myClerk] += 500;
			passClerkBribed[myClerk] = TRUE;
			*cashDollars -= 500;
		}
		printf("Customer [%d] goes to PassportClerk[%d]\n",*index,myClerk,0,"","");
		/*interact with clerk		*/	
		/*passClerkCVs[myClerk]->Signal(passClerkLocks[myClerk]);
		passClerkCVs[myClerk]->Wait(passClerkLocks[myClerk]);*/
		Signal(passClerkCVs[myClerk], passClerkLocks[myClerk]);
		Wait(passClerkCVs[myClerk], passClerkLocks[myClerk]);
		/*get passport from clerk, if not ready, go to the back of the line?*/
		if(!passPunish[myClerk])
		{
			printf("Customer [%d] is [certified] by PassportClerk[%d]\n",*index,myClerk,0,"","");
			tprintf("Customer [%d]: Passport. GET!.\n", *index,0,0,"","");						
			tprintf("Customer [%d]: Done and done.\n",*index,0,0,"","");
			/*passClerkLocks[myClerk]->Release();*/
			Release(passClerkLocks[myClerk]);
			tprintf("Customer [%d]: Going to next clerk...\n",*index,0,0,"","");
			break;				
		}
		/*passClerkLocks[myClerk]->Release();*/
		Release(passClerkLocks[myClerk]);
		printf("Customer [%d] is [not certified] by PassportClerk[%d]\n",*index,myClerk,0,"","");
		printf("Customer [%d] is being forced to wait by PassportClerk[%d]\n",*index,myClerk,0,"","");
		rando = Rand()%80+20;					
		for(x = 0; x < rando; x++)
			Yield();															
	}

}


static void doCashierClerk(int* index, int* cashDollars)
{	
	int myClerk, x, rando;
	
	while(TRUE)
	{	
		if (waitAndRestart(-1, *index)){
			continue;
		}
		myClerk = -1;
		tprintf("Customer [%d]: Going to the CashClerk\n",*index,0,0,"","");	
		/*cashLineLock->Acquire();	*/
		Acquire(cashLineLock);
		
		regCashLineLength++;
		tprintf("Customer [%d]: Waiting in the Regular Line for next available CashClerk\n",*index,0,0,"","");
		/*regCashLineCV->Wait(cashLineLock);	*/
		Wait(regCashLineCV, cashLineLock);		
				
		tprintf("Customer[%d]: Finding available CashClerk...\n",*index,0,0,"","");
		for(x = 0; x < MAX_CASH_CLERKS; x++)
		{				
			if(cashClerkStatuses[x] == CLERK_AVAILABLE)
			{
				myClerk = x;					
				tprintf("Customer [%d]: Going to chill with CashClerk[%d]\n",*index,myClerk,0,"","");					
				cashClerkStatuses[myClerk] = CLERK_BUSY;					
				break;				
			}
			else
				tprintf("Customer [%d]: CashClerk[%d] is unavailable\n",*index,x,0,"","");

		}	
		if (myClerk == -1) 
		{
			if (waitAndRestart(cashLineLock, *index))
			{           /*SENATOR*/				
				printf("Customer [%d] joins the [regular] wait queue for [Cashier] Clerk.\n",*index,0,0,"","");
				continue;
			} 
			else 
			{
				tprintf("APP_CLERK: ESCAPED WITHOUT AN AVAILABLE CLERK, AND WITHOUT GOING TO WAITING ROOM!\n",0,0,0,"","");
				Exit(1); /*FAIL */
			}	
		}
		
		/*cashClerkLocks[myClerk]->Acquire();
		cashLineLock->Release();		*/
		Acquire(cashClerkLocks[myClerk]);
		Release(cashLineLock);
							
		cashClerkSSNs[myClerk] = *index;			
		printf("Customer [%d] goes to CashClerk[%d]\n",*index,myClerk,0,"","");			
		/*interact with clerk			*/
		/*cashClerkCVs[myClerk]->Signal(cashClerkLocks[myClerk]);
		cashClerkCVs[myClerk]->Wait(cashClerkLocks[myClerk]);*/
		Signal(cashClerkCVs[myClerk], cashClerkLocks[myClerk]);
		Wait(cashClerkCVs[myClerk], cashClerkLocks[myClerk]);
		
		/*pay for passport. If it's not processed, get back in line*/
		if(!cashPunish[myClerk])
		{				
			printf("Customer [%d] gets [valid] certification by Cashier[%d]\n",*index,myClerk,0,"","");
			printf("Customer [%d] pays $100 to Cashier[%d] for their passport\n",*index,myClerk,0,"","");
			cashClerkMoney[myClerk] += 100;				
			*cashDollars-=100;
			/*cashClerkCVs[myClerk]->Signal(cashClerkLocks[myClerk]);
			cashClerkCVs[myClerk]->Wait(cashClerkLocks[myClerk]);*/
			Signal(cashClerkCVs[myClerk], cashClerkLocks[myClerk]);
			Wait(cashClerkCVs[myClerk], cashClerkLocks[myClerk]);
			
			tprintf("Customer [%d]: Passport paid for like a pro. CashDollars = [$%d]\n", *index, *cashDollars,0,"","");											
			/*cashClerkLocks[myClerk]->Release();*/
			Release(cashClerkLocks[myClerk]);
			
			printf("Customer [%d] leaves the passport office...\n",*index,0,0,"","");
			/*customerOfficeLock->Acquire();*/
			Acquire(entryLock);
			customersInOffice--;
			/*customerOfficeLock->Release();*/
			Release(entryLock);
			break;				
		}
		/*cashClerkLocks[myClerk]->Release();*/
		Release(cashClerkLocks[myClerk]);
		printf("Customer [%d] gets [invalid] certification by Cashier[%d]\n",*index,myClerk,0,"","");
		printf("Customer [%d] is punished to wait by Cashier[%d]\n",*index,myClerk,0,"","");
		tprintf("Customer [%d]: NOT READY!? Going back to the end of the line...\n",*index,0,0,"","");
		rando = Rand()%80+20;					
		for(x = 0; x < rando; x++)
			Yield();																				
	}
}

void CustomerCashTest(){
	int cashDollars;
	int clerkStatus;
	int index;
	int tid;
	Acquire(entryLock);
	
	tid = GetThreadID();
	cashDollars = 100;

	printf("Customer [%d] has money = [$%d] ... tid = %d\n",index,cashDollars,tid,"","");
	customersInOffice++;
	Release(entryLock);
	
	doCashierClerk(&index, &cashDollars);
}