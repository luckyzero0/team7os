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

int initPassClerkData() {
	int myUID;
	char* name;
	int id;

	/* line locks, conditions, and monitors */
	passLineLock = CreateLock("passLineLock",12);
	regPassLineCV = CreateCondition("regPassLineCV",13);
	privPassLineCV = CreateCondition("privPassLineCV",14);

	regPassLineLength = CreateMonitor("regPassLineLength",17);
	privPassLineLength = CreateMonitor("privPassLineLength",18);

	/* uid lock and monitors */
	passClerkUIDLock = CreateLock("passClerkUIDLock",16);
	passClerkUID = CreateMonitor("passClerkUID", 12);

	/* pass filed monitor array */
	passFiled = CreateMonitorArray("passFiled", 9, NUM_CUSTOMERS, FALSE);

	/* entry lock so we can check whether current customres are customers or senators */
	entryLock = CreateLock("entryLock", 10);
	senatorsInOffice = CreateMonitor("senatorsInOffice", 17);

	/* all the arrays */
	passClerkStatuses = CreateMonitorArray("passClerkStatuses", 17, NUM_OF_EACH_TYPE_OF_CLERK, 0);/* of monitors, default to 0 == CLERK_NOT_AVAILABLE*/
	passClerkLocks = CreateMonitorArray("passClerkLocks", 14, NUM_OF_EACH_TYPE_OF_CLERK, -1);/* of lockIDs, default to -1*/
	passClerkCVs = CreateMonitorArray("passClerkCVs", 12, NUM_OF_EACH_TYPE_OF_CLERK, -1);/* of conditionIDs, default to -1*/
	passClerkSSNs = CreateMonitorArray("passClerkSSNs", 13, NUM_OF_EACH_TYPE_OF_CLERK, -1);/* of monitors, default to -1*/
	passClerkMoney = CreateMonitorArray("passClerkMoney", 14, NUM_OF_EACH_TYPE_OF_CLERK, 0);/* of monitors, default to 0*/
	passClerkBribed = CreateMonitorArray("passClerkBribed", 15, NUM_OF_EACH_TYPE_OF_CLERK, FALSE);/* of monitors, default to 0 */
	passPunish = CreateMonitorArray("passPunish", 10, NUM_OF_EACH_TYPE_OF_CLERK, FALSE); /* of monitors, default to FALSE */

	/* obtain SSN */
	Acquire(passClerkUIDLock);
	myUID = GetMonitor(passClerkUID);
	SetMonitor(passClerkUID, myUID + 1);
	Release(passClerkUIDLock);

	/* initialize necessary values for arrays at my index */
	name = "passClerk?Lock";
	name[9] = myUID + '0';
	id = CreateLock(name, 14);
	SetMonitorArrayValue(passClerkLocks, myUID, id); /* set the value at my index in the lock array, to the handle to my lock */

	name = "passClerk?Condition";
	name[9] = myUID + '0';
	id = CreateCondition(name, 19);
	SetMonitorArrayValue(passClerkCVs, myUID, id); /* now set the value for our condition in the condition array */

	return myUID;
}

void PassClerkRun(){
	int SSN;
	int index;
	LockID myLockID;
	ConditionID myConditionID;

	index = initPassClerkData();

	while (TRUE){		
		/*passLineLock->Acquire();*/
		Acquire(passLineLock);		

		/*Checking if anyone is in line*/
		if (GetMonitor(privPassLineLength) + GetMonitor(regPassLineLength) > 0){
			tprintf("PassClerk %d: there are people in line.\n", index,0,0,"","");
			if (GetMonitor(privPassLineLength) > 0){ /*Checking if anyone is in priv line*/
				tprintf("PassClerk %d: people in my priv line.\n", index,0,0,"","");
				tprintf("PassClerk %d: has spotted %s in privPassLine(length = %d)\n",index, GetMonitor(privPassLineLength), 0,getCustomerType(),""); 
				SetMonitor(privPassLineLength, GetMonitor(privPassLineLength) - 1);
				tprintf("PassClerk %d: Becoming Available!\n",index,0,0,"","");
				SetMonitorArrayValue(passClerkStatuses, index, CLERK_AVAILABLE);
				tprintf("PassClerk %d: Signaling Condition variable (length of priv line is now %d)\n", index, GetMonitor(privPassLineLength),0,"","");
				/*privPassLineCV->Signal(passLineLock);*/
				Signal(privPassLineCV, passLineLock);
			}
			else{ /*Check if anyone is in reg line*/
				tprintf("PassClerk %d: has spotted %s in regPassLine (length = %d)\n",index, GetMonitor(regPassLineLength), 0, getCustomerType(),"");
				SetMonitor(regPassLineLength, GetMonitor(regPassLineLength) - 1);
				tprintf("PassClerk %d: Becoming Available!\n",index,0,0,"","");
				SetMonitorArrayValue(passClerkStatuses, index, CLERK_AVAILABLE);
				tprintf("PassClerk %d: Signaling Condition variable (length of reg line is now %d)\n", index,GetMonitor(regPassLineLength),0,"","");
				/*regPassLineCV->Signal(passLineLock);*/
				Signal(regPassLineCV, passLineLock);
			}

			/*Customer/Senator - Clerk interaction*/
			tprintf("PassClerk %d: Acquiring my own lock\n",index,0,0,"","");
			/*passClerkLocks[index]->Acquire();*/
			
			myLockID = GetMonitorArrayValue(passClerkLocks, index);
			myConditionID = GetMonitorArrayValue(passClerkCVs, index);
			Acquire(myLockID);
			tprintf("PassClerk %d: Releasing passLineLock\n",index,0,0,"","");
			/*passLineLock->Release();*/
			Release(passLineLock);
			tprintf("PassClerk %d: Putting myself to sleep...\n",index,0,0,"","");
			/*passClerkCVs[index]->Wait(passClerkLocks[index]);*/
			Wait(myConditionID, myLockID);
			tprintf("PassClerk %d: Just woke up!\n",index,0,0,"","");
			
			SSN = GetMonitorArrayValue(passClerkSSNs, index); /* use this locally instead of calling get on the array all the time */

			if (GetMonitorArrayValue(passClerkBribed, index) == TRUE){
				printf("PassportClerk [%d] accepts money = 500 from %s with SSN %d\n", index, SSN, 0,getCustomerType(),"");
				SetMonitorArrayValue(passClerkBribed, index, FALSE);
			}

			if (GetMonitorArrayValue(appFiled, SSN) == FALSE || GetMonitorArrayValue(picFiled, SSN) == FALSE){
				printf("PassportClerk [%d] gives invalid certification to %s with SSN %d\n", index, SSN, 0, getCustomerType(),""); 
				printf("PassportClerk [%d] punishes %s with SSN %d to wait\n", index, SSN, 0, getCustomerType(),""); 
				SetMonitorArrayValue(passPunish, index, TRUE);
			} else {
				printf("PassportClerk [%d] gives valid certification to %s with SSN %d\n", index, SSN, 0, getCustomerType(),""); 
				
				SetMonitorArrayValue(passPunish, index, FALSE);

				/*ForkWithArg(passClerkFileData, SSN);*/
				SetMonitorArrayValue(passFiled, SSN, TRUE);
				printf("PassportClerk [%d] informs %s with SSN %d that the procedure has completed.\n", index, SSN, 0, getCustomerType(),""); 
			}
			
			tprintf("PassClerk %d: Signaling my passClerkCV\n", index,0,0,"","");
			/*passClerkCVs[index]->Signal(passClerkLocks[index]);*/
			Signal(myConditionID, myLockID);
			tprintf("PassClerk %d: Releasing my own lock\n", index,0,0,"","");
			/*passClerkLocks[index]->Release();*/
			Release(myLockID);

		}
		else{ /*No one in line*/
			/*passLineLock->Release();*/
			Release(passLineLock);
			/*passClerkLocks[index]->Acquire();*/
			/*tprintf("PassportClerk [%d] acquiring his own lock\n", index,0,0,"","");*/
			Acquire(myLockID);
			printf("PassportClerk [%d] is going on break\n", index,0,0,"","");
			SetMonitorArrayValue(passClerkStatuses, index, CLERK_ON_BREAK);
			/*passClerkCVs[index]->Wait(passClerkLocks[index]);*/
			Wait(myConditionID, myLockID);
			printf("PassportClerk [%d] returned from break\n", index,0,0,"","");
			/*passClerkLocks[index]->Release();*/
			Release(myLockID);
		}
		Yield();
	}
	printf("About to exit!", 0,0,0,"","");
}

int main() {
	PassClerkRun();
	Exit(0);
}