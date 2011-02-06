//Senator
#include "office."h
#include "system.h"

using namespace std;
void doAppClerk(int* index, int* cashDollars);
void doPicClerk(int* index, int* cashDollars);
void doPassPortClerk(int* index, int* cashDollars);
void doCashierClerk(int* index, int* cashDollars);
void SenatorRun(int index)
{
	tprintf("Senator[%d]: Acquiring senatorOfficeLock\n", index);
	senatorOfficeLock->Acquire();
	if (senatorsInOffice == 0){
		tprintf("Senator[%d]: There are no other Senators in office, going to Senator waiting room\n", index);
		senatorOfficeLock->Release();
		senatorWaitingRoomLock->Acquire();
		senatorsInWaitingRoom++;
		tprintf("Senator[%d]: In the waiting room, taking a nap...\n", index);
		senatorWaitingRoomCV->Wait(senatorWaitingRoomLock);
		tprintf("Senator[%d]: Waking up, going to the passport office!\n", index);
		senatorsInWaitingRoom--;
		//senatorWaitingRoomLock->Release();
		//totalSenatorsLock->Acquire();
	}
	
		
	//Start Behaving like a Customer
	printf("Senator[%d]: Entering the passport office...\n",index);
	
	//senator start up code		
	int cashDollars = ((rand() % 4) * 500) + 100;	
	int clerkStatus;
		
	printf("Senator[%d]: With $%d in my pocket\n",index,cashDollars);
	
	senatorsInOffice++;
	senatorWaitingRoomLock->Release();
	senatorOfficeLock->Release();
	
	//choose line		
	printf("Senator[%d]: Deciding between AppClerk and PictureClerk...\n", index);
	if(cashDollars > 100) //find priveledged line with shortest length
	{
		if(privAppLineLength<=privPicLineLength)
		{
			printf("Senator[%d]: Going to AppClerk first.\n",index);			
			doAppClerk(&index, &cashDollars);			
		       	doPicClerk(&index, &cashDollars);
		}
		else
		{
			printf("Senator[%d]: Going to PicClerk first.\n",index);			
		       	doPicClerk(&index, &cashDollars);			
			doAppClerk(&index, &cashDollars);			
		}	
	}
	else//find regular line with shortest length
	{
		if(regAppLineLength<=regPicLineLength)
		{
			printf("Senator[%d]: Going to AppClerk first.\n",index);
			doAppClerk(&index, &cashDollars);
		       	doPicClerk(&index, &cashDollars);
		}
		else
		{
			printf("Senator[%d]: Going to PicClerk first.\n",index);
		       	doPicClerk(&index, &cashDollars);
			doAppClerk(&index, &cashDollars);			
		}	
	}
	
	//hit up the passport clerk
	doPassPortClerk(&index, &cashDollars);
	
	//proceed to cashier
	doCashierClerk(&index, &cashDollars);
	
	
}

void doAppClerk(int* index, int* cashDollars)
{
	bool privLine = false;
	int myClerk = -1;
	printf("Senator[%d]: Going to the AppClerk\n",*index);
	appPicLineLock->Acquire();	
		while(true)
		{		
			printf("Senator[%d]: What line should I choose for the AppClerk?\n",*index);
			//check for senator
			if(*cashDollars > 100) //get in a privledged line
			{					
				privLine = true;					
				printf("Senator[%d]: Priveleged line, baby. CashDollars = $%d\n",*index,*cashDollars);
				privAppLineLength++;
				printf("Senator[%d]: Waiting in the Priveledged Line for next available AppClerk\n",*index);
				privAppLineCV->Wait(appPicLineLock);							
			}
			else //get in a normal line
			{
				printf("Senator[%d]: Regular line, suckas. CashDollars = $%d\n",*index,*cashDollars);
				regAppLineLength++;
				printf("Senator[%d]: Waiting in the Regular Line for next available AppClerk\n",*index);
				regAppLineCV->Wait(appPicLineLock);			
			}				
			printf("Senator[%d]: Finding available AppClerk...\n",*index);
			for(int x = 0; x < MAX_APP_CLERKS; x++)
			{				
				if(appClerkStatuses[x] == CLERK_AVAILABLE)
				{
					myClerk = x;					
					printf("Senator[%d]: Going to chill with AppClerk[%d]\n",*index,myClerk);					
					appClerkStatuses[myClerk] = CLERK_BUSY;					
					break;				
				}
				else
					printf("Senator[%d]: AppClerk[%d] is unavailable\n",*index,x);
				
			}
			appClerkLocks[myClerk]->Acquire();
			appPicLineLock->Release();							
			if(privLine)
			{
				printf("Senator[%d]: Paying AppClerk[%d] $500 to fastpass the line\n",*index, myClerk);				
				appClerkMoney[myClerk] += 500;
				appClerkBribed[myClerk] = true;
				*cashDollars -= 500;
			}
			printf("Senator[%d]: Interacting with AppClerk[%d]\n",*index,myClerk);
			//interact with clerk
			appClerkSSNs[myClerk] = *index; //could also just use the adr for a more ssn-like number
			printf("Senator[%d]: Application handed in like a boss.\n", *index);			
			appClerkCVs[myClerk]->Signal(appClerkLocks[myClerk]);
			appClerkCVs[myClerk]->Wait(appClerkLocks[myClerk]);
			printf("Senator[%d]: Application received like a boss.\n", *index);
			//more shit			
			printf("Senator[%d]: Done and done.\n",*index);
			appClerkLocks[myClerk]->Release();
			printf("Senator[%d]: Going to next clerk...\n",*index);
			break;
		}
}

void doPicClerk(int* index, int* cashDollars)
{
	int myClerk;
	bool privLine = false;
	printf("Senator[%d]: Going to the PicClerk\n",*index);
	appPicLineLock->Acquire();	
		while(true)
		{		
			printf("Senator[%d]: What line should I choose for the PicClerk?\n",*index);
			//check for senator
			if(*cashDollars > 100) //get in a privledged line
			{						
				privLine = true;
				printf("Senator[%d]: Priveleged line, baby. CashDollars = $%d\n",*index,*cashDollars);
				privPicLineLength++;
				printf("Senator[%d]: Waiting in the Priveledged Line for next available PicClerk\n",*index);
				privPicLineCV->Wait(appPicLineLock);			
			}
			else //get in a normal line
			{
				printf("Senator[%d]: Regular line, suckas. CashDollars = $%d\n",*index,*cashDollars);
				regPicLineLength++;
				printf("Senator[%d]: Waiting in the Regluar Line for next available PicClerk\n",*index);
				regPicLineCV->Wait(appPicLineLock);			
			}				
			printf("Senator[%d]: Finding available PicClerk...\n",*index);
			for(int x = 0; x < MAX_PIC_CLERKS; x++)
			{					
				
				if(picClerkStatuses[x] == CLERK_AVAILABLE)
				{
					myClerk = x;					
					printf("Senator[%d]: Going to chill with PicClerk[%d]\n",*index,myClerk);					
					picClerkStatuses[myClerk] = CLERK_BUSY;					
					break;				
				}
				else
					printf("Senator[%d]: PicClerk[%d] is unavailable\n",*index,x);
			}
			picClerkLocks[myClerk]->Acquire();
			appPicLineLock->Release();				
			picClerkSSNs[myClerk] = *index;		
			if(privLine)
			{
				printf("Senator[%d]: Paying PicClerk[%d] $500 for fastpassing the line\n",*index, myClerk);				
				picClerkMoney[myClerk] += 500;
				picClerkBribed[myClerk] = true;
				*cashDollars -= 500;
			}
			printf("Senator[%d]: Interacting with Pic Clerk\n",*index);
			//interact with clerk
				
			
			while(happyWithPhoto[myClerk] == FALSE)
			{

				printf("Senator[%d]: Getting my picture taken...\n",*index);
				picClerkCVs[myClerk]->Signal(picClerkLocks[myClerk]);
				picClerkCVs[myClerk]->Wait(picClerkLocks[myClerk]);		
						
				//did I like my picture?
				if(rand()%10 > 5)
				{
					happyWithPhoto[myClerk] = TRUE;
					printf("Senator[%d]: This picture is awesome!\n", *index);
				}
				else
				{
					printf("Senator[%d]: This picture sucks! Take it again!\n",*index);
				}		
				
				picClerkCVs[myClerk]->Signal(picClerkLocks[myClerk]);	
				picClerkCVs[myClerk]->Wait(picClerkLocks[myClerk]);						
						
			}

			happyWithPhoto[myClerk] = FALSE;
			
			printf("Senator[%d]: Picture taken. Like a boss.\n",*index);
			picClerkLocks[myClerk]->Release();			
			
			printf("Senator[%d]: Done and done.\n",*index);											
			break;
		}
}

void doPassPortClerk(int *index, int* cashDollars){	
	int myClerk = -1;
	bool privLined = false;
	bool bribed = false;
	printf("Senator[%d]: Going to the PassportClerk\n",*index);
		while(true)
		{		
			printf("Senator[%d]: Acquiring PassLineLock...\n",*index);		
			passLineLock->Acquire();
			printf("Senator[%d]: What line should I choose for the PassportClerk?\n",*index);
			//check for senator
			if(*cashDollars > 100 || privLined) //get in a privledged line
			{								
				privLined = true;				
				printf("Senator[%d]: Priveleged line, baby. CashDollars = $%d\n",*index,*cashDollars);
				privPassLineLength++;
				printf("Senator[%d]: Waiting in the Priveledged Line for next available PassportClerk\n",*index);
				privPassLineCV->Wait(passLineLock);			
			}
			else //get in a normal line
			{
				printf("Senator[%d]: Regular line, suckas. CashDollars = $%d\n",*index,*cashDollars);
				regPassLineLength++;
				printf("Senator[%d]: Waiting in the Regular Line for next available PassportClerk\n",*index);
				regPassLineCV->Wait(passLineLock);			
			}				
			printf("Senator[%d]: Finding available PassClerk...\n",*index);
			for(int x = 0; x < MAX_PASS_CLERKS; x++)
			{				
				if(passClerkStatuses[x] == CLERK_AVAILABLE)
				{
					myClerk = x;					
					printf("Senator[%d]: Going to chill with PassClerk[%d]\n",*index,myClerk);					
					passClerkStatuses[myClerk] = CLERK_BUSY;					
					break;				
				}
				else
					printf("Senator[%d]: PassClerk[%d] is unavailable\n",*index,x);
				
			}
			passClerkLocks[myClerk]->Acquire();
			passLineLock->Release();							
			passClerkSSNs[myClerk] = *index;
			if(privLined && !bribed)
			{
				bribed = true;
				printf("Senator[%d]: Paying PassClerk[%d] $500 to fastpass the line\n",*index, myClerk);				
				passClerkMoney[myClerk] += 500;
				passClerkBribed[myClerk] = true;
				*cashDollars -= 500;
			}
			printf("Senator[%d]: Interacting with PassClerk[%d]\n",*index,myClerk);
			//interact with clerk			
			passClerkCVs[myClerk]->Signal(passClerkLocks[myClerk]);
			passClerkCVs[myClerk]->Wait(passClerkLocks[myClerk]);
			//get passport from clerk, if not ready, go to the back of the line?
			if(!passPunish[myClerk])
			{
				printf("Senator[%d]: Passport. GET!.\n", *index);						
				printf("Senator[%d]: Done and done.\n",*index);
				passClerkLocks[myClerk]->Release();
				printf("Senator[%d]: Going to next clerk...\n",*index);
				break;				
			}
			passClerkLocks[myClerk]->Release();
			printf("Senator[%d]: NOT READY!? Going back to the end of the line...\n",*index);
			int rando = rand()%80+20;					
			for(int x = 0; x < rando; x++)
				currentThread->Yield();															
		}
	
}


void doCashierClerk(int* index, int* cashDollars)
{	
	int myClerk = -1;
	printf("Senator[%d]: Going to the CashClerk\n",*index);	
		while(true)
		{		
			cashLineLock->Acquire();	
			printf("Senator[%d]: What line should I choose for the CashClerk?\n",*index);
			//check for senator
			/*if(*cashDollars > 100) //get in a privledged line
			{						
				privLine = true;
				printf("Senator[%d]: Priveleged line, baby. CashDollars = $%d\n",*index,*cashDollars);
				privCashLineLength++;
				printf("Senator[%d]: Waiting in the Priveledged Line for next available CashClerk\n",*index);
				privCashLineCV->Wait(cashLineLock);			
			}
			else*/ //you can never get in the priveledged line for the cashier, given a max of $1600
			{
				printf("Senator[%d]: Regular line, suckas. CashDollars = $%d\n",*index,*cashDollars);
				regCashLineLength++;
				printf("Senator[%d]: Waiting in the Regular Line for next available CashClerk\n",*index);
				regCashLineCV->Wait(cashLineLock);			
			}				
			printf("Senator[%d]: Finding available CashClerk...\n",*index);
			for(int x = 0; x < MAX_CASH_CLERKS; x++)
			{				
				if(cashClerkStatuses[x] == CLERK_AVAILABLE)
				{
					myClerk = x;					
					printf("Senator[%d]: Going to chill with CashClerk[%d]\n",*index,myClerk);					
					cashClerkStatuses[myClerk] = CLERK_BUSY;					
					break;				
				}
				else
					printf("Senator[%d]: CashClerk[%d] is unavailable\n",*index,x);
				
			}			
			cashClerkLocks[myClerk]->Acquire();
			cashLineLock->Release();							
			cashClerkSSNs[myClerk] = *index;			
			printf("Senator[%d]: Interacting with CashClerk[%d]\n",*index,myClerk);			
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
				printf("Senator[%d]: Passport paid for like a pro. CashDollars = [$%d]\n", *index, *cashDollars);											
				cashClerkLocks[myClerk]->Release();
				printf("Senator[%d]: GTFOing the office...\n",*index);
				totalSenatorsLock->Acquire();
				senatorsInOffice--;
				totalSenatorsLock->Release();
				break;				
			}
			cashClerkLocks[myClerk]->Release();
			printf("Senator[%d]: NOT READY!? Going back to the end of the line...\n",*index);
			int rando = rand()%80+20;					
			for(int x = 0; x < rando; x++)
				currentThread->Yield();																				
		}
}
