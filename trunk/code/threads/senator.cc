//Senator
#include "office.h"
#include "system.h"

using namespace std;
void doAppClerk(int* index, int* cashDollars);
void doPicClerk(int* index, int* cashDollars);
void doPassPortClerk(int* index, int* cashDollars);
void doCashierClerk(int* index, int* cashDollars);
void SenatorRun(int index)
{
//Might need a lock for this check
	senatorsInOfficeLock->Acquire();
	if (senatorsInOffice == FALSE){
		senatorsInOfficeLock->Release();
		senatorWaitingRoomLock->Acquire();
		senatorsInWaitingRoom++;
		senatorWaitingRoomCV->Wait(senatorWaitingRoomLock);
		senatorsInWaitingRoom--;
		senatorWaitingRoomLock->Release();
	}
	senatorsInOfficeLock->Acquire();
	senatorsInOffice++;
	senatorsInOfficeLock->Release();
	
	//Start Behaving like a Customer
	
	
}

void doAppClerk(int* index, int* cashDollars)
{
	bool privLine = false;
	int myClerk = -1;
	printf("Customer[%d]: Going to the AppClerk\n",*index);
	appPicLineLock->Acquire();	
		while(true)
		{		
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

void doPicClerk(int* index, int* cashDollars)
{
	int myClerk;
	bool privLine = false;
	printf("Customer[%d]: Going to the PicClerk\n",*index);
	appPicLineLock->Acquire();	
		while(true)
		{		
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

void doPassPortClerk(int *index, int* cashDollars){	
	int myClerk = -1;
	bool privLined = false;
	bool bribed = false;
	printf("Customer[%d]: Going to the PassportClerk\n",*index);
		while(true)
		{		
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


void doCashierClerk(int* index, int* cashDollars)
{	
	int myClerk = -1;
	printf("Customer[%d]: Going to the CashClerk\n",*index);	
		while(true)
		{		
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
				totalCustomersLock->Acquire();
				totalCustomersInOffice--;
				totalCustomersLock->Release();
				break;				
			}
			cashClerkLocks[myClerk]->Release();
			printf("Customer[%d]: NOT READY!? Going back to the end of the line...\n",*index);
			int rando = rand()%80+20;					
			for(int x = 0; x < rando; x++)
				currentThread->Yield();																				
		}
}
