/* File for the Application, Picture, Passport and Cashier Clerks*/
#include "utils.h"
#include "office.h"


char* getCustomerType(){
	int check=0;
	/*senatorOfficeLock->Acquire();*/
	Acquire(entryLock);
	if (senatorsInOffice>0){
		check = 1;
	}
	/*senatorOfficeLock->Release();*/
	Release(entryLock);
	if (check == 0){
		return "Customer";
	}else{
		return "Senator";
	}
}

void appClerkFileData(){ /*int SSN*/
	int randomNum; 
	int i;
	int SSN;

	SSN = GetForkArg();

	randomNum = Rand()%80 + 20;
	for(i = 0; i< randomNum; i++){
		Yield();
	}
	appFiled[SSN] = TRUE;
	tprintf("ApplicationClerk informs %s with SSN %d that the application has been filed\n", SSN,0, 0,getCustomerType(),""); 
	Exit(0);
}

void picClerkFileData(){ /*int SSN*/
	int randomNum; 
	int i;
	int SSN;

	SSN = GetForkArg();

	randomNum = Rand()%80 + 20;
	for(i = 0; i< randomNum; i++){
		Yield();
	}
	picFiled[SSN] = TRUE;
	tprintf("PictureClerk informs %s with SSN %d that the picture has been filed\n",SSN,0,0,getCustomerType(),""); 
	Exit(0);
}

void passClerkFileData(){ /*int SSN*/
	int randomNum; 
	int i;
	int SSN;

	SSN = GetForkArg();

	randomNum = Rand()%80 + 20;
	for(i = 0; i< randomNum; i++){
		Yield();
	}
	passFiled[SSN] = TRUE;
	tprintf("PassportClerk informs %s with SSN %d that the passport has been filed\n", SSN,0,0,getCustomerType(),""); 
	Exit(0);

}




void AppClerkRun(){
	int SSN, i;
	int index;
	Acquire(appClerkUIDLock);
	index = appClerkUID++;
	Release(appClerkUIDLock);

	while (TRUE){		
		/*appPicLineLock->Acquire();*/
		Acquire(appPicLineLock);		

		/*Checking if anyone is in line*/
		if (privAppLineLength+regAppLineLength>0){
			tprintf("AppClerk %d: there are people in line.\n", index,0,0,"","");
			if (privAppLineLength>0){ /*Checking if anyone is in priv line*/
				tprintf("AppClerk %d: people in my priv line.\n", index,0,0,"","");
				tprintf("AppClerk %d: has spotted %s in privAppLine(length = %d)\n",index, privAppLineLength, 0,getCustomerType(),""); 
				privAppLineLength--;
				tprintf("AppClerk %d: Becoming Available!\n",index,0,0,"","");
				appClerkStatuses[index] = CLERK_AVAILABLE;
				tprintf("AppClerk %d: Signaling Condition variable (length of priv line is now %d)\n", index,privAppLineLength,0,"","");
				/*privAppLineCV->Signal(appPicLineLock);*/
				Signal(privAppLineCV, appPicLineLock);
			}
			else{ /*Check if anyone is in reg line*/
				tprintf("AppClerk %d: has spotted %s in regAppLine (length = %d)\n",index, regAppLineLength, 0, getCustomerType(),"");
				regAppLineLength--;
				tprintf("AppClerk %d: Becoming Available!\n",index,0,0,"","");
				appClerkStatuses[index] = CLERK_AVAILABLE;
				tprintf("AppClerk %d: Signaling Condition variable (length of reg line is now %d)\n", index,regAppLineLength,0,"","");
				/*regAppLineCV->Signal(appPicLineLock);*/
				Signal(regAppLineCV, appPicLineLock);
			}

			/*Customer/Senator - Clerk interaction*/
			tprintf("AppClerk %d: Acquiring my own lock\n",index,0,0,"","");
			/*appClerkLocks[index]->Acquire();*/
			Acquire(appClerkLocks[index]);
			tprintf("AppClerk %d: Releasing appPicLineLock\n",index,0,0,"","");
			/*appPicLineLock->Release();*/
			Release(appPicLineLock);
			tprintf("AppClerk %d: Putting myself to sleep...\n",index,0,0,"","");
			/*appClerkCVs[index]->Wait(appClerkLocks[index]);*/
			Wait(appClerkCVs[index], appClerkLocks[index]);
			tprintf("AppClerk %d: Just woke up!\n",index,0,0,"","");
			
			SSN = appClerkSSNs[index];
			tprintf("AppClerk %d: Just receieved %s's SSN: %d\n",index, SSN, 0, getCustomerType(),"");

			if (appClerkBribed[index] == TRUE){
				printf("ApplicationClerk [%d] accepts money = 500 from %s with SSN %d\n", index, appClerkSSNs[index], 0,getCustomerType(),"");
				appClerkBribed[index] = FALSE;
			}

			printf("ApplicationClerk [%d] informs %s with SSN %d that the procedure has been completed.\n", index, SSN, 0, getCustomerType(),""); 
			
			/*Thread* newThread = new Thread("Filing Thread");
			newThread->Fork((VoidFunctionPtr)appClerkFileData, SSN);
			
			NEED TO CONVERT THIS*/
			ForkWithArg(appClerkFileData, SSN);
			/* appFiled[SSN] = TRUE; GET RID OF THIS ONCE WE FIX SHIT*/
			
			/*for (i=0; i<10; i++){
				tprintf("AppFiled: %d,    PicFiled: %d\n",appFiled[i],picFiled[i]);
			}*/
			
			tprintf("AppClerk %d: Signaling my appClerkCV\n", index,0,0,"","");
			/*appClerkCVs[index]->Signal(appClerkLocks[index]);*/
			Signal(appClerkCVs[index], appClerkLocks[index]);
			tprintf("AppClerk %d: Releasing my own lock\n", index,0,0,"","");
			/*appClerkLocks[index]->Release();*/
			Release(appClerkLocks[index]);

		}
		else{ /*No one in line*/
			/*appPicLineLock->Release();*/
			Release(appPicLineLock);
			/*appClerkLocks[index]->Acquire();*/
			/*tprintf("ApplicationClerk [%d] acquiring his own lock\n", index,0,0,"","");*/
			Acquire(appClerkLocks[index]);
			printf("ApplicationClerk [%d] is going on break\n", index,0,0,"","");
			appClerkStatuses[index] = CLERK_ON_BREAK;
			/*appClerkCVs[index]->Wait(appClerkLocks[index]);*/
			Wait(appClerkCVs[index], appClerkLocks[index]);
			printf("ApplicationClerk [%d] returned from break\n", index,0,0,"","");
			/*appClerkLocks[index]->Release();*/
			Release(appClerkLocks[index]);
		}
		Yield();
	}
	printf("About to exit!", 0,0,0,"","");
	Exit(0);
}



/*--------------------------------------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------------------------------------*/

void PicClerkRun(){
	
	int SSN, count, i;
	int index;
	Acquire(picClerkUIDLock);
	index = picClerkUID++;
	Release(picClerkUIDLock);
	while (TRUE){
		/*appPicLineLock->Acquire();*/
		Acquire(appPicLineLock);

		/*Checking if anyone is line*/
		if(privPicLineLength+regPicLineLength>0){

			if (privPicLineLength>0){	/*Checking if anyone is in priv line*/
				tprintf("PicClerk %d: has spotted %s in privPicLine(length = %d)\n",index, privPicLineLength, 0,getCustomerType(),"");
				privPicLineLength--;
				tprintf("PicClerk %d: Becoming Available!\n",index,0,0,"","");
				picClerkStatuses[index] = CLERK_AVAILABLE;
				tprintf("PicClerk %d: Signaling Condition variable (length of priv line is now %d)\n", index,privPicLineLength,0,"","");
				/*privPicLineCV->Signal(appPicLineLock);*/
				Signal(privPicLineCV, appPicLineLock);
			}
			else{ /*Checking if anyone is in reg line*/
				tprintf("PicClerk %d: has spotted %s in regPicLine (length = %d)\n",index, regPicLineLength, 0,getCustomerType(),"");
				regPicLineLength--;
				tprintf("PicClerk %d: Becoming Available!\n",index,0,0,"","");
				picClerkStatuses[index] = CLERK_AVAILABLE;
				tprintf("PicClerk %d: Signaling Condition variable (length of reg line is now %d)\n", index,regPicLineLength,0,"","");
				/*regPicLineCV->Signal(appPicLineLock);*/
				Signal(regPicLineCV, appPicLineLock);
			}

			/*Customer/Senator - Clerk Interaction*/

			tprintf("PicClerk %d: Acquiring my own lock\n",index,0,0,"","");
			/*picClerkLocks[index]->Acquire();*/
			Acquire(picClerkLocks[index]);
			tprintf("PicClerk %d: Releasing appPicLineLock\n",index,0,0,"","");
			/*appPicLineLock->Release();*/
			Release(appPicLineLock);

			tprintf("PicClerk %d: Putting myself to sleep...\n",index,0,0,"","");
			/*picClerkCVs[index]->Wait(picClerkLocks[index]);*/
			Wait(picClerkCVs[index], picClerkLocks[index]);
			SSN = picClerkSSNs[index];
			if (picClerkBribed[index] == TRUE){
				/*printf("PicClerk %d: I've just been bribed for $500! Total amount I've collected to so far: $%d\n", index, picClerkMoney[index]);*/
				printf("PictureClerk %d accepts money = 500 from %s with SSN %d\n", index, SSN,0,getCustomerType(),"");
				picClerkBribed[index] = FALSE;
			}
			count = 1;
			do{
				if (count==1){
					printf("PictureClerk [%d] takes picture of %s with SSN %d\n", index, SSN, 0, getCustomerType(),"");
				}else{
					printf("PictureClerk [%d] takes picture of %s with SSN %d again\n", index, SSN, 0, getCustomerType(),"");
				}
				tprintf("PicClerk %d: Taking picture of %s for the %dst/nd/rd/th time (Signaling my CV)!\n",index, count, 0, getCustomerType(),"");
				/*picClerkCVs[index]->Signal(picClerkLocks[index]);*/
				Signal(picClerkCVs[index], picClerkLocks[index]);
				tprintf("PicClerk %d: Going to sleep...\n",index,0,0,"","");
				/*picClerkCVs[index]->Wait(picClerkLocks[index]);*/
				Wait(picClerkCVs[index], picClerkLocks[index]);

				if(happyWithPhoto[index] == TRUE){
					SSN = picClerkSSNs[index];
					
					/*Thread* t = new Thread("Pass Filing Thread");
					t->Fork((VoidFunctionPtr)picClerkFileData, SSN);
					NEEDS TO BE DEALT WITH*/

					ForkWithArg(picClerkFileData, SSN);
					/* picFiled[SSN] = TRUE; /*GET RID OF THIS ONCE WE FIX SHIT*/
					
				/*	for (i=0; i<10; i++){
						tprintf("AppFiled: %d,    PicFiled: %d\n",appFiled[i],picFiled[i],0,"","");
					}*/
					tprintf("PicClerk %d: Just woke up, %s with SSN %d liked their picture!\n",index, SSN, 0,getCustomerType(),"");
				}
				else{
					tprintf("PicClerk %d: Just woke up, %s did not like their picture.\n",index, 0, 0,getCustomerType(),"");
				}

				count++;

			}while(happyWithPhoto[index] == FALSE);
			printf("PictureClerk [%d] informs %s with SSN %d that the procedure has been completed\n", index,SSN, 0,getCustomerType(),""); 

			/*picClerkCVs[index]->Signal(picClerkLocks[index]);*/
			Signal(picClerkCVs[index], picClerkLocks[index]);
			
			tprintf("PicClerk %d: Releasing my own lock\n", index,0,0,"","");
			/*picClerkLocks[index]->Release();*/
			Release(picClerkLocks[index]);
		}

		else{ /*No one in line*/
			/*appPicLineLock->Release();*/
			Release(appPicLineLock);
			/*picClerkLocks[index]->Acquire();*/
			Acquire(picClerkLocks[index]);
			
			printf("PictureClerk [%d] is going on break\n", index,0,0,"","");
			picClerkStatuses[index] = CLERK_ON_BREAK;
			/*picClerkCVs[index]->Wait(picClerkLocks[index]);*/
			Wait(picClerkCVs[index], picClerkLocks[index]);
			printf("PictureClerk [%d] returned from break\n", index,0,0,"","");
			/*picClerkLocks[index]->Release();*/
			Release(picClerkLocks[index]);
		}
		Yield();
	}
	Exit(0);
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------------------------------------*/

void PassClerkRun(){

	int SSN, i;
	int index;
	Acquire(passClerkUIDLock);
	index = passClerkUID++;
	Release(passClerkUIDLock);
	while (TRUE){
		/*  printf("PassClerk %d: has acquired the passLineLock\n", index);
		passLineLock->Acquire();*/
		Acquire(passLineLock);

		/*Checking if anyone is in line*/
		if (privPassLineLength+regPassLineLength>0){

			if (privPassLineLength>0){ /*Checking if anyone is in priv line*/
				tprintf("PassClerk %d: has spotted %s in privPassLine(length = %d)\n", index, privPassLineLength, 0,getCustomerType(),"");
				privPassLineLength--;
				tprintf("PassClerk %d: Becoming Available!\n",index,0,0,"","");
				passClerkStatuses[index] = CLERK_AVAILABLE;
				tprintf("PassClerk %d: Signaling Condition variable (length of priv line is now %d)\n", index,privPassLineLength,0,"","");
				/*privPassLineCV->Signal(passLineLock);*/
				Signal(privPassLineCV, passLineLock);
			}
			else{ /*Check if anyone is in reg line*/
				tprintf("PassClerk %d: has spotted %s in regPassLine (length = %d)\n",index, regPassLineLength, 0, getCustomerType(),"");
				regPassLineLength--;
				tprintf("PassClerk %d: Becoming Available!\n",index,0,0,"","");
				passClerkStatuses[index] = CLERK_AVAILABLE;
				tprintf("PassClerk %d: Signaling Condition variable (length of reg line is now %d)\n", index,regPassLineLength,0,"","");
				/*regPassLineCV->Signal(passLineLock);*/
				Signal(regPassLineCV, passLineLock);
			}
			/*Customer/Senator - Clerk interaction*/

			tprintf("PassClerk %d: Acquiring my own lock\n",index,0,0,"","");
			/*passClerkLocks[index]->Acquire();*/
			Acquire(passClerkLocks[index]);
			tprintf("PassClerk %d: Releasing passLineLock\n",index,0,0,"","");
			/*passLineLock->Release();*/
			Release(passLineLock);
			tprintf("PassClerk %d: Putting myself to sleep...\n",index,0,0,"","");
			/*passClerkCVs[index]->Wait(passClerkLocks[index]);*/
			Wait(passClerkCVs[index], passClerkLocks[index]);
			tprintf("PassClerk %d: Just woke up!\n",index,0,0,"","");
			SSN = passClerkSSNs[index];
			if (passClerkBribed[index] == TRUE){
				
				printf("PassportClerk %d accepts money = 500 from %s with SSN %d\n", index, SSN, 0,getCustomerType(),"");
				passClerkBribed[index] = FALSE;
			}
					
			if(appFiled[SSN] == FALSE || picFiled[SSN] == FALSE){
			/*	for (i=0; i<10; i++){
					tprintf("AppFiled: %d,    PicFiled: %d,     PassFiled: %d\n",appFiled[i],picFiled[i], passFiled[i],"","");
				}*/
				
				printf("PassportClerk [%d] gives invalid certification to %s with SSN %d\n", index, SSN, 0, getCustomerType(),""); 
				printf("PassportClerk [%d] punishes %s with SSN %d to wait\n", index, SSN, 0, getCustomerType(),""); 
				passPunish[index] = TRUE;
			}
			else{
			
				printf("PassportClerk [%d] gives valid certification to %s with SSN %d\n", index, SSN, 0, getCustomerType(),""); 
				
				passPunish[index] = FALSE;
				/*Thread* newThread = new Thread("Passport Filing Thread");
				newThread->Fork((VoidFunctionPtr)passClerkFileData, SSN);
				NEEDS TO BE DEALT WITH*/

				ForkWithArg(passClerkFileData, SSN);
				/* passFiled[SSN] = TRUE; /*GET RID OF THIS ONCE WE FIX SHIT*/

				/*DEBUG
				for (int i=0; i<10; i++){
					tprintf("AppFiled: %d,    PicFiled: %d,     PassFiled: %d\n", appFiled[i], picFiled[i], passFiled[i]);
				}*/
				printf("PassportClerk [%d] informs %s with SSN %d that the procedure has completed.\n", index, SSN, 0, getCustomerType(),""); 
			}
			

			
			tprintf("PassClerk %d: Signaling my passClerkCV\n", index,0,0,"","");
			/*passClerkCVs[index]->Signal(passClerkLocks[index]);*/
			Signal(passClerkCVs[index], passClerkLocks[index]);
			tprintf("PassClerk %d: Releasing my own lock\n", index,0,0,"","");
			/*passClerkLocks[index]->Release();*/
			Release(passClerkLocks[index]);

		}
		else{ /*No one in line*/
			/*passLineLock->Release();*/
			Release(passLineLock);
			/*passClerkLocks[index]->Acquire();*/
			Acquire(passClerkLocks[index]);
			printf("PassportClerk [%d] is going on break\n", index,0,0,"","");
			passClerkStatuses[index] = CLERK_ON_BREAK;
			/*passClerkCVs[index]->Wait(passClerkLocks[index]);*/
			Wait(passClerkCVs[index], passClerkLocks[index]);
			printf("PassportClerk [%d] returned from break\n", index,0,0,"","");
			/*passClerkLocks[index]->Release();*/
			Release(passClerkLocks[index]);
		}
		Yield();
	}
	Exit(0);
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------------------------------------*/

void CashClerkRun(){
	int SSN;
	int index;
	Acquire(cashClerkUIDLock);
	index = cashClerkUID++;
	Release(cashClerkUIDLock);
	while (TRUE){
	
		/*cashLineLock->Acquire();*/
		Acquire(cashLineLock);

		
		if (regCashLineLength>0){ /*Check if anyone is in reg line*/
			tprintf("CashClerk %d: has spotted %s in regCashLine (length = %d)\n",index, regCashLineLength, 0,getCustomerType(),"");
			regCashLineLength--;
			tprintf("CashClerk %d: Becoming Available!\n",index,0,0,"","");
			cashClerkStatuses[index] = CLERK_AVAILABLE;
			tprintf("CashClerk %d: Signaling Condition variable (length of reg line is now %d)\n", index,regCashLineLength,0,"","");
			/*regCashLineCV->Signal(cashLineLock);*/
			Signal(regCashLineCV, cashLineLock);

			/*Customer/Senator - Clerk interaction*/

			tprintf("CashClerk %d: Acquiring my own lock\n",index,0,0,"","");
			/*cashClerkLocks[index]->Acquire();*/
			Acquire(cashClerkLocks[index]);
			tprintf("CashClerk %d: Releasing cashLineLock\n",index,0,0,"","");
			/*cashLineLock->Release();*/
			Release(cashLineLock);
			tprintf("CashClerk %d: Putting myself to sleep...\n",index,0,0,"","");
			/*cashClerkCVs[index]->Wait(cashClerkLocks[index]);*/
			Wait(cashClerkCVs[index], cashClerkLocks[index]);
			tprintf("CashClerk %d: Just woke up!\n",index,0,0,"","");
			SSN = cashClerkSSNs[index];
		
			if(passFiled[SSN] == FALSE){
				
				printf("Cashier [%d] gives invalid certification to %s with SSN %d\n", index, SSN, 0, getCustomerType(),"");
				printf("Cashier [%d] punishes %s with SSN %d to wait\n", index, SSN, 0, getCustomerType(),""); 
				
				cashPunish[index] = TRUE;
			}
			else{
				tprintf("CashClerk %d: %s with SSN %d has everything filed correctly!\n",index,SSN,0,getCustomerType(),"");
				printf("Cashier [%d] gives valid certification to %s with SSN %d\n", index, SSN, 0, getCustomerType(),"");
				printf("Cashier [%d] records %s with SSN %d's passport\n", index, SSN, 0, getCustomerType(),"");
				printf("Cashier [%d] has recorded the passport for %s with SSN %d\n", index, SSN, 0, getCustomerType(),"");
				cashPunish[index] = FALSE;
				cashFiled[SSN] = TRUE;
				printf("Cashier [%d] accepts money = 100 from %s with SSN %d\n", index, SSN, 0, getCustomerType(),"");
				/*cashClerkCVs[index]->Signal(cashClerkLocks[index]);*/
				Signal(cashClerkCVs[index], cashClerkLocks[index]);
				/*cashClerkCVs[index]->Wait(cashClerkLocks[index]);*/
				Wait(cashClerkCVs[index], cashClerkLocks[index]);
				tprintf("CashClerk %d: Total money collected: $%d\n",index, cashClerkMoney[index],0,"","");

				
			}

			tprintf("CashClerk %d: Signaling my cashClerkCV\n", index,0,0,"","");
			/*cashClerkCVs[index]->Signal(cashClerkLocks[index]);*/
			Signal(cashClerkCVs[index], cashClerkLocks[index]);
			tprintf("CashClerk %d: Releasing my own lock\n", index,0,0,"","");
			/*cashClerkLocks[index]->Release();*/
			Release(cashClerkLocks[index]);


		}
		else{ /*No one in line*/

			/*cashLineLock->Release();*/
			Release(cashLineLock);
			/*cashClerkLocks[index]->Acquire();*/
			Acquire(cashClerkLocks[index]);
			printf("Cashier [%d] is going on break\n", index,0,0,"","");
			cashClerkStatuses[index] = CLERK_ON_BREAK;
			/*cashClerkCVs[index]->Wait(cashClerkLocks[index]);*/
			Wait(cashClerkCVs[index], cashClerkLocks[index]);
			printf("Cashier [%d] returned from break\n", index,0,0,"","");
			/*cashClerkLocks[index]->Release();*/
			Release(cashClerkLocks[index]);
		}
		Yield();

	}
	Exit(0);

}
