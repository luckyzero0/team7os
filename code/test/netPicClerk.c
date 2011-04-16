#include "officeData.h"
#include "utils.h"

char* getCustomerType() {
	char* result;

	Acquire(entryLock);
	if (GetMonitor(senatorsInOffice) > 0){
		result = "Senator";
	} else {
		result = "Customer";
	}
	Release(entryLock);

	return result;
}

int initPicClerkData() {
	int myUID;
	char* name;
	int id;

	/* line locks, conditions, and monitors */
	appPicLineLock = CreateLock("appPicLineLock",14);
	regPicLineCV = CreateCondition("regPicLineCV",12);
	privPicLineCV = CreateCondition("privPicLineCV",13);

	regPicLineLength = CreateMonitor("regPicLineLength", 15);
	privPicLineLength = CreateMonitor("privPicLineLength", 16);

	/* uid lock and monitors */
	picClerkUIDLock = CreateLock("picClerkUIDLock",15);
	picClerkUID = CreateMonitor("picClerkUID", 11);

	/* pic filed monitor array */
	picFiled = CreateMonitorArray("picFiled", 8, NUM_CUSTOMERS, FALSE);

	/* entry lock so we can check whether current customres are customers or senators */
	entryLock = CreateLock("entryLock", 9);
	senatorsInOffice = CreateMonitor("senatorsInOffice", 16);

	/* all the arrays */
	picClerkStatuses = CreateMonitorArray("picClerkStatuses", 16, NUM_OF_EACH_TYPE_OF_CLERK, 0);/* of monitors, default to 0 == CLERK_NOT_AVAILABLE*/
	picClerkLocks = CreateMonitorArray("picClerkLocks", 13, NUM_OF_EACH_TYPE_OF_CLERK, -1);/* of lockIDs, default to -1*/
	picClerkCVs = CreateMonitorArray("picClerkCVs", 11, NUM_OF_EACH_TYPE_OF_CLERK, -1);/* of conditionIDs, default to -1*/
	picClerkSSNs = CreateMonitorArray("picClerkSSNs", 12, NUM_OF_EACH_TYPE_OF_CLERK, -1);/* of monitors, default to -1*/
	picClerkMoney = CreateMonitorArray("picClerkMoney", 13, NUM_OF_EACH_TYPE_OF_CLERK, 0);/* of monitors, default to 0*/
	picClerkBribed = CreateMonitorArray("picClerkBribed", 14, NUM_OF_EACH_TYPE_OF_CLERK, FALSE);/* of monitors, default to 0 */

	/* obtain SSN */
	Acquire(picClerkUIDLock);
	myUID = GetMonitor(picClerkUID);
	SetMonitor(picClerkUID, myUID + 1);
	Release(picClerkUIDLock);

	/* initialize necessary values for arrays at my index */
	name = "picClerk?Lock";
	name[8] = myUID + '0';
	id = CreateLock(name, 13);
	SetMonitorArrayValue(picClerkLocks, myUID, id); /* set the value at my index in the lock array, to the handle to my lock */

	name = "picClerk?Condition";
	name[8] = myUID + '0';
	id = CreateCondition(name, 18);
	SetMonitorArrayValue(picClerkCVs, myUID, id); /* now set the value for our condition in the condition array */

	return myUID;
}

void PicClerkRun(){
	int SSN;
	int index;
	LockID myLockID;
	ConditionID myConditionID;

	index = initPicClerkData();

	while (TRUE){		
		/*appPicLineLock->Acquire();*/
		Acquire(appPicLineLock);		

		/*Checking if anyone is in line*/
		if (GetMonitor(privPicLineLength) + GetMonitor(regPicLineLength) > 0){
			tprintf("PicClerk %d: there are people in line.\n", index,0,0,"","");
			if (GetMonitor(privPicLineLength) > 0){ /*Checking if anyone is in priv line*/
				tprintf("PicClerk %d: people in my priv line.\n", index,0,0,"","");
				tprintf("PicClerk %d: has spotted %s in privPicLine(length = %d)\n",index, GetMonitor(privPicLineLength), 0,getCustomerType(),""); 
				SetMonitor(privPicLineLength, GetMonitor(privPicLineLength) - 1);
				tprintf("PicClerk %d: Becoming Available!\n",index,0,0,"","");
				SetMonitorArrayValue(picClerkStatuses, index, CLERK_AVAILABLE);
				tprintf("PicClerk %d: Signaling Condition variable (length of priv line is now %d)\n", index, GetMonitor(privPicLineLength),0,"","");
				/*privPicLineCV->Signal(appPicLineLock);*/
				Signal(privPicLineCV, appPicLineLock);
			}
			else{ /*Check if anyone is in reg line*/
				tprintf("PicClerk %d: has spotted %s in regPicLine (length = %d)\n",index, GetMonitor(regPicLineLength), 0, getCustomerType(),"");
				SetMonitor(regPicLineLength, GetMonitor(regPicLineLength) - 1);
				tprintf("PicClerk %d: Becoming Available!\n",index,0,0,"","");
				SetMonitorArrayValue(picClerkStatuses, index, CLERK_AVAILABLE);
				tprintf("PicClerk %d: Signaling Condition variable (length of reg line is now %d)\n", index,GetMonitor(regPicLineLength),0,"","");
				/*regPicLineCV->Signal(appPicLineLock);*/
				Signal(regPicLineCV, appPicLineLock);
			}

			/*Customer/Senator - Clerk interaction*/
			tprintf("PicClerk %d: Acquiring my own lock\n",index,0,0,"","");
			/*picClerkLocks[index]->Acquire();*/
			
			myLockID = GetMonitorArrayValue(picClerkLocks, index);
			myConditionID = GetMonitorArrayValue(picClerkCVs, index);
			Acquire(myLockID);
			tprintf("PicClerk %d: Releasing appPicLineLock\n",index,0,0,"","");
			/*appPicLineLock->Release();*/
			Release(appPicLineLock);
			tprintf("PicClerk %d: Putting myself to sleep...\n",index,0,0,"","");
			/*picClerkCVs[index]->Wait(picClerkLocks[index]);*/
			Wait(myConditionID, myLockID);
			tprintf("PicClerk %d: Just woke up!\n",index,0,0,"","");
			
			SSN = GetMonitorArrayValue(picClerkSSNs, index); /* use this locally instead of calling get on the array all the time */
			tprintf("PicClerk %d: Just receieved %s's SSN: %d\n",index, SSN, 0, getCustomerType(),"");

			if (GetMonitorArrayValue(picClerkBribed, index) == TRUE){
				printf("PiclicationClerk [%d] accepts money = 500 from %s with SSN %d\n", index, SSN, 0,getCustomerType(),"");
				SetMonitorArrayValue(picClerkBribed, index, FALSE);
			}

			printf("PiclicationClerk [%d] informs %s with SSN %d that the procedure has been completed.\n", index, SSN, 0, getCustomerType(),""); 
			
			/*Thread* newThread = new Thread("Filing Thread");
			newThread->Fork((VoidFunctionPtr)picClerkFileData, SSN);
			
			NEED TO CONVERT THIS*/
			/*
			ForkWithArg(picClerkFileData, SSN);*/

			SetMonitorArrayValue(picFiled, SSN, TRUE);
			/* picFiled[SSN] = TRUE; GET RID OF THIS ONCE WE FIX SHIT*/
			
			/*for (i=0; i<10; i++){
				tprintf("PicFiled: %d,    PicFiled: %d\n",picFiled[i],picFiled[i]);
			}*/
			
			tprintf("PicClerk %d: Signaling my picClerkCV\n", index,0,0,"","");
			/*picClerkCVs[index]->Signal(picClerkLocks[index]);*/
			Signal(myConditionID, myLockID);
			tprintf("PicClerk %d: Releasing my own lock\n", index,0,0,"","");
			/*picClerkLocks[index]->Release();*/
			Release(myLockID);

		}
		else{ /*No one in line*/
			/*appPicLineLock->Release();*/
			Release(appPicLineLock);
			/*picClerkLocks[index]->Acquire();*/
			/*tprintf("PiclicationClerk [%d] acquiring his own lock\n", index,0,0,"","");*/
			Acquire(myLockID);
			printf("PiclicationClerk [%d] is going on break\n", index,0,0,"","");
			SetMonitorArrayValue(picClerkStatuses, index, CLERK_ON_BREAK);
			/*picClerkCVs[index]->Wait(picClerkLocks[index]);*/
			Wait(myConditionID, myLockID);
			printf("PiclicationClerk [%d] returned from break\n", index,0,0,"","");
			/*picClerkLocks[index]->Release();*/
			Release(myLockID);
		}
		Yield();
	}
	printf("About to exit!", 0,0,0,"","");
}

int main() {
	PicClerkRun();
	Exit(0);
}