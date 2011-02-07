//Customer.cc
#include "office.h"
#include "system.h"
#include <iostream>

using namespace std;
static void doAppClerk(int* index, int* cashDollars);
static void doPicClerk(int* index, int* cashDollars);
static void doPassPortClerk(int* index, int* cashDollars);
static void doCashierClerk(int* index, int* cashDollars);

int waitAndRestart(Lock* lineToExit, int index){
	tprintf("Customer [%d]: waitAndRestart entered\n", index);
	senatorWaitingRoomLock->Acquire();
	if (senatorsInWaitingRoom>0){
		if(lineToExit){
			lineToExit->Release();
		}
		tprintf("Customer [%d]: There are %d senators in waiting room\n", senatorsInWaitingRoom, index);
		senatorWaitingRoomLock->Release();
		customerWaitingRoomLock->Acquire();
		customerOfficeLock->Acquire();
		customersInWaitingRoom++;
		customersInOffice--;
		customerOfficeLock->Release();
		tprintf("Customer [%d]: Waiting in the Waiting room..\n", index);
		customerWaitingRoomCV->Wait(customerWaitingRoomLock);
		tprintf("Customer [%d]: No more senators! Returning to passport office\n", index);
		customerOfficeLock->Acquire();
		customersInWaitingRoom--;
		customersInOffice++;
		customerOfficeLock->Release();
		customerWaitingRoomLock->Release();
		if(lineToExit){
			lineToExit->Acquire();
		}
		return TRUE;
	}else{
		tprintf("Customer [%d]: There are no senators waiting.. carrying on\n", index);
		senatorWaitingRoomLock->Release();
		return FALSE;
	}

}

void CustomerRun(int index) {	


	printf("Customer[%d]: Deciding whether to go to waiting room or straight into office...\n",index);
	senatorWaitingRoomLock->Acquire();
	senatorOfficeLock->Acquire();
	while(senatorsInOffice+senatorsInWaitingRoom > 0){
		printf("Customer[%d]: Senator is in the office or waiting room.. have to wait...\n",index);
		senatorOfficeLock->Release();
		senatorWaitingRoomLock->Release();
		customerWaitingRoomLock->Acquire();
		customersInWaitingRoom++;
		tprintf("Customer[%d]: In the waiting room, taking a nap...\n", index);
		customerWaitingRoomCV->Wait(customerWaitingRoomLock);
		printf("Customer[%d]: No more senators! Time to enter the office",index);
		customersInWaitingRoom++;
		customerWaitingRoomLock->Release(); //MIGHT NEED TO MOVE FOR MANAGER
		senatorWaitingRoomLock->Acquire();
		senatorOfficeLock->Acquire();

	}

	printf("Customer[%d]: Entering the passport office...\n",index);

	//customer start up code		
	int cashDollars = ((rand() % 4) * 500) + 100;	
	int clerkStatus;

	printf("Customer[%d]: With $%d in my pocket\n",index,cashDollars);
	customerOfficeLock->Acquire();
	customersInOffice++;
	customerOfficeLock->Release();
	senatorOfficeLock->Release();
	senatorWaitingRoomLock->Release();

	//choose line		
	printf("Customer[%d]: Deciding between AppClerk and PictureClerk...\n", index);
	if(cashDollars > 100) //find priveledged line with shortest length
	{
		appPicLineLock->Acquire();
		if(privAppLineLength<=privPicLineLength)
		{
			appPicLineLock->Release();
			printf("Customer[%d]: Going to AppClerk first.\n",index);			
			doAppClerk(&index, &cashDollars);
			waitAndRestart(NULL, index); //SENATOR
			doPicClerk(&index, &cashDollars);
			waitAndRestart(NULL, index); //SENATOR
		}
		else
		{
			appPicLineLock->Release();
			printf("Customer[%d]: Going to PicClerk first.\n",index);			
			doPicClerk(&index, &cashDollars);
			waitAndRestart(NULL, index); //SENATOR
			doAppClerk(&index, &cashDollars);
			waitAndRestart(NULL, index); //SENATOR
		}	
	}
	else//find regular line with shortest length
	{
		appPicLineLock->Acquire();
		if(regAppLineLength<=regPicLineLength)
		{
			appPicLineLock->Release();
			printf("Customer[%d]: Going to AppClerk first.\n",index);
			doAppClerk(&index, &cashDollars);
			waitAndRestart(NULL, index); //SENATOR
			doPicClerk(&index, &cashDollars);
			waitAndRestart(NULL, index); //SENATOR
		}
		else
		{
			appPicLineLock->Release();
			printf("Customer[%d]: Going to PicClerk first.\n",index);
			doPicClerk(&index, &cashDollars);
			waitAndRestart(NULL, index; //SENATOR
			doAppClerk(&index, &cashDollars);
			waitAndRestart(NULL, index); //SENATOR
		}	
	}

	//hit up the passport clerk
	doPassPortClerk(&index, &cashDollars);
	

	//proceed to cashier
	doCashierClerk(&index, &cashDollars);
}

static void doAppClerk(int* index, int* cashDollars)
{
	bool privLine = false;


	while(true)
	{	
		int myClerk = -1;
		printf("Customer[%d]: Going to the AppClerk\n",*index);
		appPicLineLock->Acquire();		
		printf("Customer[%d]: What line should I choose for the AppClerk?\n",*index);
		//check for senator
		if(*cashDollars > 100) //get in a privledged line
		{					
			privLine = true;					
			printf("Customer[%d]: Priveleged line, baby. CashDollars = $%d\n",*index,*cashDollars);
			privAppLineLength++;
			printf("Customer[%d]: Waiting in the Priveledged Line for next available AppClerk\n",*index);
			privAppLineCV->Wait(appPicLineLock);							
		}
		else //get in a normal line
		{
			printf("Customer[%d]: Regular line, suckas. CashDollars = $%d\n",*index,*cashDollars);
			regAppLineLength++;
			printf("Customer[%d]: Waiting in the Regular Line for next available AppClerk\n",*index);
			regAppLineCV->Wait(appPicLineLock);			
		}

		printf("Customer[%d]: Finding available AppClerk...\n",*index);
		for(int x = 0; x < MAX_APP_CLERKS; x++)
		{				
			if(appClerkStatuses[x] == CLERK_AVAILABLE)
			{
				myClerk = x;					
				printf("Customer[%d]: Going to chill with AppClerk[%d]\n",*index,myClerk);					
				appClerkStatuses[myClerk] = CLERK_BUSY;					
				break;				
			}
			else
				printf("Customer[%d]: AppClerk[%d] is unavailable\n",*index,x);

		}
		if (myClerk == -1) {
			if (waitAndRestart(appPicLineLock, *index)){           //SENATOR
				continue;
			} else {
				printf("APP_CLERK: ESCAPED WITHOUT AN AVAILABLE CLERK, AND WITHOUT GOING TO WAITING ROOM!");
				exit(1); //FAIL 
			}	
		}
		appClerkLocks[myClerk]->Acquire();
		appPicLineLock->Release();							
		if(privLine)
		{
			printf("Customer[%d]: Paying AppClerk[%d] $500 to fastpass the line\n",*index, myClerk);				
			appClerkMoney[myClerk] += 500;
			appClerkBribed[myClerk] = true;
			*cashDollars -= 500;
		}
		printf("Customer[%d]: Interacting with AppClerk[%d]\n",*index,myClerk);
		//interact with clerk
		appClerkSSNs[myClerk] = *index; //could also just use the adr for a more ssn-like number
		printf("Customer[%d]: Application handed in like a boss.\n", *index);			
		appClerkCVs[myClerk]->Signal(appClerkLocks[myClerk]);
		appClerkCVs[myClerk]->Wait(appClerkLocks[myClerk]);
		printf("Customer[%d]: Application received like a boss.\n", *index);
		//more shit			
		printf("Customer[%d]: Done and done.\n",*index);
		appClerkLocks[myClerk]->Release();
		printf("Customer[%d]: Going to next clerk...\n",*index);
		break;
	}
}

static void doPicClerk(int* index, int* cashDollars)
{

	bool privLine = false;

	while(true)
	{	
		int myClerk = -1;
		printf("Customer[%d]: Going to the PicClerk\n",*index);
		appPicLineLock->Acquire();	
		printf("Customer[%d]: What line should I choose for the PicClerk?\n",*index);
		//check for senator
		if(*cashDollars > 100) //get in a privledged line
		{						
			privLine = true;
			printf("Customer[%d]: Priveleged line, baby. CashDollars = $%d\n",*index,*cashDollars);
			privPicLineLength++;
			printf("Customer[%d]: Waiting in the Priveledged Line for next available PicClerk\n",*index);
			privPicLineCV->Wait(appPicLineLock);			
		}
		else //get in a normal line
		{
			printf("Customer[%d]: Regular line, suckas. CashDollars = $%d\n",*index,*cashDollars);
			regPicLineLength++;
			printf("Customer[%d]: Waiting in the Regluar Line for next available PicClerk\n",*index);
			regPicLineCV->Wait(appPicLineLock);			
		}
		printf("Customer[%d]: Finding available PicClerk...\n",*index);
		for(int x = 0; x < MAX_PIC_CLERKS; x++)
		{					

			if(picClerkStatuses[x] == CLERK_AVAILABLE)
			{
				myClerk = x;					
				printf("Customer[%d]: Going to chill with PicClerk[%d]\n",*index,myClerk);					
				picClerkStatuses[myClerk] = CLERK_BUSY;					
				break;				
			}
			else
				printf("Customer[%d]: PicClerk[%d] is unavailable\n",*index,x);
		}
		if (myClerk == -1) {
			if (waitAndRestart(appPicLineLock, *index)){           //SENATOR
				continue;
			} else {
				printf("PIC_CLERK: ESCAPED WITHOUT AN AVAILABLE CLERK, AND WITHOUT GOING TO WAITING ROOM!");
				exit(1); //FAIL 
			}	
		}
		picClerkLocks[myClerk]->Acquire();
		appPicLineLock->Release();				
		picClerkSSNs[myClerk] = *index;		
		if(privLine)
		{
			printf("Customer[%d]: Paying PicClerk[%d] $500 for fastpassing the line\n",*index, myClerk);				
			picClerkMoney[myClerk] += 500;
			picClerkBribed[myClerk] = true;
			*cashDollars -= 500;
		}
		printf("Customer[%d]: Interacting with Pic Clerk\n",*index);
		//interact with clerk


		while(happyWithPhoto[myClerk] == FALSE)
		{

			printf("Customer[%d]: Getting my picture taken...\n",*index);
			picClerkCVs[myClerk]->Signal(picClerkLocks[myClerk]);
			picClerkCVs[myClerk]->Wait(picClerkLocks[myClerk]);		

			//did I like my picture?
			if(rand()%10 > 5)
			{
				happyWithPhoto[myClerk] = TRUE;
				printf("Customer[%d]: This picture is awesome!\n", *index);
			}
			else
			{
				printf("Customer[%d]: This picture sucks! Take it again!\n",*index);
			}		

			picClerkCVs[myClerk]->Signal(picClerkLocks[myClerk]);	
			picClerkCVs[myClerk]->Wait(picClerkLocks[myClerk]);						

		}

		happyWithPhoto[myClerk] = FALSE;

		printf("Customer[%d]: Picture taken. Like a boss.\n",*index);
		picClerkLocks[myClerk]->Release();			

		printf("Customer[%d]: Done and done.\n",*index);											
		break;
	}
}

static void doPassPortClerk(int *index, int* cashDollars){	

	bool privLined = false;
	bool bribed = false;

	while(true)
	{	
		if (waitAndRestart(NULL, *index)){
			continue;
		}
		int myClerk = -1;
		printf("Customer[%d]: Going to the PassportClerk\n",*index);
		printf("Customer[%d]: Acquiring PassLineLock...\n",*index);		
		passLineLock->Acquire();
		printf("Customer[%d]: What line should I choose for the PassportClerk?\n",*index);
		//check for senator
		if(*cashDollars > 100 || privLined) //get in a privledged line
		{								
			privLined = true;				
			printf("Customer[%d]: Priveleged line, baby. CashDollars = $%d\n",*index,*cashDollars);
			privPassLineLength++;
			printf("Customer[%d]: Waiting in the Priveledged Line for next available PassportClerk\n",*index);
			privPassLineCV->Wait(passLineLock);			
		}
		else //get in a normal line
		{
			printf("Customer[%d]: Regular line, suckas. CashDollars = $%d\n",*index,*cashDollars);
			regPassLineLength++;
			printf("Customer[%d]: Waiting in the Regular Line for next available PassportClerk\n",*index);
			regPassLineCV->Wait(passLineLock);			
		}
		printf("Customer[%d]: Finding available PassClerk...\n",*index);
		for(int x = 0; x < MAX_PASS_CLERKS; x++)
		{				
			if(passClerkStatuses[x] == CLERK_AVAILABLE)
			{
				myClerk = x;					
				printf("Customer[%d]: Going to chill with PassClerk[%d]\n",*index,myClerk);					
				passClerkStatuses[myClerk] = CLERK_BUSY;					
				break;				
			}
			else
				printf("Customer[%d]: PassClerk[%d] is unavailable\n",*index,x);

		}
		if (myClerk == -1) {
			if (waitAndRestart(passLineLock, *index)){           //SENATOR
				continue;
			} else {
				printf("PASS_CLERK: ESCAPED WITHOUT AN AVAILABLE CLERK, AND WITHOUT GOING TO WAITING ROOM!");
				exit(1); //FAIL 
			}	
		}
		passClerkLocks[myClerk]->Acquire();
		passLineLock->Release();							
		passClerkSSNs[myClerk] = *index;
		if(privLined && !bribed)
		{
			bribed = true;
			printf("Customer[%d]: Paying PassClerk[%d] $500 to fastpass the line\n",*index, myClerk);				
			passClerkMoney[myClerk] += 500;
			passClerkBribed[myClerk] = true;
			*cashDollars -= 500;
		}
		printf("Customer[%d]: Interacting with PassClerk[%d]\n",*index,myClerk);
		//interact with clerk			
		passClerkCVs[myClerk]->Signal(passClerkLocks[myClerk]);
		passClerkCVs[myClerk]->Wait(passClerkLocks[myClerk]);
		//get passport from clerk, if not ready, go to the back of the line?
		if(!passPunish[myClerk])
		{
			printf("Customer[%d]: Passport. GET!.\n", *index);						
			printf("Customer[%d]: Done and done.\n",*index);
			passClerkLocks[myClerk]->Release();
			printf("Customer[%d]: Going to next clerk...\n",*index);
			break;				
		}
		passClerkLocks[myClerk]->Release();
		printf("Customer[%d]: NOT READY!? Going back to the end of the line...\n",*index);
		int rando = rand()%80+20;					
		for(int x = 0; x < rando; x++)
			currentThread->Yield();															
	}

}


static void doCashierClerk(int* index, int* cashDollars)
{	
	
	while(true)
	{	
		if (waitAndRestart(NULL, *index)){
			continue;
		}
		int myClerk = -1;
		printf("Customer[%d]: Going to the CashClerk\n",*index);	
		cashLineLock->Acquire();	
		printf("Customer[%d]: What line should I choose for the CashClerk?\n",*index);
		//check for senator
		/*if(*cashDollars > 100) //get in a privledged line
		{						
		privLine = true;
		printf("Customer[%d]: Priveleged line, baby. CashDollars = $%d\n",*index,*cashDollars);
		privCashLineLength++;
		printf("Customer[%d]: Waiting in the Priveledged Line for next available CashClerk\n",*index);
		privCashLineCV->Wait(cashLineLock);			
		}
		else*/ //you can never get in the priveledged line for the cashier, given a max of $1600
		{
			printf("Customer[%d]: Regular line, suckas. CashDollars = $%d\n",*index,*cashDollars);
			regCashLineLength++;
			printf("Customer[%d]: Waiting in the Regular Line for next available CashClerk\n",*index);
			regCashLineCV->Wait(cashLineLock);			
		}		
		printf("Customer[%d]: Finding available CashClerk...\n",*index);
		for(int x = 0; x < MAX_CASH_CLERKS; x++)
		{				
			if(cashClerkStatuses[x] == CLERK_AVAILABLE)
			{
				myClerk = x;					
				printf("Customer[%d]: Going to chill with CashClerk[%d]\n",*index,myClerk);					
				cashClerkStatuses[myClerk] = CLERK_BUSY;					
				break;				
			}
			else
				printf("Customer[%d]: CashClerk[%d] is unavailable\n",*index,x);

		}	
		if (myClerk == -1) {
			if (waitAndRestart(cashLineLock, *index)){           //SENATOR
				continue;
			} else {
				printf("CASH_CLERK: ESCAPED WITHOUT AN AVAILABLE CLERK, AND WITHOUT GOING TO WAITING ROOM!");
				exit(1); //FAIL 
			}	
		}
		cashClerkLocks[myClerk]->Acquire();
		cashLineLock->Release();							
		cashClerkSSNs[myClerk] = *index;			
		printf("Customer[%d]: Interacting with CashClerk[%d]\n",*index,myClerk);			
		//interact with clerk			
		cashClerkCVs[myClerk]->Signal(cashClerkLocks[myClerk]);
		cashClerkCVs[myClerk]->Wait(cashClerkLocks[myClerk]);
		//pay for passport. If it's not processed, get back in line
		if(!cashPunish[myClerk])
		{				
			cashClerkMoney[myClerk] += 100;				
			*cashDollars-=100;
			cashClerkCVs[myClerk]->Signal(cashClerkLocks[myClerk]);
			cashClerkCVs[myClerk]->Wait(cashClerkLocks[myClerk]);
			printf("Customer[%d]: Passport paid for like a pro. CashDollars = [$%d]\n", *index, *cashDollars);											
			cashClerkLocks[myClerk]->Release();
			printf("Customer[%d]: GTFOing the office...\n",*index);
			customerOfficeLock->Acquire();
			customersInOffice--;
			customerOfficeLock->Release();
			break;				
		}
		cashClerkLocks[myClerk]->Release();
		printf("Customer[%d]: NOT READY!? Going back to the end of the line...\n",*index);
		int rando = rand()%80+20;					
		for(int x = 0; x < rando; x++)
			currentThread->Yield();																				
	}
}
