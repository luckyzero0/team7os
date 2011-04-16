
#include "officeData.h"
#include "utils.h"

static void doAppClerk(int* index, int* cashDollars);
static void doPicClerk(int* index, int* cashDollars);
static void doPassPortClerk(int* index, int* cashDollars);
static void doCashierClerk(int* index, int* cashDollars);
void initServerData();
void SenatorRun();




int main(){
	printf("INT MAIN BITCHEZ\n",0,0,0,"","");
	initServerData();
	SenatorRun();
	Exit(0);
	
}

void initServerData(){

/*make all the locks*/
	appPicLineLock = CreateLock("appPicLineLock",14);
	passLineLock = CreateLock("passLineLock",12);
	cashLineLock = CreateLock("cashLineLock",12);
	entryLock = CreateLock("entryLock", 9);
	appClerkUIDLock = CreateLock("appClerkUIDLock",15);
	picClerkUIDLock = CreateLock("picClerkUIDLock",15);
	passClerkUIDLock = CreateLock("passClerkUIDLock",16);
	cashClerkUIDLock = CreateLock("cashClerkUIDLock",16);
	customerSenatorUIDLock = CreateLock("customerSenatorUIDLock",22);
	
	/*make all CVs*/
	regAppLineCV = CreateCondition("regAppLineCV",12);
	privAppLineCV = CreateCondition("privAppLineCV",13);
	regPicLineCV = CreateCondition("regPicLineCV",12);
	privPicLineCV = CreateCondition("privPicLineCV",13);
	regPassLineCV = CreateCondition("regPassLineCV",13);
	privPassLineCV = CreateCondition("privPassLineCV",14);
	regCashLineCV = CreateCondition("regCashLineCV",13);
	senatorWaitingRoomCV = CreateCondition("senatorWaitingRoomCV",20);
	customerWaitingRoomCV = CreateCondition("customerWaitingRoomCV",21);	
	managerWaitForCustomersCV = CreateCondition("managerWaitForCustomersCV", 25);
	
	/*make all MVs*/	
	appClerkUID = CreateMonitor("appClerkUID",11);
	picClerkUID = CreateMonitor("picClerkUID",11);
	passClerkUID = CreateMonitor("passClerkUID",12);
	cashClerkUID = CreateMonitor("cashClerkUID",12);
	customerSenatorUID = CreateMonitor("customerSenatorUID",18);	
	regAppLineLength = CreateMonitor("regAppLineLength",16);
	privAppLineLength = CreateMonitor("privAppLineLength",17);
	regPicLineLength = CreateMonitor("regPicLineLength",16);
	privPicLineLength = CreateMonitor("privPicLineLength",17);
	regPassLineLength = CreateMonitor("regPassLineLength",17);
	privPassLineLength = CreateMonitor("privPassLineLength",18);
	regCashLineLength = CreateMonitor("regCashLineLength",17);
	senatorsInWaitingRoom = CreateMonitor("senatorsInWaitingRoom",21);
	senatorsInOffice = CreateMonitor("senatorsInOffice",16);
	customersInWaitingRoom = CreateMonitor("customersInWaitingRoom",22);
	customersInOffice = CreateMonitor("customersInOffice",17);
	
	/*make all MVArrs*/
	appClerkStatuses = CreateMonitorArray("appClerkStatuses",16,NUM_OF_EACH_TYPE_OF_CLERK,0);
	picClerkStatuses = CreateMonitorArray("picClerkStatuses",16,NUM_OF_EACH_TYPE_OF_CLERK,0);
	appClerkLocks = CreateMonitorArray("appClerkLocks",13,NUM_OF_EACH_TYPE_OF_CLERK,-1);
	picClerkLocks = CreateMonitorArray("picClerkLocks",13,NUM_OF_EACH_TYPE_OF_CLERK,-1);
	appClerkCVs = CreateMonitorArray("appClerkCVs",11,NUM_OF_EACH_TYPE_OF_CLERK,-1);
	picClerkCVs = CreateMonitorArray("picClerkCVs",11,NUM_OF_EACH_TYPE_OF_CLERK,-1);
	appClerkSSNs = CreateMonitorArray("appClerkSSNs",12, NUM_OF_EACH_TYPE_OF_CLERK,-1);
	appClerkMoney = CreateMonitorArray("appClerkMoney",13,NUM_OF_EACH_TYPE_OF_CLERK,0);
	appClerkBribed = CreateMonitorArray("appClerkBribed",14,NUM_OF_EACH_TYPE_OF_CLERK,0);
	picClerkSSNs = CreateMonitorArray("picClerkSSNs",12,NUM_OF_EACH_TYPE_OF_CLERK,-1);
	picClerkMoney = CreateMonitorArray("picClerkMoney",13,NUM_OF_EACH_TYPE_OF_CLERK,0);
	picClerkBribed = CreateMonitorArray("picClerkBribed",14,NUM_OF_EACH_TYPE_OF_CLERK,0);
	happyWithPhoto = CreateMonitorArray("happyWithPhoto",14,NUM_OF_EACH_TYPE_OF_CLERK,0);			
	
	passClerkStatuses = CreateMonitorArray("passClerkStatuses",17,NUM_OF_EACH_TYPE_OF_CLERK,0);
	passClerkLocks = CreateMonitorArray("passClerkLocks",14,NUM_OF_EACH_TYPE_OF_CLERK,-1);
	passClerkCVs = CreateMonitorArray("passClerkCVs",12,NUM_OF_EACH_TYPE_OF_CLERK,-1);
	passClerkSSNs = CreateMonitorArray("passClerkSSNs",13,NUM_OF_EACH_TYPE_OF_CLERK,0);
	passClerkMoney = CreateMonitorArray("passClerkMoney",14,NUM_OF_EACH_TYPE_OF_CLERK,0);
	passClerkBribed = CreateMonitorArray("passClerkBribed",15,NUM_OF_EACH_TYPE_OF_CLERK,FALSE);
	passPunish = CreateMonitorArray("passPunish",10,NUM_OF_EACH_TYPE_OF_CLERK,FALSE);
	
	cashClerkStatuses = CreateMonitorArray("cashClerkStatuses",17,NUM_OF_EACH_TYPE_OF_CLERK,0);
	cashClerkLocks = CreateMonitorArray("cashClerkLocks",14,NUM_OF_EACH_TYPE_OF_CLERK,-1);
	cashClerkCVs = CreateMonitorArray("cashClerkCVs",12,NUM_OF_EACH_TYPE_OF_CLERK,-1);
	cashClerkSSNs = CreateMonitorArray("cashClerkSSNs",13,NUM_OF_EACH_TYPE_OF_CLERK,-1);
	cashClerkMoney = CreateMonitorArray("cashClerkMoney",14,NUM_OF_EACH_TYPE_OF_CLERK,0);
	cashPunish = CreateMonitorArray("cashPunish",10,NUM_OF_EACH_TYPE_OF_CLERK,FALSE);
	
	appFiled = CreateMonitorArray("appFiled",8,NUM_CUSTOMERS,FALSE);
	picFiled = CreateMonitorArray("picFiled",8,NUM_CUSTOMERS,FALSE);
	passFiled = CreateMonitorArray("passFiled",9,NUM_CUSTOMERS,FALSE);
	cashFiled = CreateMonitorArray("cashFiled",9,NUM_CUSTOMERS,FALSE);
	
}


void SenatorRun() {	

	int cashDollars;
	int clerkStatus;
	int index;
	int tid;

	tid = GetThreadID();

	Acquire(customerSenatorUIDLock);
	/*index = customerSenatorUID++;*/
	index = GetMonitor(customerSenatorUID);
	index++;
	SetMonitor(customerSenatorUID,index);
	Release(customerSenatorUIDLock);

	tprintf("Senator[%d]: Acquiring customerOfficeLock\n", index,0,0,"","");
	/*customerOfficeLock->Acquire();*/
	Acquire(entryLock);
	/*if (customersInOffice > 0){*/
	if(GetMonitor(customersInOffice)>0){
		tprintf("Senator[%d]: There are other Customers in office, going to Senator waiting room\n", index,0,0,"","");
		/*senatorsInWaitingRoom++;*/
		SetMonitor(senatorsInWaitingRoom,GetMonitor(senatorsInWaitingRoom)+1);
		tprintf("Senator[%d]: In the waiting room, taking a nap...\n", index,0,0,"","");
		/*senatorWaitingRoomCV->Wait(senatorWaitingRoomLock);*/
		Wait(senatorWaitingRoomCV, entryLock);
		tprintf("Senator[%d]: Waking up, going to the passport office!\n", index,0,0,"","");
		/*senatorsInWaitingRoom--;		*/
		SetMonitor(senatorsInWaitingRoom,GetMonitor(senatorsInWaitingRoom)-1);
	}
	
	tprintf("Senator [%d]: Entering the passport office...\n",index,0,0,"","");

	/*senator start up code*/		
	cashDollars = ((Rand() % 4) * 500) + 100;	

	printf("Senator [%d] has money = [$%d] ... tid = %d\n",index,cashDollars,tid,"","");
	/*senatorsInOffice++;*/
	SetMonitor(senatorsInOffice,GetMonitor(senatorsInOffice)+1);
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
			/*privAppLineLength++;*/
			SetMonitor(privAppLineLength, GetMonitor(privAppLineLength)+1);
			tprintf("Senator [%d]: Waiting in the Priveledged Line for next available AppClerk\n",*index,0,0,"","");
			/*privAppLineCV->Wait(appPicLineLock); //wait for clerk	*/
			Wait(privAppLineCV, appPicLineLock);					
		}
		else /*get in a normal line*/
		{
			tprintf("Senator [%d]: Regular line, suckas. CashDollars = $%d\n",*index,*cashDollars,0,"","");
			/*regAppLineLength++;*/
			SetMonitor(regAppLineLength,GetMonitor(regAppLineLength)+1);
			tprintf("Senator [%d]: Waiting in the Regular Line for next available AppClerk\n",*index,0,0,"","");
			/*regAppLineCV->Wait(appPicLineLock); //wait for clerk	*/
			Wait(regAppLineCV, appPicLineLock);
		}

		tprintf("Senator [%d]: Finding available AppClerk...\n",*index,0,0,"","");
		for(x = 0; x < NUM_OF_EACH_TYPE_OF_CLERK; x++)
		{				
			/*if(appClerkStatuses[x] == CLERK_AVAILABLE) /*find available clerk*/
			if(GetMonitorArrayValue(appClerkStatuses,x) == CLERK_AVAILABLE)
			{
				myClerk = x;					
				tprintf("Senator [%d]: Going to chill with AppClerk [%d]\n",*index,myClerk,0,"","");					
				/*appClerkStatuses[myClerk] = CLERK_BUSY;*/
				SetMonitorArrayValue(appClerkStatuses,myClerk,CLERK_BUSY);
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
		/*Acquire(appClerkLocks[myClerk]);*/
		Acquire(GetMonitorArrayValue(appClerkLocks,myClerk));
		Release(appPicLineLock);							
		if(privLine)
		{
			printf("Senator [%d] is willing to pay $500 to ApplicationClerk [%d] for moving ahead in line\n",*index, myClerk,0,"","");				
			/*appClerkMoney[myClerk] += 500;*/
			SetMonitorArrayValue(appClerkMoney,myClerk,GetMonitorArrayValue(appClerkMoney,myClerk)+500);
			/*appClerkBribed[myClerk] = TRUE;*/
			SetMonitorArrayValue(appClerkBribed,myClerk,TRUE);
			*cashDollars -= 500;
		}
		tprintf("Senator [%d]: Interacting with AppClerk [%d]\n",*index,myClerk,0,"","");
		/*interact with clerk	*/
		printf("Senator [%d] gives application to ApplicationClerk [%d] = [SSN: %d].\n", *index,myClerk,*index,"","");		
		/*appClerkSSNs[myClerk] = *index; 	*/
		SetMonitorArrayValue(appClerkSSNs,myClerk,*index);
		/*appClerkCVs[myClerk]->Signal(appClerkLocks[myClerk]);*/
		/*Signal(appClerkCVs[myClerk], appClerkLocks[myClerk]);*/
		Signal(GetMonitorArrayValue(appClerkCVs,myClerk),GetMonitorArrayValue(appClerkLocks,myClerk));
		/*appClerkCVs[myClerk]->Wait(appClerkLocks[myClerk]);*/
		/*Wait(appClerkCVs[myClerk], appClerkLocks[myClerk]);*/
		Wait(GetMonitorArrayValue(appClerkCVs,myClerk),GetMonitorArrayValue(appClerkLocks,myClerk));
		printf("Senator [%d] is informed by ApplicationClerk [%d] that the application has been filed.\n", *index, myClerk,0,"","");
		tprintf("Senator [%d]: Done and done.\n",*index,0,0,"","");
		/*appClerkLocks[myClerk]->Release();*/
		/*Release(appClerkLocks[myClerk]);*/
		Release(GetMonitorArrayValue(appClerkLocks,myClerk));
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
			/*privPicLineLength++;*/
			SetMonitor(privPicLineLength,GetMonitor(privPicLineLength)+1);
			tprintf("Senator [%d]: Waiting in the Priveledged Line for next available PicClerk\n",*index,0,0,"","");
			/*privPicLineCV->Wait(appPicLineLock);		*/
			Wait(privPicLineCV, appPicLineLock);	
		}
		else /*get in a normal line*/
		{
			tprintf("Senator [%d]: Regular line, suckas. CashDollars = $%d\n",*index,*cashDollars,0,"","");
			/*regPicLineLength++;*/
			SetMonitor(regPicLineLength,GetMonitor(regPicLineLength)+1);
			tprintf("Senator [%d]: Waiting in the Regluar Line for next available PicClerk\n",*index,0,0,"","");
			/*regPicLineCV->Wait(appPicLineLock);			*/
			Wait(regPicLineCV, appPicLineLock);
		}
		tprintf("Senator [%d]: Finding available PicClerk...\n",*index,0,0,"","");
		for(x = 0; x < NUM_OF_EACH_TYPE_OF_CLERK; x++)
		{					

			/*if(picClerkStatuses[x] == CLERK_AVAILABLE)*/
			if(GetMonitorArrayValue(picClerkStatuses,x)==CLERK_AVAILABLE)
			{
				myClerk = x;					
				tprintf("Senator [%d]: Going to chill with PicClerk[%d]\n",*index,myClerk,0,"","");					
				/*picClerkStatuses[myClerk] = CLERK_BUSY;					*/
				SetMonitorArrayValue(picClerkStatuses,myClerk,CLERK_BUSY);
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
		/*Acquire(picClerkLocks[myClerk]);*/
		Acquire(GetMonitorArrayValue(picClerkLocks,myClerk));
		Release(appPicLineLock);
						
		/*picClerkSSNs[myClerk] = *index;		*/
		SetMonitorArrayValue(picClerkSSNs,myClerk,*index);
		if(privLine)
		{
			printf("Senator [%d] is willing to pay $500 to PictureClerk[%d] for moving ahead in line\n",*index, myClerk,0,"","");				
			/*picClerkMoney[myClerk] += 500;*/
			SetMonitorArrayValue(picClerkMoney,myClerk,GetMonitorArrayValue(picClerkMoney,myClerk)+500);
			/*picClerkBribed[myClerk] = TRUE;*/
			SetMonitorArrayValue(picClerkBribed,myClerk,TRUE);
			*cashDollars -= 500;
		}
		tprintf("Senator [%d] Goes to PictureClerk[%d]\n",*index,myClerk,0,"","");
		/*interact with clerk, loop enabled picture to be taken multiple times*/
		/*while(happyWithPhoto[myClerk] == FALSE)*/
		while(GetMonitorArrayValue(happyWithPhoto,myClerk)==FALSE)
		{

			tprintf("Senator [%d]: Getting my picture taken...\n",*index,0,0,"","");
			/*picClerkCVs[myClerk]->Signal(picClerkLocks[myClerk]);
			picClerkCVs[myClerk]->Wait(picClerkLocks[myClerk]);	*/
			/*Signal(picClerkCVs[myClerk], picClerkLocks[myClerk]);*/
			Signal(GetMonitorArrayValue(picClerkCVs,myClerk),GetMonitorArrayValue(picClerkLocks,myClerk));
			/*Wait(picClerkCVs[myClerk], picClerkLocks[myClerk]);*/
			Wait(GetMonitorArrayValue(picClerkCVs,myClerk),GetMonitorArrayValue(picClerkLocks,myClerk));

			/*did I like my picture?*/
			if(Rand()%10 > 5)
			{
				/*happyWithPhoto[myClerk] = TRUE;*/
				SetMonitorArrayValue(happyWithPhoto,myClerk,TRUE);
				printf("Senator [%d] [likes] the picture provided by PictureClerk[%d]\n", *index,myClerk,0,"","");
			}
			else
			{
				printf("Senator [%d] [dislikes] the picture provided by PictureClerk[%d]\n", *index,myClerk,0,"","");
				printf("The picture of Senator [%d] will be taken again.\n", *index,0,0,"","");
			}		

			/*picClerkCVs[myClerk]->Signal(picClerkLocks[myClerk]);	
			picClerkCVs[myClerk]->Wait(picClerkLocks[myClerk]);		*/
			/*Signal(picClerkCVs[myClerk], picClerkLocks[myClerk]);*/
			Signal(GetMonitorArrayValue(picClerkCVs,myClerk),GetMonitorArrayValue(picClerkLocks,myClerk));
			/*Wait(picClerkCVs[myClerk], picClerkLocks[myClerk]);		*/
			Wait(GetMonitorArrayValue(picClerkCVs,myClerk),GetMonitorArrayValue(picClerkLocks,myClerk));		

		}

		/*happyWithPhoto[myClerk] = FALSE; /*needs to be reset for future customers*/
		SetMonitorArrayValue(happyWithPhoto,myClerk,FALSE);
		tprintf("Senator [%d]: Picture taken. Like a boss.\n",*index,0,0,"","");
		/*picClerkLocks[myClerk]->Release();			*/
		/*Release(picClerkLocks[myClerk]);*/
		Release(GetMonitorArrayValue(picClerkLocks,myClerk));

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
			/*privPassLineLength++;*/
			SetMonitor(privPassLineLength,GetMonitor(privPassLineLength)+1);
			tprintf("Senator [%d]: Waiting in the Priveledged Line for next available PassportClerk\n",*index,0,0,"","");
			/*privPassLineCV->Wait(passLineLock);*/
			Wait(privPassLineCV, passLineLock);			
		}
		else /*get in a normal line*/
		{
			tprintf("Senator [%d]: Regular line, suckas. CashDollars = $%d\n",*index,*cashDollars,0,"","");
			/*regPassLineLength++;*/
			SetMonitor(regPassLineLength,GetMonitor(regPassLineLength)+1);
			tprintf("Senator [%d]: Waiting in the Regular Line for next available PassportClerk\n",*index,0,0,"","");
			/*regPassLineCV->Wait(passLineLock);		*/
			Wait(regPassLineCV, passLineLock);	
		}
		tprintf("Senator [%d]: Finding available PassClerk...\n",*index,0,0,"","");
		for(x = 0; x < NUM_OF_EACH_TYPE_OF_CLERK; x++)
		{				
			/*if(passClerkStatuses[x] == CLERK_AVAILABLE)*/
			if(GetMonitorArrayValue(passClerkStatuses,x)==CLERK_AVAILABLE)
			{
				myClerk = x;					
				tprintf("Senator [%d]: Going to chill with PassClerk[%d]\n",*index,myClerk,0,"","");					
				/*passClerkStatuses[myClerk] = CLERK_BUSY;*/
				SetMonitorArrayValue(passClerkStatuses,myClerk,CLERK_BUSY);
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
		/*Acquire(passClerkLocks[myClerk]);*/
		Acquire(GetMonitorArrayValue(passClerkLocks,myClerk));
		/*passLineLock->Release();	*/
		Release(passLineLock);
		/*passClerkSSNs[myClerk] = *index;*/
		SetMonitorArrayValue(passClerkSSNs,myClerk,*index);
		if(privLined && !bribed)
		{
			bribed = TRUE;
			printf("Senator [%d] is willing to pay $500 to PassportClerk[%d] for moving ahead in line\n",*index,myClerk,0,"","");
			/*passClerkMoney[myClerk] += 500;*/
			SetMonitorArrayValue(passClerkMoney,myClerk,GetMonitorArrayValue(passClerkMoney,myClerk)+500);
			/*passClerkBribed[myClerk] = TRUE;*/
			SetMonitorArrayValue(passClerkBribed,myClerk,TRUE);
			*cashDollars -= 500;
		}
		printf("Senator [%d] goes to PassportClerk[%d]\n",*index,myClerk,0,"","");
		/*interact with clerk		*/	
		/*passClerkCVs[myClerk]->Signal(passClerkLocks[myClerk]);
		passClerkCVs[myClerk]->Wait(passClerkLocks[myClerk]);*/
		/*Signal(passClerkCVs[myClerk], passClerkLocks[myClerk]);*/
		Signal(GetMonitorArrayValue(passClerkCVs,myClerk),GetMonitorArrayValue(passClerkLocks,myClerk));
		/*Wait(passClerkCVs[myClerk], passClerkLocks[myClerk]);*/
		Wait(GetMonitorArrayValue(passClerkCVs,myClerk),GetMonitorArrayValue(passClerkLocks,myClerk));
		/*get passport from clerk, if not ready, go to the back of the line?*/
		/*if(!passPunish[myClerk])*/
		if(!GetMonitorArrayValue(passPunish,myClerk))
		{
			printf("Senator [%d] is [certified] by PassportClerk[%d]\n",*index,myClerk,0,"","");
			tprintf("Senator [%d]: Passport. GET!.\n", *index,0,0,"","");						
			tprintf("Senator [%d]: Done and done.\n",*index,0,0,"","");
			/*passClerkLocks[myClerk]->Release();*/
			/*Release(passClerkLocks[myClerk]);*/
			Release(GetMonitorArrayValue(passClerkLocks,myClerk));
			tprintf("Senator [%d]: Going to next clerk...\n",*index,0,0,"","");
			break;				
		}
		/*passClerkLocks[myClerk]->Release();*/
		/*Release(passClerkLocks[myClerk]);*/
		Release(GetMonitorArrayValue(passClerkLocks,myClerk));
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
		
		/*regCashLineLength++;*/
		SetMonitor(regCashLineLength,GetMonitor(regCashLineLength)+1);
		tprintf("Senator [%d]: Waiting in the Regular Line for next available CashClerk\n",*index,0,0,"","");
		/*regCashLineCV->Wait(cashLineLock);	*/
		Wait(regCashLineCV, cashLineLock);		
				
		tprintf("Senator[%d]: Finding available CashClerk...\n",*index,0,0,"","");
		for(x = 0; x < NUM_OF_EACH_TYPE_OF_CLERK; x++)
		{				
			/*if(cashClerkStatuses[x] == CLERK_AVAILABLE)*/
			if(GetMonitorArrayValue(cashClerkStatuses,x)==CLERK_AVAILABLE)
			{
				myClerk = x;					
				tprintf("Senator [%d]: Going to chill with CashClerk[%d]\n",*index,myClerk,0,"","");					
				/*cashClerkStatuses[myClerk] = CLERK_BUSY;*/
				SetMonitorArrayValue(cashClerkStatuses,myClerk,CLERK_BUSY);
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
		/*Acquire(cashClerkLocks[myClerk]);*/
		Acquire(GetMonitorArrayValue(cashClerkLocks,myClerk));
		Release(cashLineLock);
							
		/*cashClerkSSNs[myClerk] = *index;			*/
		SetMonitorArrayValue(cashClerkSSNs,myClerk,*index);
		printf("Senator [%d] goes to CashClerk[%d]\n",*index,myClerk,0,"","");			
		/*interact with clerk			*/
		/*cashClerkCVs[myClerk]->Signal(cashClerkLocks[myClerk]);
		cashClerkCVs[myClerk]->Wait(cashClerkLocks[myClerk]);*/
		/*Signal(cashClerkCVs[myClerk], cashClerkLocks[myClerk]);*/
		Signal(GetMonitorArrayValue(cashClerkCVs,myClerk),GetMonitorArrayValue(cashClerkLocks,myClerk));
		/*Wait(cashClerkCVs[myClerk], cashClerkLocks[myClerk]);*/
		Wait(GetMonitorArrayValue(cashClerkCVs,myClerk),GetMonitorArrayValue(cashClerkLocks,myClerk));
		
		/*pay for passport. If it's not processed, get back in line*/
		/*if(!cashPunish[myClerk])*/
		if(!GetMonitorArrayValue(cashPunish,myClerk))
		{				
			printf("Senator [%d] gets [valid] certification by Cashier[%d]\n",*index,myClerk,0,"","");
			printf("Senator [%d] pays $100 to Cashier[%d] for their passport\n",*index,myClerk,0,"","");
			/*cashClerkMoney[myClerk] += 100;				*/
			SetMonitorArrayValue(cashClerkMoney,myClerk,GetMonitorArrayValue(cashClerkMoney,myClerk)+100);
			*cashDollars-=100;
			/*cashClerkCVs[myClerk]->Signal(cashClerkLocks[myClerk]);
			cashClerkCVs[myClerk]->Wait(cashClerkLocks[myClerk]);*/
			/*Signal(cashClerkCVs[myClerk], cashClerkLocks[myClerk]);*/
			Signal(GetMonitorArrayValue(cashClerkCVs,myClerk),GetMonitorArrayValue(cashClerkLocks,myClerk));
			/*Wait(cashClerkCVs[myClerk], cashClerkLocks[myClerk]);*/
			Wait(GetMonitorArrayValue(cashClerkCVs,myClerk),GetMonitorArrayValue(cashClerkLocks,myClerk));
			
			tprintf("Senator [%d]: Passport paid for like a pro. CashDollars = [$%d]\n", *index, *cashDollars,0,"","");											
			/*cashClerkLocks[myClerk]->Release();*/
			/*Release(cashClerkLocks[myClerk]);*/
			Release(GetMonitorArrayValue(cashClerkLocks,myClerk));
			
			printf("Senator [%d] leaves the passport office...\n",*index,0,0,"","");
			/*customerOfficeLock->Acquire();*/
			Acquire(entryLock);
			/*senatorsInOffice--;*/
			SetMonitor(senatorsInOffice,GetMonitor(senatorsInOffice)-1);
			/*customerOfficeLock->Release();*/
			Release(entryLock);
			break;				
		}
		/*cashClerkLocks[myClerk]->Release();*/
		/*Release(cashClerkLocks[myClerk]);*/
		Release(GetMonitorArrayValue(cashClerkLocks,myClerk));
		printf("Senator [%d] gets [invalid] certification by Cashier[%d]\n",*index,myClerk,0,"","");
		printf("Senator [%d] is punished to wait by Cashier[%d]\n",*index,myClerk,0,"","");
		tprintf("Senator [%d]: NOT READY!? Going back to the end of the line...\n",*index,0,0,"","");
		rando = Rand()%80+20;					
		for(x = 0; x < rando; x++)
			Yield();																				
	}
}
/*netSenator.c*/
