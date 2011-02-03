//Customer.cc
#include "office.h"

void CustomerRun(){
	
	//customer start up code	
	srand( time(NULL) );
	int cashDollars = ((rand() % 3) * 500) + 100;
	//acquire ACPCLock
	appPicLineLock->Acquire();	
	totalCustomersInOffice++;
	
	//choose line		
		
	//app clerk		
	while(true)
	{		
		//check for senator
		if(cashDollars > 100) //get in a privledged line
		{			
			cashDollars -= 500;
			privAppLineLength++;
			privAppLineCV->Wait(appPicLineLock);
			privAppLineLength--;
		}
		else //get in a normal line
		{
			regAppLineLength++;
			regAppLineCV->Wait(appPicLineLock);
			regAppLineLength--;
		}						
		//interact with clerk
		appPicLineLock->Release();
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


