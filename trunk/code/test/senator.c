
#include "office.h"
#include "utils.h"

static void doAppClerk(int* index, int* cashDollars);
static void doPicClerk(int* index, int* cashDollars);
static void doPassPortClerk(int* index, int* cashDollars);
static void doCashierClerk(int* index, int* cashDollars);


void SenatorRun() {	

	int cashDollars;
	int clerkStatus;
	int index;
	int tid;

	tid = GetThreadID();

	Acquire(customerSenatorUIDLock);
	index = customerSenatorUID++;
	Release(customerSenatorUIDLock);

	tprintf("Senator[%d]: Acquiring customerOfficeLock\n", index,0,0,"","");
	/*customerOfficeLock->Acquire();*/
	Acquire(entryLock);
	if (customersInOffice > 0){
		tprintf("Senator[%d]: There are other Customers in office, going to Senator waiting room\n", index,0,0,"","");
		senatorsInWaitingRoom++;
		tprintf("Senator[%d]: In the waiting room, taking a nap...\n", index,0,0,"","");
		/*senatorWaitingRoomCV->Wait(senatorWaitingRoomLock);*/
		Wait(senatorWaitingRoomCV, entryLock);
		tprintf("Senator[%d]: Waking up, going to the passport office!\n", index,0,0,"","");
		senatorsInWaitingRoom--;		
	}
	
	tprintf("Senator [%d]: Entering the passport office...\n",index,0,0,"","");

	/*senator start up code*/		
	cashDollars = ((Rand() % 4) * 500) + 100;	

	printf("Senator [%d] has money = [$%d] ... tid = %d\n",index,cashDollars,tid,"","");
	senatorsInOffice++;
	Release(entryLock);

	/*choose line*/		
	tprintf("Senator [%d]: Deciding between AppClerk and PictureClerk...\n", index,0,0,"","");
	if(cashDollars > 100) /*find priveledged line with shortest length*/
	{
		/*appPicLineLock->Acquire();*/
		Acquire(appPicLineLock);
		printf("Senator [%d] finds the minimum [priveledged] queue for [ApplicationClerk/PictureClerk]\n", index,0,0,"","");
		if(privAppLineLength<=privPicLineLength)
		{
			/*appPicLineLock->Release();*/
			Release(appPicLineLock);
			printf("Senator [%d] goes to [ApplicationClerk] first.\n",index,0,0,"","");			
			doAppClerk(&index, &cashDollars);
			doPicClerk(&index, &cashDollars);
			
		}
		else
		{
			/*appPicLineLock->Release();*/
			Release(appPicLineLock);
			printf("Senator [%d] goes to [PictureClerk] first.\n",index,0,0,"","");	
			doPicClerk(&index, &cashDollars);
			doAppClerk(&index, &cashDollars);
		
		}	
	}
	else/*find regular line with shortest length*/
	{
		/*appPicLineLock->Acquire();*/
		Acquire(appPicLineLock);
		printf("Senator [%d] finds the minimum [regular] queue for [ApplicationClerk/PictureClerk]\n", index,0,0,"","");
		if(regAppLineLength<=regPicLineLength)
		{
			/*appPicLineLock->Release();*/
			Release(appPicLineLock);
			printf("Senator [%d] goes to [ApplicationClerk].\n",index,0,0,"","");	
			doAppClerk(&index, &cashDollars);
			doPicClerk(&index, &cashDollars);
			
		}
		else
		{
			/*appPicLineLock->Release();*/
			Release(appPicLineLock);
			printf("Senator [%d] goes to [PictureClerk].\n",index,0,0,"","");
			doPicClerk(&index, &cashDollars);
			doAppClerk(&index, &cashDollars);
		}	
	}

	/*hit up the passport clerk*/
	doPassPortClerk(&index, &cashDollars);
	

	/*proceed to cashier*/
	doCashierClerk(&index, &cashDollars);

	Exit(0);
}

static void doAppClerk(int* index, int* cashDollars)
{
	bool privLine = FALSE;
	int myClerk, x;


	while(TRUE)
	{	
		myClerk = -1;
		tprintf("Senator [%d]: Going to the AppClerk\n",*index,0,0,"","");
		/*appPicLineLock->Acquire();	*/
		Acquire(appPicLineLock);	
		tprintf("Senator [%d]: What line should I choose for the AppClerk?\n",*index,0,0,"","");
		/*check for senator*/
		if(*cashDollars > 100) /*get in a privledged line*/
		{					
			privLine = TRUE;					
			tprintf("Senator [%d]: Priveleged line, baby. CashDollars = $%d\n",*index,*cashDollars,0,"","");
			privAppLineLength++;
			tprintf("Senator [%d]: Waiting in the Priveledged Line for next available AppClerk\n",*index,0,0,"","");
			/*privAppLineCV->Wait(appPicLineLock); //wait for clerk	*/
			Wait(privAppLineCV, appPicLineLock);					
		}
		else /*get in a normal line*/
		{
			tprintf("Senator [%d]: Regular line, suckas. CashDollars = $%d\n",*index,*cashDollars,0,"","");
			regAppLineLength++;
			tprintf("Senator [%d]: Waiting in the Regular Line for next available AppClerk\n",*index,0,0,"","");
			/*regAppLineCV->Wait(appPicLineLock); //wait for clerk	*/
			Wait(regAppLineCV, appPicLineLock);
		}

		tprintf("Senator [%d]: Finding available AppClerk...\n",*index,0,0,"","");
		for(x = 0; x < MAX_APP_CLERKS; x++)
		{				
			if(appClerkStatuses[x] == CLERK_AVAILABLE) /*find available clerk*/
			{
				myClerk = x;					
				tprintf("Senator [%d]: Going to chill with AppClerk [%d]\n",*index,myClerk,0,"","");					
				appClerkStatuses[myClerk] = CLERK_BUSY;					
				break;				
			}
			else
				tprintf("Senator [%d]: AppClerk [%d] is unavailable\n",*index,x,0,"","");

		}

		if (myClerk == -1) {
			printf("Senator [%d]: Woke up with a AppClerk index of -1, halting the machine for now.",*index,0,0,"","");
			Halt();
		}
	
		/*appClerkLocks[myClerk]->Acquire();
		appPicLineLock->Release();*/
		Acquire(appClerkLocks[myClerk]);
		Release(appPicLineLock);							
		if(privLine)
		{
			printf("Senator [%d] is willing to pay $500 to ApplicationClerk [%d] for moving ahead in line\n",*index, myClerk,0,"","");				
			appClerkMoney[myClerk] += 500;
			appClerkBribed[myClerk] = TRUE;
			*cashDollars -= 500;
		}
		tprintf("Senator [%d]: Interacting with AppClerk [%d]\n",*index,myClerk,0,"","");
		/*interact with clerk	*/
		printf("Senator [%d] gives application to ApplicationClerk [%d] = [SSN: %d].\n", *index,myClerk,*index,"","");		
		appClerkSSNs[myClerk] = *index; 	
		/*appClerkCVs[myClerk]->Signal(appClerkLocks[myClerk]);*/
		Signal(appClerkCVs[myClerk], appClerkLocks[myClerk]);
		/*appClerkCVs[myClerk]->Wait(appClerkLocks[myClerk]);*/
		Wait(appClerkCVs[myClerk], appClerkLocks[myClerk]);
		printf("Senator [%d] is informed by ApplicationClerk [%d] that the application has been filed.\n", *index, myClerk,0,"","");
		tprintf("Senator [%d]: Done and done.\n",*index,0,0,"","");
		/*appClerkLocks[myClerk]->Release();*/
		Release(appClerkLocks[myClerk]);
		tprintf("Senator [%d]: Going to next clerk...\n",*index,0,0,"","");
		break;
	}
}

static void doPicClerk(int* index, int* cashDollars)
{

	bool privLine = FALSE;
	int myClerk, x;
	
	while(TRUE)
	{	
		myClerk = -1;
		tprintf("Senator [%d]: Going to the PicClerk\n",*index,0,0,"","");
		/*appPicLineLock->Acquire();	*/
		Acquire(appPicLineLock);
		tprintf("Senator [%d]: What line should I choose for the PicClerk?\n",*index,0,0,"","");
		/*check for senator*/
		if(*cashDollars > 100) /*get in a privledged line*/
		{						
			privLine = TRUE;
			tprintf("Senator [%d]: Priveleged line, baby. CashDollars = $%d\n",*index,*cashDollars,0,"","");
			privPicLineLength++;
			tprintf("Senator [%d]: Waiting in the Priveledged Line for next available PicClerk\n",*index,0,0,"","");
			/*privPicLineCV->Wait(appPicLineLock);		*/
			Wait(privPicLineCV, appPicLineLock);	
		}
		else /*get in a normal line*/
		{
			tprintf("Senator [%d]: Regular line, suckas. CashDollars = $%d\n",*index,*cashDollars,0,"","");
			regPicLineLength++;
			tprintf("Senator [%d]: Waiting in the Regluar Line for next available PicClerk\n",*index,0,0,"","");
			/*regPicLineCV->Wait(appPicLineLock);			*/
			Wait(regPicLineCV, appPicLineLock);
		}
		tprintf("Senator [%d]: Finding available PicClerk...\n",*index,0,0,"","");
		for(x = 0; x < MAX_PIC_CLERKS; x++)
		{					

			if(picClerkStatuses[x] == CLERK_AVAILABLE)
			{
				myClerk = x;					
				tprintf("Senator [%d]: Going to chill with PicClerk[%d]\n",*index,myClerk,0,"","");					
				picClerkStatuses[myClerk] = CLERK_BUSY;					
				break;				
			}
			else
				tprintf("Senator [%d]: PicClerk [%d] is unavailable\n",*index,x,0,"","");
		}

		if (myClerk == -1) {
			printf("Senator [%d]: Woke up with a PicClerk index of -1, halting the machine for now.",*index,0,0,"","");
			Halt();
		}
		
		/*picClerkLocks[myClerk]->Acquire();
		appPicLineLock->Release();*/
		Acquire(picClerkLocks[myClerk]);
		Release(appPicLineLock);
						
		picClerkSSNs[myClerk] = *index;		
		if(privLine)
		{
			printf("Senator [%d] is willing to pay $500 to PictureClerk[%d] for moving ahead in line\n",*index, myClerk,0,"","");				
			picClerkMoney[myClerk] += 500;
			picClerkBribed[myClerk] = TRUE;
			*cashDollars -= 500;
		}
		tprintf("Senator [%d] Goes to PictureClerk[%d]\n",*index,myClerk,0,"","");
		/*interact with clerk, loop enabled picture to be taken multiple times*/
		while(happyWithPhoto[myClerk] == FALSE)
		{

			tprintf("Senator [%d]: Getting my picture taken...\n",*index,0,0,"","");
			/*picClerkCVs[myClerk]->Signal(picClerkLocks[myClerk]);
			picClerkCVs[myClerk]->Wait(picClerkLocks[myClerk]);	*/
			Signal(picClerkCVs[myClerk], picClerkLocks[myClerk]);
			Wait(picClerkCVs[myClerk], picClerkLocks[myClerk]);	

			/*did I like my picture?*/
			if(Rand()%10 > 5)
			{
				happyWithPhoto[myClerk] = TRUE;
				printf("Senator [%d] [likes] the picture provided by PictureClerk[%d]\n", *index,myClerk,0,"","");
			}
			else
			{
				printf("Senator [%d] [dislikes] the picture provided by PictureClerk[%d]\n", *index,myClerk,0,"","");
				printf("The picture of Senator [%d] will be taken again.\n", *index,0,0,"","");
			}		

			/*picClerkCVs[myClerk]->Signal(picClerkLocks[myClerk]);	
			picClerkCVs[myClerk]->Wait(picClerkLocks[myClerk]);		*/
			Signal(picClerkCVs[myClerk], picClerkLocks[myClerk]);
			Wait(picClerkCVs[myClerk], picClerkLocks[myClerk]);				

		}

		happyWithPhoto[myClerk] = FALSE; /*needs to be reset for future customers*/

		tprintf("Senator [%d]: Picture taken. Like a boss.\n",*index,0,0,"","");
		/*picClerkLocks[myClerk]->Release();			*/
		Release(picClerkLocks[myClerk]);

		tprintf("Senator [%d]: Done and done.\n",*index,0,0,"","");											
		break;
	}
}

static void doPassPortClerk(int *index, int* cashDollars){	

	bool privLined = FALSE;
	bool bribed = FALSE;
	int myClerk, x, rando;

	while(TRUE)
	{	
	
		myClerk = -1;
		tprintf("Senator [%d]: Going to the PassportClerk\n",*index,0,0,"","");
		tprintf("Senator [%d]: Acquiring PassLineLock...\n",*index,0,0,"","");		
		/*passLineLock->Acquire();*/
		Acquire(passLineLock);
		printf("Senator [%d]: What line should I choose for the PassportClerk?\n",*index,0,0,"","");
		/*check for senator*/
		if(*cashDollars > 100 || privLined) /*get in a privledged line*/
		{								
			privLined = TRUE;				
			tprintf("Senator [%d]: Priveleged line, baby. CashDollars = $%d\n",*index,*cashDollars,0,"","");
			privPassLineLength++;
			tprintf("Senator [%d]: Waiting in the Priveledged Line for next available PassportClerk\n",*index,0,0,"","");
			/*privPassLineCV->Wait(passLineLock);*/
			Wait(privPassLineCV, passLineLock);			
		}
		else /*get in a normal line*/
		{
			tprintf("Senator [%d]: Regular line, suckas. CashDollars = $%d\n",*index,*cashDollars,0,"","");
			regPassLineLength++;
			tprintf("Senator [%d]: Waiting in the Regular Line for next available PassportClerk\n",*index,0,0,"","");
			/*regPassLineCV->Wait(passLineLock);		*/
			Wait(regPassLineCV, passLineLock);	
		}
		tprintf("Senator [%d]: Finding available PassClerk...\n",*index,0,0,"","");
		for(x = 0; x < MAX_PASS_CLERKS; x++)
		{				
			if(passClerkStatuses[x] == CLERK_AVAILABLE)
			{
				myClerk = x;					
				tprintf("Senator [%d]: Going to chill with PassClerk[%d]\n",*index,myClerk,0,"","");					
				passClerkStatuses[myClerk] = CLERK_BUSY;					
				break;				
			}
			else
				tprintf("Senator [%d]: PassClerk[%d] is unavailable\n",*index,x,0,"","");

		}

		if (myClerk == -1) {
			printf("Senator [%d]: Woke up with a PassClerk index of -1, halting the machine for now.",*index,0,0,"","");
			Halt();
		}
		
		/*passClerkLocks[myClerk]->Acquire();*/
		Acquire(passClerkLocks[myClerk]);
		/*passLineLock->Release();	*/
		Release(passLineLock);						
		passClerkSSNs[myClerk] = *index;
		if(privLined && !bribed)
		{
			bribed = TRUE;
			printf("Senator [%d] is willing to pay $500 to PassportClerk[%d] for moving ahead in line\n",*index,myClerk,0,"","");
			passClerkMoney[myClerk] += 500;
			passClerkBribed[myClerk] = TRUE;
			*cashDollars -= 500;
		}
		printf("Senator [%d] goes to PassportClerk[%d]\n",*index,myClerk,0,"","");
		/*interact with clerk		*/	
		/*passClerkCVs[myClerk]->Signal(passClerkLocks[myClerk]);
		passClerkCVs[myClerk]->Wait(passClerkLocks[myClerk]);*/
		Signal(passClerkCVs[myClerk], passClerkLocks[myClerk]);
		Wait(passClerkCVs[myClerk], passClerkLocks[myClerk]);
		/*get passport from clerk, if not ready, go to the back of the line?*/
		if(!passPunish[myClerk])
		{
			printf("Senator [%d] is [certified] by PassportClerk[%d]\n",*index,myClerk,0,"","");
			tprintf("Senator [%d]: Passport. GET!.\n", *index,0,0,"","");						
			tprintf("Senator [%d]: Done and done.\n",*index,0,0,"","");
			/*passClerkLocks[myClerk]->Release();*/
			Release(passClerkLocks[myClerk]);
			tprintf("Senator [%d]: Going to next clerk...\n",*index,0,0,"","");
			break;				
		}
		/*passClerkLocks[myClerk]->Release();*/
		Release(passClerkLocks[myClerk]);
		printf("Senator [%d] is [not certified] by PassportClerk[%d]\n",*index,myClerk,0,"","");
		printf("Senator [%d] is being forced to wait by PassportClerk[%d]\n",*index,myClerk,0,"","");
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
		
		myClerk = -1;
		tprintf("Senator [%d]: Going to the CashClerk\n",*index,0,0,"","");	
		/*cashLineLock->Acquire();	*/
		Acquire(cashLineLock);
		
		regCashLineLength++;
		tprintf("Senator [%d]: Waiting in the Regular Line for next available CashClerk\n",*index,0,0,"","");
		/*regCashLineCV->Wait(cashLineLock);	*/
		Wait(regCashLineCV, cashLineLock);		
				
		tprintf("Senator[%d]: Finding available CashClerk...\n",*index,0,0,"","");
		for(x = 0; x < MAX_CASH_CLERKS; x++)
		{				
			if(cashClerkStatuses[x] == CLERK_AVAILABLE)
			{
				myClerk = x;					
				tprintf("Senator [%d]: Going to chill with CashClerk[%d]\n",*index,myClerk,0,"","");					
				cashClerkStatuses[myClerk] = CLERK_BUSY;					
				break;				
			}
			else
				tprintf("Senator [%d]: CashClerk[%d] is unavailable\n",*index,x,0,"","");

		}	
		
		if (myClerk == -1) {
			printf("Senator [%d]: Woke up with a CashClerk index of -1, halting the machine for now.",*index,0,0,"","");
			Halt();
		}

		/*cashClerkLocks[myClerk]->Acquire();
		cashLineLock->Release();		*/
		Acquire(cashClerkLocks[myClerk]);
		Release(cashLineLock);
							
		cashClerkSSNs[myClerk] = *index;			
		printf("Senator [%d] goes to CashClerk[%d]\n",*index,myClerk,0,"","");			
		/*interact with clerk			*/
		/*cashClerkCVs[myClerk]->Signal(cashClerkLocks[myClerk]);
		cashClerkCVs[myClerk]->Wait(cashClerkLocks[myClerk]);*/
		Signal(cashClerkCVs[myClerk], cashClerkLocks[myClerk]);
		Wait(cashClerkCVs[myClerk], cashClerkLocks[myClerk]);
		
		/*pay for passport. If it's not processed, get back in line*/
		if(!cashPunish[myClerk])
		{				
			printf("Senator [%d] gets [valid] certification by Cashier[%d]\n",*index,myClerk,0,"","");
			printf("Senator [%d] pays $100 to Cashier[%d] for their passport\n",*index,myClerk,0,"","");
			cashClerkMoney[myClerk] += 100;				
			*cashDollars-=100;
			/*cashClerkCVs[myClerk]->Signal(cashClerkLocks[myClerk]);
			cashClerkCVs[myClerk]->Wait(cashClerkLocks[myClerk]);*/
			Signal(cashClerkCVs[myClerk], cashClerkLocks[myClerk]);
			Wait(cashClerkCVs[myClerk], cashClerkLocks[myClerk]);
			
			tprintf("Senator [%d]: Passport paid for like a pro. CashDollars = [$%d]\n", *index, *cashDollars,0,"","");											
			/*cashClerkLocks[myClerk]->Release();*/
			Release(cashClerkLocks[myClerk]);
			
			printf("Senator [%d] leaves the passport office...\n",*index,0,0,"","");
			/*customerOfficeLock->Acquire();*/
			Acquire(entryLock);
			senatorsInOffice--;
			/*customerOfficeLock->Release();*/
			Release(entryLock);
			break;				
		}
		/*cashClerkLocks[myClerk]->Release();*/
		Release(cashClerkLocks[myClerk]);
		printf("Senator [%d] gets [invalid] certification by Cashier[%d]\n",*index,myClerk,0,"","");
		printf("Senator [%d] is punished to wait by Cashier[%d]\n",*index,myClerk,0,"","");
		tprintf("Senator [%d]: NOT READY!? Going back to the end of the line...\n",*index,0,0,"","");
		rando = Rand()%80+20;					
		for(x = 0; x < rando; x++)
			Yield();																				
	}
}
