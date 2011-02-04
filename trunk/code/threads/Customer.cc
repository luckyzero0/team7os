//Customer.cc
#include "office.h"
#include <iostream>

using namespace std;
void doAppClerk(int* index, int* cashDollars);
void doPicClerk(int* index, int* cashDollars);
void doPPClerk(int* index, int* cashDollars);
void doCashierClerk(int* index, int* cashDollars);
void CustomerRun(int index) {	
	
	printf("Customer[%d]: Entering the passport office...\n",index);
	
	//customer start up code		
	int cashDollars = ((rand() % 4) * 500) + 100;	
	int clerkStatus;
		
	printf("Customer[%d]: With $%d in my pocket\n",index,cashDollars);
	//acquire ACPCLock
	appPicLineLock->Acquire();	
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
}

void doAppClerk(int* index, int* cashDollars)
{
	int myClerk = 9001;
	printf("Customer[%d]: Going to the AppClerk\n",*index);
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
			appClerkData[myClerk] = *index; //could also just use the adr for a more ssn-like number
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
				printf("Customer[%d]: Waiting in the Priveledged Line for next available PicClerk\n",*index);
				regPicLineCV->Wait(appPicLineLock);			
			}				
			printf("Customer[%d]: Finding available PicClerk...\n",*index);
			for(int x = 0; x < MAX_APP_CLERKS; x++)
			{
				if(picClerkStatuses[x] == CLERK_AVAILABLE)
				{
					myClerk = x;
					picClerkStatuses[myClerk] = CLERK_BUSY;
					printf("Customer[%d]: Going to chill with Pic Clerk #%d\n",*index,myClerk);
					break;				
				}
			}
			appPicLineLock->Release();		
			picClerkLocks[myClerk]->Acquire();		
			printf("Customer[%d]: Interacting with Pic Clerk\n",*index);
			//interact with clerk
			
			while(picClerkData[myClerk] == FALSE)
			{
				printf("Customer[%d]: Getting my picture taken...\n",*index);
				picClerkCVs[myClerk]->Signal(picClerkLocks[myClerk]);
				picClerkCVs[myClerk]->Wait(picClerkLocks[myClerk]);
				//did I like my picture?
				if(true)
				{
					picClerkData[myClerk] = TRUE;
					printf("Customer[%d]: This picture is awesome!\n", *index);
				}
				else
				{
					printf("Customer[%d]: This picture sucks! Take it again!\n",*index);
				}						
			}
			
			printf("Customer[%d]: Picture taken. Like a boss.\n",*index);
			picClerkLocks[myClerk]->Release();			
			
			printf("Customer[%d]: Done and done.\n",*index);											
			break;
		}
}

