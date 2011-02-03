//Customer.cc
#include "office.h"
#include <iostream>

using namespace std;
void doAppClerk(int* index, int* cashDollars);
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
	if(rand()%2 == 0)
	{
		clerkStatus = APPLICATION;
	}
	else
	{
		clerkStatus = PICTURE;
	}
	
	doAppClerk(&index, &cashDollars);
	
	//check for senator
	
	
	
	//choose normal or priv line
	
	//wait
	
	//interact with clerk
	
	//check for senator
	
	//go to other clerk
	
	//choose line
	
	//wait
	
	//interact with clerk
	
	//etc etc
	
}

void doAppClerk(int* index, int* cashDollars)
{
	int myClerk;
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
				printf("Customer[%d]: Waiting in the Priveledged Line for next available AppClerk\n",*index);
				regAppLineCV->Wait(appPicLineLock);			
			}				
			printf("Customer[%d]: Finding available AppClerk...\n",*index);
			for(int x = 0; x < MAX_APP_CLERKS; x++)
			{
				if(appClerkStatuses[x] == CLERK_AVAILABLE)
				{
					myClerk = x;
					appClerkStatuses[myClerk] = CLERK_BUSY;
					printf("Customer[%d]: Going to chill with App Clerk #%d\n",*index,myClerk);
					break;				
				}
			}			
			appPicLineLock->Release();							
			appClerkLocks[myClerk]->Acquire();
			printf("Customer[%d]: Interacting with app clerk\n",*index);
			//interact with clerk
			appClerkData[myClerk] = *index; //could also just use the adr for a more ssn-like number
			printf("Customer[%d]: Application handed in like a boss.\n", *index);			
			appClerkCVs[myClerk]->Signal(appClerkLocks[myClerk]);
			appClerkCVs[myClerk]->Wait(appClerkLocks[myClerk]);
			printf("Customer[%d]: Application received like a boss.\n", *index);
			//more shit			
			printf("Customer[%d]: Done and done.\n",*index);
			appClerkLocks[myClerk]->Release();
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
				privAppLineLength++;
				printf("Customer[%d]: Waiting in the Priveledged Line for next available PicClerk\n",*index);
				privAppLineCV->Wait(appPicLineLock);			
			}
			else //get in a normal line
			{
				printf("Customer[%d]: Regular line, suckas. CashDollars = $%d\n",*index,*cashDollars);
				regAppLineLength++;
				printf("Customer[%d]: Waiting in the Priveledged Line for next available PicClerk\n",*index);
				regAppLineCV->Wait(appPicLineLock);			
			}				
			printf("Customer[%d]: Finding available PicClerk...\n",*index);
			for(int x = 0; x < MAX_APP_CLERKS; x++)
			{
				if(appClerkStatuses[x] == CLERK_AVAILABLE)
				{
					myClerk = x;
					appClerkStatuses[myClerk] = CLERK_BUSY;
					printf("Customer[%d]: Going to chill with Pic Clerk #%d\n",*index,myClerk);
					break;				
				}
			}
			appPicLineLock->Release();		
			picClerkLocks[myClerk]->Acquire();		
			printf("Customer[%d]: Interacting with Pic Clerk\n",*index);
			//interact with clerk
			printf("Customer[%d]: Getting my picture taken...\n",*index);
			//did I like my picture?
			//if not, retake it
			//else
			
			printf("Customer[%d]: Picture taken. Like a boss.\n",*index);
			picClerkLocks[myClerk]->Release();			
			
			printf("Customer[%d]: Done and done.\n",*index);											
			break;
		}
}

