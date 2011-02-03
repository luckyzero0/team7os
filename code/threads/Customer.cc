//Customer.cc
#include "office.h"

void CustomerRun(){
	cout<<"Entering the passport office..."<<endl;
	
	//customer start up code	
	srand( time(NULL) );
	int cashDollars = ((rand() % 3) * 500) + 100;
	int myClerk;
	
	cout<<"With $"<<cashDollars<<" in my pocket"<<endl;
	//acquire ACPCLock
	appPicLineLock->Acquire();	
	totalCustomersInOffice++;
	
	//choose line		
	cout<<"Deciding between AppClerk and PictureClerk..."<<endl;
		
	//app clerk		
	cout<<"Going to the AppClerk"<<endl;
	while(true)
	{		
		cout<<"What line should I choose for the AppClerk?"<<endl;
		//check for senator
		if(cashDollars > 100) //get in a privledged line
		{						
			cashDollars -= 500;
			cout<<"Priveleged line, baby. CashDollars = $"<<cashDollars<<endl 
			privAppLineLength++;
			cout<<"Waiting in the Priveledged Line for next available clerk"<<endl;
			privAppLineCV->Wait(appPicLineLock);			
		}
		else //get in a normal line
		{
			cout<<"Regular line, suckahs. CashDollars = $"<<cashDollars<<endl;
			regAppLineLength++;
			cout<<"Waiting in the Regular Line for next available clerk"<<endl;
			regAppLineCV->Wait(appPicLineLock);			
		}				
		cout<<"Finding available clerk..."<<endl;		
		for(int x = 0; x < MAX_APP_CLERKS; x++)
		{
			if(appClerkStatuses[x] == ClerkStatus.CLERK_AVAILABLE)
			{
				myClerk = x;
				cout<<"Going to chill with App Clerk #"<<myClerk<<endl;				
				break;				
			}
		}
		appPicLineLock->Release();			
		cout<<"Interacting with app clerk"<<endl;
		//interact with clerk
		cout<<"Done and done."<<endl;
		
		break;
	}	
	
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


