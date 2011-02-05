//Customer.cc
#include "office.h"
#include "system.h"
#include <iostream>

using namespace std;
void doAppClerk(int* index, int* cashDollars);
void doPicClerk(int* index, int* cashDollars);
void doPassPortClerk(int* index, int* cashDollars);
void doCashierClerk(int* index, int* cashDollars);
void CustomerRun(int index) {	
	
	printf("Customer[%d]: Entering the passport office...\n",index);
	
	//customer start up code		
	int cashDollars = ((rand() % 4) * 500) + 100;	
	int clerkStatus;
		
	printf("Customer[%d]: With $%d in my pocket\n",index,cashDollars);	
	totalCustomersInOffice++;
	
	//choose line		
	printf("Customer[%d]: Deciding between AppClerk and PictureClerk...\n", index);
	if(cashDollars > 100) //find priveledged line with shortest length
	{
		if(privAppLineLength<=privPicLineLength)
		{
			printf("Customer[%d]: Going to AppClerk first.\n",index);			
			doAppClerk(&index, &cashDollars);			
			doPicClerk(&index, &cashDollars);
		}
		else
		{
			printf("Customer[%d]: Going to PicClerk first.\n",index);			
			doPicClerk(&index, &cashDollars);			
			doAppClerk(&index, &cashDollars);			
		}	
	}
	else//find regular line with shortest length
	{
		if(regAppLineLength<=regPicLineLength)
		{
			printf("Customer[%d]: Going to AppClerk first.\n",index);
			doAppClerk(&index, &cashDollars);
			doPicClerk(&index, &cashDollars);
		}
		else
		{
			printf("Customer[%d]: Going to PicClerk first.\n",index);
			doPicClerk(&index, &cashDollars);
			doAppClerk(&index, &cashDollars);			
		}	
	}		
	
	//hit up the passport clerk
	doPassPortClerk(&index, &cashDollars);
	doCashierClerk(&index, &cashDollars);
}

void doAppClerk(int* index, int* cashDollars)
{
	
	int myClerk = -1;
	printf("Customer[%d]: Going to the AppClerk\n",*index);
	appPicLineLock->Acquire();	
		while(true)
		{		
			printf("Customer[%d]: What line should I choose for the AppClerk?\n",*index);
			//check for senator
			if(*cashDollars > 100) //get in a privledged line
			{						
				*cashDollars -= 500;
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
			appPicLineLock->Release();							
			appClerkLocks[myClerk]->Acquire();
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

void doPicClerk(int* index, int* cashDollars)
{
	int myClerk;
	printf("Customer[%d]: Going to the PicClerk\n",*index);
	appPicLineLock->Acquire();	
		while(true)
		{		
			printf("Customer[%d]: What line should I choose for the PicClerk?\n",*index);
			//check for senator
			if(*cashDollars > 100) //get in a privledged line
			{						
				*cashDollars -= 500;
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
			appPicLineLock->Release();		
			picClerkLocks[myClerk]->Acquire();		
			picClerkSSNs[myClerk] = *index;		
			
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

void doPassPortClerk(int *index, int* cashDollars){	
	int myClerk = -1;
	bool privLined = false;
	printf("Customer[%d]: Going to the PassportClerk\n",*index);
		while(true)
		{		
			printf("Customer[%d]: Acquiring PassLineLock...\n",*index);		
			passLineLock->Acquire();
			printf("Customer[%d]: What line should I choose for the PassportClerk?\n",*index);
			//check for senator
			if(*cashDollars > 100 || privLined) //get in a privledged line
			{	
				if(!privLined)
					*cashDollars -= 500;					
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
			passLineLock->Release();							
			passClerkLocks[myClerk]->Acquire();
			passClerkSSNs[myClerk] = *index;
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


void doCashierClerk(int* index, int* cashDollars)
{
	
	int myClerk = -1;
	printf("Customer[%d]: Going to the CashClerk\n",*index);
	cashLineLock->Acquire();	
		while(true)
		{		
			printf("Customer[%d]: What line should I choose for the CashClerk?\n",*index);
			//check for senator
			if(*cashDollars > 100) //get in a privledged line
			{						
				*cashDollars -= 500;
				printf("Customer[%d]: Priveleged line, baby. CashDollars = $%d\n",*index,*cashDollars);
				privCashLineLength++;
				printf("Customer[%d]: Waiting in the Priveledged Line for next available CashClerk\n",*index);
				privCashLineCV->Wait(cashLineLock);			
			}
			else //get in a normal line
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
			cashLineLock->Release();							
			cashClerkLocks[myClerk]->Acquire();
			cashClerkSSNs[myClerk] = *index;
			printf("Customer[%d]: Interacting with CashClerk[%d]\n",*index,myClerk);			
			//interact with clerk			
			cashClerkCVs[myClerk]->Signal(cashClerkLocks[myClerk]);
			cashClerkCVs[myClerk]->Wait(cashClerkLocks[myClerk]);
			//pay for passport. If it's not processed, get back in line
			if(!cashPunish[myClerk])
			{
				cashDollars-=100;
				printf("Customer[%d]: Passport paid for like a pro. CashDollars = [$%d]\n", *index, *cashDollars);											
				cashClerkLocks[myClerk]->Release();
				printf("Customer[%d]: GTFOing the office...\n",*index);
				break;				
			}
			cashClerkLocks[myClerk]->Release();
			printf("Customer[%d]: NOT READY!? Going back to the end of the line...\n",*index);
			int rando = rand()%80+20;					
			for(int x = 0; x < rando; x++)
				currentThread->Yield();																				
		}
}
