#include "officeData.h"
#include "utils.h"


int initAppClerkData() { //return id;
	int myUID;
	char* name;
	int id;

	// line locks, conditions, and monitors
	appPicLineLock = CreateLock("appPicLineLock",14);
	regAppLineCV = CreateCondition("regAppLineCV",12);
	privAppLineCV = CreateCondition("privAppLineCV",13);

	regAppLineLength = CreateMonitor("regAppLineLength", 15);
	privAppLineLength = CreateMonitor("privAppLineLength", 16);

	// uid lock and monitors
	appClerkUIDLock = CreateLock("appClerkUIDLock",15);
	appClerkUID = CreateMonitor("appClerkUID", 11);

	// app filed monitor array
	appFiled = CreateMonitorArray("appFiled", 8, NUM_CUSTOMERS, FALSE);

	// all the arrays 
	appClerkStatuses = CreateMonitorArray("appClerkStatuses", 16, NUM_OF_EACH_TYPE_OF_CLERK, 0);//[MAX_APP_CLERKS]; // of monitors, default to 0 == CLERK_NOT_AVAILABLE
	appClerkLocks = CreateMonitorArray("appClerkLocks", 13, NUM_OF_EACH_TYPE_OF_CLERK, -1);//[MAX_APP_CLERKS]; // of lockIDs, default to -1
	appClerkCVs = CreateMonitorArray("appClerkCVs", 11, NUM_OF_EACH_TYPE_OF_CLERK, -1);//[MAX_APP_CLERKS]; // of conditionIDs, default to -1
	appClerkSSNs = CreateMonitorArray("appClerkSSNs", 12, NUM_OF_EACH_TYPE_OF_CLERK, -1);//[MAX_APP_CLERKS]; // of monitors, default to -1
	appClerkMoney = CreateMonitorArray("appClerkMoney", 13, NUM_OF_EACH_TYPE_OF_CLERK, 0);//[MAX_APP_CLERKS]; // of monitors, default to 0
	appClerkBribed = CreateMonitorArray("appClerkBribed", 14, NUM_OF_EACH_TYPE_OF_CLERK, FALSE);//[MAX_APP_CLERKS]; // of monitors, default to 0

	// obtain SSN
	Acquire(appClerkUIDLock);
	myUID = GetMonitor(appClerkUID);
	SetMonitor(appClerkUID, myUID + 1);
	Release(appClerkUIDLock);

	// initialize necessary values for arrays at my index
	name = "appClerk?Lock";
	name[8] = myUID + '0';
	id = CreateLock(name, 13);
	SetMonitorArrayValue(appClerkLocks, myUID, id); // set the value at my index in the lock array, to the handle to my lock

	name = "appClerk?Condition";
	name[8] = myUID + '0';
	id = CreateCondition(name, 18);
	SetMonitorArrayValue(appClerkCVs, myUID, id); // now set the value for our condition in the condition array

	return myUID;
}

void AppClerkRun(){
	int SSN;
	int index;
	LockID myLockID;
	ConditionID myConditionID;

	index = initAppClerkData();

	while (TRUE){		
		/*appPicLineLock->Acquire();*/
		Acquire(appPicLineLock);		

		/*Checking if anyone is in line*/
		if (GetMonitor(privAppLineLength) + GetMonitor(regAppLineLength) > 0){
			tprintf("AppClerk %d: there are people in line.\n", index,0,0,"","");
			if (GetMonitor(privAppLineLength) > 0){ /*Checking if anyone is in priv line*/
				tprintf("AppClerk %d: people in my priv line.\n", index,0,0,"","");
				tprintf("AppClerk %d: has spotted %s in privAppLine(length = %d)\n",index, GetMonitor(privAppLineLength), 0,getCustomerType(),""); 
				SetMonitor(privAppLineLength, GetMonitor(privAppLineLength) - 1);
				tprintf("AppClerk %d: Becoming Available!\n",index,0,0,"","");
				SetMonitorArrayValue(appClerkStatuses, index, CLERK_AVAILABLE);
				tprintf("AppClerk %d: Signaling Condition variable (length of priv line is now %d)\n", index, GetMonitor(privAppLineLength),0,"","");
				/*privAppLineCV->Signal(appPicLineLock);*/
				Signal(privAppLineCV, appPicLineLock);
			}
			else{ /*Check if anyone is in reg line*/
				tprintf("AppClerk %d: has spotted %s in regAppLine (length = %d)\n",index, GetMonitor(regAppLineLength), 0, getCustomerType(),"");
				SetMonitor(regAppLineLength, GetMonitor(regAppLineLength) - 1);
				tprintf("AppClerk %d: Becoming Available!\n",index,0,0,"","");
				SetMonitorArrayValue(appClerkStatuses, index, CLERK_AVAILABLE);
				tprintf("AppClerk %d: Signaling Condition variable (length of reg line is now %d)\n", index,GetMonitor(regAppLineLength),0,"","");
				/*regAppLineCV->Signal(appPicLineLock);*/
				Signal(regAppLineCV, appPicLineLock);
			}

			/*Customer/Senator - Clerk interaction*/
			tprintf("AppClerk %d: Acquiring my own lock\n",index,0,0,"","");
			/*appClerkLocks[index]->Acquire();*/
			
			myLockID = GetMonitorArrayValue(appClerkLocks, index);
			myConditionID = GetMonitorArrayValue(appClerkCVs, index);
			Acquire(myLockID);
			tprintf("AppClerk %d: Releasing appPicLineLock\n",index,0,0,"","");
			/*appPicLineLock->Release();*/
			Release(appPicLineLock);
			tprintf("AppClerk %d: Putting myself to sleep...\n",index,0,0,"","");
			/*appClerkCVs[index]->Wait(appClerkLocks[index]);*/
			Wait(myConditionID, myLockID);
			tprintf("AppClerk %d: Just woke up!\n",index,0,0,"","");
			
			SSN = GetMonitorArrayValue(appClerkSSNs, index); // use this locally instead of calling get on the array all the time
			tprintf("AppClerk %d: Just receieved %s's SSN: %d\n",index, SSN, 0, getCustomerType(),"");

			if (GetMonitorArrayValue(appClerkBribed, index) == TRUE){
				printf("ApplicationClerk [%d] accepts money = 500 from %s with SSN %d\n", index, SSN, 0,getCustomerType(),"");
				SetMonitorArrayValue(appClerkBribed, index, FALSE);
			}

			printf("ApplicationClerk [%d] informs %s with SSN %d that the procedure has been completed.\n", index, SSN, 0, getCustomerType(),""); 
			
			/*Thread* newThread = new Thread("Filing Thread");
			newThread->Fork((VoidFunctionPtr)appClerkFileData, SSN);
			
			NEED TO CONVERT THIS*/
			/*
			ForkWithArg(appClerkFileData, SSN);*/

			SetMonitorArrayValue(appFiled, SSN, TRUE);
			/* appFiled[SSN] = TRUE; GET RID OF THIS ONCE WE FIX SHIT*/
			
			/*for (i=0; i<10; i++){
				tprintf("AppFiled: %d,    PicFiled: %d\n",appFiled[i],picFiled[i]);
			}*/
			
			tprintf("AppClerk %d: Signaling my appClerkCV\n", index,0,0,"","");
			/*appClerkCVs[index]->Signal(appClerkLocks[index]);*/
			Signal(myConditionID, myLockID);
			tprintf("AppClerk %d: Releasing my own lock\n", index,0,0,"","");
			/*appClerkLocks[index]->Release();*/
			Release(myLockID);

		}
		else{ /*No one in line*/
			/*appPicLineLock->Release();*/
			Release(appPicLineLock);
			/*appClerkLocks[index]->Acquire();*/
			/*tprintf("ApplicationClerk [%d] acquiring his own lock\n", index,0,0,"","");*/
			Acquire(myLockID);
			printf("ApplicationClerk [%d] is going on break\n", index,0,0,"","");
			SetMonitorArrayValue(appClerkStatuses, index, CLERK_ON_BREAK);
			/*appClerkCVs[index]->Wait(appClerkLocks[index]);*/
			Wait(myConditionID, myLockID);
			printf("ApplicationClerk [%d] returned from break\n", index,0,0,"","");
			/*appClerkLocks[index]->Release();*/
			Release(myLockID);
		}
		Yield();
	}
	printf("About to exit!", 0,0,0,"","");
	Exit(0);
}